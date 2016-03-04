/*
 * REDEFINEMCInstrScheduler.cpp
 *
 *  Created on: 06-Dec-2015
 *      Author: kavitha
 */

#include "REDEFINEMCInstrScheduler.h"
#include "REDEFINETargetMachine.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/Support/Debug.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/LiveIntervalAnalysis.h"
#include "llvm/CodeGen/LiveInterval.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
using namespace llvm;

REDEFINEMCInstrScheduler::REDEFINEMCInstrScheduler(MachineSchedContext *C, MachineSchedStrategy *S) :
		ScheduleDAGMI(C, S) {
	ceCount = ((REDEFINETargetMachine&) TM).getSubtargetImpl()->getCeCount();
	frameSize = ((REDEFINETargetMachine&) TM).getSubtargetImpl()->getCfSize();
}

REDEFINEMCInstrScheduler::~REDEFINEMCInstrScheduler() {
	// TODO Auto-generated destructor stub
}

/// If this iterator is a debug value, increment until reaching the End or a
/// non-debug instruction.
static MachineBasicBlock::iterator nextIfDebug(MachineBasicBlock::iterator I, MachineBasicBlock::iterator End) {
	for (; I != End; ++I) {
		if (!I->isDebugValue()) break;
	}
	return I;
}

/// Decrement this iterator until reaching the top or a non-debug instr.
static MachineBasicBlock::iterator priorNonDebug(MachineBasicBlock::iterator I, MachineBasicBlock::iterator Beg) {
	assert(I != Beg && "reached the top of the region, cannot decrement");
	while (--I != Beg) {
		if (!I->isDebugValue()) break;
	}
	return I;
}

void REDEFINEMCInstrScheduler::schedule() {

	errs() << "Initial basic block state:";
	BB->dump();
	buildDAGWithRegPressure();

	Topo.InitDAGTopologicalSorting();

	postprocessDAG();

	SmallVector<SUnit*, 8> TopRoots, BotRoots;
	findRootsAndBiasEdges(TopRoots, BotRoots);

	// Initialize the strategy before modifying the DAG.
	// This may initialize a DFSResult to be used for queue priority.
	SchedImpl->initialize(this);

	DEBUG(for (unsigned su = 0, e = SUnits.size(); su != e; ++su) SUnits[su].dumpAll(this));

	// Initialize ready queues now that the DAG and priority data are finalized.
	initQueues(TopRoots, BotRoots);

	DEBUG(for (unsigned su = 0, e = SUnits.size(); su != e; ++su) SUnits[su].dumpAll(this));

	bool IsTopNode = true;
	unsigned currentCE = 0;

	//Distributing instructions round-robin
	while (SUnit *SU = SchedImpl->pickNode(IsTopNode)) {
		scheduleMI(SU, IsTopNode);
		instructionAndPHyperOpMap.push_back(make_pair(SU, currentCE));
		currentCE = (currentCE + 1) % ceCount;
		updateQueues(SU, IsTopNode);
	}
	assert(CurrentTop == CurrentBottom && "Nonempty unscheduled zone.");

	placeDebugValues();

	DEBUG( {
		unsigned BBNum = begin()->getParent()->getNumber();
		dbgs() << "*** Final schedule for BB#" << BBNum << " ***\n"
		;
		dumpSchedule()
		;
		dbgs() << '\n'
		;
	}
);

}

/// Move an instruction and update register pressure.
void REDEFINEMCInstrScheduler::scheduleMI(SUnit *SU, bool IsTopNode) {
// Move the instruction to its new location in the instruction stream.
MachineInstr *MI = SU->getInstr();
if (IsTopNode) {
	assert(SU->isTopReady() && "node still has unscheduled dependencies");
	if (&*CurrentTop == MI) {
		CurrentTop = nextIfDebug(++CurrentTop, CurrentBottom);
	}
	else {
		moveInstruction(MI, CurrentTop);
		TopRPTracker.setPos(MI);
	}

	// Update top scheduled pressure.
	TopRPTracker.advance();
	assert(TopRPTracker.getPos() == CurrentTop && "out of sync");
	updateScheduledPressure(TopRPTracker.getPressure().MaxSetPressure);
}
else {
	assert(SU->isBottomReady() && "node still has unscheduled dependencies");
	MachineBasicBlock::iterator priorII = priorNonDebug(CurrentBottom, CurrentTop);
	if (&*priorII == MI) CurrentBottom = priorII;
	else {
		if (&*CurrentTop == MI) {
			CurrentTop = nextIfDebug(++CurrentTop, priorII);
			TopRPTracker.setPos(CurrentTop);
		}
		moveInstruction(MI, CurrentBottom);
		CurrentBottom = MI;
	}
	// Update bottom scheduled pressure.
	BotRPTracker.recede();
	assert(BotRPTracker.getPos() == CurrentBottom && "out of sync");
	updateScheduledPressure(BotRPTracker.getPressure().MaxSetPressure);
}
}

