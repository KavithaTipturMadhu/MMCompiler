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
#include "llvm/IR/REDEFINEUtils.h"
using namespace llvm;
using namespace std;

static bool isRegDependence(SDep dependence) {
	SDep::Kind dependenceKind = dependence.getKind();
	if (dependenceKind == SDep::Data || dependenceKind == SDep::Anti || dependenceKind == SDep::Output) {
		return true;
	}
	return false;
}

static inline void addToLISSlot(LiveIntervals *LIS, MachineInstr* instr) {
	if(!LIS->getSlotIndexes()->hasIndex(instr)){
		LIS->getSlotIndexes()->insertMachineInstrInMaps(instr);
	}
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
//Create a phop tree with SUnits
//Map of each SUnit and its parents
	list<SUnit*> leafNodes;
	list<SUnit*> orderOfTraversal;
	while (SUnit *SU = SchedImpl->pickNode(IsTopNode)) {
		scheduleMI(SU, IsTopNode);
		if (SU->Succs.empty()) {
			leafNodes.push_back(SU);
		}
		orderOfTraversal.push_back(SU);
		updateQueues(SU, IsTopNode);
	}

	unsigned ceIndex = 0;
	list<SUnit*> previouslyScheduledList;
	map<SUnit*, unsigned> instructionAndCEMap;
//Traverse the tree depth first
	while (!leafNodes.empty()) {
		SUnit* SU = leafNodes.front();
		leafNodes.pop_front();
//		METIS TODO: traversedSUnitsWithIndex[SU] = numVertices;
//		METIS TODO:  numVertices++;
		if (ceCount == 1) {
			instructionAndCEMap[SU] = 0;
		} else {
			list<unsigned> sourceCEList;
			list<SUnit*> parentInstructions;
			list<SUnit*> childInstructions;
			int sunitCEIndex = -1;
			if (instructionAndCEMap.find(SU) != instructionAndCEMap.end()) {
				sunitCEIndex = instructionAndCEMap[SU];
			} else {
				if (SU->getInstr()->mayLoad() || SU->getInstr()->mayStore()) {
					//Check if any parent or child is already assigned a CE
					for (auto succItr = SU->Succs.begin(); succItr != SU->Succs.end(); succItr++) {
						SDep dependence = *succItr;
						if (dependence.isNormalMemory()) {
							unsigned childCE = instructionAndCEMap[dependence.getSUnit()];
							if (find(sourceCEList.begin(), sourceCEList.end(), childCE) == sourceCEList.end()) {
								sourceCEList.push_back(childCE);
								childInstructions.push_back(dependence.getSUnit());
							}
						}
					}

					for (auto predItr = SU->Preds.begin(); predItr != SU->Preds.end(); predItr++) {
						SDep dependence = *predItr;
						if (dependence.isNormalMemory()) {
							unsigned parentCE = instructionAndCEMap[dependence.getSUnit()];
							if (find(sourceCEList.begin(), sourceCEList.end(), parentCE) == sourceCEList.end()) {
								sourceCEList.push_back(parentCE);
								parentInstructions.push_back(dependence.getSUnit());
							}
						}
					}
				} else if (SU->getInstr()->isCopy() && TRI->isPhysicalRegister(SU->getInstr()->getOperand(1).getReg())) {
					//All physical regs contain args to hyperop that are mapped to the register set of the first pHyperOp
					sourceCEList.push_back(0);
				}
				assert((sourceCEList.size() == 0 || sourceCEList.size() == 1) && "Conflicting CE assignments possible to an instruction");
				if (sourceCEList.empty()) {
					if (SU->getInstr()->mayLoad() || SU->getInstr()->mayStore()) {
						//first instruction to access the location in the basic block, check if the same location has been accessed before by another CE
						//There can only be one memory operand in riscv instr set
						list<unsigned> sourceCEList;
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

							if (previousDefOfMemOperand != -1) {
								//This memory location is accessed the first time
								sunitCEIndex = ceIndex;
								memoryLocationsAccessedInCE[sunitCEIndex].push_back(memOperand->getValue());
								ceIndex = (ceIndex + 1) % ceCount;
							} else {
								sourceCEList.push_back(previousDefOfMemOperand);
							}
						}
					} else if (SU->getInstr()->isCopy() && TRI->isPhysicalRegister(SU->getInstr()->getOperand(1).getReg())) {
						sunitCEIndex = 0;
					} else {
						sunitCEIndex = ceIndex;
						ceIndex = (ceIndex + 1) % ceCount;
					}
				} else {
					sunitCEIndex = sourceCEList.front();
				}
			}

			//Mark all parent and child instructions that depend on the current SU for memory as belonging to the same ce
			for (auto parentItr = parentInstructions.begin(); parentItr != parentInstructions.end(); parentItr++) {
				if (instructionAndCEMap.find(*parentItr) == instructionAndCEMap.end()) {
					instructionAndCEMap[*parentItr] = sunitCEIndex;
					leafNodes.push_back(*parentItr);
				}
			}

			for (auto childItr = childInstructions.begin(); childItr != childInstructions.end(); childItr++) {
				if (instructionAndCEMap.find(*childItr) == instructionAndCEMap.end()) {
					instructionAndCEMap[*childItr] = sunitCEIndex;
					leafNodes.push_back(*childItr);
				}
			}
		}
	}

	for (auto suItr = orderOfTraversal.begin(); suItr != orderOfTraversal.end(); suItr++) {
		instructionAndPHyperOpMapForRegion.push_back(make_pair(*suItr, instructionAndCEMap[*suItr]));
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
endOfBBLoopEdgeCovered = false;
for (int i = 0; i < ceCount; i++) {
	faninOfHyperOp.push_back(0);
	for (unsigned j = 0; j < ceCount; j++) {
		registerContainingBaseAddress[i][j] = -1;
	}
}
allInstructionsOfBB.clear();
registersUsedInBB.clear();

/*Expand getmemframe pseudo instruction */
const Module* parentModule = BB->getParent()->getFunction()->getParent();
unsigned maxGlobalSize = 0;
if (!parentModule->getGlobalList().empty()) {
	for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
		const GlobalVariable *globalVar = &*globalArgItr;
		maxGlobalSize += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
	}
}

