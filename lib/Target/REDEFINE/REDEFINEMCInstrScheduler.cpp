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
#include "REDEFINEUtils.h"
using namespace llvm;
using namespace std;

////TODO Used only 8 bit number here because I need this to address the sync locations and frame locations for replication only which sums upto 20 4 byte locations only

static inline int32_t SignExtend8BitNumberTo12Bits(int8_t x) {
	return int32_t(x << 4) >> 4;
}

static bool isRegDependence(SDep dependence) {
	SDep::Kind dependenceKind = dependence.getKind();
	if (dependenceKind == SDep::Data || dependenceKind == SDep::Anti || dependenceKind == SDep::Output) {
		return true;
	}
	return false;
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
	unsigned ceIndex = 0;
	while (SUnit *SU = SchedImpl->pickNode(IsTopNode)) {
		scheduleMI(SU, IsTopNode);
//		METIS TODO: traversedSUnitsWithIndex[SU] = numVertices;
//		METIS TODO:  numVertices++;
		if (ceCount == 1) {
			instructionAndPHyperOpMapForRegion.push_back(make_pair(SU, 0));
		} else {
			list<unsigned> sourceCEList;
			if (SU->getInstr()->mayLoad() || SU->getInstr()->mayStore()) {
				for (SmallVector<SDep, 4>::iterator predecessorItr = SU->Preds.begin(); predecessorItr != SU->Preds.end(); predecessorItr++) {
					SDep dependence = *predecessorItr;
					if (dependence.isNormalMemory()) {
						//There cannot be multiple memory dependences
						for (list<pair<SUnit*, unsigned> >::iterator ScheduledInstrItr = instructionAndPHyperOpMapForRegion.begin(); ScheduledInstrItr != instructionAndPHyperOpMapForRegion.end(); ScheduledInstrItr++) {
							if (ScheduledInstrItr->first == dependence.getSUnit()) {
								sourceCEList.push_back(ScheduledInstrItr->second);
								break;
							}
						}
					}
				}
			}
			assert((sourceCEList.size() == 0 || sourceCEList.size() == 1) || "Multiple memory dependences shouldn't exist to a memory operation since it accesses only one memory location!");
			if (sourceCEList.empty()) {
				if (SU->getInstr()->mayLoad() || SU->getInstr()->mayStore()) {
					//first instruction to access the location in the basic block, check if the same location has been accessed before by another CE
					//There can only be one memory operand in riscv instr set
					list<unsigned> predecessorCEList;
					for (MachineInstr::mmo_iterator mmoItr = SU->getInstr()->memoperands_begin(); mmoItr != SU->getInstr()->memoperands_end(); mmoItr++) {
						MachineMemOperand * memOperand = *mmoItr;
						unsigned previousDefOfMemOperand = -1;
						for (unsigned i = 0; i < ceCount; i++) {
							//TODO will it suffice to compare Value pointers? should I be using exact locations?
							if (find(memoryLocationsAccessedInCE[i].begin(), memoryLocationsAccessedInCE[i].end(), memOperand->getValue()) != memoryLocationsAccessedInCE[i].end()) {
								previousDefOfMemOperand = i;
								break;
							}
						}

						if (previousDefOfMemOperand == -1) {
							//This memory location is accessed the first time
							memoryLocationsAccessedInCE[ceIndex].push_back(memOperand->getValue());
						} else {
							predecessorCEList.push_back(previousDefOfMemOperand);
						}
					}

					if (predecessorCEList.empty()) {
						instructionAndPHyperOpMapForRegion.push_back(make_pair(SU, ceIndex));
						ceIndex = (ceIndex + 1) % ceCount;
					} else {
						assert(predecessorCEList.size() == 1 && "There cannot be more than one memory operands to an instruction in REDEFINE ISA");
						instructionAndPHyperOpMapForRegion.push_back(make_pair(SU, predecessorCEList.front()));
					}
				} else {
					instructionAndPHyperOpMapForRegion.push_back(make_pair(SU, ceIndex));
					ceIndex = (ceIndex + 1) % ceCount;
				}
			} else {
				instructionAndPHyperOpMapForRegion.push_back(make_pair(SU, (*sourceCEList.begin())));
			}
		}
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

faninOfHyperOp.clear();
for (int i = 0; i < ceCount; i++) {
	faninOfHyperOp.push_back(0);
	for (unsigned j = 0; j < ceCount; j++) {
		registerContainingBaseAddress[i][j] = -1;
	}
}
allInstructionsOfBB.clear();
registersUsedInBB.clear();
if (BB != MF.begin()) {
	HyperOpInteractionGraph * graph = ((REDEFINETargetMachine&) TM).HIG;
	HyperOp* currentHyperOp = graph->getHyperOp(const_cast<Function*>(MF.getFunction()));
	for (map<HyperOpEdge*, HyperOp*>::iterator contextMemItr = currentHyperOp->ParentMap.begin(); contextMemItr != currentHyperOp->ParentMap.end(); contextMemItr++) {
		if (contextMemItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS) {
			unsigned physicalReg = REDEFINEphysRegs[contextMemItr->first->getPositionOfContextSlot()];
			MF.addLiveIn(physicalReg, TRI->getMinimalPhysRegClass(physicalReg));
			BB->addLiveIn(physicalReg);
		}
	}
//	MRI.EmitLiveInCopies(BB, *TRI, *TII);
}
}

void REDEFINEMCInstrScheduler::exitRegion() {
unsigned currentCE = 0;
DebugLoc location = BB->begin()->getDebugLoc();
MachineFunction &MF = *(BB->getParent());
MachineBasicBlock& parentBasicBlock = *BB;

map<unsigned, unsigned> registerAndDefiningCEMap;
//Map containing the renamed register and its replacement
map<unsigned, map<unsigned, unsigned> > replacementRegisterMap;

//Map containing ce and redefinition of a register via a readpm instruction
//Key of the first map represents the CE and the list contains all the registers already locally defined and hence don't require a readpm from the original definition
map<unsigned, list<unsigned> > redefinitionsInCE;

if (RegionBegin == BB->begin() && BB == &BB->getParent()->front()) {
	HyperOpInteractionGraph * graph = ((REDEFINETargetMachine&) TM).HIG;
	const Module* parentModule = BB->getParent()->getFunction()->getParent();
	unsigned maxGlobalSize = 0;
	if (!parentModule->getGlobalList().empty()) {
		for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
			const GlobalVariable *globalVar = &*globalArgItr;
			maxGlobalSize += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
		}
	}

	MachineInstr* insertionPoint = BB->begin();
	for (unsigned i = 0; i < ceCount; i++) {
		unsigned registerForCopyOfInstId = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		unsigned registerForIncrOfInstId = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		virtualRegistersForInstAddr[i] = make_pair(registerForCopyOfInstId, registerForIncrOfInstId);
		MachineInstr* firstInstruction = insertionPoint;
		string globalAddressString = "\"ga";

		globalAddressString.append("#").append(itostr(maxGlobalSize)).append("\"");

		MachineInstrBuilder registerCopy = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(registerForCopyOfInstId, RegState::Define).addReg(REDEFINE::t5).addReg(REDEFINE::zero);
		LIS->getSlotIndexes()->insertMachineInstrInMaps(registerCopy.operator ->());
		allInstructionsOfRegion.push_back(make_pair(registerCopy.operator->(), make_pair(i, insertPosition++)));

		if (firstInstructionOfpHyperOpInRegion[i] == 0) {
			firstInstructionOfpHyperOpInRegion[i] = registerCopy.operator->();
		}
		//add global address to r31 of REDEFINE
		unsigned registerForTopBits = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder lui = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::LUI)).addReg(registerForTopBits, RegState::Define);
		string hiGlobalAddrString = string("%hi(").append(globalAddressString).append(")");
		MCSymbol* hiSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(hiGlobalAddrString));
		lui.addSym(hiSymbol);
		LIS->getSlotIndexes()->insertMachineInstrInMaps(lui.operator ->());
		allInstructionsOfRegion.push_back(make_pair(lui.operator->(), make_pair(i, insertPosition++)));

		unsigned registerForBottomBitsLui = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder addiMacroLui = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::LUI)).addReg(registerForBottomBitsLui, RegState::Define);
		string lowGlobalAddrString = string("%lo(").append(globalAddressString).append(")");
		MCSymbol* loSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(lowGlobalAddrString));
		addiMacroLui.addSym(loSymbol);
		allInstructionsOfRegion.push_back(make_pair(addiMacroLui.operator->(), make_pair(i, insertPosition++)));

		unsigned registerForBottomBitsSrli = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder addiMacroSrli = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(registerForBottomBitsSrli, RegState::Define).addReg(registerForBottomBitsLui).addImm(12);
		allInstructionsOfRegion.push_back(make_pair(addiMacroSrli.operator->(), make_pair(i, insertPosition++)));

		unsigned registerForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder add = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(registerForGlobalAddr, RegState::Define).addReg(registerForTopBits).addReg(registerForBottomBitsSrli);
		LIS->getSlotIndexes()->insertMachineInstrInMaps(add.operator ->());
		allInstructionsOfRegion.push_back(make_pair(add.operator->(), make_pair(i, insertPosition++)));
		memoryFrameBaseAddress[i] = registerForGlobalAddr;

		unsigned registerForMulOperand = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder addiForMul = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(registerForMulOperand, RegState::Define).addReg(REDEFINE::zero).addImm(graph->getMaxMemFrameSize());
		LIS->getSlotIndexes()->insertMachineInstrInMaps(addiForMul.operator ->());
		allInstructionsOfRegion.push_back(make_pair(addiForMul.operator->(), make_pair(i, insertPosition++)));
		memoryFrameMaxSizeReg[i] = registerForMulOperand;

		//TODO
		unsigned crId = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder shiftForCRId = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(crId, RegState::Define).addReg(REDEFINE::t5).addImm(21);
		allInstructionsOfRegion.push_back(make_pair(shiftForCRId.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForCRId.operator llvm::MachineInstr *());

		unsigned shiftedPageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder shiftForPageNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(shiftedPageNumber, RegState::Define).addReg(REDEFINE::t5).addImm(11);
		allInstructionsOfRegion.push_back(make_pair(shiftForPageNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForPageNumber.operator llvm::MachineInstr *());
		unsigned pageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder bitmaskForPageNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ANDI)).addReg(pageNumber, RegState::Define).addReg(shiftedPageNumber).addImm(15);
		allInstructionsOfRegion.push_back(make_pair(bitmaskForPageNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(bitmaskForPageNumber.operator llvm::MachineInstr *());

		unsigned shiftedFrameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder shiftForFrameNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(shiftedFrameNumber, RegState::Define).addReg(REDEFINE::t5).addImm(6);
		allInstructionsOfRegion.push_back(make_pair(shiftForFrameNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForFrameNumber.operator llvm::MachineInstr *());
		unsigned frameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder bitmaskForFrameNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ANDI)).addReg(frameNumber, RegState::Define).addReg(shiftedFrameNumber).addImm(63);
		allInstructionsOfRegion.push_back(make_pair(bitmaskForFrameNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(bitmaskForFrameNumber.operator llvm::MachineInstr *());

		unsigned loadImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulOperandForCRBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(loadImmInReg, RegState::Define).addReg(crId).addImm(800);
		allInstructionsOfRegion.push_back(make_pair(mulOperandForCRBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(mulOperandForCRBase.operator llvm::MachineInstr *());

		unsigned crBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulForCRBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(crBase, RegState::Define).addReg(crId).addReg(loadImmInReg);
		allInstructionsOfRegion.push_back(make_pair(mulForCRBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(mulForCRBase.operator llvm::MachineInstr *());

		unsigned loadPageImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulOperandForPageBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(loadPageImmInReg, RegState::Define).addReg(crId).addImm(52);
		allInstructionsOfRegion.push_back(make_pair(mulOperandForPageBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(mulOperandForPageBase.operator llvm::MachineInstr *());

		unsigned pageBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulForPageBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(pageBase, RegState::Define).addReg(pageNumber).addReg(loadPageImmInReg);
		allInstructionsOfRegion.push_back(make_pair(mulForPageBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(mulForPageBase.operator llvm::MachineInstr *());

		unsigned frameBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder addForFrameBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(frameBase, RegState::Define).addReg(pageBase).addReg(frameNumber);
		allInstructionsOfRegion.push_back(make_pair(addForFrameBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(addForFrameBase.operator llvm::MachineInstr *());

		unsigned crIdBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder addForCRBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(crIdBase, RegState::Define).addReg(frameBase).addReg(crBase);
		allInstructionsOfRegion.push_back(make_pair(addForCRBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(addForCRBase.operator llvm::MachineInstr *());
		//END TODO

		unsigned registerForMul = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulForFrameSize = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(registerForMul, RegState::Define).addReg(crIdBase).addReg(registerForMulOperand);
		LIS->getSlotIndexes()->insertMachineInstrInMaps(mulForFrameSize.operator ->());
		allInstructionsOfRegion.push_back(make_pair(mulForFrameSize.operator->(), make_pair(i, insertPosition++)));

		MachineInstrBuilder addForGlobalAddr = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(registerForIncrOfInstId, RegState::Define).addReg(registerForGlobalAddr).addReg(registerForMul);
		LIS->getSlotIndexes()->insertMachineInstrInMaps(addForGlobalAddr.operator ->());
		allInstructionsOfRegion.push_back(make_pair(addForGlobalAddr.operator->(), make_pair(i, insertPosition++)));
		insertionPoint = firstInstruction;
	}

//	for (MachineFunction::iterator bbItr = MF.begin(); bbItr != MF.end(); bbItr++) {
//		MachineBasicBlock* tempBB = &*bbItr;
//		if (tempBB != BB) {
//			for (unsigned i = 0; i < ceCount; i++) {
////				tempBB->addLiveIn(memoryFrameBaseAddress[i]);
////				tempBB->addLiveIn(memoryFrameMaxSizeReg[i]);
//			}
//		}
//	}
}

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
	if ((additionalFanin + faninOfHyperOp[currentCE] > SPLOCATIONS - datawidth * (ceil(log2(ceCount)) * ceCount + frameSize))) {
		//Add reduction tree
		unsigned nextCe = 2;
		unsigned depth = 0;
		unsigned numNodesAtDepth = 0;
		unsigned tempCECount = 1;
		while (depth < ceil(log2(ceCount))) {
			depth++;
			unsigned increment = nextCe / 2;
			for (unsigned i = 0; i < ceCount; i += increment) {
				if (i % 2 == 0 && (i + increment) < ceCount) {
					{
						MachineInstrBuilder luiOfTargetCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForTargetAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfTargetCE.addReg(registerForTargetAddr, RegState::Define);
						luiOfTargetCE.addImm(i + increment);
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
						//TODO
						int32_t immediateSPOffset = SPLOCATIONS - ((depth * i + numNodesAtDepth + frameSize) * datawidth);
						writepm.addImm(immediateSPOffset);
						allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());

						MachineInstrBuilder luiOfCurrentCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForCurrentAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfCurrentCE.addReg(registerForCurrentAddr, RegState::Define);
						luiOfCurrentCE.addImm(i);
						LIS->getSlotIndexes()->insertMachineInstrInMaps(luiOfCurrentCE.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(luiOfCurrentCE.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

						//Add readpm to first CE
						MachineInstrBuilder readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::DREADPM));
						unsigned readpmTarget = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						readpm.addReg(readpmTarget, RegState::Define);
						//Dummy data
						readpm.addReg(registerForCurrentAddr);
						//TODO
						immediateSPOffset = SPLOCATIONS - ((depth * (i + 1) + numNodesAtDepth + frameSize) * datawidth);
						readpm.addImm(immediateSPOffset);
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
					}

					{
						MachineInstrBuilder luiOfTargetCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForTargetAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfTargetCE.addReg(registerForTargetAddr, RegState::Define);
						luiOfTargetCE.addImm(i);
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
						//TODO
						int32_t immediateSPOffset = SPLOCATIONS - ((depth * (i + 1) + numNodesAtDepth + frameSize) * datawidth);
						writepm.addImm(immediateSPOffset);
						allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());

						MachineInstrBuilder luiOfCurrentCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForCurrentAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfCurrentCE.addReg(registerForCurrentAddr, RegState::Define);
						luiOfCurrentCE.addImm(i + increment);
						LIS->getSlotIndexes()->insertMachineInstrInMaps(luiOfCurrentCE.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(luiOfCurrentCE.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));

						//Add readpm to first CE
						MachineInstrBuilder readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::DREADPM));
						unsigned readpmTarget = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						readpm.addReg(readpmTarget, RegState::Define);
						//Dummy data
						readpm.addReg(registerForCurrentAddr);
						//TODO
						immediateSPOffset = SPLOCATIONS - ((depth * i + numNodesAtDepth + frameSize) * datawidth);
						readpm.addImm(immediateSPOffset);
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
						LIS->getOrCreateInterval(readpmTarget);
						LIS->addLiveRangeToEndOfBlock(readpmTarget, readpm.operator ->());
					}
				}
			}
			numNodesAtDepth = numNodesAtDepth + floor(ceCount / (nextCe / 2));
			nextCe = nextCe * 2;
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
			//TODO this bit isn't necessary because registers are not live-in across basic blocks in mem form
			if (ceContainingPredecessorInstruction < 0 && isRegDependence(dependence)) {
				//Find the memory location allocated to the live-out register
				if (registerAndFrameLocation.find(dependence.getReg()) != registerAndFrameLocation.end()) {
					unsigned frameLocationToReadFrom = registerAndFrameLocation.find(dependence.getReg())->second;
					//Add an instruction to read from memory
					MachineInstrBuilder loadFromMemory = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::ADDI));
					loadFromMemory.addReg(dependence.getReg(), RegState::Define);
//					loadFromMemory.addReg(virtualRegistersForInstAddr[ceContainingInstruction].second);
					loadFromMemory.addReg(REDEFINE::t5);
					loadFromMemory.addFrameIndex(frameLocationToReadFrom);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(loadFromMemory.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(loadFromMemory.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, insertPosition++)));
				}
			} else if (ceContainingPredecessorInstruction != ceContainingInstruction) {
				//If the register is already read from in the current ce via a readpm instruction, there is no need to read it all over again
				if (isRegDependence(dependence) && redefinitionsInCE.find(ceContainingInstruction) != redefinitionsInCE.end()
						&& find(redefinitionsInCE[ceContainingInstruction].begin(), redefinitionsInCE[ceContainingInstruction].end(), dependence.getReg()) != redefinitionsInCE[ceContainingInstruction].end()) {
					continue;
				}

				unsigned offsetInScratchpad = faninOfHyperOp[ceContainingInstruction];
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

				//TODO this is not required since spm size isn't so large
//				//TODO If number of registers spills into larger than 12 bit size because immediate value supported by readpm and writepm is a signed 11 bit field
//				if (ceil(log2(offsetInScratchpad)) > 11) {
//					//Add addi instruction incrementing the base address field in source
//					MachineInstrBuilder sourceAddi = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::ADDI));
//					sourceAddi.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction]);
//					sourceAddi.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction]);
//					sourceAddi.addImm(offsetInScratchpad - datawidth);
//					allInstructionsOfRegion.push_back(make_pair(sourceAddi.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, insertPosition++)));
//
//					//Add addi instruction incrementing the base address field in target
//					MachineInstrBuilder targetAddi = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::ADDI));
//					targetAddi.addReg(registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction]);
//					targetAddi.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction]);
//					targetAddi.addImm(offsetInScratchpad);
//					allInstructionsOfRegion.push_back(make_pair(targetAddi.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, insertPosition++)));
//
//					offsetInScratchpad = faninOfHyperOp[ceContainingInstruction] = datawidth;
//				}

				//Write the data onto the ce's scratchpad and read from there
				//Add writepm or fwritepm in predCE
				MachineInstrBuilder writepm;
				MachineInstrBuilder readpm;
				//Memory dependences get chained into the same pHyperOp and hence, we don't actually need the following check
				if (isRegDependence(dependence)) {
					const TargetRegisterClass* regClass;
					if (TRI->isPhysicalRegister(dependence.getReg())) {
						regClass = TRI->getMinimalPhysRegClass(dependence.getReg());
					}
					//Virtual reg
					else {
						regClass = ((REDEFINETargetMachine&) TM).FuncInfo->RegInfo->getRegClass(dependence.getReg());
					}
					if (*regClass->vt_begin() == MVT::i32) {
						writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::WRITEPM));
						readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::DREADPM));
					} else if (*regClass->vt_begin() == MVT::f32) {
						writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::FWRITEPM));
						readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::FDREADPM));
					}
				} else {
					writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::WRITEPM));
					readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::DREADPM));
				}
				LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());
				LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());

				//The previous and next if-else block is not merged because of the following writepm.addReg statement
				writepm.addReg(registerContainingBaseAddress[ceContainingPredecessorInstruction][ceContainingInstruction]);

				if (isRegDependence(dependence)) {
					//Use register operand in the inserted writepm
					writepm.addReg(dependence.getReg());
					readpm.addReg(dependence.getReg(), RegState::Define);
					if (registerAndDefiningCEMap.find(dependence.getReg()) == registerAndDefiningCEMap.end()) {
						registerAndDefiningCEMap[dependence.getReg()] = ceContainingPredecessorInstruction;
					}
					//The register is redefined in the current CE and doesn't need to be read again, caching for this purpose
					redefinitionsInCE[ceContainingInstruction].push_back(dependence.getReg());
				} else {
					//dependence still exists but is not a true dependence, associate a dummy virtual register
					writepm.addReg(REDEFINE::zero);
					//Zero cannot be defined, hence adding a new dummy register
					unsigned dummyRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					readpm.addReg(dummyRegister, RegState::Define);
					//Interval should be manually added
					LIS->getOrCreateInterval(dummyRegister);
					LIS->addLiveRangeToEndOfBlock(dummyRegister, readpm.operator ->());
				}

				writepm.addImm(offsetInScratchpad);
				readpm.addReg(registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction]);
				readpm.addImm(offsetInScratchpad);

				allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, insertPosition++)));
				allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, insertPosition++)));
				//Increment by 4 since scratchpad is byte addressable
				faninOfHyperOp[ceContainingInstruction] = faninOfHyperOp[ceContainingInstruction] + datawidth;
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
	unsigned nextCe = 2;
	unsigned depth = 0;
	unsigned numNodesAtDepth = 0;
	while (depth < ceil(log2(ceCount))) {
		depth++;
		unsigned increment = nextCe / 2;
		for (unsigned i = 0; i < ceCount; i += increment) {
			if (i % 2 == 0 && (i + increment) < ceCount) {
				{
					MachineInstrBuilder luiOfTargetCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
					unsigned registerForTargetAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					luiOfTargetCE.addReg(registerForTargetAddr, RegState::Define);
					luiOfTargetCE.addImm(i + increment);
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
					//TODO (SPLOCATIONS * 3) since we are using only 1/4th of address space
					int32_t immediateSPOffset = SPLOCATIONS - ((depth * i + numNodesAtDepth + frameSize) * datawidth);
					writepm.addImm(immediateSPOffset);
					allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());

					MachineInstrBuilder luiOfCurrentCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
					unsigned registerForCurrentAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					luiOfCurrentCE.addReg(registerForCurrentAddr, RegState::Define);
					luiOfCurrentCE.addImm(i);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(luiOfCurrentCE.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(luiOfCurrentCE.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

					//Add readpm to first CE
					MachineInstrBuilder readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::DREADPM));
					unsigned readpmTarget = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					readpm.addReg(readpmTarget, RegState::Define);
					//Dummy data
					readpm.addReg(registerForCurrentAddr);
					//TODO
					immediateSPOffset = SPLOCATIONS - ((depth * (i + 1) + numNodesAtDepth + frameSize) * datawidth);
					readpm.addImm(immediateSPOffset);
					allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
					LIS->getOrCreateInterval(readpmTarget);
					LIS->addLiveRangeToEndOfBlock(readpmTarget, readpm.operator ->());
				}

				{
					MachineInstrBuilder luiOfTargetCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
					unsigned registerForTargetAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					luiOfTargetCE.addReg(registerForTargetAddr, RegState::Define);
					luiOfTargetCE.addImm(i);
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
					//TODO
					int32_t immediateSPOffset = SPLOCATIONS - ((depth * (i + 1) + numNodesAtDepth + frameSize) * datawidth);
					writepm.addImm(immediateSPOffset);
					allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());

					MachineInstrBuilder luiOfCurrentCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
					unsigned registerForCurrentAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					luiOfCurrentCE.addReg(registerForCurrentAddr, RegState::Define);
					luiOfCurrentCE.addImm(i + increment);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(luiOfCurrentCE.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(luiOfCurrentCE.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));

					//Add readpm to first CE
					MachineInstrBuilder readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::DREADPM));
					unsigned readpmTarget = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					readpm.addReg(readpmTarget, RegState::Define);
					//Dummy data
					readpm.addReg(registerForCurrentAddr);
					//TODO
					immediateSPOffset = SPLOCATIONS - ((depth * i + numNodesAtDepth + frameSize) * datawidth);
					readpm.addImm(immediateSPOffset);
					allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
					LIS->getOrCreateInterval(readpmTarget);
					LIS->addLiveRangeToEndOfBlock(readpmTarget, readpm.operator ->());
				}
			}
		}
		numNodesAtDepth = numNodesAtDepth + floor(ceCount / (nextCe / 2));
		nextCe = nextCe * 2;
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
//						loadFromMemory.addReg(virtualRegistersForInstAddr[ceContainingInstruction].second);
						loadFromMemory.addReg(REDEFINE::t5);
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
							readpm = BuildMI(parentBasicBlock, RegionEnd, location, TII->get(REDEFINE::DREADPM));
						} else if (*regClass->vt_begin() == MVT::f32) {
							writepm = BuildMI(parentBasicBlock, RegionEnd, location, TII->get(REDEFINE::FWRITEPM));
							readpm = BuildMI(parentBasicBlock, RegionEnd, location, TII->get(REDEFINE::FDREADPM));
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
			nopInstruction.addReg(REDEFINE::zero, RegState::Define);
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
				const TargetRegisterClass* regClass;
				if (TRI->isPhysicalRegister(originalRegister)) {
					regClass = TRI->getMinimalPhysRegClass(originalRegister);
				} else {
					regClass = MRI.getRegClass(originalRegister);
				}

				if (*regClass->vt_begin() == MVT::i32) {
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

if (firstInstruction != 0) {
	RegionBegin = firstInstruction;
}

for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::reverse_iterator instItr = allInstructionsOfRegion.rbegin(); instItr != allInstructionsOfRegion.rend(); instItr++) {
	allInstructionsOfBB.push_front(*instItr);
}

}