void REDEFINEMCInstrScheduler::finishBlock() {
errs() << "finalizing basic block bb#" << BB->getNumber();
BB->dump();
unsigned currentCE = 0;
DebugLoc location = BB->begin()->getDebugLoc();

unsigned faninOfHyperOp[ceCount];
//We need this to do additional code motion and ease creation of pHyperOp bundles
MachineInstr* firstInstructionOfpHyperOp[ceCount];
unsigned firstInstructionPosition[ceCount];
//First index corresponds to the CE and the value corresponds to the register containing the base address of the scratch pad location of the consumer CE to which the producer CE is writing to
int registerContainingBaseAddress[ceCount][ceCount];

for (unsigned i = 0; i < ceCount; i++) {
	faninOfHyperOp[i] = 0;
	firstInstructionOfpHyperOp[i] = 0;
	firstInstructionPosition[i] = 0;
	for (unsigned j = 0; j < ceCount; j++) {
		registerContainingBaseAddress[i][j] = -1;
	}
}
MachineFunction &MF = *(BB->getParent());

//Second pair contains the CE id and the position of the instruction in the instruction sequence
list<pair<MachineInstr*, pair<unsigned, unsigned> > > allInstructionsOfPHyperOps;
unsigned position = 0;
MachineBasicBlock& parentBasicBlock = *BB;

for (list<pair<SUnit*, unsigned> >::iterator ScheduledInstrItr = instructionAndPHyperOpMap.begin(); ScheduledInstrItr != instructionAndPHyperOpMap.end(); ScheduledInstrItr++) {
	unsigned ceContainingInstruction = ScheduledInstrItr->second;
	SUnit* SU = ScheduledInstrItr->first;
	MachineInstr* machineInstruction = SU->getInstr();
	unsigned additionalFanin = 0;
	for (SmallVector<SDep, 4>::iterator predecessorItr = SU->Preds.begin(); predecessorItr != SU->Preds.end(); predecessorItr++) {
		for (list<pair<SUnit*, unsigned> >::iterator predecessorInstrItr = instructionAndPHyperOpMap.begin(); predecessorInstrItr != instructionAndPHyperOpMap.end(); predecessorInstrItr++) {
			if (predecessorInstrItr->first->getInstr() == predecessorItr->getSUnit()->getInstr() && predecessorInstrItr->second != ceContainingInstruction) {
				additionalFanin++;
			}
		}
	}
	if (additionalFanin + faninOfHyperOp[currentCE] > SPLOCATIONS - ceCount - 1) {
		//Add sync barrier in all CEs
		for (unsigned i = 0; i < ceCount; i++) {
			unsigned sourceSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			//Write to all other CEs
			for (unsigned j = 0; j < ceCount; j++) {
				if (j != i) {
					MachineInstrBuilder sourceLui = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
					sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
					//TODO May need changing later
					sourceLui.addImm(j);

					MachineInstrBuilder writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::WRITEPM));
					writepm.addReg(sourceSpAddressRegister, RegState::InternalRead);
					//Dummy data
					writepm.addReg(sourceSpAddressRegister, RegState::InternalRead);
					writepm.addImm(SPLOCATIONS - i - 4);

					allInstructionsOfPHyperOps.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(i, position++)));
					allInstructionsOfPHyperOps.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i, position++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(sourceLui.operator llvm::MachineInstr *());
					LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());
					LIS->getOrCreateInterval(sourceSpAddressRegister);
				}
			}

			//Read from all other CEs
			for (unsigned j = 0; j < ceCount; j++) {
				if (j != i) {
					MachineInstrBuilder sourceLui = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
					sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
					//TODO May need changing later
					sourceLui.addImm(i);

					MachineInstrBuilder readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::READPM));
					readpm.addReg(sourceSpAddressRegister, RegState::Kill);
					//Dummy data
					readpm.addReg(sourceSpAddressRegister, RegState::InternalRead);
					readpm.addImm(SPLOCATIONS - j - 4);

					allInstructionsOfPHyperOps.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(i, position++)));
					allInstructionsOfPHyperOps.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i, position++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(sourceLui.operator llvm::MachineInstr *());
					LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
					LIS->getOrCreateInterval(sourceSpAddressRegister);
				}
			}
			for (unsigned i = 0; i < ceCount; i++) {
				faninOfHyperOp[i] = 0;
			}
		}
	}

	if (!SU->Preds.empty()) {
		for (SmallVector<SDep, 4>::iterator predecessorItr = SU->Preds.begin(); predecessorItr != SU->Preds.end(); predecessorItr++) {
			SDep dependence = (*predecessorItr);
			SUnit* predecessor = predecessorItr->getSUnit();
			unsigned ceContainingPredecessorInstruction = -1;
			for (list<pair<SUnit*, unsigned> >::iterator predecessorInstrItr = instructionAndPHyperOpMap.begin(); predecessorInstrItr != instructionAndPHyperOpMap.end(); predecessorInstrItr++) {
				if (predecessorInstrItr->first->getInstr() == predecessor->getInstr()) {
					ceContainingPredecessorInstruction = predecessorInstrItr->second;
					break;
				}
			}
			//Data might be live in register from a previous basic block; read from memory location
			if (ceContainingPredecessorInstruction < 0 && dependence.isAssignedRegDep()) {
				//Find the memory location allocated to the live-out register
				if (registerAndFrameLocation.find(dependence.getReg()) != registerAndFrameLocation.end()) {
					unsigned frameLocationToReadFrom = registerAndFrameLocation.find(dependence.getReg())->second;
					//Add an instruction to read from memory
					MachineInstrBuilder loadFromMemory = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::ADDI));
					loadFromMemory.addReg(dependence.getReg(), RegState::Define);
					loadFromMemory.addReg(REDEFINE::zero, RegState::InternalRead);
					loadFromMemory.addFrameIndex(frameLocationToReadFrom);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(loadFromMemory.operator llvm::MachineInstr *());
					allInstructionsOfPHyperOps.push_back(make_pair(loadFromMemory.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, position++)));
				}
			}

			else if (ceContainingPredecessorInstruction != ceContainingInstruction) {
				unsigned offsetInScratchpad = faninOfHyperOp[ceContainingInstruction];
				unsigned shiftAmountForBaseAddress = (unsigned) ceil(log2(ceContainingInstruction * SPLOCATIONS));
				//Load the base scratchpad address to a register in the producer CE for the first time
				if (registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction] == -1) {
					MachineInstrBuilder sourceLui = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
					unsigned sourceSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
					sourceLui.addImm(ceContainingInstruction);

					LIS->getSlotIndexes()->insertMachineInstrInMaps(sourceLui.operator llvm::MachineInstr *());
					LIS->getOrCreateInterval(sourceSpAddressRegister);
					allInstructionsOfPHyperOps.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, position++)));
					registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction] = (int) sourceSpAddressRegister;
				}

				//Load the base scratchpad address to a register in the consumer CE the first time
				if (registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction] == -1) {
					MachineInstrBuilder targetLui = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
					unsigned targetSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					targetLui.addReg(targetSpAddressRegister, RegState::Define);
					//TODO May need changing later
					targetLui.addImm(ceContainingInstruction);

					LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());
					LIS->getOrCreateInterval(targetSpAddressRegister);
					allInstructionsOfPHyperOps.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, position++)));
					registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction] = (int) targetSpAddressRegister;

					if (firstInstructionOfpHyperOp[ceContainingInstruction] == 0) {
						firstInstructionOfpHyperOp[ceContainingInstruction] = targetLui;
						firstInstructionPosition[ceContainingInstruction] = position;
					}

				}

				//TODO If number of registers spills into larger than 12 bit size because immediate value supported by readpm and writepm is a signed 11 bit field
				if (ceil(log2(offsetInScratchpad)) > 11) {
					//Add addi instruction incrementing the base address field in source
					MachineInstrBuilder sourceAddi = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::ADDI));
					sourceAddi.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction], RegState::Kill);
					sourceAddi.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction], RegState::InternalRead);
					sourceAddi.addImm(offsetInScratchpad - 4);
					allInstructionsOfPHyperOps.push_back(make_pair(sourceAddi.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, position++)));

					//Add addi instruction incrementing the base address field in target
					MachineInstrBuilder targetAddi = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::ADDI));
					targetAddi.addReg(registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction], RegState::Kill);
					targetAddi.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction], RegState::InternalRead);
					targetAddi.addImm(offsetInScratchpad);
					allInstructionsOfPHyperOps.push_back(make_pair(targetAddi.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, position++)));

					offsetInScratchpad = faninOfHyperOp[ceContainingInstruction] = 4;
				}

				//Write the data onto the ce's scratchpad and read from there
				//Add writepm or fwritepm in predCE
				MachineInstrBuilder writepm;
				MachineInstrBuilder readpm;
				if (dependence.isAssignedRegDep()) {
					const TargetRegisterClass* regClass = ((REDEFINETargetMachine&) TM).FuncInfo->RegInfo->getRegClass(dependence.getReg());
					if (*regClass->vt_begin() == MVT::i32) {
						writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::WRITEPM));
						readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::READPM));
					}
					else if (*regClass->vt_begin() == MVT::f32) {
						writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::FWRITEPM));
						readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::FREADPM));
					}
				}
				else {
					writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::WRITEPM));
					readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::READPM));
				}
				writepm.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction], RegState::InternalRead);

				if (dependence.isAssignedRegDep()) {
					//Use register operand in the inserted writepm
					writepm.addReg(dependence.getReg(), RegState::InternalRead);
					readpm.addReg(dependence.getReg(), RegState::Kill);
				}
				else {
					//dependence still exists, associate a dummy virtual register
					unsigned dummySource = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					writepm.addReg(dummySource, RegState::InternalRead);
					unsigned dummyTarget = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					readpm.addReg(dummyTarget, RegState::Kill);
					LIS->getOrCreateInterval(dummySource);
					LIS->getOrCreateInterval(dummyTarget);
				}
				writepm.addImm(offsetInScratchpad);
				readpm.addReg(registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction], RegState::InternalRead);
				readpm.addImm(offsetInScratchpad);

				allInstructionsOfPHyperOps.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, position++)));
				allInstructionsOfPHyperOps.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, position++)));
				//Increment by 4 since scratchpad is byte addressable
				faninOfHyperOp[ceContainingInstruction] = faninOfHyperOp[ceContainingInstruction] + 4;
				LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());
				LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
			}
		}
	}
	if (firstInstructionOfpHyperOp[ceContainingInstruction] == 0) {
		firstInstructionOfpHyperOp[ceContainingInstruction] = machineInstruction;
		firstInstructionPosition[ceContainingInstruction] = position;
	}
	allInstructionsOfPHyperOps.push_back(make_pair(machineInstruction, make_pair(ceContainingInstruction, position++)));
}