list<MachineInstr*> deleteList;
vector<unsigned> regs;
/* Replace getmemframe pseudo instruction with the macro for computing memory frame offset of the argument hyperop address */
for (auto instItr = bb->instr_begin(); instItr != bb->instr_end(); instItr++) {
	MachineInstr* inst = instItr;
	if(inst->getOpcode() == REDEFINE::PSFBIND){
		unsigned registerForHopId = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		string hyperOpId = ".HyOp#";
		hyperOpId.append(itostr(inst->getOperand(0).getImm()));
		MCSymbol* hyOpSym = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(hyperOpId));
		MachineInstrBuilder movaddr = BuildMI(*BB, inst, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR));
		movaddr.addReg(registerForHopId, RegState::Define).addSym(hyOpSym);
		addToLISSlot(LIS, movaddr.operator ->());

		MachineInstrBuilder replacementInst = BuildMI(*BB, inst, BB->begin()->getDebugLoc(), TII->get(REDEFINE::FBIND));
		assert(inst->getOperand(0).isImm() && "Fbind can only have integer immediate input operand\n");
		replacementInst.addReg(registerForHopId).addOperand(inst->getOperand(0));
		addToLISSlot(LIS, replacementInst.operator ->());
		deleteList.push_back(inst);
		regs.push_back(registerForHopId);
	}
	else if(inst->getOpcode() == REDEFINE::PSGETMEMFRAME){
		MachineInstr *insertionPoint = inst;
		MachineOperand& frameAddress = inst->getOperand(0);
		assert(frameAddress.isReg() && "Get memory frame pseudo's argument must be in reg\n");
		unsigned frameInReg = frameAddress.getReg();
//add global address to r31 of REDEFINE
		string globalAddressString = "ga#";
		globalAddressString.append(itostr(maxGlobalSize));
		MCSymbol* gaSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(globalAddressString));

		string frameSizeString = "fs";
		MCSymbol* frameSizeSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(frameSizeString));

		unsigned registerForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder movimm = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR)).addReg(registerForGlobalAddr, RegState::Define).addSym(gaSymbol);
		addToLISSlot(LIS, movimm.operator ->());

		unsigned registerForMulOperand = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder addiForMul = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(registerForMulOperand, RegState::Define).addReg(REDEFINE::zero).addSym(frameSizeSymbol);
		addToLISSlot(LIS, addiForMul.operator ->());

//TODO
		unsigned crId = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder shiftForCRId = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(crId, RegState::Define).addReg(frameInReg).addImm(SHIFT_FOR_CRID);
		addToLISSlot(LIS, shiftForCRId.operator ->());

		unsigned shiftedPageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder shiftForPageNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(shiftedPageNumber, RegState::Define).addReg(frameInReg).addImm(SHIFT_FOR_PAGENUMBER);
		unsigned pageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder bitmaskForPageNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ANDI)).addReg(pageNumber, RegState::Define).addReg(shiftedPageNumber).addImm(PAGE_NUMBER_MASK);
		addToLISSlot(LIS, bitmaskForPageNumber.operator ->());

		unsigned shiftedFrameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder shiftForFrameNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(shiftedFrameNumber, RegState::Define).addReg(frameInReg).addImm(SHIFT_FOR_FRAMENUMBER);
		addToLISSlot(LIS, shiftForFrameNumber.operator llvm::MachineInstr *());

		unsigned frameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder bitmaskForFrameNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ANDI)).addReg(frameNumber, RegState::Define).addReg(shiftedFrameNumber).addImm(FRAME_NUMBER_MASK);
		addToLISSlot(LIS, bitmaskForFrameNumber.operator ->());

		unsigned loadImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulOperandForCRBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(loadImmInReg, RegState::Define).addReg(REDEFINE::zero).addImm(FRAMES_PER_CR);
		addToLISSlot(LIS, mulOperandForCRBase.operator ->());

		unsigned crBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulForCRBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(crBase, RegState::Define).addReg(crId).addReg(loadImmInReg);
		addToLISSlot(LIS, mulForCRBase.operator ->());

		unsigned loadPageImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulOperandForPageBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(loadPageImmInReg, RegState::Define).addReg(REDEFINE::zero).addImm(FRAMES_PER_PAGE);
		addToLISSlot(LIS, mulOperandForPageBase.operator ->());

		unsigned pageBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulForPageBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(pageBase, RegState::Define).addReg(pageNumber).addReg(loadPageImmInReg);
		addToLISSlot(LIS, mulForPageBase.operator ->());

		unsigned frameBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder addForFrameBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(frameBase, RegState::Define).addReg(pageBase).addReg(frameNumber);
		addToLISSlot(LIS, addForFrameBase.operator ->());

		unsigned crIdBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder addForCRBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(crIdBase, RegState::Define).addReg(frameBase).addReg(crBase);
		addToLISSlot(LIS, addForCRBase.operator ->());