void REDEFINEMCInstrScheduler::finishBlock() {
//If the basic block is the terminator
if (BB->getName().compare(MF.back().getName()) == 0) {
	Module * parentModule = const_cast<Module*>(BB->getParent()->getFunction()->getParent());
	DebugLoc location = BB->begin()->getDebugLoc();

	//fbind is in case of context frames being reused statically and hence, is added optionally; returns are assumed to be merged during HyperOp creation
	HyperOpInteractionGraph * graph = ((REDEFINETargetMachine&) TM).HIG;
	Function* Fn = const_cast<Function*>(MF.getFunction());
	HyperOp* hyperOp = ((REDEFINETargetMachine&) TM).HIG->getHyperOp(Fn);

//TODO:Assuming here that there is only one exit block since we merge return, make sure this is indeed correct
	MachineBasicBlock& lastBB = MF.back();
	MachineInstr* lastInstruction = lastBB.end();

	firstInstructionOfpHyperOpInRegion.clear();
	for (unsigned i = 0; i < ceCount; i++) {
		firstInstructionOfpHyperOpInRegion.push_back(0);
	}

	allInstructionsOfRegion.clear();
	insertPosition = 0;

	map<HyperOp*, unsigned> regContainingMemFrameBaseAddress;

	DEBUG(dbgs() << "Adding falloc, writecm(for sync instructions) and fbind instructions\n");
	map<HyperOp*, pair<unsigned, unsigned> > registerContainingHyperOpFrameAddressAndCEWithFalloc;
	unsigned currentCE = 0;
	for (list<HyperOp*>::iterator childHyperOpItr = graph->Vertices.begin(); childHyperOpItr != graph->Vertices.end(); childHyperOpItr++) {
		//Among the HyperOps immediately dominated by the hyperOp, add fbind for those HyperOps that require it
		if ((*childHyperOpItr)->getImmediateDominator() == hyperOp) {
			if (registerContainingHyperOpFrameAddressAndCEWithFalloc.find(*childHyperOpItr) == registerContainingHyperOpFrameAddressAndCEWithFalloc.end()) {
				if (!(*childHyperOpItr)->isStaticHyperOp()) {
					//Add falloc instruction to create dynamic HyperOp instances
					unsigned registerContainingConsumerFrameAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder falloc = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::FALLOC));
					falloc.addReg(registerContainingConsumerFrameAddr, RegState::Define);
					falloc.addImm(1);
					allInstructionsOfRegion.push_back(make_pair(falloc.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
					registerContainingHyperOpFrameAddressAndCEWithFalloc.insert(make_pair(*childHyperOpItr, make_pair(registerContainingConsumerFrameAddr, currentCE)));
					if (firstInstructionOfpHyperOpInRegion[currentCE] == 0) {
						firstInstructionOfpHyperOpInRegion[currentCE] = falloc.operator llvm::MachineInstr *();
					}
				} else {
					unsigned registerContainingConsumerFrameAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder addi = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADDI));
					addi.addReg(registerContainingConsumerFrameAddr, RegState::Define);
					addi.addReg(REDEFINE::zero);
					addi.addImm(REDEFINEUtils::getHyperOpId(*childHyperOpItr));
					if (firstInstructionOfpHyperOpInRegion[currentCE] == 0) {
						firstInstructionOfpHyperOpInRegion[currentCE] = addi.operator llvm::MachineInstr *();
					}
					allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
					registerContainingHyperOpFrameAddressAndCEWithFalloc.insert(make_pair(*childHyperOpItr, make_pair(registerContainingConsumerFrameAddr, currentCE)));
				}
			}
			if (!(*childHyperOpItr)->isStaticHyperOp()) {
				unsigned registerContainingConsumerFrameAddr = registerContainingHyperOpFrameAddressAndCEWithFalloc.find(*childHyperOpItr)->second.first;
				int hyperOpId = (*childHyperOpItr)->getHyperOpId();
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
			if ((*childHyperOpItr)->isBarrierHyperOp()) {
				unsigned registerContainingConsumerFrameAddr = registerContainingHyperOpFrameAddressAndCEWithFalloc.find(*childHyperOpItr)->second.first;
				//TODO need to add more code for dynamic hyperops synchronizing with the same sync barrier
				unsigned registerWithSyncCount = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder addi = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADDI));
				addi.addReg(registerWithSyncCount, RegState::Define);
				addi.addReg(REDEFINE::zero);
				addi.addImm((*childHyperOpItr)->getSyncCount());
				allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
				if (firstInstructionOfpHyperOpInRegion[currentCE] == 0) {
					firstInstructionOfpHyperOpInRegion[currentCE] = addi.operator llvm::MachineInstr *();
				}
				MachineInstrBuilder writecm = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::WRITECM));
				writecm.addReg(registerContainingConsumerFrameAddr);
				writecm.addReg(registerWithSyncCount);
				writecm.addImm(60);

				//Add the writecm instructions to the function map so that they can be patched for the right context frame location later
				Function* consumerFunction = (*childHyperOpItr)->getFunction();

				allInstructionsOfRegion.push_back(make_pair(writecm.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(writecm.operator llvm::MachineInstr *());
				currentCE = (currentCE + 1) % ceCount;
			}
		}

		if (!(*childHyperOpItr)->isStartHyperOp()) {
			HyperOp* liveStartOfVertex = (*childHyperOpItr)->getImmediateDominator();
			HyperOp* liveEndOfVertex = 0;
			if (liveStartOfVertex != 0) {
				liveEndOfVertex = liveStartOfVertex->getImmediatePostDominator();
			} else {
				liveEndOfVertex = (*childHyperOpItr);
			}
			//Add fdelete instruction from r30
			if (liveEndOfVertex == hyperOp) {
				MachineInstrBuilder fdelete;
				if (liveEndOfVertex == (*childHyperOpItr)) {
					//Add an instruction to delete the frame of the HyperOp
					fdelete = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::FDELETE));
//					fdelete.addReg(virtualRegistersForInstAddr[0].first);
					fdelete.addReg(REDEFINE::t4);
					fdelete.addImm(0);
//				} else if ((*childHyperOpItr)->isStaticHyperOp()) {
//					//Immediate is larger than 12 bits
//					if (REDEFINEUtils::getHyperOpId(*childHyperOpItr) != 0 && Log2_32_Ceil(REDEFINEUtils::getHyperOpId(*childHyperOpItr)) > 10) {
//						unsigned registerWithContextAddress = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
//						MachineInstrBuilder addi = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADDI));
//						addi.addReg(registerWithContextAddress, RegState::Define);
//						addi.addReg(REDEFINE::zero);
//						addi.addImm(REDEFINEUtils::getHyperOpId(*childHyperOpItr));
//						if (firstInstructionOfpHyperOpInRegion[0] == 0) {
//							firstInstructionOfpHyperOpInRegion[0] = addi.operator llvm::MachineInstr *();
//						}
//						LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
//						allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
//						fdelete = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::FDELETE)).addReg(registerWithContextAddress).addImm(0);
//					} else {
//						fdelete = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::FDELETE)).addReg(REDEFINE::zero).addImm(REDEFINEUtils::getHyperOpId(*childHyperOpItr));
//					}
				} else {
					//Find the edge corresponding to the context frame of the child hyperop
					for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = liveEndOfVertex->ParentMap.begin(); parentItr != liveEndOfVertex->ParentMap.end(); parentItr++) {
						if (parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS && parentItr->first->getContextFrameAddress() == (*childHyperOpItr)) {
							//Get the slot to read from which translates to a register anyway
							int contextSlot = parentItr->first->getPositionOfContextSlot();
							fdelete = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::FDELETE)).addReg(REDEFINEphysRegs[contextSlot]).addImm(0);