//If the basic block has a terminator, add synchronization barrier to ensure memory operations are complete
if (BB->getFirstTerminator() != BB->end()) {
	//Add sync barrier in all CEs
	for (unsigned i = 0; i < ceCount; i++) {
		unsigned sourceSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		//Write to all other CEs
		for (unsigned j = 0; j < ceCount; j++) {
			if (j != i) {
				MachineInstrBuilder sourceLui = BuildMI(parentBasicBlock, BB->getFirstTerminator(), location, TII->get(REDEFINE::LUI));
				sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
				//TODO May need changing later
				sourceLui.addImm(j);

				//Addi to bump up address to half the size of the entire DGM block of a CR since full address does not fit in 12 bits operand field of addi
				MachineInstrBuilder firstSourceAddi = BuildMI(parentBasicBlock, BB->getFirstTerminator(), location, TII->get(REDEFINE::ADDI));
				firstSourceAddi.addReg(sourceSpAddressRegister, RegState::Kill);
				firstSourceAddi.addReg(sourceSpAddressRegister, RegState::InternalRead);
				firstSourceAddi.addImm(SPLOCATIONS / 2 - 2 - 2 * i);

				//Addi to bump up the address to the end
				MachineInstrBuilder secondSourceAddi = BuildMI(parentBasicBlock, BB->getFirstTerminator(), location, TII->get(REDEFINE::ADDI));
				secondSourceAddi.addReg(sourceSpAddressRegister, RegState::Kill);
				secondSourceAddi.addReg(sourceSpAddressRegister, RegState::InternalRead);
				secondSourceAddi.addImm(SPLOCATIONS / 2 - 2 - 2 * i);

				MachineInstrBuilder writepm = BuildMI(parentBasicBlock, BB->getFirstTerminator(), location, TII->get(REDEFINE::WRITEPM));
				writepm.addReg(sourceSpAddressRegister, RegState::InternalRead);
				//Dummy data
				writepm.addReg(sourceSpAddressRegister, RegState::InternalRead);
				writepm.addImm(0);

				allInstructionsOfPHyperOps.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(i, position++)));
				allInstructionsOfPHyperOps.push_back(make_pair(firstSourceAddi.operator llvm::MachineInstr *(), make_pair(i, position++)));
				allInstructionsOfPHyperOps.push_back(make_pair(secondSourceAddi.operator llvm::MachineInstr *(), make_pair(i, position++)));
				allInstructionsOfPHyperOps.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i, position++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(sourceLui.operator llvm::MachineInstr *());
				LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());
				LIS->getOrCreateInterval(sourceSpAddressRegister);
			}
		}

		//Read from all other CEs
		for (unsigned j = 0; j < ceCount; j++) {
			if (j != i) {
				MachineInstrBuilder targetLui = BuildMI(parentBasicBlock, BB->getFirstTerminator(), location, TII->get(REDEFINE::LUI));
				targetLui.addReg(sourceSpAddressRegister, RegState::Define);
				//TODO May need changing later
				targetLui.addImm(i);

				//Addi to bump up address to half the size of the entire DGM block of a CR since full address does not fit in 12 bits operand field of addi
				MachineInstrBuilder firstTargetAddi = BuildMI(parentBasicBlock, BB->getFirstTerminator(), location, TII->get(REDEFINE::ADDI));
				firstTargetAddi.addReg(sourceSpAddressRegister, RegState::Kill);
				firstTargetAddi.addReg(sourceSpAddressRegister, RegState::InternalRead);
				firstTargetAddi.addImm(SPLOCATIONS / 2 - 2 - 2 * j);

				//Addi to bump up the address to the end
				MachineInstrBuilder secondTargetAddi = BuildMI(parentBasicBlock, BB->getFirstTerminator(), location, TII->get(REDEFINE::ADDI));
				secondTargetAddi.addReg(sourceSpAddressRegister, RegState::Kill);
				secondTargetAddi.addReg(sourceSpAddressRegister, RegState::InternalRead);
				secondTargetAddi.addImm(SPLOCATIONS / 2 - 2 - 2 * j);

				MachineInstrBuilder readpm = BuildMI(parentBasicBlock, BB->getFirstTerminator(), location, TII->get(REDEFINE::READPM));
				readpm.addReg(sourceSpAddressRegister, RegState::Kill);
				//Dummy data
				readpm.addReg(sourceSpAddressRegister, RegState::InternalRead);
				readpm.addImm(0);

				allInstructionsOfPHyperOps.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(i, position++)));
				allInstructionsOfPHyperOps.push_back(make_pair(firstTargetAddi.operator llvm::MachineInstr *(), make_pair(i, position++)));
				allInstructionsOfPHyperOps.push_back(make_pair(secondTargetAddi.operator llvm::MachineInstr *(), make_pair(i, position++)));
				allInstructionsOfPHyperOps.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i, position++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());
				LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
				LIS->getOrCreateInterval(sourceSpAddressRegister);
			}
		}
	}
}