//END TODO

		unsigned registerForMul = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulForFrameSize = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(registerForMul, RegState::Define).addReg(crIdBase).addReg(registerForMulOperand);
		addToLISSlot(LIS, mulForFrameSize.operator ->());

		MachineInstrBuilder addForGlobalAddr = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addOperand(inst->getOperand(0)).addReg(registerForGlobalAddr).addReg(registerForMul);
		addToLISSlot(LIS, addForGlobalAddr.operator ->());
		deleteList.push_back(inst);
	}
}
for (auto instItr = deleteList.begin(); instItr != deleteList.end(); instItr++) {
	MachineInstr* inst = *instItr;
	inst->eraseFromParent();
}
LIS->repairIntervalsInRange(BB, BB->begin(), BB->end(), regs);
bb->dump();
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

if (RegionBegin == BB->begin()) {
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
		unsigned registerForCopyOfInstId = REDEFINE::t5;
		unsigned registerForIncrOfInstId = REDEFINE::t4;

		MachineInstrBuilder registerCopy = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(registerForIncrOfInstId, RegState::Define).addReg(registerForCopyOfInstId).addReg(REDEFINE::zero);
		addToLISSlot(LIS, registerCopy.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(registerCopy.operator->(), make_pair(i, insertPosition++)));
		if(firstInstructionOfpHyperOpInRegion[i] == 0){
			firstInstructionOfpHyperOpInRegion[i] = registerCopy.operator->();
		}

//add global address to r31 of REDEFINE
		string globalAddressString = "ga#";
		globalAddressString.append(itostr(maxGlobalSize));
		MCSymbol* gaSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(globalAddressString));

		string frameSizeString = "fs";
		MCSymbol* frameSizeSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(frameSizeString));

		unsigned registerForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder movimm = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR)).addReg(registerForGlobalAddr, RegState::Define).addSym(gaSymbol);
		addToLISSlot(LIS, movimm.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(movimm.operator->(), make_pair(i, insertPosition++)));

		memoryFrameBaseAddress[i] = registerForGlobalAddr;

		unsigned registerForMulOperand = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder addiForMul = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(registerForMulOperand, RegState::Define).addReg(REDEFINE::zero).addSym(frameSizeSymbol);
		addToLISSlot(LIS, addiForMul.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(addiForMul.operator->(), make_pair(i, insertPosition++)));
		memoryFrameMaxSizeReg[i] = registerForMulOperand;

