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
#include "llvm/IR/Module.h"
#include "llvm/MC/MCContext.h"
#include "llvm/ADT/StringExtras.h"
using namespace llvm;
using namespace std;

//TODO Used only 8 bit number here because I need this to address the sync locations and frame locations for replication only which sums upto 20 4 byte locations only
static int32_t SignExtend8BitNumberTo12Bits(int8_t x) {
	return int32_t(x << 4) >> 4;
}

//Returns true if firstInstr appears before secondInstr in the MBB
static bool instructionAppearsBefore(MachineBasicBlock* MBB, MachineInstr* firstInstr, MachineInstr* secondInstr) {
	int firstPosition = -1, secondPosition = -1;
	unsigned currentPosition = 0;
	for (MachineBasicBlock::iterator instrItr = MBB->begin(); instrItr != MBB->end(); instrItr++, currentPosition++) {
		MachineInstr* instr = instrItr;
		if (instr == firstInstr) {
			firstPosition = currentPosition;
		} else if (instr == secondInstr) {
			secondPosition = currentPosition;
		}
		if (firstPosition >= 0 && secondPosition >= 0) {
			break;
		}
	}

	if (firstPosition < secondPosition) {
		return true;
	}
	return false;
}

/**
 * SPM is partitioned 3-way:
 * _______________________
 *|                       |
 *|   SP locations        |
 *|   for inter-ce        |
 *|   communication       |
 *|_______________________|
 *|    	#cecount          |
 *|    locations to       |
 *| synchronize between   |
 *| CEs once SP locations |
 *|  for communication    |
 *|   are all used up     |
 *|_______________________|
 *|     #framesize        |
 *| registers to allow    |
 *| input data to a       |
 *| HyperOp to be         |
 *| replicated            |
 *|_______________________|
 */

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
		if (!I->isDebugValue())
			break;
	}
	return I;
}

/// Decrement this iterator until reaching the top or a non-debug instr.
static MachineBasicBlock::iterator priorNonDebug(MachineBasicBlock::iterator I, MachineBasicBlock::iterator Beg) {
	assert(I != Beg && "reached the top of the region, cannot decrement");
	while (--I != Beg) {
		if (!I->isDebugValue())
			break;
	}
	return I;
}

void REDEFINEMCInstrScheduler::enterRegion(MachineBasicBlock *bb, MachineBasicBlock::iterator begin, MachineBasicBlock::iterator end, unsigned endcount) {
	ScheduleDAGMI::enterRegion(bb, begin, end, endcount);
	instructionAndPHyperOpMapForRegion.clear();
	allInstructionsOfRegion.clear();
	firstInstructionOfpHyperOpInRegion.clear();
	for (unsigned i = 0; i < ceCount; i++) {
		firstInstructionOfpHyperOpInRegion.push_back(0);
		for (unsigned j = 0; j < ceCount; j++) {
			registerContainingBaseAddress[i][j] = -1;
		}
	}
	insertPosition = 0;
}

void REDEFINEMCInstrScheduler::schedule() {
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

//	idx_t numVertices = 0;

	//List of traversed SUnits and an associated index;
//	METIS TODO : map<SUnit*, idx_t> traversedSUnitsWithIndex;

	//Since map doesn't maintain the order of insertion, using a list
	list<SUnit*> traversedSUnitList;
	unsigned ceIndex = 0;
	while (SUnit *SU = SchedImpl->pickNode(IsTopNode)) {
		scheduleMI(SU, IsTopNode);
//		METIS TODO: traversedSUnitsWithIndex[SU] = numVertices;
//		METIS TODO:  numVertices++;

		traversedSUnitList.push_back(SU);
		if (ceCount == 1) {
			instructionAndPHyperOpMapForRegion.push_back(make_pair(SU, 0));
		} else {
			instructionAndPHyperOpMapForRegion.push_back(make_pair(SU, ceIndex));
		}
		ceIndex = (ceIndex + 1) % ceCount;
		updateQueues(SU, IsTopNode);
	}
// METIS TODO:
//	if (ceCount > 1) {
//		//Number of balancing constraints
//		idx_t ncon = 1;
//		//CSR format of storage
//		idx_t xadj[numVertices + 1];
//		vector<idx_t> adjcny;
//		idx_t nparts = ceCount;
//		idx_t objval;
//
//		idx_t part[numVertices + 1];
//		//Create a metis partitioning problem and solve using partkway API
//		idx_t currentAdjIndex = 0;
//		xadj[0] = currentAdjIndex;
//		unsigned xadjIndex = 1;
//		for (list<SUnit*>::iterator traversedSUnitItr = traversedSUnitList.begin(); traversedSUnitItr != traversedSUnitList.end(); traversedSUnitItr++, xadjIndex++) {
//			SUnit* traversedSUnit = *traversedSUnitItr;
//			idx_t indexOfSUnit = traversedSUnitsWithIndex[traversedSUnit];
//			currentAdjIndex += traversedSUnit->Succs.size() - 1;
//			if (currentAdjIndex == -1) {
//				currentAdjIndex = 0;
//			}
//			xadj[xadjIndex] = currentAdjIndex;
//			for (SmallVector<SDep, 4>::iterator successorItr = traversedSUnit->Succs.begin(); successorItr != traversedSUnit->Succs.end(); successorItr++) {
//				SUnit* successor = successorItr->getSUnit();
//				idx_t successorIndex = traversedSUnitsWithIndex.find(successor)->second;
//				adjcny.push_back(successorIndex);
//			}
//		}
//
//		idx_t* adjcnyArray = &adjcny[0];
//		METIS_PartGraphKway(&numVertices, &ncon, xadj, &adjcny[0], NULL, NULL, NULL, &nparts, NULL, NULL, NULL, &objval, part);
//
//		unsigned indexOfSUnit = 0;
//		for (list<SUnit*>::iterator traversedSUnitItr = traversedSUnitList.begin(); traversedSUnitItr != traversedSUnitList.end(); traversedSUnitItr++) {
//			instructionAndPHyperOpMapForRegion.push_back(make_pair(*traversedSUnitItr, part[indexOfSUnit]));
//			indexOfSUnit++;
//		}
//	}

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
	} else {
		moveInstruction(MI, CurrentTop);
		TopRPTracker.setPos(MI);
	}