//Copy all terminators to a list before adding nop instructions after the terminators
list<MachineInstr*> terminatorInstructions;
for (MachineBasicBlock::instr_iterator terminatorItr = BB->getFirstInstrTerminator(); terminatorItr != BB->end(); terminatorItr++) {
	terminatorInstructions.push_back(terminatorItr);
}

//Reset HyperOp fanin to zero
for (unsigned i = 0; i < ceCount; i++) {
	faninOfHyperOp[i] = 0;
}

MachineInstr* lastInstr = BB->getFirstInstrTerminator();
//Dump in memory all the registers that are live-out
for (unsigned i = 0, e = RPTracker.getPressure().LiveOutRegs.size(); i < e; i++) {
	unsigned liveoutRegister = RPTracker.getPressure().LiveOutRegs[i];
	SmallVector<unsigned, 8> LiveInRegs = RPTracker.getPressure().LiveInRegs;
	//Find out the definition of the live-out register
	VNInfo* regValueNumber = LIS->getInterval(liveoutRegister).getVNInfoBefore(LIS->getInstructionIndex(&BB->back()));
	MachineInstr* reachingDefinitionInstruction = LIS->getInstructionFromIndex(regValueNumber->def);
	int ceContainingInstruction = -1;
	for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator allInstructionItr = allInstructionsOfPHyperOps.begin(); allInstructionItr != allInstructionsOfPHyperOps.end(); allInstructionItr++) {
		if (allInstructionItr->first == reachingDefinitionInstruction) {
			ceContainingInstruction = allInstructionItr->second.first;
			break;
		}
	}

	//If the register was live-in and not redefined in the basic block, do nothing since the data is in memory already; Otherwise, add a sw instruction
	if (!(ceContainingInstruction == -1 && find(LiveInRegs.begin(), LiveInRegs.end(), liveoutRegister) != LiveInRegs.end())) {
		MachineInstrBuilder storeInMem = BuildMI(parentBasicBlock, lastInstr, location, TII->get(REDEFINE::SW));
		storeInMem.addReg(REDEFINE::zero, RegState::InternalRead);
		storeInMem.addReg(liveoutRegister, RegState::InternalRead);
		storeInMem.addFrameIndex(nextFrameLocation);
		registerAndFrameLocation.insert(make_pair(liveoutRegister, nextFrameLocation));
		nextFrameLocation += 1;
		LIS->getSlotIndexes()->insertMachineInstrInMaps(storeInMem.operator llvm::MachineInstr *());
		allInstructionsOfPHyperOps.push_back(make_pair(storeInMem.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, position++)));
	}
}

