/*
 * RISCVMCInstrScheduler.cpp
 *
 *  Created on: 06-Dec-2015
 *      Author: kavitha
 */

#include "RISCVMCInstrScheduler.h"

#include "RISCVTargetMachine.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/Support/Debug.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
using namespace llvm;

RISCVMCInstrScheduler::RISCVMCInstrScheduler(MachineSchedContext *C, MachineSchedStrategy *S) :
		ScheduleDAGMI(C, S) {
}

RISCVMCInstrScheduler::~RISCVMCInstrScheduler() {
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

void RISCVMCInstrScheduler::schedule() {
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

	DebugLoc location = RegionBegin->getDebugLoc();

	DEBUG(for (unsigned su = 0, e = SUnits.size(); su != e; ++su) SUnits[su].dumpAll(this));

	list<pair<SUnit*, unsigned> > instructionAndPHyperOpMap;
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

	MIBundleBuilder* pHyperOpBundleBuilder[ceCount];
	unsigned faninOfHyperOp[ceCount];
	//We need this to do additional code motion and ease creation of pHyperOp bundles
	MachineInstr* firstInstructionOfpHyperOp[ceCount];
	unsigned firstInstructionPosition[ceCount];

	for (unsigned i = 0; i < ceCount; i++) {
		faninOfHyperOp[i] = 0;
		pHyperOpBundleBuilder[i] = 0;
		firstInstructionOfpHyperOp[i] = 0;
		firstInstructionPosition[i] = 0;
	}

//	MachineFunction &MF = *(BB->getParent());
//	//Second pair contains the ce id and the position of the instruction in the instruction sequence
//	list<pair<MachineInstr*, pair<unsigned, unsigned> > > allInstructionsOfPHyperOps;
//	unsigned position = 0;
//
//	for (list<pair<SUnit*, unsigned> >::iterator ScheduledInstrItr = instructionAndPHyperOpMap.begin(); ScheduledInstrItr != instructionAndPHyperOpMap.end(); ScheduledInstrItr++) {
//		unsigned ceContainingInstruction = ScheduledInstrItr->second;
//		SUnit* SU = ScheduledInstrItr->first;
//		MachineInstr* machineInstruction = SU->getInstr();
//		MachineBasicBlock& parentBasicBlock = *(machineInstruction->getParent());
//		unsigned additionalFanin = 0;
//		for (SmallVector<SDep, 4>::iterator predecessorItr = SU->Preds.begin(); predecessorItr != SU->Preds.end(); predecessorItr++) {
//			for (list<pair<SUnit*, unsigned> >::iterator predecessorInstrItr = instructionAndPHyperOpMap.begin(); predecessorInstrItr != instructionAndPHyperOpMap.end(); predecessorInstrItr++) {
//				if (predecessorInstrItr->first->getInstr() == predecessorItr->getSUnit()->getInstr() && predecessorInstrItr->second != ceContainingInstruction) {
//					additionalFanin++;
//				}
//			}
//		}
//		if (additionalFanin + faninOfHyperOp[currentCE] > SPLOCATIONS - ceCount - 1) {
//			//Add sync barrier in all CEs
//			for (unsigned i = 0; i < ceCount; i++) {
//				unsigned sourceSpAddressRegister = ((RISCVTargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
//				//Write to all other CEs
//				for (unsigned j = 0; j < ceCount; j++) {
//					if (j != i) {
//						MachineInstrBuilder sourceLui = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(RISCV::LUI));
//						sourceLui.addReg(sourceSpAddressRegister);
//						unsigned scratchpadDestAddress = ((j + 1) * SPLOCATIONS - i - 1) << 12;
//						sourceLui.addImm(scratchpadDestAddress);
//
//						MachineInstrBuilder writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(RISCV::WRITEPM));
//						writepm.addReg(sourceSpAddressRegister);
//						//Dummy data
//						writepm.addReg(sourceSpAddressRegister);
//						writepm.addImm(0);
//
//						allInstructionsOfPHyperOps.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(i, position++)));
//						allInstructionsOfPHyperOps.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i, position++)));
//					}
//				}
//
//				//Read from all other CEs
//				for (unsigned j = 0; j < ceCount; j++) {
//					if (j != i) {
//						MachineInstrBuilder sourceLui = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(RISCV::LUI));
//						sourceLui.addReg(sourceSpAddressRegister);
//						unsigned scratchpadDestAddress = ((i + 1) * SPLOCATIONS - j - 1) << 12;
//						sourceLui.addImm(scratchpadDestAddress);
//
//						MachineInstrBuilder readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(RISCV::READPM));
//						readpm.addReg(sourceSpAddressRegister);
//						//Dummy data
//						readpm.addReg(sourceSpAddressRegister);
//						readpm.addImm(0);
//
//						allInstructionsOfPHyperOps.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(i, position++)));
//						allInstructionsOfPHyperOps.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i, position++)));
//					}
//				}
//				for (unsigned i = 0; i < ceCount; i++) {
//					faninOfHyperOp[i] = 0;
//				}
//			}
//
//		} else if (!SU->Preds.empty()) {
//			for (SmallVector<SDep, 4>::iterator predecessorItr = SU->Preds.begin(); predecessorItr != SU->Preds.end(); predecessorItr++) {
//				SDep dependence = (*predecessorItr);
//				SUnit* predecessor = predecessorItr->getSUnit();
//				unsigned int ceContainingPredecessorInstruction;
//				for (list<pair<SUnit*, unsigned> >::iterator predecessorInstrItr = instructionAndPHyperOpMap.begin(); predecessorInstrItr != instructionAndPHyperOpMap.end(); predecessorInstrItr++) {
//					if (predecessorInstrItr->first->getInstr() == predecessor->getInstr()) {
//						ceContainingPredecessorInstruction = predecessorInstrItr->second;
//						break;
//					}
//				}
//				if (ceContainingPredecessorInstruction != ceContainingInstruction) {
//					//Load scratchpad address to register
//					unsigned scratchPadAddress = ((ceContainingInstruction * SPLOCATIONS) + faninOfHyperOp[ceContainingInstruction]) << 12;
//					MachineInstrBuilder sourceLui = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(RISCV::LUI));
//					unsigned sourceSpAddressRegister = ((RISCVTargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
//					sourceLui.addReg(sourceSpAddressRegister);
//					sourceLui.addImm(scratchPadAddress);
//
//					//Add writepm in predCE
//					MachineInstrBuilder writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(RISCV::WRITEPM));
//					writepm.addReg(sourceSpAddressRegister);
//
//					MachineInstrBuilder targetLui = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(RISCV::LUI));
//					unsigned targetSpAddressRegister = ((RISCVTargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
//					targetLui.addReg(targetSpAddressRegister);
//					targetLui.addImm(scratchPadAddress);
//
//					//Add readpm instruction in the target CE
//					MachineInstrBuilder readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(RISCV::READPM));
//
//					if (dependence.isAssignedRegDep()) {
//						//Use register operand in the inserted writepm
//						writepm.addReg(dependence.getReg());
//						readpm.addReg(dependence.getReg());
//					} else {
//						//dependence still exists, associate a dummy virtual register
//						writepm.addReg(((RISCVTargetMachine&) TM).FuncInfo->CreateReg(MVT::i32));
//						readpm.addReg(((RISCVTargetMachine&) TM).FuncInfo->CreateReg(MVT::i32));
//					}
//					writepm.addImm(0);
//
//					readpm.addReg(targetSpAddressRegister);
//					readpm.addImm(0);
//
//					allInstructionsOfPHyperOps.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, position++)));
//					allInstructionsOfPHyperOps.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, position++)));
//					allInstructionsOfPHyperOps.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, position++)));
//					if (firstInstructionOfpHyperOp[ceContainingInstruction] == 0) {
//						firstInstructionOfpHyperOp[ceContainingInstruction] = targetLui;
//						firstInstructionPosition[ceContainingInstruction] = position;
//					}
//					allInstructionsOfPHyperOps.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, position++)));
//
//					faninOfHyperOp[ceContainingInstruction] = faninOfHyperOp[ceContainingInstruction] + 1;
//				}
//			}
//		}
//		if (firstInstructionOfpHyperOp[ceContainingInstruction] == 0) {
//			firstInstructionOfpHyperOp[ceContainingInstruction] = machineInstruction;
//			firstInstructionPosition[ceContainingInstruction] = position;
//		}
//		//Barrier sync at the end of basic block from every other CE to the first CE
////		if (machineInstruction == BB->getFirstTerminator()) {
////			unsigned sourceSpAddressRegister = ((RISCVTargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
////			for (unsigned i = 0; i < ceCount; i++) {
////				if (i != ceContainingInstruction) {
////					//Add a writepm instruction
////					MachineInstrBuilder sourceLui = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(RISCV::LUI));
////					sourceLui.addReg(sourceSpAddressRegister);
////					unsigned scratchpadDestAddress = ((currentCE + 1) * SPLOCATIONS - i - 1) << 12;
////					sourceLui.addImm(scratchpadDestAddress);
////
////					MachineInstrBuilder writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(RISCV::WRITEPM));
////					writepm.addReg(sourceSpAddressRegister);
////					//Dummy data
////					writepm.addReg(sourceSpAddressRegister);
////					writepm.addImm(0);
////					allInstructionsOfPHyperOps.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(i, position++)));
////					allInstructionsOfPHyperOps.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i, position++)));
////				} else {
////					//Add readpm instructions
////					MachineInstrBuilder sourceLui = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(RISCV::LUI));
////					sourceLui.addReg(sourceSpAddressRegister);
////					unsigned scratchpadDestAddress = ((i + 1) * SPLOCATIONS - currentCE - 1) << 12;
////					sourceLui.addImm(scratchpadDestAddress);
////
////					MachineInstrBuilder readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(RISCV::READPM));
////					readpm.addReg(sourceSpAddressRegister);
////					//Dummy data
////					readpm.addReg(sourceSpAddressRegister);
////					readpm.addImm(0);
////
////					allInstructionsOfPHyperOps.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(currentCE, position++)));
////					allInstructionsOfPHyperOps.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(currentCE, position++)));
////				}
////			}
////		}
//		allInstructionsOfPHyperOps.push_back(make_pair(machineInstruction, make_pair(ceContainingInstruction, position++)));
//	}
//
//	//Shuffle instructions
//	for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator allInstructionItr = allInstructionsOfPHyperOps.begin(); allInstructionItr != allInstructionsOfPHyperOps.end(); allInstructionItr++) {
//		MachineInstr* instruction = allInstructionItr->first;
//		unsigned ce = allInstructionItr->second.first;
//		unsigned position = allInstructionItr->second.second;
//		instruction->setFlag(MachineInstr::NoFlags);
//		if (ce < ceCount - 1) {
//			MachineInstr* nextCeInstruction = firstInstructionOfpHyperOp[ce + 1];
//			unsigned nextCeInstructionPosition = firstInstructionPosition[ce + 1];
//			if (nextCeInstructionPosition < position) {
//				BB->splice(nextCeInstruction, BB, instruction);
//			}
//		}
//	}
//
//	list<MachineInstr*> startAndEndPointsOfHyperOp;
//	for (unsigned i = 0; i < ceCount; i++) {
//		if (i == ceCount - 1) {
//			startAndEndPointsOfHyperOp.push_back(BB->end());
//		} else {
//			startAndEndPointsOfHyperOp.push_back(firstInstructionOfpHyperOp[i + 1]);
//		}
//	}
//	basicBlockAndpHopMap.insert(make_pair(BB, startAndEndPointsOfHyperOp));
}

/// Move an instruction and update register pressure.
void RISCVMCInstrScheduler::scheduleMI(SUnit *SU, bool IsTopNode) {
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