// Update top scheduled pressure.
	TopRPTracker.advance();
	assert(TopRPTracker.getPos() == CurrentTop && "out of sync");
	updateScheduledPressure(TopRPTracker.getPressure().MaxSetPressure);
} else {
	assert(SU->isBottomReady() && "node still has unscheduled dependencies");
	MachineBasicBlock::iterator priorII = priorNonDebug(CurrentBottom, CurrentTop);
	if (&*priorII == MI)
		CurrentBottom = priorII;
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

void REDEFINEMCInstrScheduler::startBlock(MachineBasicBlock *bb) {
BB = bb;
DEBUG(dbgs() << "\n-------------\nStarting new basic block BB#" << BB->getNumber() << "\n");

firstInstructionOfpHyperOp.clear();
faninOfHyperOp.clear();
for (int i = 0; i < ceCount; i++) {
	faninOfHyperOp.push_back(0);
}
registersUsedInBB.clear();
}

void REDEFINEMCInstrScheduler::exitRegion() {
unsigned currentCE = 0;
DebugLoc location = BB->begin()->getDebugLoc();
MachineFunction &MF = *(BB->getParent());
MachineBasicBlock& parentBasicBlock = *BB;

map<unsigned, unsigned> registerAndDefiningCEMap;
//Map containing the renamed register and its
map<unsigned, map<unsigned, unsigned> > replacementRegisterMap;

//Map containing ce and redefinition of a register via a readpm instruction
//Key of the first map represents the CE and the list contains all the registers already locally defined and hence don't require a readpm from the original definition
map<unsigned, list<unsigned> > redefinitionsInCE;

DEBUG(dbgs() << "Partitioning instructions of the region into multiple CEs\n");

for (list<pair<SUnit*, unsigned> >::iterator ScheduledInstrItr = instructionAndPHyperOpMapForRegion.begin(); ScheduledInstrItr != instructionAndPHyperOpMapForRegion.end(); ScheduledInstrItr++) {
	unsigned ceContainingInstruction = ScheduledInstrItr->second;
	SUnit* SU = ScheduledInstrItr->first;
	MachineInstr* machineInstruction = SU->getInstr();
	unsigned additionalFanin = 0;
	for (SmallVector<SDep, 4>::iterator predecessorItr = SU->Preds.begin(); predecessorItr != SU->Preds.end(); predecessorItr++) {
		for (list<pair<SUnit*, unsigned> >::iterator predecessorInstrItr = instructionAndPHyperOpMapForRegion.begin(); predecessorInstrItr != instructionAndPHyperOpMapForRegion.end(); predecessorInstrItr++) {
			if (predecessorInstrItr->first->getInstr() == predecessorItr->getSUnit()->getInstr() && predecessorInstrItr->second != ceContainingInstruction) {
				additionalFanin++;
			}
		}
	}
//If spm locations have all been exhausted or this is the last instruction in the region and is followed by a branch operation, add sync barrier in all CEs (implemented as a reduction tree of synchronization instructions)
	if ((additionalFanin + faninOfHyperOp[currentCE] > SPLOCATIONS - datawidth * (3 + frameSize))) {
		//Add reduction tree
		unsigned level = 2;
		while (level <= ceCount + 1) {
			unsigned increment = level / 2;
			for (unsigned i = 0; i < ceCount; i += increment) {
				if (i % 2 == 0 && (i + increment) < ceCount) {
					{
						MachineInstrBuilder luiOfTargetCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForTargetAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfTargetCE.addReg(registerForTargetAddr, RegState::Define);
						luiOfTargetCE.addImm(i + increment + 1);
						LIS->getSlotIndexes()->insertMachineInstrInMaps(luiOfTargetCE.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(luiOfTargetCE.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

						if (firstInstructionOfpHyperOpInRegion[i] == 0) {
							firstInstructionOfpHyperOpInRegion[i] = luiOfTargetCE.operator llvm::MachineInstr *();
						}

						//Add writepm to first CE
						MachineInstrBuilder writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::WRITEPM));
						writepm.addReg(registerForTargetAddr);
						//Dummy data
						writepm.addReg(registerForTargetAddr);
						int8_t immediateSPOffset = -((3 + frameSize) * datawidth);
						writepm.addImm(SignExtend8BitNumberTo12Bits(immediateSPOffset));
						allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());

						MachineInstrBuilder luiOfCurrentCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForCurrentAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfCurrentCE.addReg(registerForCurrentAddr, RegState::Define);
						luiOfCurrentCE.addImm(i + 1);
						LIS->getSlotIndexes()->insertMachineInstrInMaps(luiOfCurrentCE.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(luiOfCurrentCE.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

						//Add readpm to first CE
						MachineInstrBuilder readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::READPM));
						unsigned readpmTarget = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						readpm.addReg(readpmTarget, RegState::Define);
						LIS->getOrCreateInterval(readpmTarget);
						//Dummy data
						readpm.addReg(registerForCurrentAddr);
						immediateSPOffset = -((4 + frameSize) * datawidth);
						readpm.addImm(SignExtend8BitNumberTo12Bits(immediateSPOffset));
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
					}

					{
						MachineInstrBuilder luiOfTargetCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForTargetAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfTargetCE.addReg(registerForTargetAddr, RegState::Define);
						luiOfTargetCE.addImm(i + 1);
						LIS->getSlotIndexes()->insertMachineInstrInMaps(luiOfTargetCE.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(luiOfTargetCE.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));

						if (firstInstructionOfpHyperOpInRegion[i + increment] == 0) {
							firstInstructionOfpHyperOpInRegion[i + increment] = luiOfTargetCE.operator llvm::MachineInstr *();
						}
						//Add writepm to second CE
						MachineInstrBuilder writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::WRITEPM));
						writepm.addReg(registerForTargetAddr);
						//Dummy data
						writepm.addReg(registerForTargetAddr);
						unsigned immediateSPOffset = -((4 + frameSize) * datawidth);
						writepm.addImm(SignExtend8BitNumberTo12Bits(immediateSPOffset));
						allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());

						MachineInstrBuilder luiOfCurrentCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForCurrentAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfCurrentCE.addReg(registerForCurrentAddr, RegState::Define);
						luiOfCurrentCE.addImm(i + increment + 1);
						LIS->getSlotIndexes()->insertMachineInstrInMaps(luiOfCurrentCE.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(luiOfCurrentCE.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));

						//Add readpm to first CE
						MachineInstrBuilder readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::READPM));
						unsigned readpmTarget = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						readpm.addReg(readpmTarget, RegState::Define);
						LIS->getOrCreateInterval(readpmTarget);
						//Dummy data
						readpm.addReg(registerForCurrentAddr);
						immediateSPOffset = -((3 + frameSize) * datawidth);
						readpm.addImm(SignExtend8BitNumberTo12Bits(immediateSPOffset));
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
					}
				}
			}
			level = level * 2;
		}
		for (unsigned i = 0; i < ceCount; i++) {
			faninOfHyperOp[i] = 0;
		}
	}

//If the instruction depends on other instructions
	if (!SU->Preds.empty()) {
		for (SmallVector<SDep, 4>::iterator predecessorItr = SU->Preds.begin(); predecessorItr != SU->Preds.end(); predecessorItr++) {
			SDep dependence = (*predecessorItr);
			SUnit* predecessor = predecessorItr->getSUnit();
			unsigned ceContainingPredecessorInstruction = -1;
			for (list<pair<SUnit*, unsigned> >::iterator predecessorInstrItr = instructionAndPHyperOpMapForRegion.begin(); predecessorInstrItr != instructionAndPHyperOpMapForRegion.end(); predecessorInstrItr++) {
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
					loadFromMemory.addReg(REDEFINE::zero);
					loadFromMemory.addFrameIndex(frameLocationToReadFrom);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(loadFromMemory.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(loadFromMemory.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, insertPosition++)));
				}
			} else if (ceContainingPredecessorInstruction != ceContainingInstruction) {
				//If the register is already read from in the current ce via a readpm instruction, there is no need to read it all over again
				if (dependence.isAssignedRegDep() && redefinitionsInCE.find(ceContainingInstruction) != redefinitionsInCE.end()
						&& find(redefinitionsInCE[ceContainingInstruction].begin(), redefinitionsInCE[ceContainingInstruction].end(), dependence.getReg()) != redefinitionsInCE[ceContainingInstruction].end()) {
					continue;
				}

				unsigned offsetInScratchpad = faninOfHyperOp[ceContainingInstruction];
				unsigned shiftAmountForBaseAddress = (unsigned) ceil(log2(ceContainingInstruction * SPLOCATIONS));
				//Load the base scratchpad address to a register in the producer CE for the first time
				if (registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction] == -1) {
					MachineInstrBuilder sourceLui = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
					unsigned sourceSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
					sourceLui.addImm(ceContainingInstruction);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(sourceLui.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, insertPosition++)));
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
					allInstructionsOfRegion.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, insertPosition++)));
					registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction] = (int) targetSpAddressRegister;

					if (firstInstructionOfpHyperOpInRegion[ceContainingInstruction] == 0) {
						firstInstructionOfpHyperOpInRegion[ceContainingInstruction] = targetLui;
					}

				}

				//TODO If number of registers spills into larger than 12 bit size because immediate value supported by readpm and writepm is a signed 11 bit field
				if (ceil(log2(offsetInScratchpad)) > 11) {
					//Add addi instruction incrementing the base address field in source
					MachineInstrBuilder sourceAddi = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::ADDI));
					sourceAddi.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction]);
					sourceAddi.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction]);
					sourceAddi.addImm(offsetInScratchpad - datawidth);
					allInstructionsOfRegion.push_back(make_pair(sourceAddi.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, insertPosition++)));

					//Add addi instruction incrementing the base address field in target
					MachineInstrBuilder targetAddi = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::ADDI));
					targetAddi.addReg(registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction]);
					targetAddi.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction]);
					targetAddi.addImm(offsetInScratchpad);
					allInstructionsOfRegion.push_back(make_pair(targetAddi.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, insertPosition++)));

					offsetInScratchpad = faninOfHyperOp[ceContainingInstruction] = datawidth;
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
					} else if (*regClass->vt_begin() == MVT::f32) {
						writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::FWRITEPM));
						readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::FREADPM));
					}
				} else {
					writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::WRITEPM));
					readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::READPM));
				}
				//The previous and next if-else block is not merged because of the following writepm.addReg statement
				writepm.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction]);

				if (dependence.isAssignedRegDep()) {
					//Use register operand in the inserted writepm
					writepm.addReg(dependence.getReg());
					readpm.addReg(dependence.getReg(), RegState::Define);
					if (registerAndDefiningCEMap.find(dependence.getReg()) == registerAndDefiningCEMap.end()) {
						registerAndDefiningCEMap[dependence.getReg()] = ceContainingPredecessorInstruction;
					}
					//The register is redefined in the current CE and doesn't need to be read again, caching for this purpose
					redefinitionsInCE[ceContainingInstruction].push_back(dependence.getReg());
				} else {
					//dependence still exists, associate a dummy virtual register
					writepm.addReg(REDEFINE::zero);
					readpm.addReg(REDEFINE::zero);
				}

				writepm.addImm(offsetInScratchpad);
				readpm.addReg(registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction]);
				readpm.addImm(offsetInScratchpad);

				allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, insertPosition++)));
				allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, insertPosition++)));
				//Increment by 4 since scratchpad is byte addressable
				faninOfHyperOp[ceContainingInstruction] = faninOfHyperOp[ceContainingInstruction] + datawidth;
				LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());
				LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
			}
		}
	}
	if (firstInstructionOfpHyperOpInRegion[ceContainingInstruction] == 0) {
		firstInstructionOfpHyperOpInRegion[ceContainingInstruction] = machineInstruction;
	}
	allInstructionsOfRegion.push_back(make_pair(machineInstruction, make_pair(ceContainingInstruction, insertPosition++)));
}