//Add all terminators to all pHyperOps
for (list<MachineInstr*>::iterator terminatorInstrItr = terminatorInstructions.begin(); terminatorInstrItr != terminatorInstructions.end(); terminatorInstrItr++) {
	MachineInstr* terminatorItr = (*terminatorInstrItr);
	errs() << "terminator of bb#" << terminatorItr->getParent()->getNumber();
	terminatorItr->dump();
	//	See if some predecessors exist which require additional inter-pHyperOp communication instructions
	for (unsigned i = 0; i < terminatorItr->getNumOperands(); i++) {
		MachineOperand& operand = terminatorItr->getOperand(i);
		if (operand.isReg()) {
			SlotIndex regSlotIndex = LIS->getInstructionIndex(terminatorItr);
			VNInfo * reachingDefinition = LIS->getInterval(operand.getReg()).getVNInfoBefore(regSlotIndex);
			MachineInstr* reachingDefinitionInstruction = LIS->getInstructionFromIndex(reachingDefinition->def);
			int ceContainingPredecessorInstruction = -1;
			for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator allInstructionItr = allInstructionsOfPHyperOps.begin(); allInstructionItr != allInstructionsOfPHyperOps.end(); allInstructionItr++) {
				if (allInstructionItr->first == reachingDefinitionInstruction) {
					ceContainingPredecessorInstruction = allInstructionItr->second.first;
					break;
				}
			}
			//Data might be live in register from a previous basic block; read from memory location
			if (ceContainingPredecessorInstruction < 0) {
				//Find the memory location allocated to the live-out register
				if (registerAndFrameLocation.find(operand.getReg()) != registerAndFrameLocation.end()) {
					unsigned frameLocationToReadFrom = registerAndFrameLocation.find(operand.getReg())->second;
					//Add an instruction to read from memory in all CEs
					for (unsigned ceContainingInstruction = 0; ceContainingInstruction < ceCount; ceContainingInstruction++) {
						MachineInstrBuilder loadFromMemory = BuildMI(parentBasicBlock, terminatorItr, location, TII->get(REDEFINE::ADDI));
						loadFromMemory.addReg(operand.getReg(), RegState::Define);
						loadFromMemory.addReg(REDEFINE::zero, RegState::InternalRead);
						loadFromMemory.addFrameIndex(frameLocationToReadFrom);
						LIS->getSlotIndexes()->insertMachineInstrInMaps(loadFromMemory.operator llvm::MachineInstr *());
						allInstructionsOfPHyperOps.push_back(make_pair(loadFromMemory.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, position++)));
					}
				}
			}
			else {
				for (unsigned ceContainingInstruction = 0; ceContainingInstruction < ceCount; ceContainingInstruction++) {
					if (ceContainingInstruction != ceContainingPredecessorInstruction) {
						//Hardcoded first location to be used for communicating the operand of branch instruction because there is no use in doing any fancy register allocation in this
						//Since sync barrier has already been executed, sp locations can be reused from 0
						//TODO while technically we can run out of communication registers here as well, it is highly unlikely we ever will since the number of branch instructions cant be so high
						unsigned offsetInScratchpad = faninOfHyperOp[ceContainingInstruction];
						//Add instruction to communicate the operand to the pHyperOp
						if (registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction] == -1) {
							MachineInstrBuilder sourceLui = BuildMI(parentBasicBlock, terminatorItr, location, TII->get(REDEFINE::LUI));
							unsigned sourceSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
							sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
							//TODO May need changing later
							sourceLui.addImm(ceContainingInstruction);

							LIS->getSlotIndexes()->insertMachineInstrInMaps(sourceLui.operator llvm::MachineInstr *());
							LIS->getOrCreateInterval(sourceSpAddressRegister);
							allInstructionsOfPHyperOps.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, position++)));
							registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction] = (int) sourceSpAddressRegister;
						}

						//Load the base scratchpad address to a register in the consumer CE the first time
						if (registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction] == -1) {
							MachineInstrBuilder targetLui = BuildMI(parentBasicBlock, terminatorItr, location, TII->get(REDEFINE::LUI));
							unsigned targetSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
							targetLui.addReg(targetSpAddressRegister, RegState::Define);
							//TODO May need changing later
							targetLui.addImm(ceContainingInstruction);

							LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());
							LIS->getOrCreateInterval(targetSpAddressRegister);
							allInstructionsOfPHyperOps.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, position++)));
							registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction] = (int) targetSpAddressRegister;

							if (firstInstructionOfpHyperOp[ceContainingInstruction] == 0) {
								firstInstructionOfpHyperOp[ceContainingInstruction] = targetLui;
								firstInstructionPosition[ceContainingInstruction] = position;
							}
						}

						//TODO If number of registers spills into larger than 12 bit size because immediate value supported by readpm and writepm is a signed 11 bit field
						if (ceil(log2(offsetInScratchpad)) > 11) {
							//Add addi instruction incrementing the base address field in source
							MachineInstrBuilder sourceAddi = BuildMI(parentBasicBlock, terminatorItr, location, TII->get(REDEFINE::ADDI));
							sourceAddi.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction], RegState::Kill);
							sourceAddi.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction], RegState::InternalRead);
							sourceAddi.addImm(offsetInScratchpad - 4);
							allInstructionsOfPHyperOps.push_back(make_pair(sourceAddi.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, position++)));

							//Add addi instruction incrementing the base address field in target
							MachineInstrBuilder targetAddi = BuildMI(parentBasicBlock, terminatorItr, location, TII->get(REDEFINE::ADDI));
							targetAddi.addReg(registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction], RegState::Kill);
							targetAddi.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction], RegState::InternalRead);
							targetAddi.addImm(offsetInScratchpad);
							allInstructionsOfPHyperOps.push_back(make_pair(targetAddi.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, position++)));

							offsetInScratchpad = faninOfHyperOp[ceContainingInstruction] = 4;
						}
						//Add writepm in predCE
						MachineInstrBuilder writepm;
						MachineInstrBuilder readpm;
						const TargetRegisterClass* regClass = ((REDEFINETargetMachine&) TM).FuncInfo->RegInfo->getRegClass(operand.getReg());
						if (*regClass->vt_begin() == MVT::i32) {
							writepm = BuildMI(parentBasicBlock, terminatorItr, location, TII->get(REDEFINE::WRITEPM));
							readpm = BuildMI(parentBasicBlock, terminatorItr, location, TII->get(REDEFINE::READPM));
						}
						else if (*regClass->vt_begin() == MVT::f32) {
							writepm = BuildMI(parentBasicBlock, terminatorItr, location, TII->get(REDEFINE::FWRITEPM));
							readpm = BuildMI(parentBasicBlock, terminatorItr, location, TII->get(REDEFINE::FREADPM));
						}

						writepm.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction], RegState::InternalRead);
						//Use register operand in the inserted writepm
						writepm.addReg(operand.getReg(), RegState::InternalRead);
						writepm.addImm(offsetInScratchpad);

						//Add readpm instruction in the target CE
						readpm.addReg(operand.getReg(), RegState::Kill);
						readpm.addReg(registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction], RegState::InternalRead);
						readpm.addImm(offsetInScratchpad);

						allInstructionsOfPHyperOps.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, position++)));
						allInstructionsOfPHyperOps.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, position++)));

						//Increment by 4 since scratchpad is byte addressable
						faninOfHyperOp[ceContainingInstruction] = faninOfHyperOp[ceContainingInstruction] + 4;
						LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());
						LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
					}
				}
			}

		}
	}

	//Add the jump instruction to the first pHyperOp
	allInstructionsOfPHyperOps.push_back(make_pair(terminatorItr, make_pair(0, position++)));
	//Replicate the branch instruction in other CEs
	for (unsigned i = 1; i < ceCount; i++) {
		MachineInstrBuilder duplicateTerminatorInstr = BuildMI(parentBasicBlock, terminatorItr, location, TII->get(terminatorItr->getOpcode()));
		for (unsigned operandIndex = 0; operandIndex < terminatorItr->getNumOperands(); operandIndex++) {
			duplicateTerminatorInstr.addOperand(terminatorItr->getOperand(operandIndex));
		}
		allInstructionsOfPHyperOps.push_back(make_pair(duplicateTerminatorInstr.operator llvm::MachineInstr *(), make_pair(i, position++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(duplicateTerminatorInstr.operator llvm::MachineInstr *());
	}

	MachineInstr* successorOfTerminator;
	if (terminatorItr->getNextNode() != parentBasicBlock.end()) {
		successorOfTerminator = terminatorItr->getNextNode();
	}
	else {
		successorOfTerminator = parentBasicBlock.end();
	}
	//After every terminator, add 2 nops since there is no rollback
	for (unsigned i = 0; i < ceCount; i++) {
		for (unsigned j = 0; j < 2; j++) {
			MachineInstrBuilder nopInstruction = BuildMI(parentBasicBlock, successorOfTerminator, location, TII->get(REDEFINE::ADDI));
			nopInstruction.addReg(REDEFINE::zero, RegState::InternalRead);
			nopInstruction.addReg(REDEFINE::zero, RegState::InternalRead);
			nopInstruction.addImm(0);
			LIS->getSlotIndexes()->insertMachineInstrInMaps(nopInstruction.operator llvm::MachineInstr *());
			allInstructionsOfPHyperOps.push_back(make_pair(nopInstruction.operator llvm::MachineInstr *(), make_pair(i, position++)));
		}
	}
}

//Check if this is the last basic block of the function and add writecm instructions and fbind( fbind is in case of context frames being reused statically and hence, is added optionally); returns are assumed to be merged
//I am assuming here that there is only one exit block since we merge return
if (BB->getFullName().compare(MF.back().getFullName()) == 0) {
	HyperOpInteractionGraph * graph = ((REDEFINETargetMachine&) TM).HIG;
	Function* Fn = const_cast<Function*>(MF.getFunction());
	HyperOp* hyperOp = ((REDEFINETargetMachine&) TM).HIG->getHyperOp(Fn);
	MachineBasicBlock& lastBB = MF.back();
	MachineInstr* insertPosition = lastBB.end();

	//Add Fbind
	//Among the HyperOps immediately dominated by the hyperOp, add fbind for those HyperOps that require it
	list<HyperOp*> immediatelyDominatedHyperOp;
	for (list<HyperOp*>::iterator hyperOpItr = graph->Vertices.begin(); hyperOpItr != graph->Vertices.end(); hyperOpItr++) {
		if (*hyperOpItr != hyperOp && (*hyperOpItr)->isFbindRequired() && (*hyperOpItr)->getImmediateDominator() == hyperOp) {
			//Fbind instruction added to the immediate dominator of the HyperOp
			unsigned hyperOpId = (*hyperOpItr)->getHyperOpId();
			MachineInstrBuilder fbind = BuildMI(lastBB, insertPosition, location, TII->get(REDEFINE::FBIND));
			fbind.addReg(REDEFINE::zero);
			fbind.addReg(REDEFINE::zero);
			fbind.addImm((*hyperOpItr)->getContextFrame());
			allInstructionsOfPHyperOps.push_back(make_pair(fbind.operator llvm::MachineInstr *(), make_pair(currentCE, position++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(fbind.operator llvm::MachineInstr *());
		}
	}
	//End of add fbind

	//LLVM IR is assumed to be in mem form; Just load from the memory location with the same name as the value should do for all the data being communicated between HyperOps
	vector<list<pair<HyperOp*, unsigned> > > consumerHyperOps;
	consumerHyperOps.reserve(ceCount);
	for (unsigned j = 0; j < ceCount; j++) {
		list<pair<HyperOp*, unsigned> > consumerList;
		consumerHyperOps.push_back(consumerList);
	}

	//Cyclically distribute the writecm stubs among pHyperOps
	unsigned currentCE = 0;
	for (map<HyperOpEdge*, HyperOp*>::iterator childItr = hyperOp->ChildMap.begin(); childItr != hyperOp->ChildMap.end(); childItr++) {
		HyperOpEdge* edge = childItr->first;
		HyperOp* consumer = childItr->second;
		//position is multiplied by 4 since the context memory is byte addressable
		unsigned contextFrameOffset = edge->getPositionOfInput() * 4;
		if (edge->getPositionOfInput() == -1) {
			//Scheduling predicate forced to be the last entry in context frame
			contextFrameOffset = frameSize - 1;
		}

		unsigned registerContainingConsumerBase = -1;
		//Check if the consumer's context frame address has already been loaded to memory; If not, add an instruction to load the context frame address to a register
		for (list<pair<HyperOp*, unsigned> >::iterator consumerItr = consumerHyperOps[currentCE].begin(); consumerItr != consumerHyperOps[currentCE].end(); consumerItr++) {
			if (consumerItr->first == consumer) {
				registerContainingConsumerBase = consumerItr->second;
				break;
			}
		}
		if (registerContainingConsumerBase == -1) {
			registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder addi = BuildMI(lastBB, insertPosition, insertPosition->getDebugLoc(), TII->get(REDEFINE::ADDI));
			addi.addReg(registerContainingConsumerBase, RegState::Define);
			addi.addReg(REDEFINE::zero, RegState::InternalRead);
			addi.addImm(consumer->getContextFrame() * frameSize);
			consumerHyperOps[currentCE].push_back(make_pair(consumer, registerContainingConsumerBase));
			//Add instruction to bundle
			allInstructionsOfPHyperOps.push_back(make_pair(addi.operator ->(), make_pair(currentCE, position++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
			LIS->getOrCreateInterval(registerContainingConsumerBase);
		}

		unsigned objectIndex = -1;
		//Get the index of the stack allocated object, starting from 0 because negative offsets from fp contain function arguments
		for (int i = 0; i < MF.getFrameInfo()->getObjectIndexEnd(); i++) {
			const AllocaInst* allocInstr = MF.getFrameInfo()->getObjectAllocation(i);
			//TODO Bad code, why do you use string comparison?
			if (edge->getName().compare(allocInstr->getName()) == 0) {
				//Find the offset of the object wrt SP
				objectIndex = i;
				break;
			}
		}

		//If the edge is a forced control edge for precedence
		if (objectIndex == -1) {
			//There can only be one such incoming control edge predicating execution, hence not tracking them separately
			objectIndex = MF.getFrameInfo()->getObjectOffset(MF.getFrameInfo()->getObjectIndexEnd() - 1) + 1;
		}

		//TODO Add a load instruction to get data from memory onto a register; There could be forced schedule edges that we don't want to add load instructions for the same
		unsigned registerContainingData = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		if (objectIndex != -1) {
			MachineInstrBuilder loadInstr = BuildMI(lastBB, insertPosition, insertPosition->getDebugLoc(), TII->get(REDEFINE::LW));
			loadInstr.addReg(registerContainingData, RegState::Define);
			loadInstr.addReg(REDEFINE::zero);
			loadInstr.addFrameIndex(objectIndex);
			//Add instruction to bundle
			allInstructionsOfPHyperOps.push_back(make_pair(loadInstr.operator ->(), make_pair(currentCE, position++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(loadInstr.operator llvm::MachineInstr *());
			LIS->getOrCreateInterval(registerContainingData);

		}
		//No need to access memory for a true predicate, copying via addi will do
		else {
			MachineInstrBuilder addi = BuildMI(lastBB, insertPosition, insertPosition->getDebugLoc(), TII->get(REDEFINE::ADDI));
			addi.addReg(registerContainingData, RegState::Define);
			addi.addReg(REDEFINE::zero, RegState::InternalRead);
			addi.addImm(1);
			//Add instruction to bundle
			allInstructionsOfPHyperOps.push_back(make_pair(addi.operator ->(), make_pair(currentCE, position++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
			LIS->getOrCreateInterval(registerContainingData);
		}
		MachineInstrBuilder writeToContextFrame;
		if (edge->Type == HyperOpEdge::DATA) {
			writeToContextFrame = BuildMI(lastBB, insertPosition, insertPosition->getDebugLoc(), TII->get(REDEFINE::WRITECM));
		}
		else if (edge->Type == HyperOpEdge::CONTROL) {
			//Forced serialization edges need not be added if there is any other edge between the producer and consumer HyperOps
			writeToContextFrame = BuildMI(lastBB, insertPosition, insertPosition->getDebugLoc(), TII->get(REDEFINE::WRITECMP));
		}
		writeToContextFrame.addReg(registerContainingConsumerBase, RegState::InternalRead);
		writeToContextFrame.addReg(registerContainingData, RegState::InternalRead);
		writeToContextFrame.addImm(contextFrameOffset);
		//Add instruction to bundle
		allInstructionsOfPHyperOps.push_back(make_pair(writeToContextFrame.operator ->(), make_pair(currentCE, position++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(writeToContextFrame.operator llvm::MachineInstr *());
		currentCE = (currentCE + 1) % ceCount;
	}

	//Add endHyperOp instruction and 2 nops in each pHyperOp
	for (unsigned i = 0; i < ceCount; i++) {
		MachineInstrBuilder endInstruction = BuildMI(lastBB, insertPosition, location, TII->get(REDEFINE::END));
		endInstruction.addImm(0);
		allInstructionsOfPHyperOps.push_back(make_pair(endInstruction.operator llvm::MachineInstr *(), make_pair(i, position++)));
		//If no instructions in the bb, add end hyperop as the first instr
		if (firstInstructionOfpHyperOp[i] == 0) {
			firstInstructionOfpHyperOp[i] = endInstruction.operator ->();
		}

		for (unsigned j = 0; j < 2; j++) {
			MachineInstrBuilder nopInstruction = BuildMI(lastBB, insertPosition, location, TII->get(REDEFINE::ADDI));
			nopInstruction.addReg(REDEFINE::zero);
			nopInstruction.addReg(REDEFINE::zero);
			nopInstruction.addImm(0);
			allInstructionsOfPHyperOps.push_back(make_pair(nopInstruction.operator llvm::MachineInstr *(), make_pair(i, position++)));
		}
	}
}

//Shuffle instructions
for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator allInstructionItr = allInstructionsOfPHyperOps.begin(); allInstructionItr != allInstructionsOfPHyperOps.end(); allInstructionItr++) {
	MachineInstr* instruction = allInstructionItr->first;
	unsigned ce = allInstructionItr->second.first;
	unsigned position = allInstructionItr->second.second;
	instruction->setFlag(MachineInstr::NoFlags);
	if (ce < ceCount - 1) {
		MachineInstr* nextCeInstruction = firstInstructionOfpHyperOp[ce + 1];
		if (nextCeInstruction == 0) {
			nextCeInstruction = BB->end();
		}
		unsigned nextCeInstructionPosition = firstInstructionPosition[ce + 1];
		if (nextCeInstructionPosition < position) {
			BB->splice(nextCeInstruction, BB, instruction);
		}
	}
}

errs() << "final basic block state:";
BB->dump();
for (unsigned i = 0; i < ceCount; i++) {
	MachineInstr* firstInstructionInCE = firstInstructionOfpHyperOp[i];
	MachineInstr* firstInstructionInNextCE;

	if (firstInstructionInCE == BB->end() || firstInstructionInCE == 0) {
		continue;
	}
	if (i < ceCount - 1) {
		firstInstructionInNextCE = firstInstructionOfpHyperOp[i + 1];
	}
	else {
		firstInstructionInNextCE = BB->end();
	}

	if (firstInstructionInNextCE == 0) {
		firstInstructionInNextCE = BB->end();
	}
	MIBundleBuilder* bundleBuilder = new MIBundleBuilder(*BB, firstInstructionInCE, firstInstructionInNextCE);
}
BB = 0;
}

void REDEFINEMCInstrScheduler::startBlock(MachineBasicBlock *bb) {
BB = bb;
//Initialize frame index to point to the first free location in memory offset by frame address
if (BB->getBasicBlock()->getName().compare(BB->getBasicBlock()->getParent()->getEntryBlock().getName()) == 0) {
	nextFrameLocation = BB->getParent()->getFrameInfo()->getObjectIndexEnd();
}
instructionAndPHyperOpMap.clear();

}