//							unsigned virtualReg = lastBB.getParent()->addLiveIn(REDEFINEphysRegs[contextSlot],TRI->getMinimalPhysRegClass(REDEFINEphysRegs[contextSlot]));
//							fdelete = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::FDELETE)).addReg(virtualReg).addImm(0);
//							LIS->getOrCreateInterval(virtualReg);
//							LIS->addLiveRangeToEndOfBlock(virtualReg,fdelete.operator ->());
							break;
						}
					}
				}
				if (firstInstructionOfpHyperOpInRegion[0] == 0) {
					firstInstructionOfpHyperOpInRegion[0] = fdelete.operator llvm::MachineInstr *();
				}
				LIS->getSlotIndexes()->insertMachineInstrInMaps(fdelete.operator llvm::MachineInstr *());
				allInstructionsOfRegion.push_back(make_pair(fdelete.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
			}
		}
	}

	DEBUG(dbgs() << "Adding writecm instructions to hyperOp " << hyperOp->asString() << "\n");
//Cyclically distribute the writecm stubs among pHyperOps
	for (map<HyperOpEdge*, HyperOp*>::iterator childItr = hyperOp->ChildMap.begin(); childItr != hyperOp->ChildMap.end(); childItr++) {
		HyperOpEdge* edge = childItr->first;
		HyperOp* consumer = childItr->second;
		errs() << "consumer hop:" << consumer->asString() << " with edge type " << edge->getType() << "\n";

		if (edge->getType() == HyperOpEdge::SCALAR || edge->getType() == HyperOpEdge::LOCAL_REFERENCE || edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS) {
			unsigned registerContainingConsumerBase;
			unsigned targetCE = currentCE;
			if (edge->getType() == HyperOpEdge::SCALAR || edge->getType() == HyperOpEdge::LOCAL_REFERENCE) {
				for (unsigned i = 0; i < ceCount; i++) {
					vector<const Value*> memoryLocationsAccessed = memoryLocationsAccessedInCE[i];
					for (unsigned j = 0; j < memoryLocationsAccessed.size(); j++) {
						if (memoryLocationsAccessed[j] == edge->getValue()) {
							targetCE = i;
							break;
						}
					}
				}
			} else if (edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS && edge->getContextFrameAddress()->getImmediateDominator() == hyperOp) {
				targetCE = registerContainingHyperOpFrameAddressAndCEWithFalloc[edge->getContextFrameAddress()].second;
			}
			if (consumer->getImmediateDominator() != hyperOp) {
				//Just use the physical register that's mapped to the consumer hyperOp
				for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = hyperOp->ParentMap.begin(); parentItr != hyperOp->ParentMap.end(); parentItr++) {
					if (parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS && parentItr->first->getContextFrameAddress() == consumer) {
						//Get the slot to read from which translates to a register anyway
						int contextSlot = parentItr->first->getPositionOfContextSlot();
						registerContainingConsumerBase = REDEFINEphysRegs[contextSlot];
						errs() << "reg thru frame with slot:" << contextSlot << "\n";
//								lastBB.getParent()->addLiveIn(REDEFINEphysRegs[liveInPhysRegisters.size() + contextSlot], TRI->getMinimalPhysRegClass(REDEFINEphysRegs[liveInPhysRegisters.size() + contextSlot]));
//						MachineInstrBuilder copyInstr = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::COPY)).addReg(registerContainingConsumerBase, RegState::Define).addReg(REDEFINEphysRegs[liveInPhysRegisters.size() + contextSlot]);
//						LIS->getSlotIndexes()->insertMachineInstrInMaps(copyInstr.operator llvm::MachineInstr *());
//						allInstructionsOfRegion.push_back(make_pair(copyInstr.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
//
//						lastBB.addLiveIn(REDEFINEphysRegs[liveInPhysRegisters.size() + contextSlot]);
						break;
					}
				}
			}
//			//consumer hyperOp frame address has not been loaded yet, which means that the HyperOp is either static or the current hyperOp is not the creator of the consumer's frame
//			else if (registerContainingHyperOpFrameAddressAndCEWithFalloc.find(consumer) == registerContainingHyperOpFrameAddressAndCEWithFalloc.end() && consumer->isStaticHyperOp()) {
//				//The consumer is static, all we need to do is load the immediate to a new register locally
//				registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
//				MachineInstrBuilder addi = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADDI));
//				addi.addReg(registerContainingConsumerBase, RegState::Define);
//				addi.addReg(REDEFINE::zero);
//				addi.addImm(REDEFINEUtils::getHyperOpId(consumer));
//				if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
//					firstInstructionOfpHyperOpInRegion[targetCE] = addi.operator llvm::MachineInstr *();
//				}
//				allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
//				LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
//				registerContainingHyperOpFrameAddressAndCEWithFalloc.insert(make_pair(consumer, make_pair(registerContainingConsumerBase, targetCE)));
//			}
			else if (registerContainingHyperOpFrameAddressAndCEWithFalloc[consumer].second != targetCE) {
//					&& (consumer->isStaticHyperOp() || consumer->getImmediateDominator() == hyperOp)) {
				unsigned sourceCEContainingFrameAddress = registerContainingHyperOpFrameAddressAndCEWithFalloc[consumer].second;
				//Load the base scratchpad address to a register in the producer CE for the first time
				if (registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE] == -1) {
					MachineInstrBuilder sourceLui = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::LUI));
					unsigned sourceSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
					sourceLui.addImm(targetCE);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(sourceLui.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(sourceCEContainingFrameAddress, insertPosition++)));
					registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE] = (int) sourceSpAddressRegister;
					if (firstInstructionOfpHyperOpInRegion[sourceCEContainingFrameAddress] == 0) {
						firstInstructionOfpHyperOpInRegion[sourceCEContainingFrameAddress] = sourceLui.operator llvm::MachineInstr *();
					}
				}

				//Add a writepm and readpm instruction pair to communicate the address
				MachineInstrBuilder writepm = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::WRITEPM));
				writepm.addReg(registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE]);
				writepm.addReg(registerContainingHyperOpFrameAddressAndCEWithFalloc[consumer].first);
				writepm.addImm(faninOfHyperOp[targetCE]);

				allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(sourceCEContainingFrameAddress, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());

				//Load the base scratchpad address to a register in the consumer CE the first time
				if (registerContainingBaseAddress[targetCE][targetCE] == -1) {
					MachineInstrBuilder targetLui = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::LUI));
					unsigned targetSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					targetLui.addReg(targetSpAddressRegister, RegState::Define);
					//TODO May need changing later
					targetLui.addImm(targetCE);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					registerContainingBaseAddress[targetCE][targetCE] = (int) targetSpAddressRegister;

					if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
						firstInstructionOfpHyperOpInRegion[targetCE] = targetLui.operator llvm::MachineInstr *();
					}
					errs() << "target lui added to " << targetCE << ":";
					targetLui.operator ->()->dump();
				}

				registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder readpm = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::DREADPM));
				readpm.addReg(registerContainingConsumerBase, RegState::Define);
				readpm.addReg(registerContainingBaseAddress[targetCE][targetCE]);
				readpm.addImm(faninOfHyperOp[targetCE]);

				allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
				registerContainingHyperOpFrameAddressAndCEWithFalloc.insert(make_pair(consumer, make_pair(registerContainingConsumerBase, targetCE)));
				faninOfHyperOp[targetCE] = faninOfHyperOp[targetCE] + datawidth;
			} else {
				registerContainingConsumerBase = registerContainingHyperOpFrameAddressAndCEWithFalloc[consumer].first;
			}
			if (edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS) {
				unsigned registerContainingData;
				if (edge->getContextFrameAddress()->getImmediateDominator() == hyperOp) {
					registerContainingData = registerContainingHyperOpFrameAddressAndCEWithFalloc[edge->getContextFrameAddress()].first;
				} else {
					//The address was forwarded to the current HyperOp
					for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = hyperOp->ParentMap.begin(); parentItr != hyperOp->ParentMap.end(); parentItr++) {
						if (parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS && parentItr->first->getContextFrameAddress() == edge->getContextFrameAddress()) {
							//Get the slot to read from which translates to a register anyway
							int contextSlot = parentItr->first->getPositionOfContextSlot();
							registerContainingData = REDEFINEphysRegs[contextSlot];
//									lastBB.getParent()->addLiveIn(REDEFINEphysRegs[liveInPhysRegisters.size() + contextSlot], TRI->getMinimalPhysRegClass(REDEFINEphysRegs[liveInPhysRegisters.size() + contextSlot]));
//							MachineInstrBuilder copyInstr = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::COPY)).addReg(registerContainingData, RegState::Define).addReg(REDEFINEphysRegs[liveInPhysRegisters.size() + contextSlot]);
//							LIS->getSlotIndexes()->insertMachineInstrInMaps(copyInstr.operator llvm::MachineInstr *());
//							allInstructionsOfRegion.push_back(make_pair(copyInstr.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
//
//							lastBB.addLiveIn(REDEFINEphysRegs[liveInPhysRegisters.size() + contextSlot]);
							break;
						}
					}
				}

				errs() << "edge goes from" << hyperOp->asString() << " and " << consumer->asString() << " containing data " << edge->getContextFrameAddress()->asString() << " and position of context slot:" << edge->getPositionOfContextSlot() << "\n";
				errs() << "adding writecm between " << hyperOp->asString() << " and " << consumer->asString() << " with edge position:" << edge->getPositionOfContextSlot() << "\n";
				MachineInstrBuilder writeToContextFrame = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::WRITECM));
				writeToContextFrame.addReg(registerContainingConsumerBase);
				writeToContextFrame.addReg(registerContainingData);
				writeToContextFrame.addImm(edge->getPositionOfContextSlot() * datawidth);
				writeToContextFrame.operator ->()->dump();
				if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
					firstInstructionOfpHyperOpInRegion[targetCE] = writeToContextFrame.operator llvm::MachineInstr *();
				}

				allInstructionsOfRegion.push_back(make_pair(writeToContextFrame.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				//Add instruction to bundle
				LIS->getSlotIndexes()->insertMachineInstrInMaps(writeToContextFrame.operator llvm::MachineInstr *());

				//Add the writecm instructions to the function map so that they can be patched for the right context frame location later
				Function* consumerFunction = consumer->getFunction();
				list<MachineInstr*> writeInstructionsToConsumer;
				if (writeInstrToContextFrame.find(consumerFunction) != writeInstrToContextFrame.end()) {
					writeInstructionsToConsumer = writeInstrToContextFrame.find(consumerFunction)->second;
					writeInstrToContextFrame.erase(writeInstrToContextFrame.find(consumerFunction));
				}

				writeInstructionsToConsumer.push_back(writeToContextFrame.operator ->());
				writeInstrToContextFrame.insert(make_pair(consumerFunction, writeInstructionsToConsumer));
			} else if (edge->getType() == HyperOpEdge::SCALAR) {
				//position is multiplied by 4 since the context memory is byte addressable
				unsigned contextFrameOffset = edge->getPositionOfContextSlot() * datawidth;
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

				//TODO Add a load instruction to get data from memory onto a register; There could be forced schedule edges that we don't want to add load instructions for the same
				unsigned registerContainingData = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				assert(objectIndex != -1 || "Object not allocated memory");
				MachineInstrBuilder loadInstr = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::LW));
				loadInstr.addReg(registerContainingData, RegState::Define);