//If the instruction is a branch operation
if (RegionEnd != BB->end() && RegionEnd->isBranch()) {
	MachineInstr* machineInstruction = RegionEnd;
	MachineInstr* firstInsertedInstruction = 0;

//Check if the branch edge is a backedge
//Add barrier reduction tree for synchronization
	unsigned level = 2;
	while (level <= ceCount + 1) {
		unsigned increment = level / 2;
		for (unsigned i = 0; i < ceCount; i += increment) {
			if (i % 2 == 0 && (i + increment) < ceCount) {
				{
					MachineInstrBuilder luiOfTargetCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
					unsigned registerForTargetAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					luiOfTargetCE.addReg(registerForTargetAddr, RegState::Define);
					luiOfTargetCE.addImm(i + increment + 1);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(luiOfTargetCE.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(luiOfTargetCE.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

					if (firstInstructionOfpHyperOpInRegion[i] == 0) {
						firstInstructionOfpHyperOpInRegion[i] = luiOfTargetCE.operator llvm::MachineInstr *();
					}
					//Add writepm to first CE
					MachineInstrBuilder writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::WRITEPM));
					writepm.addReg(registerForTargetAddr);
					//Dummy data
					writepm.addReg(registerForTargetAddr);
					int8_t immediateSPOffset = -((3 + frameSize) * datawidth);
					writepm.addImm(SignExtend8BitNumberTo12Bits(immediateSPOffset));
					allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());

					MachineInstrBuilder luiOfCurrentCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
					unsigned registerForCurrentAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					luiOfCurrentCE.addReg(registerForCurrentAddr, RegState::Define);
					luiOfCurrentCE.addImm(i + 1);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(luiOfCurrentCE.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(luiOfCurrentCE.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

					//Add readpm to first CE
					MachineInstrBuilder readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::READPM));
					unsigned readpmTarget = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					readpm.addReg(readpmTarget, RegState::Define);
					LIS->getOrCreateInterval(readpmTarget);
					//Dummy data
					readpm.addReg(registerForCurrentAddr);
					immediateSPOffset = -((4 + frameSize) * datawidth);
					readpm.addImm(SignExtend8BitNumberTo12Bits(immediateSPOffset));
					allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
				}

				{
					MachineInstrBuilder luiOfTargetCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
					unsigned registerForTargetAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					luiOfTargetCE.addReg(registerForTargetAddr, RegState::Define);
					luiOfTargetCE.addImm(i + 1);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(luiOfTargetCE.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(luiOfTargetCE.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));

					if (firstInstructionOfpHyperOpInRegion[i + increment] == 0) {
						firstInstructionOfpHyperOpInRegion[i + increment] = luiOfTargetCE.operator llvm::MachineInstr *();
					}
					//Add writepm to second CE
					MachineInstrBuilder writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::WRITEPM));
					writepm.addReg(registerForTargetAddr);
					//Dummy data
					writepm.addReg(registerForTargetAddr);
					unsigned immediateSPOffset = -((4 + frameSize) * datawidth);
					writepm.addImm(SignExtend8BitNumberTo12Bits(immediateSPOffset));
					allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());

					MachineInstrBuilder luiOfCurrentCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
					unsigned registerForCurrentAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					luiOfCurrentCE.addReg(registerForCurrentAddr, RegState::Define);
					luiOfCurrentCE.addImm(i + increment + 1);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(luiOfCurrentCE.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(luiOfCurrentCE.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));

					//Add readpm to first CE
					MachineInstrBuilder readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::READPM));
					unsigned readpmTarget = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					readpm.addReg(readpmTarget, RegState::Define);
					LIS->getOrCreateInterval(readpmTarget);
					//Dummy data
					readpm.addReg(registerForCurrentAddr);
					immediateSPOffset = -((3 + frameSize) * datawidth);
					readpm.addImm(SignExtend8BitNumberTo12Bits(immediateSPOffset));
					allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
				}
			}
		}
		level = level * 2;
	}
	for (unsigned i = 0; i < ceCount; i++) {
		faninOfHyperOp[i] = 0;
	}