//TODO
		unsigned crId = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder shiftForCRId = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(crId, RegState::Define).addReg(registerForIncrOfInstId).addImm(SHIFT_FOR_CRID);
		addToLISSlot(LIS, shiftForCRId.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(shiftForCRId.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

		unsigned shiftedPageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder shiftForPageNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(shiftedPageNumber, RegState::Define).addReg(registerForIncrOfInstId).addImm(SHIFT_FOR_PAGENUMBER);
		addToLISSlot(LIS, shiftForPageNumber.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(shiftForPageNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

		unsigned pageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder bitmaskForPageNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ANDI)).addReg(pageNumber, RegState::Define).addReg(shiftedPageNumber).addImm(PAGE_NUMBER_MASK);
		addToLISSlot(LIS, bitmaskForPageNumber.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(bitmaskForPageNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

		unsigned shiftedFrameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder shiftForFrameNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(shiftedFrameNumber, RegState::Define).addReg(registerForIncrOfInstId).addImm(SHIFT_FOR_FRAMENUMBER);
		addToLISSlot(LIS, shiftForFrameNumber.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(shiftForFrameNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

		unsigned frameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder bitmaskForFrameNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ANDI)).addReg(frameNumber, RegState::Define).addReg(shiftedFrameNumber).addImm(FRAME_NUMBER_MASK);
		addToLISSlot(LIS, bitmaskForFrameNumber.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(bitmaskForFrameNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

		unsigned loadImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulOperandForCRBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(loadImmInReg, RegState::Define).addReg(REDEFINE::zero).addImm(FRAMES_PER_CR);
		addToLISSlot(LIS, mulOperandForCRBase.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(mulOperandForCRBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

		unsigned crBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulForCRBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(crBase, RegState::Define).addReg(crId).addReg(loadImmInReg);
		addToLISSlot(LIS, mulForCRBase.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(mulForCRBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

		unsigned loadPageImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulOperandForPageBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(loadPageImmInReg, RegState::Define).addReg(REDEFINE::zero).addImm(FRAMES_PER_PAGE);
		addToLISSlot(LIS, mulOperandForPageBase.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(mulOperandForPageBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

		unsigned pageBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulForPageBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(pageBase, RegState::Define).addReg(pageNumber).addReg(loadPageImmInReg);
		addToLISSlot(LIS, mulForPageBase.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(mulForPageBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

		unsigned frameBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder addForFrameBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(frameBase, RegState::Define).addReg(pageBase).addReg(frameNumber);
		addToLISSlot(LIS, addForFrameBase.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(addForFrameBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

		unsigned crIdBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder addForCRBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(crIdBase, RegState::Define).addReg(frameBase).addReg(crBase);
		addToLISSlot(LIS, addForCRBase.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(addForCRBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
	//END TODO

		unsigned registerForMul = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulForFrameSize = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(registerForMul, RegState::Define).addReg(crIdBase).addReg(registerForMulOperand);
		addToLISSlot(LIS, shiftForFrameNumber.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(mulForFrameSize.operator->(), make_pair(i, insertPosition++)));

		MachineInstrBuilder addForGlobalAddr = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(registerForIncrOfInstId, RegState::Define).addReg(registerForGlobalAddr).addReg(registerForMul);
		addToLISSlot(LIS, addForGlobalAddr.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(addForGlobalAddr.operator->(), make_pair(i, insertPosition++)));
	}
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
					MachineInstrBuilder writepmFirst, writepmSecond, readpmFirst, readpmSecond;
					{
						MachineInstrBuilder luiOfTargetCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForTargetAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfTargetCE.addReg(registerForTargetAddr, RegState::Define);
						luiOfTargetCE.addImm(i + increment);
						addToLISSlot(LIS, luiOfTargetCE.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(luiOfTargetCE.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

						if (firstInstructionOfpHyperOpInRegion[i] == 0) {
							firstInstructionOfpHyperOpInRegion[i] = luiOfTargetCE.operator llvm::MachineInstr *();
						}

						//Add writepm to first CE
						MachineInstrBuilder writepm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::WRITEPM));
						writepm.addReg(registerForTargetAddr);
						//Dummy data
						writepm.addReg(registerForTargetAddr);
						//TODO When splocations changes
						int32_t immediateSPOffset = SPLOCATIONS - ((depth * i + numNodesAtDepth + frameSize) * datawidth);
						writepm.addImm(immediateSPOffset);
						addToLISSlot(LIS, writepm.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

						MachineInstrBuilder luiOfCurrentCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForCurrentAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfCurrentCE.addReg(registerForCurrentAddr, RegState::Define);
						luiOfCurrentCE.addImm(i);
						addToLISSlot(LIS, luiOfCurrentCE.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(luiOfCurrentCE.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

						//Add readpm to first CE
						MachineInstrBuilder readpm = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::DREADPM));
						unsigned readpmTarget = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						readpm.addReg(readpmTarget, RegState::Define);
						//Dummy data
						readpm.addReg(registerForCurrentAddr);
						//TODO When splocations changes
						immediateSPOffset = SPLOCATIONS - ((depth * (i + 1) + numNodesAtDepth + frameSize) * datawidth);
						readpm.addImm(immediateSPOffset);
						addToLISSlot(LIS, readpm.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

						writepmFirst = writepm;
						readpmFirst = readpm;
					}

					{
						MachineInstrBuilder luiOfTargetCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForTargetAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfTargetCE.addReg(registerForTargetAddr, RegState::Define);
						luiOfTargetCE.addImm(i);
						addToLISSlot(LIS, luiOfTargetCE.operator llvm::MachineInstr *());
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
						addToLISSlot(LIS, writepm.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));

						MachineInstrBuilder luiOfCurrentCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForCurrentAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfCurrentCE.addReg(registerForCurrentAddr, RegState::Define);
						luiOfCurrentCE.addImm(i + increment);
						addToLISSlot(LIS, luiOfCurrentCE.operator llvm::MachineInstr *());
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
						addToLISSlot(LIS, readpm.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));

						writepmSecond = writepm;
						readpmSecond = readpm;
					}
					pHopInteractionGraph.push_back(make_pair(writepmFirst.operator ->(), readpmSecond.operator ->()));
					pHopInteractionGraph.push_back(make_pair(writepmSecond.operator ->(), readpmFirst.operator ->()));
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
					loadFromMemory.addReg(REDEFINE::t4);
					loadFromMemory.addFrameIndex(frameLocationToReadFrom);
					addToLISSlot(LIS, loadFromMemory.operator llvm::MachineInstr *());
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
					addToLISSlot(LIS, sourceLui.operator llvm::MachineInstr *());
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
					addToLISSlot(LIS, targetLui.operator llvm::MachineInstr *());
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
				addToLISSlot(LIS, writepm.operator llvm::MachineInstr *());
				addToLISSlot(LIS, readpm.operator llvm::MachineInstr *());

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
				}

				writepm.addImm(offsetInScratchpad);
				readpm.addReg(registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction]);
				readpm.addImm(offsetInScratchpad);

				allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(ceContainingPredecessorInstruction, insertPosition++)));
				allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, insertPosition++)));
				pHopInteractionGraph.push_back(make_pair(writepm.operator ->(), readpm.operator ->()));

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

	bool isLoopTerminator = false;
	MachineLoop* loop = MLI.getLoopFor(BB);
	if (loop != NULL && loop->getBottomBlock() != BB && !endOfBBLoopEdgeCovered) {
		isLoopTerminator = true;
		endOfBBLoopEdgeCovered = true;
	}
//Check if the branch edge is a backedge
	if (isLoopTerminator) {
//Add barrier reduction tree for synchronization
		unsigned nextCe = 2;
		unsigned depth = 0;
		unsigned numNodesAtDepth = 0;
		while (depth < ceil(log2(ceCount))) {
			depth++;
			unsigned increment = nextCe / 2;
			for (unsigned i = 0; i < ceCount; i += increment) {
				if (i % 2 == 0 && (i + increment) < ceCount) {
					MachineInstrBuilder writepmFirst, writepmSecond, readpmFirst, readpmSecond;
					{
						MachineInstrBuilder luiOfTargetCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForTargetAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfTargetCE.addReg(registerForTargetAddr, RegState::Define);
						luiOfTargetCE.addImm(i + increment);
						addToLISSlot(LIS, luiOfTargetCE.operator llvm::MachineInstr *());
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
						addToLISSlot(LIS, writepm.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

						MachineInstrBuilder luiOfCurrentCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForCurrentAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfCurrentCE.addReg(registerForCurrentAddr, RegState::Define);
						luiOfCurrentCE.addImm(i);
						addToLISSlot(LIS, luiOfCurrentCE.operator llvm::MachineInstr *());
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
						addToLISSlot(LIS, readpm.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));

						writepmFirst = writepm;
						readpmFirst = readpm;
					}

					{
						MachineInstrBuilder luiOfTargetCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForTargetAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfTargetCE.addReg(registerForTargetAddr, RegState::Define);
						luiOfTargetCE.addImm(i);
						addToLISSlot(LIS, luiOfTargetCE.operator llvm::MachineInstr *());
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
						addToLISSlot(LIS, writepm.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));

						MachineInstrBuilder luiOfCurrentCE = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::LUI));
						unsigned registerForCurrentAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						luiOfCurrentCE.addReg(registerForCurrentAddr, RegState::Define);
						luiOfCurrentCE.addImm(i + increment);
						addToLISSlot(LIS, luiOfCurrentCE.operator llvm::MachineInstr *());
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
						addToLISSlot(LIS, readpm.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i + increment, insertPosition++)));

						writepmSecond = writepm;
						readpmSecond = readpm;
					}

					pHopInteractionGraph.push_back(make_pair(writepmFirst.operator ->(), readpmSecond.operator ->()));
					pHopInteractionGraph.push_back(make_pair(writepmSecond.operator ->(), readpmFirst.operator ->()));
				}
			}
			numNodesAtDepth = numNodesAtDepth + floor(ceCount / (nextCe / 2));
			nextCe = nextCe * 2;
		}
		for (unsigned i = 0; i < ceCount; i++) {
			faninOfHyperOp[i] = 0;
		}
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
						loadFromMemory.addReg(REDEFINE::t4);
						loadFromMemory.addFrameIndex(frameLocationToReadFrom);
						addToLISSlot(LIS, loadFromMemory.operator llvm::MachineInstr *());
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

							addToLISSlot(LIS, sourceLui.operator llvm::MachineInstr *());
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

							addToLISSlot(LIS, targetLui.operator llvm::MachineInstr *());
							allInstructionsOfRegion.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(ceContainingInstruction, insertPosition++)));
							registerContainingBaseAddress[ceContainingInstruction][ceContainingInstruction] = (int) targetSpAddressRegister;

							if (firstInstructionOfpHyperOpInRegion[ceContainingInstruction] == 0) {
								firstInstructionOfpHyperOpInRegion[ceContainingInstruction] = targetLui;
							}
						}

						//TODO If number of registers spills into larger than 12 bit size because immediate value supported by readpm and writepm is a signed 11 bit field
						if (ceil(log2(offsetInScratchpad)) > SHIFT_FOR_PAGENUMBER) {
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
						pHopInteractionGraph.push_back(make_pair(writepm.operator ->(), readpm.operator ->()));

						//Increment by data width since scratchpad is byte addressable
						faninOfHyperOp[ceContainingInstruction] = faninOfHyperOp[ceContainingInstruction] + datawidth;
						addToLISSlot(LIS, writepm.operator llvm::MachineInstr *());
						addToLISSlot(LIS, readpm.operator llvm::MachineInstr *());
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
		addToLISSlot(LIS, duplicateTerminatorInstr.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(duplicateTerminatorInstr.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
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
			if (replacementRegisterMap.find(ce) == replacementRegisterMap.end() || replacementRegisterMap[ce].empty() || replacementRegisterMap[ce].find(operand.getReg()) == replacementRegisterMap[ce].end()) {
				unsigned originalRegister = operand.getReg();
				const TargetRegisterClass* regClass;
				if (TRI->isPhysicalRegister(originalRegister)) {
					regClass = TRI->getMinimalPhysRegClass(originalRegister);
				} else {
					regClass = MRI.getRegClass(originalRegister);
				}

				if (replacementRegisterMap.find(ce) == replacementRegisterMap.end()) {
					map<unsigned, unsigned> tempMap;
					replacementRegisterMap.insert(make_pair(ce, tempMap));
				}

				if (*regClass->vt_begin() == MVT::i32) {
					//First definition in ce
					replacementRegisterMap[ce][operand.getReg()] = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				} else {
					replacementRegisterMap[ce][operand.getReg()] = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::f32);
				}
			}
			unsigned replacementRegister = replacementRegisterMap[ce][operand.getReg()];
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
const Module * parentModule = BB->getParent()->getFunction()->getParent();

//If the basic block is the terminator
if (BB->getNumber() == MF.back().getNumber()) {
	MachineBasicBlock* lastBB = &MF.back();
	MachineInstr* lastInstruction = lastBB->end();
	DebugLoc dl = MF.begin()->begin()->getDebugLoc();

	for (unsigned i = 0; i < ceCount; i++) {
		firstInstructionOfpHyperOpInRegion.push_back(0);
	}

	allInstructionsOfRegion.clear();
	insertPosition = 0;

	bool isFunctionEmpty = false;
	if (BB->getParent()->empty()) {
		isFunctionEmpty = true;
	} else {
		isFunctionEmpty = true;
		for (MachineFunction::iterator bbItr = BB->getParent()->begin(); bbItr != BB->getParent()->end(); bbItr++) {
			if (!bbItr->empty()) {
				isFunctionEmpty = false;
				break;
			}
		}
	}

	if (isFunctionEmpty) {
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
			unsigned registerForCopyOfInstId = REDEFINE::t5;
			unsigned registerForIncrOfInstId = REDEFINE::t4;
			MachineInstrBuilder registerCopy = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(registerForIncrOfInstId, RegState::Define).addReg(registerForCopyOfInstId).addReg(REDEFINE::zero);
			addToLISSlot(LIS, registerCopy.operator llvm::MachineInstr *());
			allInstructionsOfRegion.push_back(make_pair(registerCopy.operator->(), make_pair(i, insertPosition++)));
			if (firstInstructionOfpHyperOpInRegion[i] == 0) {
				firstInstructionOfpHyperOpInRegion[i] = registerCopy.operator->();
			}

			//add global address to r31 of REDEFINE
			string globalAddressString = "ga#";
			globalAddressString.append(itostr(maxGlobalSize));
			MCSymbol* gaSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(globalAddressString));

			string frameSizeString = "fs";
			MCSymbol* frameSizeSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(frameSizeString));

			unsigned registerForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder movimm = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR)).addReg(registerForGlobalAddr, RegState::Define).addSym(gaSymbol);
			addToLISSlot(LIS, movimm.operator llvm::MachineInstr *());
			allInstructionsOfRegion.push_back(make_pair(movimm.operator->(), make_pair(i, insertPosition++)));

			memoryFrameBaseAddress[i] = registerForGlobalAddr;

			unsigned registerForMulOperand = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder addiForMul = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(registerForMulOperand, RegState::Define).addReg(REDEFINE::zero).addSym(frameSizeSymbol);
			addToLISSlot(LIS, addiForMul.operator llvm::MachineInstr *());
			allInstructionsOfRegion.push_back(make_pair(addiForMul.operator->(), make_pair(i, insertPosition++)));
			memoryFrameMaxSizeReg[i] = registerForMulOperand;

			//TODO
			unsigned crId = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder shiftForCRId = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(crId, RegState::Define).addReg(registerForIncrOfInstId).addImm(SHIFT_FOR_CRID);
			addToLISSlot(LIS, shiftForCRId.operator llvm::MachineInstr *());
			LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForCRId.operator llvm::MachineInstr *());

			unsigned shiftedPageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder shiftForPageNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(shiftedPageNumber, RegState::Define).addReg(registerForIncrOfInstId).addImm(SHIFT_FOR_PAGENUMBER);
			allInstructionsOfRegion.push_back(make_pair(shiftForPageNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			addToLISSlot(LIS, shiftForPageNumber.operator llvm::MachineInstr *());
			unsigned pageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder bitmaskForPageNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ANDI)).addReg(pageNumber, RegState::Define).addReg(shiftedPageNumber).addImm(PAGE_NUMBER_MASK);
			allInstructionsOfRegion.push_back(make_pair(bitmaskForPageNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			addToLISSlot(LIS, bitmaskForPageNumber.operator llvm::MachineInstr *());

			unsigned shiftedFrameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder shiftForFrameNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(shiftedFrameNumber, RegState::Define).addReg(registerForIncrOfInstId).addImm(SHIFT_FOR_FRAMENUMBER);
			allInstructionsOfRegion.push_back(make_pair(shiftForFrameNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			addToLISSlot(LIS, shiftForFrameNumber.operator llvm::MachineInstr *());
			unsigned frameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder bitmaskForFrameNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ANDI)).addReg(frameNumber, RegState::Define).addReg(shiftedFrameNumber).addImm(FRAME_NUMBER_MASK);
			allInstructionsOfRegion.push_back(make_pair(bitmaskForFrameNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			addToLISSlot(LIS, bitmaskForFrameNumber.operator llvm::MachineInstr *());

			unsigned loadImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder mulOperandForCRBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(loadImmInReg, RegState::Define).addReg(REDEFINE::zero).addImm(FRAMES_PER_CR);
			allInstructionsOfRegion.push_back(make_pair(mulOperandForCRBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			addToLISSlot(LIS, mulOperandForCRBase.operator llvm::MachineInstr *());

			unsigned crBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder mulForCRBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(crBase, RegState::Define).addReg(crId).addReg(loadImmInReg);
			allInstructionsOfRegion.push_back(make_pair(mulForCRBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			addToLISSlot(LIS, mulForCRBase.operator llvm::MachineInstr *());

			unsigned loadPageImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder mulOperandForPageBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(loadPageImmInReg, RegState::Define).addReg(REDEFINE::zero).addImm(FRAMES_PER_PAGE);
			allInstructionsOfRegion.push_back(make_pair(mulOperandForPageBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			addToLISSlot(LIS, mulOperandForPageBase.operator llvm::MachineInstr *());

			unsigned pageBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder mulForPageBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(pageBase, RegState::Define).addReg(pageNumber).addReg(loadPageImmInReg);
			allInstructionsOfRegion.push_back(make_pair(mulForPageBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			addToLISSlot(LIS, mulForPageBase.operator llvm::MachineInstr *());

			unsigned frameBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder addForFrameBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(frameBase, RegState::Define).addReg(pageBase).addReg(frameNumber);
			allInstructionsOfRegion.push_back(make_pair(addForFrameBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			addToLISSlot(LIS, addForFrameBase.operator llvm::MachineInstr *());

			unsigned crIdBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder addForCRBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(crIdBase, RegState::Define).addReg(frameBase).addReg(crBase);
			allInstructionsOfRegion.push_back(make_pair(addForCRBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			addToLISSlot(LIS, addForCRBase.operator llvm::MachineInstr *());
			//END TODO

			unsigned registerForMul = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder mulForFrameSize = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(registerForMul, RegState::Define).addReg(crIdBase).addReg(registerForMulOperand);
			addToLISSlot(LIS, mulForFrameSize.operator llvm::MachineInstr *());
			allInstructionsOfRegion.push_back(make_pair(mulForFrameSize.operator->(), make_pair(i, insertPosition++)));

			MachineInstrBuilder addForGlobalAddr = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(registerForIncrOfInstId, RegState::Define).addReg(registerForGlobalAddr).addReg(registerForMul);
			addToLISSlot(LIS, addForGlobalAddr.operator llvm::MachineInstr *());
			allInstructionsOfRegion.push_back(make_pair(addForGlobalAddr.operator->(), make_pair(i, insertPosition++)));
		}
	}
	map<int, unsigned> currentSPLocation;
	for (int i = 0; i < ceCount; i++) {
		currentSPLocation.insert(make_pair(i, SPLOCATIONS));
	}

	DEBUG(dbgs() << "splicing for building bundles\n");

	for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator allInstructionItr = allInstructionsOfRegion.begin(); allInstructionItr != allInstructionsOfRegion.end(); allInstructionItr++) {
		MachineInstr* instruction = allInstructionItr->first;
		MachineBasicBlock* currentBB = instruction->getParent();
		if (currentBB != BB)
			continue;
		unsigned ce = allInstructionItr->second.first;
		unsigned position = allInstructionItr->second.second;
		instruction->setFlag(MachineInstr::NoFlags);
		unsigned i = ce + 1;
		MachineInstr* nextCeInstruction;
		if (ce < ceCount - 1) {
			while (i <= ceCount - 1) {
				if (currentBB == BB) {
					nextCeInstruction = firstInstructionOfpHyperOpInRegion[i++];
				}
				if (nextCeInstruction != currentBB->end() && nextCeInstruction != 0) {
					break;
				}
			}
			if (nextCeInstruction == 0) {
				nextCeInstruction = currentBB->end();
			}

			for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator secondItr = allInstructionsOfRegion.begin(); secondItr != allInstructionsOfRegion.end(); secondItr++) {
				if (secondItr->first == nextCeInstruction) {
					unsigned nextCeInstructionPosition = secondItr->second.second;
					if (nextCeInstructionPosition < position) {
						currentBB->splice(nextCeInstruction, currentBB, instruction);
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
	lastBB = &BB->getParent()->back();
	lastInstruction = lastBB->end();
//End HyperOp and the two NOPs that follow are kinda like a new region that gets shuffled next
	vector<MachineInstr*> endHyperOpInstructionRegion;
//Add endHyperOp instruction and 2 nops in each pHyperOp
	for (unsigned i = 0; i < ceCount; i++) {
		MachineInstrBuilder endInstruction = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::END));
		endInstruction.addImm(0);
		endHyperOpInstructionRegion.push_back(endInstruction.operator ->());
		addToLISSlot(LIS, endInstruction.operator llvm::MachineInstr *());
		allInstructionsOfBB.push_back(make_pair(endInstruction.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
	}
}

map<unsigned, pair<MachineInstr*, unsigned> > firstInstructionInCE;
unsigned currentStartPosition = 0;
list < pair<MachineInstr*, pair<unsigned, unsigned> > > currentPositionMap;
for (list<pair<MachineInstr*, pair<unsigned, unsigned> > >::iterator allInstructionItr = allInstructionsOfBB.begin(); allInstructionItr != allInstructionsOfBB.end(); allInstructionItr++, currentStartPosition++) {
	unsigned ce = allInstructionItr->second.first;
	if (firstInstructionInCE.find(ce) == firstInstructionInCE.end()) {
		firstInstructionInCE.insert(make_pair(ce, make_pair(allInstructionItr->first, currentStartPosition)));
	}
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
		int minNextCEPosition = currentStartPosition;
		while (i < ceCount) {
			if (firstInstructionInCE.find(i) != firstInstructionInCE.end()) {
				unsigned nextPosition = firstInstructionInCE[i].second;
				if (nextPosition < minNextCEPosition) {
					minNextCEPosition = nextPosition;
					nextCeInstruction = firstInstructionInCE[i].first;
				}
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
//additional live-in regs in context memory also need to be added
//Actually we don't need to use inst_itr here cos bundles are created after this, but leaving this for now
for (MachineBasicBlock::instr_iterator instItr = BB->instr_begin(); instItr != BB->instr_end(); instItr++) {
	for (unsigned i = 0; i < instItr->getNumOperands(); i++) {
		MachineOperand& operand = instItr->getOperand(i);
		if (operand.isReg() && find(registersUsedInBB.begin(), registersUsedInBB.end(), operand.getReg()) == registersUsedInBB.end()) {
			registersUsedInBB.push_back(operand.getReg());
		}
	}
}

LIS->repairIntervalsInRange(BB, BB->begin(), BB->end(), registersUsedInBB);

//Create instruction bundles corresponding to pHyperOps, thus is necessary because instructions of a basic block maybe shuffled while the inserted instructions are in order and need not be shuffled */
if (!firstInstructionInCE.empty()) {
	DEBUG(dbgs() << "Creating pHyperOp bundles for CEs for bb" << BB->getNumber() << "\n");
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
			//pHyperOp is empty, add a nop
			MachineInstrBuilder nopInstruction = BuildMI(*BB, firstInstructionInNextCE, MF.begin()->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI));
			nopInstruction.addReg(REDEFINE::zero, RegState::Define);
			nopInstruction.addReg(REDEFINE::zero);
			nopInstruction.addImm(0);
			addToLISSlot(LIS, nopInstruction.operator llvm::MachineInstr *());
			firstInstructionofCE = nopInstruction.operator ->();
		}

		if (firstInstructionInNextCE != NULL) {
			MIBundleBuilder* bundleBuilder = new MIBundleBuilder(*BB, (MachineBasicBlock::instr_iterator) firstInstructionofCE, (MachineBasicBlock::instr_iterator) firstInstructionInNextCE);
		}
	}
}

DEBUG(dbgs() << "After bundling, state of mf:\n");
BB->getParent()->print(dbgs());
BB = 0;
}

void REDEFINEMCInstrScheduler::finalizeSchedule() {
//Find the right order of context frame inputs for a machine function
unsigned contextFrameSlotAndPhysReg[frameSize];
//No bijective map available :(
map<unsigned, unsigned> physRegAndContextFrameSlot;
//Phys reg and the ce it is live-in
map<unsigned, unsigned> physRegAndLiveIn;

Function* func = const_cast<Function*>(MF.getFunction());

map<unsigned, MachineInstr*> physRegAndFirstMachineOperand;
map<unsigned, list<unsigned> > ceAndLiveInPhysicalRegMap;
list<unsigned> liveInPhysRegisters;

unsigned liveInFrameLocation = 0;
MachineRegisterInfo& regInfo = MF.getRegInfo();
for (MachineRegisterInfo::livein_iterator liveInItr = regInfo.livein_begin(); liveInItr !=regInfo.livein_end(); liveInItr++, liveInFrameLocation++) {
	liveInPhysRegisters.push_back(liveInItr->first);
	contextFrameSlotAndPhysReg[liveInFrameLocation] = liveInItr->first;
}

//Get the live-in registers and map to the ce to which they belong
for (MachineFunction::iterator MBBI = MF.begin(), MBBE = MF.end(); MBBI != MBBE; ++MBBI) {
	int pHyperOpIndex = -1;
	for (MachineBasicBlock::instr_iterator MII = MBBI->instr_begin(); MII != MBBI->instr_end(); ++MII) {
		MachineInstr *MI = MII;
		if (!MI->isInsideBundle()) {
			pHyperOpIndex++;
		}
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
vector<int> liveInPerCe;
for (unsigned ceIndex = 0; ceIndex < ceCount; ceIndex++) {
	if (ceAndLiveInPhysicalRegMap.find(ceIndex) != ceAndLiveInPhysicalRegMap.end()) {
		list<unsigned> liveInRegs = ceAndLiveInPhysicalRegMap[ceIndex];
		liveInPerCe.push_back(liveInRegs.size());
		if (!liveInRegs.empty()) {
			for (list<unsigned>::iterator physRegItr = liveInRegs.begin(); physRegItr != liveInRegs.end(); physRegItr++) {
				unsigned physReg = *physRegItr;
				physRegAndContextFrameSlot[physReg] = contextFrameLocation;
				physRegAndLiveIn[physReg] = ceIndex;
				contextFrameLocation++;
			}
		}
	}else{
		liveInPerCe.push_back(0);
	}
}

(((REDEFINETargetMachine&)TM).pHyperOpAndNumInputsPerCE).insert(make_pair(func->getName(), liveInPerCe));

LIS->computeLiveInRegUnits();

}