//				loadInstr.addReg(virtualRegistersForInstAddr[targetCE].second);
				loadInstr.addReg(REDEFINE::t5);
				loadInstr.addFrameIndex(objectIndex);
				allInstructionsOfRegion.push_back(make_pair(loadInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(loadInstr.operator llvm::MachineInstr *());

				if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
					firstInstructionOfpHyperOpInRegion[targetCE] = loadInstr.operator llvm::MachineInstr *();
				}

				MachineInstrBuilder writeToContextFrame = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::WRITECM));
				writeToContextFrame.addReg(registerContainingConsumerBase);
				writeToContextFrame.addReg(registerContainingData);
				writeToContextFrame.addImm(contextFrameOffset);

				allInstructionsOfRegion.push_back(make_pair(writeToContextFrame.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				//Add instruction to bundle
				LIS->getSlotIndexes()->insertMachineInstrInMaps(writeToContextFrame.operator llvm::MachineInstr *());

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
				errs() << "Dealing with local reference edge from " << hyperOp->asString() << " to " << consumer->asString() << "\n";
				unsigned frameLocationOfSourceData = 0;
				Type* dataType;
				AllocaInst* allocInstr;
				if (isa<AllocaInst>(edge->getValue())) {
					allocInstr = (AllocaInst*) (edge->getValue());
					dataType = allocInstr->getAllocatedType();
					//Get the location of the stack allocated object, starting from 0 because negative offsets from fp contain function arguments
					for (unsigned int i = 0; i < MF.getFrameInfo()->getObjectIndexEnd(); i++) {
						if (MF.getFrameInfo()->getObjectAllocation(i) == allocInstr) {
							break;
						}
						frameLocationOfSourceData += MF.getFrameInfo()->getObjectSize(i);
					}
				} else if (isa<LoadInst>(edge->getValue())) {
					//Find the alloca instruction that allocates the memory location in the first place
					unsigned argIndex = 0;
					LoadInst* sourceInstr = (LoadInst*) edge->getValue();
					allocInstr = (AllocaInst*) hyperOp->loadInstrAndAllocaMap[sourceInstr];
					dataType = allocInstr->getType();
					//Get the location of the stack allocated object in the basic block containing the load instruction and not the alloca instruction because alloca might belong
					//Arguments have negative index and are added in memory locations that succeed the locals of the stack frame
					if (MF.getFrameInfo()->getObjectIndexEnd() > 0) {
						for (unsigned i = 0; i < MF.getFrameInfo()->getObjectIndexEnd(); i++) {
							frameLocationOfSourceData += MF.getFrameInfo()->getObjectSize(i);
						}
					}

					for (int i = MF.getFrameInfo()->getObjectIndexBegin(); i < 0; i++) {
						if (MF.getFrameInfo()->getObjectAllocation(i) == sourceInstr->getOperand(0)) {
							break;
						}
						frameLocationOfSourceData += MF.getFrameInfo()->getObjectSize(i);
					}
				}
				//Compute frame objects' size
				Function* consumerFunction = consumer->getFunction();
				unsigned frameLocationOfTargetData = 0;
				for (Function::iterator funcItr = consumerFunction->begin(); funcItr != consumerFunction->end(); funcItr++) {
					for (BasicBlock::iterator bbItr = funcItr->begin(); bbItr != funcItr->end(); bbItr++) {
						if (isa<AllocaInst>(bbItr)) {
							AllocaInst* targetAllocaInst = cast<AllocaInst>(bbItr);
							frameLocationOfTargetData += REDEFINEUtils::getSizeOfType(targetAllocaInst->getAllocatedType());
						}
					}
				}
				int beginArgIndex = 0;
				for (Function::arg_iterator funcArgItr = consumerFunction->arg_begin(); funcArgItr != consumerFunction->arg_end(); funcArgItr++, beginArgIndex++) {
					if (beginArgIndex == edge->getPositionOfContextSlot()) {
						break;
					}
					Argument* argument = &*funcArgItr;
					if (!consumerFunction->getAttributes().hasAttribute(beginArgIndex + 1, Attribute::InReg)) {
						frameLocationOfTargetData += REDEFINEUtils::getSizeOfType(funcArgItr->getType());
					}
				}

				//Map of primitive data types and their memory locations
				list<pair<Type*, unsigned> > primitiveTypesMap;
				list<Type*> containedTypesForTraversal;
				containedTypesForTraversal.push_front(dataType);
				unsigned memoryOfType = 0;
				//Find the primitive types of allocatedDataType
				while (!containedTypesForTraversal.empty()) {
					Type* traversingType = containedTypesForTraversal.front();
					containedTypesForTraversal.pop_front();
					if (!traversingType->isAggregateType()) {
						primitiveTypesMap.push_back(make_pair(traversingType, memoryOfType));
						memoryOfType += traversingType->getPrimitiveSizeInBits() / 8;
					} else {
						for (unsigned i = 0; i < traversingType->getNumContainedTypes(); i++) {
							containedTypesForTraversal.push_back(traversingType->getContainedType(i));
						}
					}
				}

				if (regContainingMemFrameBaseAddress.find(consumer) == regContainingMemFrameBaseAddress.end()) {
					unsigned crId = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder shiftForCRId = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(crId, RegState::Define).addReg(registerContainingConsumerBase).addImm(21);

					allInstructionsOfRegion.push_back(make_pair(shiftForCRId.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForCRId.operator llvm::MachineInstr *());

					if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
						firstInstructionOfpHyperOpInRegion[targetCE] = shiftForCRId.operator llvm::MachineInstr *();
					}
					unsigned shiftedPageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder shiftForPageNumber = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(shiftedPageNumber, RegState::Define).addReg(registerContainingConsumerBase).addImm(11);
					allInstructionsOfRegion.push_back(make_pair(shiftForPageNumber.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForPageNumber.operator llvm::MachineInstr *());
					unsigned pageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder bitmaskForPageNumber = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ANDI)).addReg(pageNumber, RegState::Define).addReg(shiftedPageNumber).addImm(15);
					allInstructionsOfRegion.push_back(make_pair(bitmaskForPageNumber.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(bitmaskForPageNumber.operator llvm::MachineInstr *());

					unsigned shiftedFrameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder shiftForFrameNumber = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(shiftedFrameNumber, RegState::Define).addReg(registerContainingConsumerBase).addImm(6);
					allInstructionsOfRegion.push_back(make_pair(shiftForFrameNumber.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForFrameNumber.operator llvm::MachineInstr *());
					unsigned frameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder bitmaskForFrameNumber = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ANDI)).addReg(frameNumber, RegState::Define).addReg(shiftedFrameNumber).addImm(63);
					allInstructionsOfRegion.push_back(make_pair(bitmaskForFrameNumber.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(bitmaskForFrameNumber.operator llvm::MachineInstr *());

					unsigned loadImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder mulOperandForCRBase = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(loadImmInReg, RegState::Define).addReg(crId).addImm(800);
					allInstructionsOfRegion.push_back(make_pair(mulOperandForCRBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(mulOperandForCRBase.operator llvm::MachineInstr *());

					unsigned crBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder mulForCRBase = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(crBase, RegState::Define).addReg(crId).addReg(loadImmInReg);
					allInstructionsOfRegion.push_back(make_pair(mulForCRBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(mulForCRBase.operator llvm::MachineInstr *());

					unsigned loadPageImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder mulOperandForPageBase = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(loadPageImmInReg, RegState::Define).addReg(crId).addImm(52);
					allInstructionsOfRegion.push_back(make_pair(mulOperandForPageBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(mulOperandForPageBase.operator llvm::MachineInstr *());

					unsigned pageBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder mulForPageBase = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(pageBase, RegState::Define).addReg(pageNumber).addReg(loadPageImmInReg);
					allInstructionsOfRegion.push_back(make_pair(mulForPageBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(mulForPageBase.operator llvm::MachineInstr *());

					unsigned frameBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder addForFrameBase = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(frameBase, RegState::Define).addReg(pageBase).addReg(frameNumber);
					allInstructionsOfRegion.push_back(make_pair(addForFrameBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(addForFrameBase.operator llvm::MachineInstr *());

					unsigned crIdBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder addForCRBase = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(crIdBase, RegState::Define).addReg(frameBase).addReg(crBase);
					allInstructionsOfRegion.push_back(make_pair(addForCRBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(addForCRBase.operator llvm::MachineInstr *());

					unsigned mulFrameSize = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder mulFrameSizeInstr = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(mulFrameSize, RegState::Define).addReg(crIdBase).addReg(memoryFrameMaxSizeReg[targetCE]);
					allInstructionsOfRegion.push_back(make_pair(mulFrameSizeInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(mulFrameSizeInstr.operator llvm::MachineInstr *());

					unsigned addFrameSize = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder addFrameSizeInstr = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(addFrameSize, RegState::Define).addReg(mulFrameSize).addReg(memoryFrameBaseAddress[targetCE]);
					allInstructionsOfRegion.push_back(make_pair(addFrameSizeInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(addFrameSizeInstr.operator llvm::MachineInstr *());
					regContainingMemFrameBaseAddress[consumer] = addFrameSize;
				}

				unsigned registerContainingMemBase = regContainingMemFrameBaseAddress[consumer];
				for (unsigned allocatedDataIndex = 0; allocatedDataIndex != ((ConstantInt*) allocInstr->getArraySize())->getZExtValue(); allocatedDataIndex++) {
					//Add a load instruction from memory and store to the memory frame of the consumer HyperOp
					for (list<pair<Type*, unsigned> >::iterator containedPrimitiveItr = primitiveTypesMap.begin(); containedPrimitiveItr != primitiveTypesMap.end(); containedPrimitiveItr++) {
						Type* containedType = containedPrimitiveItr->first;
						if (containedType->isFloatTy()) {
							unsigned floatingPointRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::f32);
							MachineInstrBuilder lw = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::FLW)).addReg(floatingPointRegister, RegState::Define);
//							lw.addReg(virtualRegistersForInstAddr[currentCE].second);
							lw.addReg(virtualRegistersForInstAddr[targetCE].second);
							lw.addImm(allocatedDataIndex * memoryOfType + containedPrimitiveItr->second + frameLocationOfSourceData);
							allInstructionsOfRegion.push_back(make_pair(lw.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
							LIS->getSlotIndexes()->insertMachineInstrInMaps(lw.operator llvm::MachineInstr *());

							if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
								firstInstructionOfpHyperOpInRegion[targetCE] = lw.operator llvm::MachineInstr *();
							}

							MachineInstrBuilder store = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::FSW));
							store.addReg(floatingPointRegister).addReg(registerContainingMemBase).addImm(allocatedDataIndex * memoryOfType + containedPrimitiveItr->second + frameLocationOfTargetData);

							allInstructionsOfRegion.push_back(make_pair(store.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
							LIS->getSlotIndexes()->insertMachineInstrInMaps(store.operator llvm::MachineInstr *());
						} else {
							unsigned integerRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
							MachineInstrBuilder lw = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::LW));
							lw.addReg(integerRegister, RegState::Define);
//							lw.addReg(virtualRegistersForInstAddr[currentCE].second);
							lw.addReg(REDEFINE::t5);

							lw.addImm(allocatedDataIndex * memoryOfType + containedPrimitiveItr->second + frameLocationOfSourceData);
							allInstructionsOfRegion.push_back(make_pair(lw.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
							LIS->getSlotIndexes()->insertMachineInstrInMaps(lw.operator llvm::MachineInstr *());

							if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
								firstInstructionOfpHyperOpInRegion[targetCE] = lw.operator llvm::MachineInstr *();
							}

							MachineInstrBuilder store = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::SW));
							store.addReg(integerRegister).addReg(registerContainingMemBase).addImm(allocatedDataIndex * memoryOfType + containedPrimitiveItr->second + frameLocationOfTargetData);

							allInstructionsOfRegion.push_back(make_pair(store.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
							LIS->getSlotIndexes()->insertMachineInstrInMaps(store.operator llvm::MachineInstr *());
						}
					}
				}
			}
		}
	}

	DEBUG(dbgs() << "Adding writecmp and sync instructions\n");
//Writecmp instructions are added after writecm and local reference writes to ensure that local reference writes do go through before a consumer is predicated for execution
	for (map<HyperOpEdge*, HyperOp*>::iterator childItr = hyperOp->ChildMap.begin(); childItr != hyperOp->ChildMap.end(); childItr++) {
		HyperOpEdge* edge = childItr->first;
		HyperOp* consumer = childItr->second;
		unsigned registerContainingConsumerBase;
		unsigned targetCE = currentCE;

		for (unsigned i = 0; i < ceCount; i++) {
			vector<const Value*> memoryLocationsAccessed = memoryLocationsAccessedInCE[i];
			for (unsigned j = 0; j < memoryLocationsAccessed.size(); j++) {
				if (memoryLocationsAccessed[j] == edge->getValue()) {
					targetCE = i;
					break;
				}
			}
		}
		if (edge->getType() == HyperOpEdge::PREDICATE || edge->getType() == HyperOpEdge::SYNC || edge->getType() == HyperOpEdge::ORDERING) {
//			if (!consumer->isStaticHyperOp() &&
			if (consumer->getImmediateDominator() != hyperOp) {
				//Just use the physical register that's mapped to the consumer hyperOp
				for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = hyperOp->ParentMap.begin(); parentItr != hyperOp->ParentMap.end(); parentItr++) {
					if (parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS && parentItr->first->getContextFrameAddress() == consumer) {
						//Get the slot to read from which translates to a register anyway
						int contextSlot = parentItr->first->getPositionOfContextSlot();
						registerContainingConsumerBase = REDEFINEphysRegs[contextSlot];
//								lastBB.getParent()->addLiveIn(REDEFINEphysRegs[liveInPhysRegisters.size() + contextSlot], TRI->getMinimalPhysRegClass(REDEFINEphysRegs[liveInPhysRegisters.size() + contextSlot]));
//						MachineInstrBuilder copyInstr = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::COPY)).addReg(registerContainingConsumerBase, RegState::Define).addReg(REDEFINEphysRegs[liveInPhysRegisters.size() + contextSlot]);
//						LIS->getSlotIndexes()->insertMachineInstrInMaps(copyInstr.operator llvm::MachineInstr *());
//						allInstructionsOfRegion.push_back(make_pair(copyInstr.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
//
//						lastBB.addLiveIn(REDEFINEphysRegs[liveInPhysRegisters.size() + contextSlot]);
						break;
					}
				}
//			}
//			//consumer hyperOp frame address has not been loaded yet, which means that the HyperOp is either static or the current hyperOp is not the creator of the consumer's frame
//			else if (registerContainingHyperOpFrameAddressAndCEWithFalloc.find(consumer) == registerContainingHyperOpFrameAddressAndCEWithFalloc.end() && consumer->isStaticHyperOp()) {
//				//The consumer is static, all we need to do is load the immediate to a new register locally
//				registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
//				MachineInstrBuilder addi = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADDI));
//				addi.addReg(registerContainingConsumerBase, RegState::Define);
//				addi.addReg(REDEFINE::zero);
//				addi.addImm(REDEFINEUtils::getHyperOpId(consumer));
//				if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
//					firstInstructionOfpHyperOpInRegion[targetCE] = addi.operator llvm::MachineInstr *();
//				}
//				allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
//				LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
//				registerContainingHyperOpFrameAddressAndCEWithFalloc.insert(make_pair(consumer, make_pair(registerContainingConsumerBase, targetCE)));
			} else if (registerContainingHyperOpFrameAddressAndCEWithFalloc[consumer].second != targetCE) {
//					&& (consumer->isStaticHyperOp() || consumer->getImmediateDominator() == hyperOp)) {
				unsigned sourceCEContainingFrameAddress = registerContainingHyperOpFrameAddressAndCEWithFalloc[consumer].second;
				//Load the base scratchpad address to a register in the producer CE for the first time
				if (registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE] == -1) {
					MachineInstrBuilder sourceLui = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::LUI));
					unsigned sourceSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
					sourceLui.addImm(targetCE);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(sourceLui.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(sourceCEContainingFrameAddress, insertPosition++)));
					registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE] = (int) sourceSpAddressRegister;
					if (firstInstructionOfpHyperOpInRegion[sourceCEContainingFrameAddress] == 0) {
						firstInstructionOfpHyperOpInRegion[sourceCEContainingFrameAddress] = sourceLui.operator ->();
					}
				}

				//Add a writepm and readpm instruction pair to communicate the address
				MachineInstrBuilder writepm = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::WRITEPM));
				writepm.addReg(registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE]);
				writepm.addReg(registerContainingHyperOpFrameAddressAndCEWithFalloc[consumer].first);
				writepm.addImm(faninOfHyperOp[targetCE]);
				LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());
				allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(sourceCEContainingFrameAddress, insertPosition++)));

				//Load the base scratchpad address to a register in the consumer CE the first time
				if (registerContainingBaseAddress[targetCE][targetCE] == -1) {
					unsigned targetSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder targetLui = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::LUI));
					targetLui.addReg(targetSpAddressRegister, RegState::Define);
					//TODO May need changing later
					targetLui.addImm(targetCE);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					registerContainingBaseAddress[targetCE][targetCE] = (int) targetSpAddressRegister;

					if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
						firstInstructionOfpHyperOpInRegion[targetCE] = targetLui.operator ->();
					}
				}

				registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder readpm = BuildMI(lastBB, lastInstruction, location, TII->get(REDEFINE::DREADPM));
				readpm.addReg(registerContainingConsumerBase, RegState::Define);
				readpm.addReg(registerContainingBaseAddress[targetCE][targetCE]);
				readpm.addImm(faninOfHyperOp[targetCE]);
				allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
				LIS->getOrCreateInterval(registerContainingConsumerBase);
				LIS->addLiveRangeToEndOfBlock(registerContainingConsumerBase, readpm.operator ->());

				faninOfHyperOp[targetCE] = faninOfHyperOp[targetCE] + datawidth;
			} else {
				registerContainingConsumerBase = registerContainingHyperOpFrameAddressAndCEWithFalloc[consumer].first;
			}

			if (edge->getType() == HyperOpEdge::PREDICATE || edge->getType() == HyperOpEdge::ORDERING) {
				//predicate can take any offset wrt context frame base, it does not have a dedicated slot
				unsigned contextFrameOffset = 0;

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

				//TODO Add a load instruction to get data from memory onto a register; There could be forced schedule edges that we don't want to add load instructions for the same
				unsigned registerContainingData;
				if (objectIndex != -1) {
					unsigned registerContainingPredicateData = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);

					MachineInstrBuilder loadInstr = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::LW));
					loadInstr.addReg(registerContainingPredicateData, RegState::Define);
					loadInstr.addReg(REDEFINE::t5);
					loadInstr.addFrameIndex(objectIndex);
					if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
						firstInstructionOfpHyperOpInRegion[targetCE] = loadInstr.operator llvm::MachineInstr *();
					}
					allInstructionsOfRegion.push_back(make_pair(loadInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(loadInstr.operator llvm::MachineInstr *());

					if (edge->getPredicateValue()) {
						registerContainingData = registerContainingPredicateData;
					} else {
						registerContainingData = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder sltiForPredicate = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::SLTIU));
						sltiForPredicate.addReg(registerContainingData, RegState::Define);
						sltiForPredicate.addReg(registerContainingPredicateData);
						//check use of sltui to find out why inverse of predicate is used
						sltiForPredicate.addImm(!edge->getPredicateValue());

						allInstructionsOfRegion.push_back(make_pair(sltiForPredicate.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(sltiForPredicate.operator llvm::MachineInstr *());
					}
				}
				//No need to access memory for a true predicate, copying via addi will do
				else {
					MachineInstrBuilder addi = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADDI));
					addi.addReg(registerContainingData, RegState::Define);
					addi.addReg(REDEFINE::zero);
					addi.addImm(1);
					if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
						firstInstructionOfpHyperOpInRegion[targetCE] = addi.operator llvm::MachineInstr *();
					}
					allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
				}
				MachineInstrBuilder writeToContextFrame = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::WRITECMP));
				writeToContextFrame.addReg(registerContainingConsumerBase);
				writeToContextFrame.addReg(registerContainingData);
				writeToContextFrame.addImm(contextFrameOffset);

				allInstructionsOfRegion.push_back(make_pair(writeToContextFrame.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				//Add instruction to bundle
				LIS->getSlotIndexes()->insertMachineInstrInMaps(writeToContextFrame.operator llvm::MachineInstr *());
			} else if (edge->getType() == HyperOpEdge::SYNC) {
				//sync always goes to the 15th slot
				unsigned contextFrameOffset = 60;

				//decrement the barrier by 1
				unsigned registerContainingData = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder addi = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::ADDI));
				addi.addReg(registerContainingData, RegState::Define);
				addi.addReg(REDEFINE::zero);
				int8_t imm = -1;
				addi.addImm(SignExtend8BitNumberTo12Bits(imm));
				if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
					firstInstructionOfpHyperOpInRegion[targetCE] = addi.operator llvm::MachineInstr *();
				}
				allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
				MachineInstrBuilder syncInstruction = BuildMI(lastBB, lastInstruction, lastInstruction->getDebugLoc(), TII->get(REDEFINE::SYNC));
				syncInstruction.addReg(registerContainingConsumerBase);
				syncInstruction.addReg(registerContainingData);
				syncInstruction.addImm(contextFrameOffset);

				allInstructionsOfRegion.push_back(make_pair(syncInstruction.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(syncInstruction.operator llvm::MachineInstr *());
			}
		}
	}
	for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator allInstructionItr = allInstructionsOfRegion.begin(); allInstructionItr != allInstructionsOfRegion.end(); allInstructionItr++) {
		MachineInstr* instruction = allInstructionItr->first;
		unsigned ce = allInstructionItr->second.first;
		unsigned position = allInstructionItr->second.second;
		instruction->setFlag(MachineInstr::NoFlags);
		unsigned i = ce + 1;
		MachineInstr* nextCeInstruction;
		if (ce < ceCount - 1) {
			while (i <= ceCount - 1) {
				nextCeInstruction = firstInstructionOfpHyperOpInRegion[i++];
				if (nextCeInstruction != BB->end() && nextCeInstruction != 0) {
					break;
				}
			}
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

	for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator instItr = allInstructionsOfRegion.begin(); instItr != allInstructionsOfRegion.end(); instItr++) {
		allInstructionsOfBB.push_back(*instItr);
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
		allInstructionsOfBB.push_back(make_pair(endInstruction.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

		for (unsigned j = 0; j < 2; j++) {
			MachineInstrBuilder nopInstruction = BuildMI(*BB, BB->end(), BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI));
			nopInstruction.addReg(REDEFINE::zero, RegState::Define);
			nopInstruction.addReg(REDEFINE::zero);
			nopInstruction.addImm(0);
			LIS->getSlotIndexes()->insertMachineInstrInMaps(nopInstruction.operator llvm::MachineInstr *());
			allInstructionsOfBB.push_back(make_pair(nopInstruction.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		}
	}
}

map<unsigned, pair<MachineInstr*, unsigned> > firstInstructionInCE;
unsigned currentStartPosition = 0;
list<pair<MachineInstr*, pair<unsigned, unsigned> > > currentPositionMap;
for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator allInstructionItr = allInstructionsOfBB.begin(); allInstructionItr != allInstructionsOfBB.end(); allInstructionItr++, currentStartPosition++) {
	unsigned ce = allInstructionItr->second.first;
	if (firstInstructionInCE.find(ce) == firstInstructionInCE.end()) {
		firstInstructionInCE.insert(make_pair(ce, make_pair(allInstructionItr->first, currentStartPosition)));
		errs() << "first instruction in ce " << ce << ":";
		allInstructionItr->first->dump();
	}
	errs() << "instr in ce " << ce << ":";
	allInstructionItr->first->dump();
	currentPositionMap.push_back(make_pair(allInstructionItr->first, make_pair(allInstructionItr->second.first, currentStartPosition)));
}

for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator allInstructionItr = currentPositionMap.begin(); allInstructionItr != currentPositionMap.end(); allInstructionItr++) {
	MachineInstr* instruction = allInstructionItr->first;
	unsigned ce = allInstructionItr->second.first;
	unsigned position = allInstructionItr->second.second;
	instruction->setFlag(MachineInstr::NoFlags);
	MachineInstr* nextCeInstruction;
	//Find the smallest position among the first instructions of successive ces
	if (ce < ceCount - 1) {
		int i = ce + 1;
		int minNextCEPosition = currentPositionMap.size();
		while (i < ceCount) {
			unsigned nextPosition = firstInstructionInCE[i].second;
			if (nextPosition < minNextCEPosition) {
				minNextCEPosition = nextPosition;
				nextCeInstruction = firstInstructionInCE[i].first;
			}
			i++;
		}
		if (minNextCEPosition < position) {
			BB->splice(nextCeInstruction, BB, instruction);
		}
	}
}

DEBUG(dbgs() << "After Shuffling regions of basic block, state of BB#" << BB->getNumber() << ":\n");
BB->print(dbgs());

list<unsigned> liveInPhysicalRegs;
for (MachineRegisterInfo::livein_iterator liveInItr = MF.getRegInfo().livein_begin(); liveInItr != MF.getRegInfo().livein_end(); liveInItr++) {
	liveInPhysicalRegs.push_back(liveInItr->first);
}

for (unsigned i = 0; i < 16; i++) {
	liveInPhysicalRegs.push_back(REDEFINEphysRegs[i]);
}

//additional live-in regs in context memory also need to be added
//ACtually we don't need to use inst_itr here cos bundles are created after this, but leaving this for now
for (MachineBasicBlock::instr_iterator instItr = BB->instr_begin(); instItr != BB->instr_end(); instItr++) {
	for (unsigned i = 0; i < instItr->getNumOperands(); i++) {
		MachineOperand& operand = instItr->getOperand(i);
		if (operand.isReg() && find(liveInPhysicalRegs.begin(), liveInPhysicalRegs.end(), operand.getReg()) == liveInPhysicalRegs.end() && find(registersUsedInBB.begin(), registersUsedInBB.end(), operand.getReg()) == registersUsedInBB.end()) {
			unsigned operandRegister = operand.getReg();
			bool ignore = false;
			for (unsigned i = 0; i < ceCount; i++) {
				if (virtualRegistersForInstAddr[i].first == operandRegister || virtualRegistersForInstAddr[i].second == operandRegister) {
					ignore = true;
					break;
				}
			}
			if (!ignore) {
				registersUsedInBB.push_back(operand.getReg());
			}
		}
	}
}
LIS->repairIntervalsInRange(BB, BB->begin(), BB->end(), registersUsedInBB);
//Create instruction bundles corresponding to pHyperOps
if (!firstInstructionInCE.empty()) {
	DEBUG(dbgs() << "Creating pHyperOp bundles for CEs\n");
	for (unsigned i = 0; i < ceCount; i++) {
		MachineInstr* firstInstructionofCE = firstInstructionInCE[i].first;
		MachineInstr* firstInstructionInNextCE = 0;
		if (i < ceCount - 1) {
			firstInstructionInNextCE = firstInstructionInCE[i + 1].first;
		}
		if (firstInstructionInNextCE == 0) {
			firstInstructionInNextCE = BB->end();
		}

		if (firstInstructionofCE == BB->end() || firstInstructionofCE == 0) {
//			continue;
			//pHyperOp is empty, add a nop
			MachineInstrBuilder nopInstruction = BuildMI(*BB, firstInstructionInNextCE, BB->front().getDebugLoc(), TII->get(REDEFINE::ADDI));
			nopInstruction.addReg(REDEFINE::zero, RegState::Define);
			nopInstruction.addReg(REDEFINE::zero);
			nopInstruction.addImm(0);
			LIS->getSlotIndexes()->insertMachineInstrInMaps(nopInstruction.operator llvm::MachineInstr *());
			firstInstructionofCE = nopInstruction.operator ->();
		}

		if (firstInstructionInNextCE != BB->end()) {
			firstInstructionInNextCE->dump();
		}
		MIBundleBuilder* bundleBuilder = new MIBundleBuilder(*BB, firstInstructionofCE, firstInstructionInNextCE);
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

HyperOpInteractionGraph * graph = ((REDEFINETargetMachine&) TM).HIG;
HyperOp* currentHyperOp = graph->getHyperOp(const_cast<Function*>(MF.getFunction()));
currentHyperOp->setNumCEs(ceCount);

map<unsigned, MachineInstr*> physRegAndFirstMachineOperand;
map<unsigned, list<unsigned> > ceAndLiveInPhysicalRegMap;
list<unsigned> liveInPhysRegisters;

for (MachineRegisterInfo::livein_iterator liveInItr = MF.getRegInfo().livein_begin(); liveInItr != MF.getRegInfo().livein_end(); liveInItr++) {
	liveInPhysRegisters.push_back(liveInItr->first);
}
//for (map<HyperOpEdge*, HyperOp*>::iterator contextMemItr = currentHyperOp->ParentMap.begin(); contextMemItr != currentHyperOp->ParentMap.end(); contextMemItr++) {
//	if (contextMemItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS) {
//		liveInPhysRegisters.push_back(REDEFINEphysRegs[contextMemItr->first->getPositionOfContextSlot()]);
//	}
//}

//Get the live-in registers and map to the ce to which they belong
for (MachineFunction::iterator MBBI = MF.begin(), MBBE = MF.end(); MBBI != MBBE; ++MBBI) {
	int pHyperOpIndex = -1;
	for (MachineBasicBlock::instr_iterator MII = MBBI->instr_begin(); MII != MBBI->instr_end(); ++MII) {
		MachineInstr *MI = MII;
		if (!MI->isInsideBundle()) {
			pHyperOpIndex++;
		}
//We only need to worry about copy instructions
		//Get the copy instruction's source register
		for (unsigned i = 0; i < MI->getNumOperands(); i++) {
			MachineOperand &MO = MI->getOperand(i);
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

unsigned contextFrameLocation = 0;
for (unsigned ceIndex = 0; ceIndex < ceCount; ceIndex++) {
	if (ceAndLiveInPhysicalRegMap.find(ceIndex) != ceAndLiveInPhysicalRegMap.end()) {
		list<unsigned> liveInRegs = ceAndLiveInPhysicalRegMap[ceIndex];
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

DEBUG(dbgs() << "Patching writecm instructions to shuffle physical registers of function " << MF.getFunction()->getName() << "\n");
//Modify the writecm instructions corresponding to writes to the current MachineFunction's context frame
//TODO This assumes that the producer HyperOps have already been dealt with and all necessary writecm instructions have been added
if (writeInstrToContextFrame.find(const_cast<Function*>(MF.getFunction())) != writeInstrToContextFrame.end()) {
	list<MachineInstr*> writeInstrToBePatched = this->writeInstrToContextFrame.find(const_cast<Function*>(MF.getFunction()))->second;
	errs() << "number of writes to be patched that write to " << MF.getFunction()->getName() << ":" << writeInstrToBePatched.size() << "\n";
	for (list<MachineInstr*>::iterator writeInstItr = writeInstrToBePatched.begin(); writeInstItr != writeInstrToBePatched.end(); writeInstItr++) {
		MachineInstr* writeInst = *writeInstItr;
		errs() << "patching write:";
		writeInst->dump();
//Replace the immediate offset of write instruction that corresponds to the context frame slot being written into
		unsigned previousFrameSlot = writeInst->getOperand(2).getImm();
		writeInst->getOperand(2).setImm(physRegAndContextFrameSlot[contextFrameSlotAndPhysReg[previousFrameSlot]]);
		errs() << "previous slot:" << previousFrameSlot << " swapped to " << physRegAndContextFrameSlot[contextFrameSlotAndPhysReg[previousFrameSlot]] << "\n";
	}
}

DEBUG(dbgs() << "Updating edges from parent nodes to shuffle the physical registers of function " << MF.getFunction()->getName() << "\n");
//Shuffle context frame slots for the HyperOp in case the parent hyperop hasn't been processed yet
map<HyperOpEdge*, HyperOp*> parentMap = graph->getHyperOp(const_cast<Function*>(MF.getFunction()))->ParentMap;
for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = parentMap.begin(); parentItr != parentMap.end(); parentItr++) {
	HyperOpEdge* edge = parentItr->first;
	if (edge->getType() == HyperOpEdge::SCALAR || edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS) {
		unsigned previousPhysReg = contextFrameSlotAndPhysReg[edge->getPositionOfContextSlot()];
		errs() << "updating the edge from " << parentItr->second->asString() << " to " << currentHyperOp->asString() << " that used to point to " << edge->getPositionOfContextSlot() << "to" << physRegAndContextFrameSlot[previousPhysReg] << " because of the use of phys reg " << previousPhysReg
				<< "\n";
		edge->setPositionOfContextSlot(physRegAndContextFrameSlot[previousPhysReg]);
	}
}

/*
 * Since REDEFINE doesn't allow replication of context memory inputs to multiple CEs, after the first CE that uses a live-in register corresponding to a HyperOp input is encountered,
 * if another CE requires the same input, it gets the input from scratch pad.
 */
//TODO Make necessary changes to check if the operand has been replicated atleast once
DEBUG(dbgs() << "Replicating HyperOp inputs\n");
map<unsigned, map<unsigned, unsigned> > replicatedRegInCE;
for (MachineFunction::iterator MBBI = MF.begin(), MBBE = MF.end(); MBBI != MBBE; ++MBBI) {
	int pHyperOpIndex = -1;
	for (MachineBasicBlock::instr_iterator MII = MBBI->instr_begin(); MII != MBBI->instr_end(); ++MII) {
		MachineInstr *MI = MII;
//First instruction in basic block
		if (!MI->isInsideBundle()) {
			//New pHyperOp
			pHyperOpIndex++;
		}
		for (unsigned i = 0; i < MI->getNumOperands(); i++) {
			MachineOperand &MO = MI->getOperand(i);
			if (MO.isReg() && find(liveInPhysRegisters.begin(), liveInPhysRegisters.end(), MO.getReg()) != liveInPhysRegisters.end()) {
				if (find(liveInPhysRegisters.begin(), liveInPhysRegisters.end(), MO.getReg()) != liveInPhysRegisters.end()) {
					unsigned physicalReg = MO.getReg();
					if ((physRegAndLiveIn[physicalReg] == -1 || physRegAndLiveIn[physicalReg] != pHyperOpIndex) && (replicatedRegInCE.find(pHyperOpIndex) == replicatedRegInCE.end() || replicatedRegInCE[pHyperOpIndex].find(physicalReg) == replicatedRegInCE[pHyperOpIndex].end())) {
						MachineInstr* firstDefinition = physRegAndFirstMachineOperand.find(physicalReg)->second;
						unsigned pHyperOpContainingDefinition = physRegAndLiveIn[physicalReg];
						MachineInstrBuilder sourceLui;
						if (registerContainingBaseAddress[pHyperOpContainingDefinition][pHyperOpIndex] == -1) {
							sourceLui = BuildMI(*(firstDefinition->getParent()), firstDefinition, firstDefinition->getDebugLoc(), TII->get(REDEFINE::LUI));
							unsigned sourceSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
							sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
							//TODO May need changing later
							sourceLui.addImm(pHyperOpIndex);
							registerContainingBaseAddress[pHyperOpContainingDefinition][pHyperOpIndex] = (int) sourceSpAddressRegister;
						}

						MachineInstrBuilder writepm = BuildMI(*(firstDefinition->getParent()), firstDefinition, firstDefinition->getDebugLoc(), TII->get(REDEFINE::WRITEPM));
						writepm.addReg(registerContainingBaseAddress[pHyperOpContainingDefinition][pHyperOpIndex]);
						writepm.addReg(physicalReg);
						//TODO
						writepm.addImm(SPLOCATIONS - (physRegAndContextFrameSlot[physicalReg] * datawidth));

						MachineInstrBuilder targetLui;
						//Load the base scratchpad address to a register in the consumer CE the first time
						if (registerContainingBaseAddress[pHyperOpIndex][pHyperOpIndex] == -1) {
							targetLui = BuildMI(*MBBI, MI, MI->getDebugLoc(), TII->get(REDEFINE::LUI));
							unsigned targetSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
							targetLui.addReg(targetSpAddressRegister, RegState::Define);
							//TODO May need changing later
							targetLui.addImm(pHyperOpIndex);

//						LIS->getOrCreateInterval(targetSpAddressRegister);
							registerContainingBaseAddress[pHyperOpIndex][pHyperOpIndex] = (int) targetSpAddressRegister;
						}
						unsigned readpmTarget = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder readpm = BuildMI(*MBBI, MI, MI->getDebugLoc(), TII->get(REDEFINE::DREADPM));
						readpm.addReg(readpmTarget, RegState::Define);
						readpm.addReg(registerContainingBaseAddress[pHyperOpIndex][pHyperOpIndex]);
						//TODO
						readpm.addImm(SPLOCATIONS - (physRegAndContextFrameSlot[physicalReg] + datawidth));
						MO.setReg(readpmTarget);

						replicatedRegInCE[pHyperOpIndex][physicalReg] = readpmTarget;
						LIS->getOrCreateInterval(readpmTarget);
						LIS->addLiveRangeToEndOfBlock(readpmTarget, readpm);
					} else if (physRegAndLiveIn[physicalReg] != pHyperOpIndex) {
						MO.setReg(replicatedRegInCE[pHyperOpIndex][physicalReg]);
					}
				}
			}
		}
	}
}

DEBUG(dbgs() << "Patching the instructions that are supposed to use the physical registers r30 and r31\n");
for (MachineFunction::iterator bbItr = MF.begin(); bbItr != MF.end(); bbItr++) {
	int ceIndex = -1;
	for (MachineBasicBlock::instr_iterator instrItr = bbItr->instr_begin(); instrItr != bbItr->instr_end(); instrItr++) {
//TODO does a bundle start not bundle with succ?
		if (!instrItr->isInsideBundle()) {
			ceIndex++;
		}
		for (unsigned i = 0; i < instrItr->getNumOperands(); i++) {
			if (instrItr->getOperand(i).isReg()) {
				if (virtualRegistersForInstAddr[ceIndex].second == instrItr->getOperand(i).getReg()) {
					instrItr->getOperand(i).setReg(REDEFINE::t5);
				} else if (virtualRegistersForInstAddr[ceIndex].first == instrItr->getOperand(i).getReg()) {
					instrItr->getOperand(i).setReg(REDEFINE::t4);
				}
			}
		}
	}
}

DEBUG(dbgs() << "Final state of Machine Function:");
MF.print(dbgs());
}