//See if some predecessors exist which require additional inter-pHyperOp communication instructions
	for (unsigned i = 0; i < machineInstruction->getNumOperands(); i++) {
		MachineOperand& operand = machineInstruction->getOperand(i);
		//Had to put a hardcoded check for zero here
		if (operand.isReg() && operand.getReg() != REDEFINE::zero) {
			SlotIndex regSlotIndex = LIS->getInstructionIndex(machineInstruction);
			VNInfo * reachingDefinition = LIS->getInterval(operand.getReg()).getVNInfoBefore(regSlotIndex);
			MachineInstr* reachingDefinitionInstruction = LIS->getInstructionFromIndex(reachingDefinition->def);
			int ceContainingPredecessorInstruction = -1;
			for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator allInstructionItr = allInstructionsOfRegion.begin(); allInstructionItr != allInstructionsOfRegion.end(); allInstructionItr++) {
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
						MachineInstrBuilder loadFromMemory = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::ADDI));
						loadFromMemory.addReg(operand.getReg(), RegState::Define);
						loadFromMemory.addReg(REDEFINE::zero);
						loadFromMemory.addFrameIndex(frameLocationToReadFrom);
						LIS->getSlotIndexes()->insertMachineInstrInMaps(loadFromMemory.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(loadFromMemory.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, insertPosition++)));
					}
				}
			} else {
				for (unsigned ceContainingInstruction = 0; ceContainingInstruction < ceCount; ceContainingInstruction++) {
					if (ceContainingInstruction != ceContainingPredecessorInstruction) {
						if (redefinitionsInCE.find(ceContainingInstruction) != redefinitionsInCE.end() && find(redefinitionsInCE[ceContainingInstruction].begin(), redefinitionsInCE[ceContainingInstruction].end(), operand.getReg()) != redefinitionsInCE[ceContainingInstruction].end()) {
							continue;
						}
						//Hardcoded first location to be used for communicating the operand of branch instruction because there is no use in doing any fancy register allocation in this
						//Since sync barrier has already been executed, sp locations can be reused from 0
						//TODO while technically we can run out of communication registers here as well, it is highly unlikely we ever will since the number of branch instructions cant be so high
						unsigned offsetInScratchpad = faninOfHyperOp[ceContainingInstruction];
						//Add instruction to communicate the operand to the pHyperOp
						if (registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction] == -1) {
							MachineInstrBuilder sourceLui = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
							unsigned sourceSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
							sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
							//TODO May need changing later
							sourceLui.addImm(ceContainingInstruction);

							LIS->getSlotIndexes()->insertMachineInstrInMaps(sourceLui.operator llvm::MachineInstr *());
							allInstructionsOfRegion.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, insertPosition++)));
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
							allInstructionsOfRegion.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, insertPosition++)));
							registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction] = (int) targetSpAddressRegister;

							if (firstInstructionOfpHyperOpInRegion[ceContainingInstruction] == 0) {
								firstInstructionOfpHyperOpInRegion[ceContainingInstruction] = targetLui;
							}
						}

						//TODO If number of registers spills into larger than 12 bit size because immediate value supported by readpm and writepm is a signed 11 bit field
						if (ceil(log2(offsetInScratchpad)) > 11) {
							//Add addi instruction incrementing the base address field in source
							MachineInstrBuilder sourceAddi = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::ADDI));
							sourceAddi.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction]);
							sourceAddi.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction]);
							sourceAddi.addImm(offsetInScratchpad - 4);
							allInstructionsOfRegion.push_back(make_pair(sourceAddi.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, insertPosition++)));

							//Add addi instruction incrementing the base address field in target
							MachineInstrBuilder targetAddi = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::ADDI));
							targetAddi.addReg(registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction]);
							targetAddi.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction]);
							targetAddi.addImm(offsetInScratchpad);
							allInstructionsOfRegion.push_back(make_pair(targetAddi.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, insertPosition++)));

							offsetInScratchpad = faninOfHyperOp[ceContainingInstruction] = 4;
						}
						//Add writepm in predCE
						MachineInstrBuilder writepm;
						MachineInstrBuilder readpm;
						const TargetRegisterClass* regClass = ((REDEFINETargetMachine&) TM).FuncInfo->RegInfo->getRegClass(operand.getReg());
						if (*regClass->vt_begin() == MVT::i32) {
							writepm = BuildMI(parentBasicBlock, RegionEnd, location, TII->get(REDEFINE::WRITEPM));
							readpm = BuildMI(parentBasicBlock, RegionEnd, location, TII->get(REDEFINE::READPM));
						} else if (*regClass->vt_begin() == MVT::f32) {
							writepm = BuildMI(parentBasicBlock, RegionEnd, location, TII->get(REDEFINE::FWRITEPM));
							readpm = BuildMI(parentBasicBlock, RegionEnd, location, TII->get(REDEFINE::FREADPM));
						}

						writepm.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction]);
						//Use register operand in the inserted writepm
						writepm.addReg(operand.getReg());
						writepm.addImm(offsetInScratchpad);

						//Add readpm instruction in the target CE
						readpm.addReg(operand.getReg(), RegState::Define);
						redefinitionsInCE[ceContainingInstruction].push_back(operand.getReg());
						if (registerAndDefiningCEMap.find(operand.getReg()) == registerAndDefiningCEMap.end()) {
							registerAndDefiningCEMap[operand.getReg()] = ceContainingPredecessorInstruction;
						}
						readpm.addReg(registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction]);
						readpm.addImm(offsetInScratchpad);

						allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, insertPosition++)));
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, insertPosition++)));
						//Increment by data width since scratchpad is byte addressable
						faninOfHyperOp[ceContainingInstruction] = faninOfHyperOp[ceContainingInstruction] + datawidth;
						LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());
						LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
					}
				}
			}

		}
	}

//Add the branch instruction to the first pHyperOp
	allInstructionsOfRegion.push_back(make_pair(machineInstruction, make_pair(0, insertPosition++)));

//Replicate the branch instruction in other CEs
	MachineInstr* successorOfTerminator;
	if (RegionEnd->getNextNode() != parentBasicBlock.end()) {
		successorOfTerminator = RegionEnd->getNextNode();
	} else {
		successorOfTerminator = parentBasicBlock.end();
	}
	for (unsigned i = 1; i < ceCount; i++) {
		MachineInstrBuilder duplicateTerminatorInstr = BuildMI(parentBasicBlock, successorOfTerminator, location, TII->get(machineInstruction->getOpcode()));
		for (unsigned operandIndex = 0; operandIndex < RegionEnd->getNumOperands(); operandIndex++) {
			duplicateTerminatorInstr.addOperand(RegionEnd->getOperand(operandIndex));
		}
		allInstructionsOfRegion.push_back(make_pair(duplicateTerminatorInstr.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(duplicateTerminatorInstr.operator llvm::MachineInstr *());
	}

//After every region terminator, add 2 nops
	for (unsigned i = 0; i < ceCount; i++) {
		for (unsigned j = 0; j < 2; j++) {
			MachineInstrBuilder nopInstruction = BuildMI(parentBasicBlock, successorOfTerminator, location, TII->get(REDEFINE::ADDI));
			nopInstruction.addReg(REDEFINE::zero);
			nopInstruction.addReg(REDEFINE::zero);
			nopInstruction.addImm(0);
			LIS->getSlotIndexes()->insertMachineInstrInMaps(nopInstruction.operator llvm::MachineInstr *());
			allInstructionsOfRegion.push_back(make_pair(nopInstruction.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		}
	}
}

//Shuffle instructions in the region
DEBUG(dbgs() << "Shuffling instructions within the region such that successive instructions belong to the same pHyperOp to ease bundle creation\n");

unsigned nextInstructionPositionForCE[ceCount - 1];
MachineInstr* nextInstructionForCE[ceCount - 1];
for (unsigned ceIndex = 1; ceIndex < ceCount; ceIndex++) {
	MachineInstr* nextCeInstruction = firstInstructionOfpHyperOpInRegion[ceIndex];
	nextInstructionForCE[ceIndex - 1] = nextCeInstruction;
	if (nextCeInstruction != BB->end()) {
		unsigned nextCeInstructionPosition;
		for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator secondItr = allInstructionsOfRegion.begin(); secondItr != allInstructionsOfRegion.end(); secondItr++) {
			if (secondItr->first == nextCeInstruction) {
				nextCeInstructionPosition = secondItr->second.second;
				break;
			}
		}
		nextInstructionPositionForCE[ceIndex - 1] = nextCeInstructionPosition;
	}
}
for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator allInstructionItr = allInstructionsOfRegion.begin(); allInstructionItr != allInstructionsOfRegion.end(); allInstructionItr++) {
	MachineInstr* instruction = allInstructionItr->first;
	unsigned ce = allInstructionItr->second.first;
	unsigned position = allInstructionItr->second.second;
	instruction->setFlag(MachineInstr::NoFlags);

	if (ce < ceCount - 1) {
		MachineInstr* nextCeInstruction;
		unsigned nextCeInstructionPosition;
		unsigned index = ce;
		while (index < ceCount - 1) {
			nextCeInstruction = nextInstructionForCE[index];
			nextCeInstructionPosition = nextInstructionPositionForCE[index];
			index++;
			if (nextCeInstruction == BB->end() || nextCeInstruction == 0) {
				continue;
			}
			if (nextCeInstructionPosition < position) {
				break;
			}
		}

		if (nextCeInstruction == BB->end() || nextCeInstruction == 0) {
			continue;
		}
		if (nextCeInstructionPosition < position) {
			BB->splice(nextCeInstruction, BB, instruction);
		}

	}
}

DEBUG(dbgs() << "Renaming registers used across CEs\n");
for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator allInstructionItr = allInstructionsOfRegion.begin(); allInstructionItr != allInstructionsOfRegion.end(); allInstructionItr++) {
	MachineInstr* instruction = allInstructionItr->first;
	unsigned ce = allInstructionItr->second.first;
//Check if the instruction uses a register defined elsewhere and rename it
	for (unsigned i = 0; i < instruction->getNumOperands(); i++) {
		MachineOperand& operand = instruction->getOperand(i);
		if (operand.isReg() && registerAndDefiningCEMap.find(operand.getReg()) != registerAndDefiningCEMap.end() && registerAndDefiningCEMap[operand.getReg()] != ce) {
			//Defined in a different CE
			if (replacementRegisterMap[ce].find(operand.getReg()) == replacementRegisterMap[ce].end()) {
				unsigned originalRegister = operand.getReg();
				if ((*MRI.getRegClass(originalRegister)->vt_begin()) == MVT::i32) {
					//First definition in ce
					replacementRegisterMap[ce][operand.getReg()] = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				} else {
					replacementRegisterMap[ce][operand.getReg()] = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::f32);
				}
			}
			unsigned replacementRegister = replacementRegisterMap[ce].find(operand.getReg())->second;
			operand.setReg(replacementRegister);
		}
	}
}

vector<MachineInstr*> firstInstrRegionCopy;
for (unsigned i = 0; i < ceCount; i++) {
	firstInstrRegionCopy.push_back(firstInstructionOfpHyperOpInRegion[i]);
}

MachineInstr* firstInstruction = 0;
//Find the first instruction of the region
for (unsigned i = 0; i < ceCount; i++) {
	firstInstruction = firstInstructionOfpHyperOpInRegion[i];
	if (firstInstruction != 0) {
		break;
	}
}

firstInstructionOfpHyperOp.push_front(firstInstrRegionCopy);
if (firstInstruction != 0) {
	RegionBegin = firstInstruction;
}
}

void REDEFINEMCInstrScheduler::finishBlock() {
//If the basic block is the terminator
if (BB->getName().compare(MF.back().getName()) == 0) {
	Module * parentModule = const_cast<Module*>(BB->getParent()->getFunction()->getParent());
	DebugLoc location = BB->begin()->getDebugLoc();
//Add writecm instructions and fbind
//	fbind is in case of context frames being reused statically and hence, is added optionally; returns are assumed to be merged during HyperOp creation
	HyperOpInteractionGraph * graph = ((REDEFINETargetMachine&) TM).HIG;
	Function* Fn = const_cast<Function*>(MF.getFunction());
	HyperOp* hyperOp = ((REDEFINETargetMachine&) TM).HIG->getHyperOp(Fn);

//TODO: I am assuming here that there is only one exit block since we merge return
	MachineBasicBlock& lastBB = MF.back();
	MachineInstr* lastInstruction = lastBB.end();

	firstInstructionOfpHyperOpInRegion.clear();
	for (unsigned i = 0; i < ceCount; i++) {
		firstInstructionOfpHyperOpInRegion.push_back(0);
	}

	allInstructionsOfRegion.clear();
	insertPosition = 0;

//Add Fbind
	DEBUG(dbgs() << "Adding fbind instructions\n");

	map<HyperOp*, unsigned> registerContainingHyperOpFrameAddress;
	unsigned currentCE = 0;
	for (list<HyperOp*>::iterator childHyperOpItr = graph->Vertices.begin(); childHyperOpItr != graph->Vertices.end(); childHyperOpItr++) {
		//Among the HyperOps immediately dominated by the hyperOp, add fbind for those HyperOps that require it
		if (*childHyperOpItr != hyperOp && (*childHyperOpItr)->isFbindRequired() && (*childHyperOpItr)->getImmediateDominator() == hyperOp) {
			int hyperOpId = (*childHyperOpItr)->getHyperOpId();
			int hyperOpFrame = (*childHyperOpItr)->getContextFrame();
			unsigned registerContainingConsumerFrameAddr;
			if (registerContainingHyperOpFrameAddress.find(*childHyperOpItr) == registerContainingHyperOpFrameAddress.end()) {
				registerContainingConsumerFrameAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder addi = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADDI));
				addi.addReg(registerContainingConsumerFrameAddr, RegState::Define);
				addi.addReg(REDEFINE::zero);
				addi.addImm(hyperOpFrame);
				allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
				registerContainingHyperOpFrameAddress.insert(make_pair(*childHyperOpItr, registerContainingConsumerFrameAddr));
			} else {
				registerContainingConsumerFrameAddr = registerContainingHyperOpFrameAddress.find(*childHyperOpItr)->second;
			}
			//Fbind instruction added to the immediate dominator of the HyperOp
			MachineInstrBuilder fbind = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::FBIND));
			fbind.addReg(REDEFINE::zero);
			fbind.addReg(registerContainingConsumerFrameAddr);
			string hyperOpIDString = HYPEROP_ID_PREFIX;
			hyperOpIDString.append(itostr(hyperOpId));
			MCSymbol* symbol = fbind.operator ->()->getParent()->getParent()->getContext().GetOrCreateSymbol(StringRef(hyperOpIDString));
			fbind.addSym(symbol);
			if (firstInstructionOfpHyperOpInRegion[currentCE] == 0) {
				firstInstructionOfpHyperOpInRegion[currentCE] = fbind.operator llvm::MachineInstr *();
			}
			allInstructionsOfRegion.push_back(make_pair(fbind.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(fbind.operator llvm::MachineInstr *());
			currentCE = (currentCE + 1) % ceCount;
		}
	}
//End of add fbind

//LLVM IR is assumed to be in mem form; Loading from the memory location with the same name as the value should do for all the data being communicated between HyperOps
	vector<list<pair<HyperOp*, unsigned> > > consumerHyperOps;
	consumerHyperOps.reserve(ceCount);
	for (unsigned j = 0; j < ceCount; j++) {
		list<pair<HyperOp*, unsigned> > consumerList;
		consumerHyperOps.push_back(consumerList);
	}

	DEBUG(dbgs() << "Adding writecm instructions\n");
//Cyclically distribute the writecm stubs among pHyperOps
	for (map<HyperOpEdge*, HyperOp*>::iterator childItr = hyperOp->ChildMap.begin(); childItr != hyperOp->ChildMap.end(); childItr++) {
		HyperOpEdge* edge = childItr->first;
		HyperOp* consumer = childItr->second;

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
			MachineInstrBuilder addi = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADDI));
			addi.addReg(registerContainingConsumerBase, RegState::Define);
			addi.addReg(REDEFINE::zero);
			addi.addImm(consumer->getContextFrame() * frameSize);
			consumerHyperOps[currentCE].push_back(make_pair(consumer, registerContainingConsumerBase));
			if (firstInstructionOfpHyperOpInRegion[currentCE] == 0) {
				firstInstructionOfpHyperOpInRegion[currentCE] = addi.operator llvm::MachineInstr *();
			}
			allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
			//Add instruction to the region
			allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
		}

		if (edge->getType() == HyperOpEdge::SCALAR || edge->getType() == HyperOpEdge::PREDICATE) {
			//position is multiplied by 4 since the context memory is byte addressable
			unsigned contextFrameOffset = edge->getPositionOfContextSlot() * 4;
			if (edge->getPositionOfContextSlot() == -1) {
				//predicate can take any offset wrt context frame base, it does not have a dedicated slot
				contextFrameOffset = 0;
			}

			unsigned objectIndex = -1;
			//Get the index of the stack allocated object, starting from 0 because negative offsets from fp contain function arguments
			for (int i = 0; i < MF.getFrameInfo()->getObjectIndexEnd(); i++) {
				const AllocaInst* allocInstr = MF.getFrameInfo()->getObjectAllocation(i);
				if (edge->getValue() == allocInstr) {
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
				MachineInstrBuilder loadInstr = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::LW));
				loadInstr.addReg(registerContainingData, RegState::Define);
				loadInstr.addReg(REDEFINE::zero);
				loadInstr.addFrameIndex(objectIndex);
				if (firstInstructionOfpHyperOpInRegion[currentCE] == 0) {
					firstInstructionOfpHyperOpInRegion[currentCE] = loadInstr.operator llvm::MachineInstr *();
				}
				allInstructionsOfRegion.push_back(make_pair(loadInstr.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
				//Add instruction to bundle
				allInstructionsOfRegion.push_back(make_pair(loadInstr.operator ->(), make_pair(currentCE, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(loadInstr.operator llvm::MachineInstr *());
			}
			//No need to access memory for a true predicate, copying via addi will do
			else {
				MachineInstrBuilder addi = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADDI));
				addi.addReg(registerContainingData, RegState::Define);
				addi.addReg(REDEFINE::zero);
				addi.addImm(1);
				if (firstInstructionOfpHyperOpInRegion[currentCE] == 0) {
					firstInstructionOfpHyperOpInRegion[currentCE] = addi.operator llvm::MachineInstr *();
				}
				allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
				//Add instruction to bundle
				allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
			}
			MachineInstrBuilder writeToContextFrame;
			if (edge->Type == HyperOpEdge::SCALAR) {
				writeToContextFrame = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::WRITECM));
			} else if (edge->Type == HyperOpEdge::PREDICATE) {
				//TODO Forced serialization edges need not be added if there is any other edge between the producer and consumer HyperOps
				writeToContextFrame = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::WRITECMP));
			}

			writeToContextFrame.addReg(registerContainingConsumerBase);
			writeToContextFrame.addReg(registerContainingData);
			writeToContextFrame.addImm(contextFrameOffset);

			allInstructionsOfRegion.push_back(make_pair(writeToContextFrame.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
			//Add instruction to bundle
			LIS->getSlotIndexes()->insertMachineInstrInMaps(writeToContextFrame.operator llvm::MachineInstr *());
			currentCE = (currentCE + 1) % ceCount;

			//Add the writecm instructions to the function map so that they can be patched for the right context frame location later
			Function* consumerFunction = consumer->getFunction();
			list<MachineInstr*> writeInstructionsToConsumer;
			if (writeInstrToContextFrame.find(consumerFunction) != writeInstrToContextFrame.end()) {
				writeInstructionsToConsumer = writeInstrToContextFrame.find(consumerFunction)->second;
				writeInstrToContextFrame.erase(writeInstrToContextFrame.find(consumerFunction));
			}

			writeInstructionsToConsumer.push_back(writeToContextFrame.operator ->());
			writeInstrToContextFrame.insert(make_pair(consumerFunction, writeInstructionsToConsumer));
		}

		//if local reference, add writes to the local memory of consumer HyperOp and remove the consumer HyperOp's argument
		else if (edge->getType() == HyperOpEdge::LOCAL_REFERENCE) {
			const AllocaInst* allocInstr;
			int frameLocationOfSourceData = 0;
			//Get the index of the stack allocated object, starting from 0 because negative offsets from fp contain function arguments
			for (unsigned int i = 0; i < MF.getFrameInfo()->getObjectIndexEnd(); i++) {
				if (edge->getValue() == MF.getFrameInfo()->getObjectAllocation(i)) {
					allocInstr = MF.getFrameInfo()->getObjectAllocation(i);
					break;
				}
				frameLocationOfSourceData += MF.getFrameInfo()->getObjectSize(i);
			}

			MachineFunction *consumerMF = ((REDEFINETargetMachine&) TM).functionMap[consumer->getFunction()];
			int frameLocationOfTargetData = 0;
			//Compute frame objects' size
			int funcArgIndex = consumerMF->getFrameInfo()->getObjectIndexBegin();
			int beginArgIndex = 0;
			for (Function::arg_iterator funcArgItr = Fn->arg_begin(); funcArgItr != Fn->arg_end(); funcArgItr++, beginArgIndex++) {
				if (beginArgIndex == edge->getPositionOfContextSlot()) {
					break;
				}
				Argument* argument = &*funcArgItr;
				if (!Fn->getAttributes().hasAttribute(beginArgIndex, Attribute::InReg)) {
					frameLocationOfTargetData += consumerMF->getFrameInfo()->getObjectSize(funcArgIndex);
					funcArgIndex++;
				}
			}

			Type* allocatedDataType = allocInstr->getAllocatedType();
			//Find the primitive types of allocatedDataType

			//Map of primitive data types and their memory locations
			list<pair<Type*, unsigned> > primitiveTypesMap;
			list<Type*> containedTypesForTraversal;
			containedTypesForTraversal.push_front(allocatedDataType);
			unsigned memoryOfType = 0;
			while (!containedTypesForTraversal.empty()) {
				Type* traversingType = containedTypesForTraversal.front();
				containedTypesForTraversal.pop_front();
				if (traversingType->isPrimitiveType()) {
					primitiveTypesMap.push_back(make_pair(traversingType, memoryOfType));
					memoryOfType += traversingType->getPrimitiveSizeInBits() / 8;
				} else {
					for (unsigned i = traversingType->getNumContainedTypes() - 1; i >= 0; i--) {
						containedTypesForTraversal.push_front(traversingType->getContainedType(i));
					}
				}
			}

			for (unsigned allocatedDataIndex = 0; allocatedDataIndex != ((ConstantInt*) allocInstr->getArraySize())->getZExtValue(); allocatedDataIndex++) {
				//Add a load instruction from memory and store to the memory frame of the consumer HyperOp
				for (list<pair<Type*, unsigned> >::iterator containedPrimitiveItr = primitiveTypesMap.begin(); containedPrimitiveItr != primitiveTypesMap.end(); containedPrimitiveItr++) {
					Type* containedType = containedPrimitiveItr->first;
					unsigned registerContainingData = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder addi = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(registerContainingData, RegState::Define).addReg(REDEFINE::t5);
					allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());

					if (containedType->isFloatTy()) {
						unsigned floatingPointRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::f32);
						MachineInstrBuilder floatingPointConversion = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::FMV_X_S)).addReg(floatingPointRegister, RegState::Define).addReg(registerContainingData);
						allInstructionsOfRegion.push_back(make_pair(floatingPointConversion.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(floatingPointConversion.operator llvm::MachineInstr *());

						MachineInstrBuilder store = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::FSW));
						store.addReg(registerContainingConsumerBase, RegState::Define).addReg(floatingPointRegister).addImm(allocatedDataIndex * memoryOfType + containedPrimitiveItr->second + frameLocationOfTargetData);

						allInstructionsOfRegion.push_back(make_pair(store.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(store.operator llvm::MachineInstr *());
					} else {
						MachineInstrBuilder store = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::SW));
						store.addReg(registerContainingConsumerBase, RegState::Define).addReg(registerContainingData).addImm(allocatedDataIndex * memoryOfType + containedPrimitiveItr->second + frameLocationOfTargetData);

						allInstructionsOfRegion.push_back(make_pair(store.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(store.operator llvm::MachineInstr *());
					}

					currentCE = (currentCE + 1) % ceCount;
				}
			}

		}
	}
//Does the HyperOp require fdelete?
	if (hyperOp->frameNeedsGC()) {
		//Add fdelete instruction
		MachineInstrBuilder shiftRight = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::SRLI));
		shiftRight.addReg(REDEFINE::zero);
		shiftRight.addReg(REDEFINE::zero);
		//Shift right by 6 bits to get the address of the frame
		shiftRight.addImm(6);
		if (firstInstructionOfpHyperOpInRegion[0] == 0) {
			firstInstructionOfpHyperOpInRegion[0] = shiftRight.operator llvm::MachineInstr *();
		}

		MachineInstrBuilder fdelete = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::FDELETE));
		fdelete.addReg(REDEFINE::zero);
		fdelete.addImm(0);

		LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftRight.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(shiftRight.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(fdelete.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(fdelete.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
	}

//Shuffle writecm and fbind instructions one last time
	for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator allInstructionItr = allInstructionsOfRegion.begin(); allInstructionItr != allInstructionsOfRegion.end(); allInstructionItr++) {
		MachineInstr* instruction = allInstructionItr->first;
		unsigned ce = allInstructionItr->second.first;
		unsigned position = allInstructionItr->second.second;
		instruction->setFlag(MachineInstr::NoFlags);
		if (ce < ceCount - 1) {
			MachineInstr* nextCeInstruction = firstInstructionOfpHyperOpInRegion[ce + 1];
			if (nextCeInstruction == 0) {
				nextCeInstruction = BB->end();
			}

			for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator secondItr = allInstructionsOfRegion.begin(); secondItr != allInstructionsOfRegion.end(); secondItr++) {
				if (secondItr->first == nextCeInstruction) {
					unsigned nextCeInstructionPosition = secondItr->second.second;
					if (nextCeInstructionPosition < position) {
						BB->splice(nextCeInstruction, BB, instruction);
						break;
					}
				}
			}
		}
	}

//	//Atleast one writecm/fbind should be added
	if (firstInstructionOfpHyperOpInRegion[0] != 0) {
		firstInstructionOfpHyperOp.push_back(firstInstructionOfpHyperOpInRegion);
	}

	DEBUG(dbgs() << "Adding endHyperOp instructions to each pHyperOp\n");
//End HyperOp and the two NOPs that follow are kinda like a new region that gets shuffled next
	vector<MachineInstr*> endHyperOpInstructionRegion;
//Add endHyperOp instruction and 2 nops in each pHyperOp
	for (unsigned i = 0; i < ceCount; i++) {
		MachineInstrBuilder endInstruction = BuildMI(*BB, BB->end(), BB->begin()->getDebugLoc(), TII->get(REDEFINE::END));
		endInstruction.addImm(0);
		endHyperOpInstructionRegion.push_back(endInstruction.operator ->());
		LIS->getSlotIndexes()->insertMachineInstrInMaps(endInstruction.operator llvm::MachineInstr *());
		for (unsigned j = 0; j < 2; j++) {
			MachineInstrBuilder nopInstruction = BuildMI(*BB, BB->end(), BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI));
			nopInstruction.addReg(REDEFINE::zero);
			nopInstruction.addReg(REDEFINE::zero);
			nopInstruction.addImm(0);
			LIS->getSlotIndexes()->insertMachineInstrInMaps(nopInstruction.operator llvm::MachineInstr *());
		}
	}

	firstInstructionOfpHyperOp.push_back(endHyperOpInstructionRegion);
}

//Shuffle instructions of region
vector<MachineInstr*> firstRegionBoundaries = firstInstructionOfpHyperOp.front();
if (firstInstructionOfpHyperOp.size() > 1) {
	DEBUG(dbgs() << "Merging regions of basic block BB#" << BB->getNumber() << "\n");
//Set the start of each region
	for (unsigned i = 0; i < ceCount; i++) {
		if (firstRegionBoundaries[i] == 0) {
			for (list<vector<MachineInstr*> >::iterator firstInstrItr = firstInstructionOfpHyperOp.begin(); firstInstrItr != firstInstructionOfpHyperOp.end(); firstInstrItr++) {
				vector<MachineInstr*> firstInstrOfNextRegion = *firstInstrItr;
				if (firstInstrOfNextRegion[i] != 0) {
					firstRegionBoundaries[i] = firstInstrOfNextRegion[i];
					break;
				}
			}
		}
	}

//Compute successive regions for merge after first regions
	for (unsigned j = 1; j < firstInstructionOfpHyperOp.size(); j++) {
		unsigned index = 0;
		vector<MachineInstr*> mergingRegionBoundaries;
		list<vector<MachineInstr*> >::iterator currentRegion;
		for (list<vector<MachineInstr*> >::iterator firstInstrOfPhop = firstInstructionOfpHyperOp.begin(); firstInstrOfPhop != firstInstructionOfpHyperOp.end(); firstInstrOfPhop++, index++) {
			if (index == j) {
				mergingRegionBoundaries = *firstInstrOfPhop;
				currentRegion = firstInstrOfPhop;
			}
		}

		//Merge pHyperOps from regions i and j
		for (unsigned ceIndex = 0; ceIndex < ceCount; ceIndex++) {
			MachineInstr* nextCeInstruction;
			MachineInstr* startMerge;
			MachineInstr* endMerge;
			startMerge = mergingRegionBoundaries[ceIndex];
			if (ceIndex + 1 != ceCount) {
				nextCeInstruction = firstRegionBoundaries[ceIndex + 1];
				endMerge = mergingRegionBoundaries[ceIndex + 1];
			} else {
				nextCeInstruction = BB->end();
				endMerge = BB->end();
			}
			if (nextCeInstruction == 0) {
				nextCeInstruction = BB->end();
			}
			if (endMerge == 0) {
				if (currentRegion == firstInstructionOfpHyperOp.end()) {
					endMerge = BB->end();
				} else {
					endMerge = (*(currentRegion++))[0];
				}
			}

			if (startMerge == BB->end() || nextCeInstruction == BB->end() || startMerge == 0 || nextCeInstruction == 0 || instructionAppearsBefore(BB, startMerge, nextCeInstruction) || startMerge == endMerge) {
				continue;
			}

			while (startMerge != endMerge) {
				MachineInstr* instructionToMerge = startMerge;
				startMerge = startMerge->getNextNode();
				BB->splice(nextCeInstruction, BB, instructionToMerge);
			}
		}
	}
}

DEBUG(dbgs() << "After Shuffling regions of basic block, state of BB#" << BB->getNumber() << ":\n");
BB->print(dbgs());

for (MachineBasicBlock::instr_iterator instItr = BB->instr_begin(); instItr != BB->instr_end(); instItr++) {
	for (unsigned i = 0; i < instItr->getNumOperands(); i++) {
		MachineOperand& operand = instItr->getOperand(i);
		if (operand.isReg() && find(registersUsedInBB.begin(), registersUsedInBB.end(), operand.getReg()) == registersUsedInBB.end()) {
			registersUsedInBB.push_back(operand.getReg());
		}
	}
}
LIS->repairIntervalsInRange(BB, BB->begin(), BB->end(), registersUsedInBB);

//Create instruction bundles corresponding to pHyperOps
if (!firstInstructionOfpHyperOp.empty()) {
	DEBUG(dbgs() << "Creating pHyperOp bundles for CEs\n");
	for (unsigned i = 0; i < ceCount; i++) {
		//TODO some changes required here if somehow an empty region is the first one to be processed
		MachineInstr* firstInstructionInCE = firstRegionBoundaries[i];
		MachineInstr* firstInstructionInNextCE = 0;

		if (firstInstructionInCE == BB->end() || firstInstructionInCE == 0) {
			continue;
		}

		int index = i + 1;
		while (index < ceCount) {
			firstInstructionInNextCE = firstRegionBoundaries[index];
			if (firstInstructionInNextCE != 0 && firstInstructionInNextCE != BB->end()) {
				break;
			}
			index++;
		}

		if (firstInstructionInNextCE == 0) {
			firstInstructionInNextCE = BB->end();
		}
		MIBundleBuilder* bundleBuilder = new MIBundleBuilder(*BB, firstInstructionInCE, firstInstructionInNextCE);
	}
}
DEBUG(dbgs() << "After bundling, state of BB" << BB->getNumber() << ":");
BB->print(dbgs());
BB = 0;
}

void REDEFINEMCInstrScheduler::finalizeSchedule() {
//Find the right order of context frame inputs for a machine function
unsigned contextFrameSlotAndPhysReg[frameSize];
//No bijective map available :(
map<unsigned, unsigned> physRegAndContextFrameSlot;
//Phys reg and the ce it is live-in
map<unsigned, unsigned> physRegAndLiveIn;

map<unsigned, MachineInstr*> physRegAndFirstMachineOperand;
map<unsigned, list<unsigned> > ceAndLiveInPhysicalRegMap;
list<unsigned> liveInPhysRegisters;

for (MachineRegisterInfo::livein_iterator liveInItr = MF.getRegInfo().livein_begin(); liveInItr != MF.getRegInfo().livein_end(); liveInItr++) {
	liveInPhysRegisters.push_back(liveInItr->first);
}

//Get the live-in registers and map to the ce to which they belong
for (MachineFunction::iterator MBBI = MF.begin(), MBBE = MF.end(); MBBI != MBBE; ++MBBI) {
	int pHyperOpIndex = -1;
	for (MachineBasicBlock::instr_iterator MII = MBBI->instr_begin(); MII != MBBI->instr_end(); ++MII) {
		MachineInstr *MI = MII;
		if (!MI->isInsideBundle()) {
			pHyperOpIndex++;
		}
		//We only need to worry about copy instructions
		if (MI->isCopy()) {
			//Get the copy instruction's source register
			MachineOperand &MO = MI->getOperand(1);
			//Just making sure that the register is live-in, although not necessary because no physical registers exist at this stage other than the ones that were passed inreg to the function
			if (MO.isReg() && find(liveInPhysRegisters.begin(), liveInPhysRegisters.end(), MO.getReg()) != liveInPhysRegisters.end()) {
				//Check if the physical register has already been marked as live-in
				int liveInForPhyperOp = -1;
				for (unsigned i = 0; i < ceCount; i++) {
					if (ceAndLiveInPhysicalRegMap.find(i) != ceAndLiveInPhysicalRegMap.end()) {
						list<unsigned> liveInRegs = ceAndLiveInPhysicalRegMap.find(i)->second;
						if (find(liveInRegs.begin(), liveInRegs.end(), MO.getReg()) != liveInRegs.end()) {
							liveInForPhyperOp = i;
							break;
						}
						if (liveInForPhyperOp != -1) {
							break;
						}
					}
				}

				if (liveInForPhyperOp == -1) {
					//Adding the register the first time as live-in
					list<unsigned> liveInRegs;
					if (ceAndLiveInPhysicalRegMap.find(pHyperOpIndex) == ceAndLiveInPhysicalRegMap.end()) {
						ceAndLiveInPhysicalRegMap.insert(make_pair(pHyperOpIndex, liveInRegs));
					}
					ceAndLiveInPhysicalRegMap.find(pHyperOpIndex)->second.push_back(MO.getReg());
					physRegAndFirstMachineOperand.insert(make_pair(MO.getReg(), MO.getParent()));
				}
			}
		}
	}
}

HyperOpInteractionGraph * graph = ((REDEFINETargetMachine&) TM).HIG;
HyperOp* currentHyperOp = graph->getHyperOp(const_cast<Function*>(MF.getFunction()));
currentHyperOp->setNumCEs(ceCount);

unsigned contextFrameLocation = 0;
for (unsigned ceIndex = 0; ceIndex < ceCount; ceIndex++) {
	if (ceAndLiveInPhysicalRegMap.find(ceIndex) != ceAndLiveInPhysicalRegMap.end()) {
		list<unsigned> liveInRegs = ceAndLiveInPhysicalRegMap.find(ceIndex)->second;
		currentHyperOp->setNumCEInputs(ceIndex, liveInRegs.size());
		if (!liveInRegs.empty()) {
			for (list<unsigned>::iterator physRegItr = liveInRegs.begin(); physRegItr != liveInRegs.end(); physRegItr++) {
				unsigned physReg = *physRegItr;
				contextFrameSlotAndPhysReg[contextFrameLocation] = physReg;
				physRegAndContextFrameSlot[physReg] = contextFrameLocation;
				physRegAndLiveIn[physReg] = ceIndex;
				contextFrameLocation++;
			}
		}
	}
}

DEBUG(dbgs() << "Patching writecm instructions to shuffle physical registers\n");
//Modify the writecm instructions corresponding to writes to the current MachineFunction's context frame
//TODO This assumes that the producer HyperOps have already been dealt with and all necessary writecm instructions have been added
if (writeInstrToContextFrame.find(const_cast<Function*>(MF.getFunction())) != writeInstrToContextFrame.end()) {
	list<MachineInstr*> writeInstrToBePatched = this->writeInstrToContextFrame.find(const_cast<Function*>(MF.getFunction()))->second;
	for (list<MachineInstr*>::iterator writeInstItr = writeInstrToBePatched.begin(); writeInstItr != writeInstrToBePatched.end(); writeInstItr++) {
		MachineInstr* writeInst = *writeInstItr;
		//Replace the immediate offset of write instruction that corresponds to the context frame slot being written into
		unsigned previousFrameSlot = writeInst->getOperand(2).getImm();
		writeInst->getOperand(2).setImm(physRegAndContextFrameSlot[contextFrameSlotAndPhysReg[previousFrameSlot]]);
	}
}
/*
 * Since REDEFINE doesn't allow replication of context memory inputs to multiple CEs, after the first CE that uses a live-in register corresponding to a HyperOp input is encountered,
 * if another CE requires the same input, it gets the input from scratch pad.
 */
//TODO Make necessary changes to check if the operand has been replicated atleast once
DEBUG(dbgs() << "Replicating HyperOp inputs\n");
for (MachineFunction::iterator MBBI = MF.begin(), MBBE = MF.end(); MBBI != MBBE; ++MBBI) {
	int pHyperOpIndex = -1;
	for (MachineBasicBlock::instr_iterator MII = MBBI->instr_begin(); MII != MBBI->instr_end(); ++MII) {
		MachineInstr *MI = MII;
		//First instruction in basic block
		if (!MI->isInsideBundle()) {
			//New pHyperOp
			pHyperOpIndex++;
		}
		if (MI->isCopy()) {
			MachineOperand &MO = MI->getOperand(1);
			if (MO.isReg() && find(liveInPhysRegisters.begin(), liveInPhysRegisters.end(), MO.getReg()) != liveInPhysRegisters.end()) {
				unsigned physicalReg = MO.getReg();
				if (physRegAndLiveIn[physicalReg] == -1 || physRegAndLiveIn[physicalReg] != pHyperOpIndex) {
					MachineInstr* firstDefinition = physRegAndFirstMachineOperand.find(physicalReg)->second;
					unsigned pHyperOpContainingDefinition = physRegAndLiveIn[physicalReg];
					MachineInstrBuilder sourceLui;
					if (registerContainingBaseAddress[pHyperOpContainingDefinition][pHyperOpIndex] == -1) {
						sourceLui = BuildMI(*(firstDefinition->getParent()), firstDefinition, firstDefinition->getDebugLoc(), TII->get(REDEFINE::LUI));
						unsigned sourceSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
						//TODO May need changing later
						sourceLui.addImm(pHyperOpIndex + 1);
						LIS->getSlotIndexes()->insertMachineInstrInMaps(sourceLui.operator llvm::MachineInstr *());
//						LIS->getOrCreateInterval(sourceSpAddressRegister);
						registerContainingBaseAddress[pHyperOpContainingDefinition][pHyperOpIndex] = (int) sourceSpAddressRegister;
					}

					MachineInstrBuilder writepm = BuildMI(*(firstDefinition->getParent()), firstDefinition, firstDefinition->getDebugLoc(), TII->get(REDEFINE::WRITEPM));
					writepm.addReg(registerContainingBaseAddress[pHyperOpContainingDefinition][pHyperOpIndex]);
					writepm.addReg(physicalReg);
					writepm.addImm(-(physRegAndContextFrameSlot[physicalReg] + datawidth));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());

					//Inherit bundle properties
					if (sourceLui.operator ->() != 0) {
						sourceLui.operator ->()->bundleWithSucc();
					}
					writepm.operator ->()->bundleWithSucc();

					if (firstDefinition->isBundledWithPred()) {
						if (sourceLui != 0) {
							sourceLui.operator ->()->bundleWithPred();
						} else {
							writepm.operator ->()->bundleWithPred();
						}
					}

					MachineInstrBuilder targetLui;
					//Load the base scratchpad address to a register in the consumer CE the first time
					if (registerContainingBaseAddress[pHyperOpIndex][pHyperOpIndex] == -1) {
						targetLui = BuildMI(*MBBI, MI, MI->getDebugLoc(), TII->get(REDEFINE::LUI));
						unsigned targetSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						targetLui.addReg(targetSpAddressRegister, RegState::Define);
						//TODO May need changing later
						targetLui.addImm(pHyperOpIndex + 1);

						LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());
//						LIS->getOrCreateInterval(targetSpAddressRegister);
						registerContainingBaseAddress[pHyperOpIndex][pHyperOpIndex] = (int) targetSpAddressRegister;
					}
					MachineInstrBuilder readpm = BuildMI(*MBBI, MI, MI->getDebugLoc(), TII->get(REDEFINE::READPM));
					readpm.addReg(MO.getReg(), RegState::Define);
					readpm.addReg(registerContainingBaseAddress[pHyperOpIndex][pHyperOpIndex]);
					readpm.addImm(-(physRegAndContextFrameSlot[physicalReg] + datawidth));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());

					//Inherit bundle properties
					if (targetLui.operator ->() != 0) {
						targetLui.operator ->()->bundleWithSucc();
					}
					readpm.operator ->()->bundleWithSucc();

					if (MI->isBundledWithPred()) {
						if (targetLui != 0) {
							sourceLui.operator ->()->bundleWithPred();
						} else {
							readpm.operator ->()->bundleWithPred();
						}
					}

					//Remove the original copy instruction
					if (MI->isInsideBundle()) {
						MI->eraseFromBundle();
					}
				}
			}

		}

	}
}

//Shuffle context frame slots for the HyperOp
map<HyperOpEdge*, HyperOp*> parentMap = graph->getHyperOp(const_cast<Function*>(MF.getFunction()))->ParentMap;
for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = parentMap.begin(); parentItr != parentMap.end(); parentItr++) {
	HyperOpEdge* edge = parentItr->first;
	unsigned previousPhysReg = contextFrameSlotAndPhysReg[edge->getPositionOfContextSlot()];
	edge->setPositionOfContextSlot(physRegAndContextFrameSlot[previousPhysReg]);
}

DEBUG(dbgs() << "Final state of Machine Function:");
MF.print(dbgs());

}
