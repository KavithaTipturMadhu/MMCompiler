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

////TODO Used only 8 bit number here because I need this to address the sync locations and frame locations for replication only which sums upto 20 4 byte locations only
const static char* HYPEROP_ID_PREFIX = ".HyOp#";

static inline int32_t SignExtend8BitNumberTo12Bits(int8_t x) {
	return int32_t(x << 4) >> 4;
}

static inline unsigned getSyncCountInReg(MachineBasicBlock* lastBB, MachineInstr* lastInst, const TargetMachine &TM, const TargetInstrInfo *TII, list<SyncValue> syncCount, list<pair<MachineInstr*, pair<unsigned, unsigned> > >* allInstructionsOfRegion, LiveIntervals* LIS, int currentCE,
		unsigned* argInsertPosition) {
	unsigned syncCountReg;
	bool first = true;
	unsigned insertPosition = *argInsertPosition;
	const Module* parentModule = lastBB->getParent()->getFunction()->getParent();
	for (list<SyncValue>::iterator syncCountIterator = syncCount.begin(); syncCountIterator != syncCount.end(); syncCountIterator++) {
		MachineInstrBuilder addInstr;
		if (syncCountIterator->getType() == SyncValueType::INT_SYNC_TYPE) {
			syncCountReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			int currentCount = syncCountIterator->getInt();
			if (first) {
				addInstr = BuildMI(*lastBB, lastInst, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI));
				addInstr.addReg(syncCountReg, RegState::Define);
				addInstr.addReg(REDEFINE::zero);
				addInstr.addImm(currentCount);
				LIS->getSlotIndexes()->insertMachineInstrInMaps(addInstr.operator llvm::MachineInstr *());
				allInstructionsOfRegion->push_back(make_pair(addInstr.operator->(), make_pair(currentCE, insertPosition++)));
				first = false;
			} else {
				addInstr = BuildMI(*lastBB, lastInst, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI));
				addInstr.addReg(syncCountReg, RegState::Define);
				addInstr.addReg(syncCountReg);
				addInstr.addImm(currentCount);
				LIS->getSlotIndexes()->insertMachineInstrInMaps(addInstr.operator llvm::MachineInstr *());
				allInstructionsOfRegion->push_back(make_pair(addInstr.operator->(), make_pair(currentCE, insertPosition++)));
			}
		} else {
			Value* min = syncCountIterator->getHyperOp()->getRangeLowerBound();
			Value* max = syncCountIterator->getHyperOp()->getRangeUpperBound();
			Value * stride = syncCountIterator->getHyperOp()->getStride();
			assert((isa<ConstantInt>(stride) && ((ConstantInt*) stride)->getZExtValue() == 1) && "stride value unsupported");

			int immediate = 0;
			MachineInstrBuilder movmin, movmax;
			unsigned minregisterForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			unsigned maxregisterForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			unsigned strideRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);

			if (isa<ConstantInt>(min)) {
				movmin = BuildMI(*lastBB, lastInst, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(minregisterForGlobalAddr, RegState::Define).addReg(REDEFINE::zero).addImm(((ConstantInt*) min)->getZExtValue());
				LIS->InsertMachineInstrInMaps(movmin.operator llvm::MachineInstr *());
				allInstructionsOfRegion->push_back(make_pair(movmin.operator ->(), make_pair(currentCE, insertPosition++)));
			} else {
				immediate = 0;
				if (!parentModule->getGlobalList().empty()) {
					for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
						const GlobalVariable *globalVar = &*globalArgItr;
						if (globalVar == min) {
							break;
						}
						immediate += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
					}
				}
				string globalAddressString = "ga#";
				globalAddressString.append(itostr(immediate));
				MCSymbol* gaSymbol = lastBB->getParent()->getContext().GetOrCreateSymbol(StringRef(globalAddressString));

				movmin = BuildMI(*lastBB, lastInst, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR)).addReg(minregisterForGlobalAddr, RegState::Define).addSym(gaSymbol);
				LIS->InsertMachineInstrInMaps(movmin.operator ->());
				allInstructionsOfRegion->push_back(make_pair(movmin.operator ->(), make_pair(currentCE, insertPosition++)));
			}
			if (isa<ConstantInt>(max)) {
				movmax = BuildMI(*lastBB, lastInst, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(maxregisterForGlobalAddr, RegState::Define).addReg(REDEFINE::zero).addImm(((ConstantInt*) max)->getZExtValue());
				LIS->InsertMachineInstrInMaps(movmax.operator ->());
				allInstructionsOfRegion->push_back(make_pair(movmax.operator ->(), make_pair(currentCE, insertPosition++)));
			} else {
				immediate = 0;
				if (!parentModule->getGlobalList().empty()) {
					for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
						const GlobalVariable *globalVar = &*globalArgItr;
						if (globalVar == max) {
							break;
						}
						immediate += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
					}
				}
				string globalAddressString = "ga#";
				globalAddressString.append(itostr(immediate));
				MCSymbol* gaSymbol = lastBB->getParent()->getContext().GetOrCreateSymbol(StringRef(globalAddressString));

				movmax = BuildMI(*lastBB, lastInst, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR)).addReg(maxregisterForGlobalAddr, RegState::Define).addSym(gaSymbol);
				LIS->InsertMachineInstrInMaps(movmax.operator ->());
				allInstructionsOfRegion->push_back(make_pair(movmax.operator ->(), make_pair(currentCE, insertPosition++)));
			}
			unsigned regContainingDiff = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder diff = BuildMI(*lastBB, lastBB->end(), lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::SUB)).addReg(regContainingDiff, RegState::Define).addReg(maxregisterForGlobalAddr).addReg(minregisterForGlobalAddr);
			LIS->InsertMachineInstrInMaps(diff.operator ->());
			allInstructionsOfRegion->push_back(make_pair(diff.operator ->(), make_pair(currentCE, insertPosition++)));
			if (first) {
				first = false;
				syncCountReg = regContainingDiff;
			} else {
				MachineInstrBuilder addInstr = BuildMI(*lastBB, lastInst, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD));
				addInstr.addReg(syncCountReg, RegState::Define);
				addInstr.addReg(syncCountReg);
				addInstr.addReg(regContainingDiff);
				LIS->getSlotIndexes()->insertMachineInstrInMaps(addInstr.operator llvm::MachineInstr *());
				allInstructionsOfRegion->push_back(make_pair(addInstr.operator ->(), make_pair(currentCE, insertPosition++)));
			}

		}
	}

	*argInsertPosition = insertPosition;
	return syncCountReg;
}

static inline MachineBasicBlock* generateRangeConditionalInstructions(MachineBasicBlock* originalBB, MachineBasicBlock* lastBB, MachineInstr* lastInst, LiveIntervals* LIS, const TargetMachine &TM, const TargetInstrInfo *TII, int currentCE, unsigned* argInsertPosition,
		list<pair<MachineInstr*, pair<unsigned, unsigned> > >* allInstructionsOfRegion, MachineBasicBlock* loopEnd, int ceCount) {
	list<MachineInstr*> insertedInstructions;
	Function* function = const_cast<Function*>(lastBB->getParent()->getFunction());
	StringRef fnName = function->getName();
	LLVMContext& ctxt = lastBB->getParent()->getFunction()->getParent()->getContext();
	list<MachineBasicBlock*> createdBBList;
	int insertPosition = *argInsertPosition;

	MachineBasicBlock* conditionalBasicBlock = lastBB->getParent()->CreateMachineBasicBlock();
	createdBBList.push_back(conditionalBasicBlock);
	assert(conditionalBasicBlock != NULL && "Failed to create mbb\n");
	LIS->insertMBBInMaps(conditionalBasicBlock);

	MachineBasicBlock* conditionalExit;

	if (loopEnd == NULL) {
		conditionalExit = lastBB->getParent()->CreateMachineBasicBlock();
		createdBBList.push_back(conditionalExit);
		assert(conditionalExit != NULL && "Failed to create mbb\n");
		LIS->insertMBBInMaps(conditionalExit);
	} else {
		conditionalExit = loopEnd;
	}
	assert(!lastBB->back().isBranch() && "Can't have a branch instruction from last bb\n");

	unsigned subValue = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
	MachineInstrBuilder sub = BuildMI(*lastBB, lastInst, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::SUB));
	sub.addReg(subValue, RegState::Define);
	sub.addReg(REDEFINE::t4);
	sub.addReg(REDEFINE::ra);
//	LIS->addLiveRangeToEndOfBlock(subValue, sub.operator ->());

	MachineInstrBuilder branchInstr = BuildMI(*lastBB, lastInst, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::BEQ));
	branchInstr.addReg(subValue);
	branchInstr.addReg(REDEFINE::zero);
	branchInstr.addMBB(conditionalExit);
	LIS->InsertMachineInstrInMaps(branchInstr.operator ->());
	allInstructionsOfRegion->push_back(make_pair(branchInstr.operator->(), make_pair(currentCE, insertPosition++)));

	for (unsigned i = 1; i < ceCount; i++) {
		//Add jump from indvar to loopstart
		MachineInstrBuilder jumpInstr = BuildMI(*lastBB, lastInst, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::JAL));
		jumpInstr.addMBB(conditionalExit);
		LIS->InsertMachineInstrInMaps(jumpInstr.operator ->());
		allInstructionsOfRegion->push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));

	}

	MachineInstrBuilder conditionalBodyjumpInstr = BuildMI(*conditionalBasicBlock, conditionalBasicBlock->end(), lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::JAL));
	conditionalBodyjumpInstr.addMBB(conditionalExit);
	LIS->InsertMachineInstrInMaps(conditionalBodyjumpInstr.operator ->());
	allInstructionsOfRegion->push_back(make_pair(conditionalBodyjumpInstr.operator->(), make_pair(currentCE, insertPosition++)));

	for (unsigned i = 1; i < ceCount; i++) {
		//Add jump from indvar to loopstart
		MachineInstrBuilder jumpInstr = BuildMI(*lastBB, lastInst, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::JAL));
		jumpInstr.addMBB(conditionalExit);
		LIS->InsertMachineInstrInMaps(jumpInstr.operator ->());
		allInstructionsOfRegion->push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
	}

	*argInsertPosition = insertPosition;
	return conditionalBasicBlock;
}

/*
 *Returns register containing allocated context frame address and last basic block
 */
static inline pair<unsigned, pair<MachineBasicBlock*, MachineInstr*> > generateBlockCreateMachineInstructions(MachineBasicBlock* originalBB, MachineBasicBlock* lastBB, MachineInstr* lastInst, Value* min, Value* max, Value* stride, HyperOp* consumer, LiveIntervals* LIS, const TargetMachine &TM,
		const TargetInstrInfo *TII, int currentCE, unsigned* argInsertPosition, int ceCount, list<pair<MachineInstr*, pair<unsigned, unsigned> > >* allInstructionsOfRegion, vector<MachineInstr*>* firstInstructionOfpHyperOpInRegion,
		map<MachineBasicBlock*, vector<MachineInstr*> >* firstInstructionsOfBB, map<HyperOp*, pair<unsigned, unsigned> >* spLocationContainingHyperOpFrameAddressAndCEWithFalloc, int MAX_SPLOCATIONS, int rangeHopId){
	list<MachineInstr*> insertedInstructions;
	Function* function = const_cast<Function*>(lastBB->getParent()->getFunction());
	StringRef fnName = function->getName();
	LLVMContext& ctxt = lastBB->getParent()->getFunction()->getParent()->getContext();
	list<MachineBasicBlock*> createdBBList;
	unsigned insertPosition = *argInsertPosition;

	MachineBasicBlock* loopStart = lastBB->getParent()->CreateMachineBasicBlock();
	createdBBList.push_back(loopStart);
	assert(loopStart != NULL && "Failed to create mbb\n");
	BasicBlock * loopInitBB = BasicBlock::Create(ctxt, fnName.str().append(".start"), function);
	lastBB->getParent()->push_back(loopStart);
	lastBB->addSuccessor(loopStart);
	LIS->insertMBBInMaps(loopStart);

	MachineBasicBlock* loopBody = lastBB->getParent()->CreateMachineBasicBlock();
	createdBBList.push_back(loopBody);
	assert(loopBody != NULL && "Failed to create mbb\n");
	lastBB->getParent()->push_back(loopBody);
	loopStart->addSuccessor(loopBody);
	loopBody->addSuccessor(loopStart);
	LIS->insertMBBInMaps(loopBody);

	MachineBasicBlock* loopEnd = lastBB->getParent()->CreateMachineBasicBlock();
	createdBBList.push_back(loopEnd);
	assert(loopEnd != NULL && "Failed to create mbb\n");
	lastBB->getParent()->push_back(loopEnd);
	loopStart->addSuccessor(loopEnd);
	LIS->insertMBBInMaps(loopEnd);

	assert(!lastBB->back().isBranch() && "Can't have a branch instruction from last bb\n");
	unsigned tripcountReg;
	unsigned allocatedFrame;

	for (int i = 0; i < currentCE; i++) {
		//Add jump from indvar to loopstart
		MachineInstrBuilder jumpInstr = BuildMI(*lastBB, lastInst, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::JAL));
		jumpInstr.addMBB(loopStart);
		LIS->InsertMachineInstrInMaps(jumpInstr.operator ->());
		allInstructionsOfRegion->push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));

		if (originalBB == lastBB && (*firstInstructionOfpHyperOpInRegion)[i] == NULL) {
			(*firstInstructionOfpHyperOpInRegion)[i] = jumpInstr.operator ->();
		}
	}

	//load trip count from memory location onto a register
	tripcountReg = REDEFINE::t3;
	MachineInstrBuilder addi = BuildMI(*lastBB, lastInst, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI));
	addi.addReg(tripcountReg, RegState::Define);
	addi.addReg(REDEFINE::zero);
	if (isa<ConstantInt>(max)) {
		addi.addImm(((ConstantInt*) max)->getZExtValue());
	} else {
		int gaOffset = 0;
		for (auto globalitr = originalBB->getParent()->getFunction()->getParent()->global_begin(); globalitr != originalBB->getParent()->getFunction()->getParent()->global_end(); globalitr++) {
			if (globalitr == max) {
				break;
			}
			gaOffset += REDEFINEUtils::getAlignedSizeOfType(globalitr->getType());
		}
		addi.addImm(gaOffset);
	}
	LIS->InsertMachineInstrInMaps(addi.operator ->());
	//Interval should be manually added
	allInstructionsOfRegion->push_back(make_pair(addi.operator->(), make_pair(currentCE, insertPosition++)));

	if (originalBB == lastBB && (*firstInstructionOfpHyperOpInRegion)[currentCE] == NULL) {
		(*firstInstructionOfpHyperOpInRegion)[currentCE] = addi.operator ->();
	}

	unsigned indVar = REDEFINE::t2;
	MachineInstrBuilder inductionVarInit = BuildMI(*lastBB, lastInst, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI));
	inductionVarInit.addReg(indVar, RegState::Define);
	inductionVarInit.addReg(REDEFINE::zero);

	if (isa<ConstantInt>(min)) {
		inductionVarInit.addImm(((ConstantInt*) min)->getZExtValue());
	} else {
		int gaOffset = 0;
		for (auto globalitr = originalBB->getParent()->getFunction()->getParent()->global_begin(); globalitr != originalBB->getParent()->getFunction()->getParent()->global_end(); globalitr++) {
			if (globalitr == min) {
				break;
			}
			gaOffset += REDEFINEUtils::getAlignedSizeOfType(globalitr->getType());
		}
		inductionVarInit.addImm(gaOffset);
	}
	LIS->InsertMachineInstrInMaps(inductionVarInit.operator ->());
	allInstructionsOfRegion->push_back(make_pair(inductionVarInit.operator->(), make_pair(currentCE, insertPosition++)));

	unsigned memLocReg = REDEFINE::t1;

	for (int i = currentCE; i < ceCount; i++) {
		//Add jump from indvar to loopstart
		MachineInstrBuilder jumpInstr = BuildMI(*lastBB, lastInst, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::JAL));
		jumpInstr.addMBB(loopStart);
		LIS->InsertMachineInstrInMaps(jumpInstr);
		allInstructionsOfRegion->push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));

		if (originalBB == lastBB && (*firstInstructionOfpHyperOpInRegion)[i] == NULL) {
			(*firstInstructionOfpHyperOpInRegion)[i] = jumpInstr.operator ->();
		}
	}

	vector<MachineInstr*> loopStartInstruction;
	loopStartInstruction.reserve(ceCount);
	for (int i = 0; i < currentCE; i++) {
		MachineInstrBuilder jumpInstr = BuildMI(*loopStart, loopStart->end(), lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::JAL));
		jumpInstr.addMBB(loopEnd);
		LIS->InsertMachineInstrInMaps(jumpInstr.operator ->());
		allInstructionsOfRegion->push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
		loopStartInstruction.push_back(jumpInstr.operator ->());
	}

	MachineInstrBuilder branchInstr = BuildMI(*loopStart, loopStart->end(), lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::BGE));
	branchInstr.addMBB(loopEnd);
	branchInstr.addReg(indVar);
	branchInstr.addReg(tripcountReg);
	LIS->InsertMachineInstrInMaps(branchInstr.operator ->());
	allInstructionsOfRegion->push_back(make_pair(branchInstr.operator->(), make_pair(currentCE, insertPosition++)));
	loopStartInstruction.push_back(branchInstr.operator ->());

//	MachineInstrBuilder jumpInstr = BuildMI(*loopStart, loopStart->end(), lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::JAL));
//	jumpInstr.addMBB(loopBody);
//	LIS->InsertMachineInstrInMaps(jumpInstr.operator ->());
//	allInstructionsOfRegion->push_back(make_pair(jumpInstr.operator->(), make_pair(currentCE, insertPosition++)));

	for (int i = currentCE + 1; i < ceCount; i++) {
		//Add jump from indvar to loopstart
		MachineInstrBuilder jumpInstr = BuildMI(*loopStart, loopStart->end(), lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::JAL));
		jumpInstr.addMBB(loopEnd);
		LIS->InsertMachineInstrInMaps(jumpInstr.operator ->());
		allInstructionsOfRegion->push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
		loopStartInstruction.push_back(jumpInstr.operator ->());
	}

	(*firstInstructionsOfBB)[loopStart] = loopStartInstruction;

	vector<MachineInstr*> loopBodyStartInstruction;
	loopBodyStartInstruction.reserve(ceCount);

	for (int i = 0; i < currentCE; i++) {
		MachineInstrBuilder jumpInstr = BuildMI(*loopBody, loopBody->end(), lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::JAL));
		jumpInstr.addMBB(loopEnd);
		LIS->InsertMachineInstrInMaps(jumpInstr.operator ->());
		allInstructionsOfRegion->push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
		loopBodyStartInstruction.push_back(jumpInstr.operator ->());
	}

	MachineInstrBuilder falloc = BuildMI(*loopBody, loopBody->end(), lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::FALLOC));
	allocatedFrame = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
	falloc.addReg(allocatedFrame, RegState::Define);
	falloc.addReg(REDEFINE::zero);
	LIS->getSlotIndexes()->insertMachineInstrInMaps(falloc.operator llvm::MachineInstr *());
	allInstructionsOfRegion->push_back(make_pair(falloc.operator->(), make_pair(currentCE, insertPosition++)));
	loopBodyStartInstruction.push_back(falloc.operator ->());

	MachineInstrBuilder movsym = BuildMI(*loopBody, loopBody->end(), lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR));
	unsigned symReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
	string hyperOpIDString = HYPEROP_ID_PREFIX;
	hyperOpIDString.append(itostr(consumer->getHyperOpId()));
	MCSymbol* symbol = movsym.operator ->()->getParent()->getParent()->getContext().GetOrCreateSymbol(StringRef(hyperOpIDString));
	movsym.addReg(symReg, RegState::Define).addSym(symbol);
	LIS->getSlotIndexes()->insertMachineInstrInMaps(movsym.operator llvm::MachineInstr *());
	allInstructionsOfRegion->push_back(make_pair(movsym.operator->(), make_pair(currentCE, insertPosition++)));

	MachineInstrBuilder fbind = BuildMI(*loopBody, loopBody->end(), lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::FBIND));
	fbind.addReg(symReg).addReg(allocatedFrame);
	LIS->getSlotIndexes()->insertMachineInstrInMaps(fbind.operator llvm::MachineInstr *());
	allInstructionsOfRegion->push_back(make_pair(fbind.operator->(), make_pair(currentCE, insertPosition++)));

	MachineInstrBuilder writepm = BuildMI(*loopBody, loopBody->end(), lastBB->begin()->getDebugLoc(),  TII->get(REDEFINE::WRITEPM));
    writepm.addReg(REDEFINE::t1);
	writepm.addReg(allocatedFrame);
    writepm.addImm(0);
	LIS->InsertMachineInstrInMaps(writepm.operator ->());
	allInstructionsOfRegion->push_back(make_pair(writepm.operator->(), make_pair(currentCE, insertPosition++)));

	MachineInstrBuilder inductionVarIncrement = BuildMI(*loopBody, loopBody->end(), lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI));
	inductionVarIncrement.addReg(indVar, RegState::Define);
	inductionVarIncrement.addReg(indVar);
	inductionVarIncrement.addImm(((ConstantInt*) stride)->getZExtValue());
	LIS->InsertMachineInstrInMaps(inductionVarIncrement.operator ->());
	allInstructionsOfRegion->push_back(make_pair(inductionVarIncrement.operator->(), make_pair(currentCE, insertPosition++)));

	MachineInstrBuilder multipleIndVar = BuildMI(*loopBody, loopBody->end(), lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI));
	multipleIndVar.addReg(memLocReg, RegState::Define);
	multipleIndVar.addReg(memLocReg);
	multipleIndVar.addImm(-4);
	LIS->InsertMachineInstrInMaps(multipleIndVar.operator ->());
	allInstructionsOfRegion->push_back(make_pair(multipleIndVar.operator->(), make_pair(currentCE, insertPosition++)));
	errs()<<"allocated spm location for "<<consumer->getFunction()->getName()<<":"<<(MAX_SPLOCATIONS - rangeHopId)<<"\n";
	spLocationContainingHyperOpFrameAddressAndCEWithFalloc->insert(make_pair(consumer, make_pair(currentCE, (MAX_SPLOCATIONS - rangeHopId))));

	MachineInstrBuilder loopbodyJump = BuildMI(*loopBody, loopBody->end(), lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::JAL));
	loopbodyJump.addMBB(loopStart);
	LIS->InsertMachineInstrInMaps(loopbodyJump.operator ->());
	allInstructionsOfRegion->push_back(make_pair(loopbodyJump.operator->(), make_pair(currentCE, insertPosition++)));

	for (int i = currentCE + 1; i < ceCount; i++) {
		//Add jump from indvar to loopstart
		MachineInstrBuilder jumpInstr = BuildMI(*loopBody, loopBody->end(), lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::JAL));
		jumpInstr.addMBB(loopEnd);
		LIS->InsertMachineInstrInMaps(jumpInstr.operator ->());
		allInstructionsOfRegion->push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
		loopBodyStartInstruction.push_back(jumpInstr.operator ->());
	}

	(*firstInstructionsOfBB)[loopBody] = loopBodyStartInstruction;

	*argInsertPosition = insertPosition;
	return std::make_pair(allocatedFrame, std::make_pair(loopBody, inductionVarIncrement.operator ->()));
}

static bool isRegDependence(SDep dependence) {
	SDep::Kind dependenceKind = dependence.getKind();
	if (dependenceKind == SDep::Data || dependenceKind == SDep::Anti || dependenceKind == SDep::Output) {
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

//Compute the number of locations
HyperOp* hyperOp = ((REDEFINETargetMachine&) TM).HIG->getHyperOp(const_cast<Function*>(BB->getParent()->getFunction()));
for (auto childItr = hyperOp->ChildMap.begin(); childItr != hyperOp->ChildMap.end(); childItr++) {
	if (childItr->second->getInRange() && hyperOp == childItr->second->getImmediateDominator()) {
		DYNAMIC_INSTANCES++;
	}
}

SPLOCATIONS = MAX_SPLOCATIONS - DYNAMIC_INSTANCES;
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
		MachineInstrBuilder registerCopy = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADD)).addReg(registerForCopyOfInstId, RegState::Define).addReg(REDEFINE::t5).addReg(REDEFINE::zero);
		LIS->getSlotIndexes()->insertMachineInstrInMaps(registerCopy.operator ->());
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
		LIS->getSlotIndexes()->insertMachineInstrInMaps(movimm.operator ->());
		allInstructionsOfRegion.push_back(make_pair(movimm.operator->(), make_pair(i, insertPosition++)));

		memoryFrameBaseAddress[i] = registerForGlobalAddr;

		unsigned registerForMulOperand = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder addiForMul = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(registerForMulOperand, RegState::Define).addReg(REDEFINE::zero).addSym(frameSizeSymbol);
		LIS->getSlotIndexes()->insertMachineInstrInMaps(addiForMul.operator ->());
		allInstructionsOfRegion.push_back(make_pair(addiForMul.operator->(), make_pair(i, insertPosition++)));
		memoryFrameMaxSizeReg[i] = registerForMulOperand;

//TODO
		unsigned crId = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder shiftForCRId = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(crId, RegState::Define).addReg(REDEFINE::t5).addImm(SHIFT_FOR_CRID);
		allInstructionsOfRegion.push_back(make_pair(shiftForCRId.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForCRId.operator llvm::MachineInstr *());

		unsigned shiftedPageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder shiftForPageNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(shiftedPageNumber, RegState::Define).addReg(REDEFINE::t5).addImm(SHIFT_FOR_PAGENUMBER);
		allInstructionsOfRegion.push_back(make_pair(shiftForPageNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForPageNumber.operator llvm::MachineInstr *());
		unsigned pageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder bitmaskForPageNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ANDI)).addReg(pageNumber, RegState::Define).addReg(shiftedPageNumber).addImm(PAGE_NUMBER_MASK);
		allInstructionsOfRegion.push_back(make_pair(bitmaskForPageNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(bitmaskForPageNumber.operator llvm::MachineInstr *());

		unsigned shiftedFrameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder shiftForFrameNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::SRLI)).addReg(shiftedFrameNumber, RegState::Define).addReg(REDEFINE::t5).addImm(SHIFT_FOR_FRAMENUMBER);
		allInstructionsOfRegion.push_back(make_pair(shiftForFrameNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForFrameNumber.operator llvm::MachineInstr *());
		unsigned frameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder bitmaskForFrameNumber = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ANDI)).addReg(frameNumber, RegState::Define).addReg(shiftedFrameNumber).addImm(FRAME_NUMBER_MASK);
		allInstructionsOfRegion.push_back(make_pair(bitmaskForFrameNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(bitmaskForFrameNumber.operator llvm::MachineInstr *());

		unsigned loadImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulOperandForCRBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(loadImmInReg, RegState::Define).addReg(REDEFINE::zero).addImm(FRAMES_PER_CR);
		allInstructionsOfRegion.push_back(make_pair(mulOperandForCRBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(mulOperandForCRBase.operator llvm::MachineInstr *());

		unsigned crBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulForCRBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::MUL)).addReg(crBase, RegState::Define).addReg(crId).addReg(loadImmInReg);
		allInstructionsOfRegion.push_back(make_pair(mulForCRBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
		LIS->getSlotIndexes()->insertMachineInstrInMaps(mulForCRBase.operator llvm::MachineInstr *());

		unsigned loadPageImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
		MachineInstrBuilder mulOperandForPageBase = BuildMI(*BB, insertionPoint, BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(loadPageImmInReg, RegState::Define).addReg(REDEFINE::zero).addImm(FRAMES_PER_PAGE);
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
					MachineInstrBuilder writepmFirst, writepmSecond, readpmFirst, readpmSecond;
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
						//TODO When splocations changes
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
						//TODO When splocations changes
						immediateSPOffset = SPLOCATIONS - ((depth * (i + 1) + numNodesAtDepth + frameSize) * datawidth);
						readpm.addImm(immediateSPOffset);
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());

						writepmFirst = writepm;
						readpmFirst = readpm;
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
//						LIS->addLiveRangeToEndOfBlock(readpmTarget, readpm.operator ->());

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
//			&& !TRI->isPhysicalRegister(dependence.getReg())) {
				//Find the memory location allocated to the live-out register
				if (registerAndFrameLocation.find(dependence.getReg()) != registerAndFrameLocation.end()) {
					unsigned frameLocationToReadFrom = registerAndFrameLocation.find(dependence.getReg())->second;
					//Add an instruction to read from memory
					MachineInstrBuilder loadFromMemory = BuildMI(parentBasicBlock, machineInstruction, location, TII->get(REDEFINE::ADDI));
					loadFromMemory.addReg(dependence.getReg(), RegState::Define);
//					loadFromMemory.addReg(virtualRegistersForInstAddr[ceContainingInstruction].second);
					loadFromMemory.addReg(REDEFINE::t5);
					loadFromMemory.addFrameIndex(frameLocationToReadFrom);
					errs() << "was this the addi instruction inserted?";
					loadFromMemory.operator ->()->dump();
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
//					LIS->addLiveRangeToEndOfBlock(dummyRegister, readpm.operator ->());
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
//						LIS->addLiveRangeToEndOfBlock(readpmTarget, readpm.operator ->());

						writepmFirst = writepm;
						readpmFirst = readpm;
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
//						LIS->addLiveRangeToEndOfBlock(readpmTarget, readpm.operator ->());

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
						loadFromMemory.addReg(REDEFINE::t5);
						loadFromMemory.addFrameIndex(frameLocationToReadFrom);
						errs() << "was this the addi instruction inserted?";
						loadFromMemory.operator ->()->dump();
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
//Additional start and end instructions for loops introduced for range hop falloc and writecm instructions
map<MachineBasicBlock*, vector<MachineInstr*> > firstInstructionsInBB;

//If the basic block is the terminator
if (BB->getName().compare(MF.back().getName()) == 0) {
//TODO:Assuming here that there is only one exit block since we merge return, make sure this is indeed correct
	MachineBasicBlock* lastBB = &MF.back();
	MachineInstr* lastInstruction = lastBB->end();
	DebugLoc dl = MF.begin()->begin()->getDebugLoc();

//Cache of registers contained hyperOp id and the target ce containing the register definition
	map<HyperOp*, list<pair<unsigned, unsigned> > > regContainingMemFrameBaseAddress;

	HyperOpInteractionGraph * graph = ((REDEFINETargetMachine&) TM).HIG;
	Function* Fn = const_cast<Function*>(MF.getFunction());
	HyperOp* hyperOp = ((REDEFINETargetMachine&) TM).HIG->getHyperOp(Fn);

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
		HyperOpInteractionGraph * graph = ((REDEFINETargetMachine&) TM).HIG;
		const Module* parentModule = BB->getParent()->getFunction()->getParent();
		unsigned maxGlobalSize = 0;
		if (!parentModule->getGlobalList().empty()) {
			for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
				const GlobalVariable *globalVar = &*globalArgItr;
				maxGlobalSize += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
			}
		}

		list < pair<unsigned, unsigned> > regList;
		for (unsigned i = 0; i < ceCount; i++) {
			unsigned registerForCopyOfInstId = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			unsigned registerForIncrOfInstId = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			virtualRegistersForInstAddr[i] = make_pair(registerForCopyOfInstId, registerForIncrOfInstId);

			MachineInstrBuilder registerCopy = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::ADD)).addReg(registerForCopyOfInstId, RegState::Define).addReg(REDEFINE::t5).addReg(REDEFINE::zero);
			LIS->getSlotIndexes()->insertMachineInstrInMaps(registerCopy.operator ->());
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
			MachineInstrBuilder movimm = BuildMI(*lastBB, lastBB->end(), BB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR)).addReg(registerForGlobalAddr, RegState::Define).addSym(gaSymbol);
			LIS->getSlotIndexes()->insertMachineInstrInMaps(movimm.operator ->());
			allInstructionsOfRegion.push_back(make_pair(movimm.operator->(), make_pair(i, insertPosition++)));

			unsigned registerForMulOperand = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder addiForMul = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::ADDI)).addReg(registerForMulOperand, RegState::Define).addReg(REDEFINE::zero).addSym(frameSizeSymbol);
			LIS->getSlotIndexes()->insertMachineInstrInMaps(addiForMul.operator ->());
			allInstructionsOfRegion.push_back(make_pair(addiForMul.operator->(), make_pair(i, insertPosition++)));
			memoryFrameMaxSizeReg[i] = registerForMulOperand;

			//TODO
			unsigned crId = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder shiftForCRId = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::SRLI)).addReg(crId, RegState::Define).addReg(REDEFINE::t5).addImm(SHIFT_FOR_CRID);
			allInstructionsOfRegion.push_back(make_pair(shiftForCRId.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForCRId.operator llvm::MachineInstr *());

			unsigned shiftedPageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder shiftForPageNumber = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::SRLI)).addReg(shiftedPageNumber, RegState::Define).addReg(REDEFINE::t5).addImm(SHIFT_FOR_PAGENUMBER);
			allInstructionsOfRegion.push_back(make_pair(shiftForPageNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForPageNumber.operator llvm::MachineInstr *());
			unsigned pageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder bitmaskForPageNumber = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::ANDI)).addReg(pageNumber, RegState::Define).addReg(shiftedPageNumber).addImm(PAGE_NUMBER_MASK);
			allInstructionsOfRegion.push_back(make_pair(bitmaskForPageNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(bitmaskForPageNumber.operator llvm::MachineInstr *());

			unsigned shiftedFrameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder shiftForFrameNumber = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::SRLI)).addReg(shiftedFrameNumber, RegState::Define).addReg(REDEFINE::t5).addImm(SHIFT_FOR_FRAMENUMBER);
			allInstructionsOfRegion.push_back(make_pair(shiftForFrameNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForFrameNumber.operator llvm::MachineInstr *());
			unsigned frameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder bitmaskForFrameNumber = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::ANDI)).addReg(frameNumber, RegState::Define).addReg(shiftedFrameNumber).addImm(FRAME_NUMBER_MASK);
			allInstructionsOfRegion.push_back(make_pair(bitmaskForFrameNumber.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(bitmaskForFrameNumber.operator llvm::MachineInstr *());

			unsigned loadImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder mulOperandForCRBase = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::ADDI)).addReg(loadImmInReg, RegState::Define).addReg(REDEFINE::zero).addImm(FRAMES_PER_CR);
			allInstructionsOfRegion.push_back(make_pair(mulOperandForCRBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(mulOperandForCRBase.operator llvm::MachineInstr *());

			unsigned crBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder mulForCRBase = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::MUL)).addReg(crBase, RegState::Define).addReg(crId).addReg(loadImmInReg);
			allInstructionsOfRegion.push_back(make_pair(mulForCRBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(mulForCRBase.operator llvm::MachineInstr *());

			unsigned loadPageImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder mulOperandForPageBase = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::ADDI)).addReg(loadPageImmInReg, RegState::Define).addReg(REDEFINE::zero).addImm(FRAMES_PER_PAGE);
			allInstructionsOfRegion.push_back(make_pair(mulOperandForPageBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(mulOperandForPageBase.operator llvm::MachineInstr *());

			unsigned pageBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder mulForPageBase = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::MUL)).addReg(pageBase, RegState::Define).addReg(pageNumber).addReg(loadPageImmInReg);
			allInstructionsOfRegion.push_back(make_pair(mulForPageBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(mulForPageBase.operator llvm::MachineInstr *());

			unsigned frameBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder addForFrameBase = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::ADD)).addReg(frameBase, RegState::Define).addReg(pageBase).addReg(frameNumber);
			allInstructionsOfRegion.push_back(make_pair(addForFrameBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(addForFrameBase.operator llvm::MachineInstr *());

			unsigned crIdBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder addForCRBase = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::ADD)).addReg(crIdBase, RegState::Define).addReg(frameBase).addReg(crBase);
			allInstructionsOfRegion.push_back(make_pair(addForCRBase.operator llvm::MachineInstr *(), make_pair(i, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(addForCRBase.operator llvm::MachineInstr *());
			//END TODO

			unsigned registerForMul = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder mulForFrameSize = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::MUL)).addReg(registerForMul, RegState::Define).addReg(crIdBase).addReg(registerForMulOperand);
			LIS->getSlotIndexes()->insertMachineInstrInMaps(mulForFrameSize.operator ->());
			allInstructionsOfRegion.push_back(make_pair(mulForFrameSize.operator->(), make_pair(i, insertPosition++)));

			MachineInstrBuilder addForGlobalAddr = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::ADD)).addReg(registerForIncrOfInstId, RegState::Define).addReg(registerForGlobalAddr).addReg(registerForMul);
			LIS->getSlotIndexes()->insertMachineInstrInMaps(addForGlobalAddr.operator ->());
			allInstructionsOfRegion.push_back(make_pair(addForGlobalAddr.operator->(), make_pair(i, insertPosition++)));

			regList.push_back(make_pair(registerForGlobalAddr, i));
		}

//Get rid of this when work stealing is enabled
		list<HyperOp*> childList = hyperOp->getChildList();
		for (auto childHopItr = childList.begin(); childHopItr != childList.end(); childHopItr++) {
			if ((*childHopItr)->getTargetResource() == hyperOp->getTargetResource()) {
				regContainingMemFrameBaseAddress[*childHopItr] = regList;
			}
		}
	}

	map<int, unsigned> currentSPLocation;
	for (int i = 0; i < ceCount; i++) {
		currentSPLocation.insert(make_pair(i, SPLOCATIONS));
	}

	/* All operations to be performed by the immediate dominator go first, since liveness of falloc register can't be maintained across basic blocks */
	DEBUG(dbgs() << "Adding all fallocs and sync instructions \n");
	map<HyperOp*, pair<unsigned, unsigned> > registerContainingHyperOpFrameAddressAndCEWithFalloc;
	map<HyperOp*, pair<unsigned, unsigned> > spLocationContainingHyperOpFrameAddressAndCEWithFalloc;
	unsigned currentCE = 0;
	list<HyperOp*> childList = hyperOp->getChildList();

	int rangeHopId = 4;
	int firstInstance = 1;
	for (list<HyperOp*>::iterator childItr = childList.begin(); childItr != childList.end(); childItr++) {
		HyperOp* currentChild = *childItr;
		if (currentChild->getImmediateDominator() == hyperOp) {
			//Add falloc instruction to create dynamic HyperOp instances
			unsigned registerContainingConsumerFrameAddr;

			if (!currentChild->isStaticHyperOp()) {
				//Is a range of hyperOps
				if (currentChild->getInRange()) {
					Value* min = currentChild->getRangeLowerBound();
					Value* max = currentChild->getRangeUpperBound();
					Value* stride = currentChild->getStride();
					assert(isa<ConstantInt>(min) && isa<ConstantInt>(max) && isa<ConstantInt>(stride));
					if (firstInstance) {
						unsigned memLocReg = REDEFINE::t1;
						MachineInstrBuilder memLocRegInit = BuildMI(*lastBB, lastInstruction, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI));
						memLocRegInit.addReg(memLocReg, RegState::Define);
						memLocRegInit.addReg(REDEFINE::zero);
						memLocRegInit.addImm(MAX_SPLOCATIONS - rangeHopId);
						LIS->InsertMachineInstrInMaps(memLocRegInit.operator ->());
						allInstructionsOfRegion.push_back(make_pair(memLocRegInit.operator->(), make_pair(currentCE, insertPosition++)));
						firstInstance = 0;
					}

					auto retval = generateBlockCreateMachineInstructions(BB, lastBB, lastInstruction, min, max, stride, currentChild, LIS, TM, TII, currentCE, &insertPosition, ceCount, &allInstructionsOfRegion, &firstInstructionOfpHyperOpInRegion, &firstInstructionsInBB,
							&spLocationContainingHyperOpFrameAddressAndCEWithFalloc, MAX_SPLOCATIONS, rangeHopId);

					rangeHopId += ((((ConstantInt*) max)->getZExtValue() - ((ConstantInt*) min)->getZExtValue()) / ((ConstantInt*) stride)->getZExtValue()) * 4;
					registerContainingConsumerFrameAddr = retval.first;
					lastBB = retval.second.first;
					lastInstruction = retval.second.second;
					for (auto parentEdgeItr = hyperOp->ChildMap.begin(); parentEdgeItr != hyperOp->ChildMap.end(); parentEdgeItr++) {
						if (parentEdgeItr->second == currentChild && parentEdgeItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_SCALAR) {
							parentEdgeItr->first->setValue((ConstantInt*) registerContainingConsumerFrameAddr);
							break;
						}
					}
				} else {
					registerContainingConsumerFrameAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder falloc = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::FALLOC));
					falloc.addReg(registerContainingConsumerFrameAddr, RegState::Define);
					falloc.addReg(REDEFINE::zero);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(falloc.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(falloc.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
					if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
						vector<MachineInstr*> startInstructions;
						startInstructions.push_back(falloc.operator ->());
						firstInstructionsInBB.insert(make_pair(lastBB, startInstructions));
					} else if (firstInstructionOfpHyperOpInRegion[currentCE] == 0) {
						firstInstructionOfpHyperOpInRegion[currentCE] = falloc.operator llvm::MachineInstr *();
					}
					int hyperOpId = currentChild->getHyperOpId();
					string hyperOpIDString = HYPEROP_ID_PREFIX;
					hyperOpIDString.append(itostr(hyperOpId));
					//Fbind instruction added to the immediate dominator of the HyperOp
					unsigned symReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder movSym = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::MOVADDR));
					MCSymbol* symbol = movSym.operator ->()->getParent()->getParent()->getContext().GetOrCreateSymbol(StringRef(hyperOpIDString));
					movSym.addReg(symReg, RegState::Define).addSym(symbol);
					allInstructionsOfRegion.push_back(make_pair(movSym.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(movSym.operator llvm::MachineInstr *());

					MachineInstrBuilder fbind = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::FBIND));
					fbind.addReg(symReg).addReg(registerContainingConsumerFrameAddr);
					allInstructionsOfRegion.push_back(make_pair(fbind.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(fbind.operator llvm::MachineInstr *());

					//Load the base scratchpad address to a register in the consumer CE the first time
					MachineInstrBuilder targetLui = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LUI));
					unsigned targetSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					targetLui.addReg(targetSpAddressRegister, RegState::Define);
					//TODO May need changing later
					targetLui.addImm(currentCE);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));

					MachineInstrBuilder writepm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::WRITEPM));
					writepm.addReg(targetSpAddressRegister);
					writepm.addReg(registerContainingConsumerFrameAddr);
					writepm.addImm(MAX_SPLOCATIONS - rangeHopId);
					allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator ->());
					spLocationContainingHyperOpFrameAddressAndCEWithFalloc.insert(make_pair(currentChild, make_pair(currentCE, (MAX_SPLOCATIONS - rangeHopId))));
					rangeHopId += 4;
				}

			} else {
				registerContainingConsumerFrameAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder addi = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
				addi.addReg(registerContainingConsumerFrameAddr, RegState::Define);
				addi.addReg(REDEFINE::zero);
				addi.addImm(REDEFINEUtils::getHyperOpId(currentChild));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
				if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
					vector<MachineInstr*> startInstructions;
					startInstructions.push_back(addi.operator ->());
					firstInstructionsInBB.insert(make_pair(lastBB, startInstructions));
				} else if (firstInstructionOfpHyperOpInRegion[currentCE] == 0) {
					firstInstructionOfpHyperOpInRegion[currentCE] = addi.operator llvm::MachineInstr *();
				}
				allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
			}

			/* Sync count needs to be delivered anyway, to either static or dynamic hyperops */
			if (currentChild->isBarrierHyperOp()) {
				unsigned ceContainingConsumerFrameAddr = currentCE;
				unsigned registerWithSyncCount;
				unsigned predicatedRegWithSyncCount;
				bool predicatedRegAdded = false;
				//Mutually exclusive sync sources, add instructions to multiply the predicate values that reaches the sync producer with the
				MachineInstrBuilder addi;
				unsigned addSync;
				if (currentChild->getSyncCount(0).size() != currentChild->getSyncCount(1).size() && !currentChild->getSyncCount(1).empty() && !currentChild->getSyncCount(0).empty()) {
					predicatedRegAdded = true;
					predicatedRegWithSyncCount = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					unsigned firstPredSyncCount = getSyncCountInReg(lastBB, lastInstruction, TM, TII, currentChild->getSyncCount(0), &allInstructionsOfRegion, LIS, currentCE, &insertPosition);
					unsigned secondPredSyncCount = getSyncCountInReg(lastBB, lastInstruction, TM, TII, currentChild->getSyncCount(1), &allInstructionsOfRegion, LIS, currentCE, &insertPosition);

					Value* firstPredicate = currentChild->getIncomingSyncPredicate(0);
					Value* secondPredicate = currentChild->getIncomingSyncPredicate(1);
					unsigned firstPredMemSize, secondPredMemSize, memsize = 0;
					if (firstPredicate == NULL && secondPredicate != NULL) {
						firstPredicate = secondPredicate;
					} else if (firstPredicate != NULL && secondPredicate == NULL) {
						secondPredicate = firstPredicate;
					}
					for (unsigned int i = 0; i < MF.getFrameInfo()->getObjectIndexEnd(); i++) {
						if (MF.getFrameInfo()->getObjectAllocation(i) == firstPredicate) {
							firstPredMemSize = memsize;
						}
						if (MF.getFrameInfo()->getObjectAllocation(i) == secondPredicate) {
							secondPredMemSize = memsize;
						}
						memsize += REDEFINEUtils::getSizeOfType(MF.getFrameInfo()->getObjectAllocation(i)->getType());
					}

					unsigned firstPred = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder firstSyncPredLoad = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
					firstSyncPredLoad.addReg(firstPred, RegState::Define);
					firstSyncPredLoad.addReg(REDEFINE::t5);
					firstSyncPredLoad.addImm(firstPredMemSize);
					allInstructionsOfRegion.push_back(make_pair(firstSyncPredLoad.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(firstSyncPredLoad.operator llvm::MachineInstr *());

					unsigned secondPred;
					if (firstPredicate == secondPredicate) {
						secondPred = firstPred;
					} else {
						secondPred = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder secondSyncPredLoad = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
						secondSyncPredLoad.addReg(secondPred, RegState::Define).addReg(REDEFINE::t5).addImm(secondPredMemSize);
						allInstructionsOfRegion.push_back(make_pair(secondSyncPredLoad.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(secondSyncPredLoad.operator llvm::MachineInstr *());
					}

					unsigned firstPredMul = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder firstSyncMul = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::MUL));
					firstSyncMul.addReg(firstPredMul, RegState::Define).addReg(firstPred).addReg(firstPredSyncCount);
					allInstructionsOfRegion.push_back(make_pair(firstSyncMul.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(firstSyncMul.operator llvm::MachineInstr *());

					unsigned invertedPred = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder sltiForPredicate = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::SLTIU));
					sltiForPredicate.addReg(invertedPred, RegState::Define).addReg(secondPred).addImm(1);
					allInstructionsOfRegion.push_back(make_pair(sltiForPredicate.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(sltiForPredicate.operator llvm::MachineInstr *());

					unsigned secondPredMul = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder secondSyncMul = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::MUL));
					secondSyncMul.addReg(secondPredMul, RegState::Define).addReg(invertedPred).addReg(secondPredSyncCount);
					allInstructionsOfRegion.push_back(make_pair(secondSyncMul.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(secondSyncMul.operator llvm::MachineInstr *());

					addi = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADD));
					addi.addReg(predicatedRegWithSyncCount, RegState::Define).addReg(firstPredMul).addReg(secondPredMul);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(ceContainingConsumerFrameAddr, insertPosition++)));

					if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
						vector<MachineInstr*> startInstructions;
						startInstructions.push_back(addi.operator ->());
						firstInstructionsInBB.insert(make_pair(lastBB, startInstructions));
					} else if (firstInstructionOfpHyperOpInRegion[ceContainingConsumerFrameAddr] == 0) {
						firstInstructionOfpHyperOpInRegion[ceContainingConsumerFrameAddr] = addi.operator llvm::MachineInstr *();
					}
				} else if (!currentChild->getSyncCount(0).empty()) {
					predicatedRegAdded = true;
					predicatedRegWithSyncCount = getSyncCountInReg(lastBB, lastInstruction, TM, TII, currentChild->getSyncCount(0), &allInstructionsOfRegion, LIS, currentCE, &insertPosition);
				} else if (!currentChild->getSyncCount(1).empty()) {
					predicatedRegAdded = true;
					predicatedRegWithSyncCount = getSyncCountInReg(lastBB, lastInstruction, TM, TII, currentChild->getSyncCount(1), &allInstructionsOfRegion, LIS, currentCE, &insertPosition);
				}

				if (!currentChild->getSyncCount(2).empty()) {
					//Add unpredicated sync count to the overall sync count
					unsigned unpredicatedSyncCount = getSyncCountInReg(lastBB, lastInstruction, TM, TII, currentChild->getSyncCount(2), &allInstructionsOfRegion, LIS, currentCE, &insertPosition);
					if (predicatedRegAdded) {
						registerWithSyncCount = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder addi = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADD));
						addi.addReg(registerWithSyncCount, RegState::Define).addReg(predicatedRegWithSyncCount).addReg(unpredicatedSyncCount);
						allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(ceContainingConsumerFrameAddr, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
					} else {
						registerWithSyncCount = unpredicatedSyncCount;
					}
				} else {
					registerWithSyncCount = predicatedRegWithSyncCount;
				}

				MachineInstrBuilder writecm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::WRITECM));
				writecm.addReg(registerContainingConsumerFrameAddr);
				writecm.addReg(registerWithSyncCount);
				writecm.addImm(SYNC_SLOT_OFFSET);

				allInstructionsOfRegion.push_back(make_pair(writecm.operator llvm::MachineInstr *(), make_pair(ceContainingConsumerFrameAddr, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(writecm.operator llvm::MachineInstr *());
			}

			lastBB = &BB->getParent()->back();
			lastInstruction = lastBB->end();
			currentCE = (currentCE + 1) % ceCount;
		}
	}

	DEBUG(dbgs() << "Adding fdelete instructions\n");
	for (list<HyperOp*>::iterator childHyperOpItr = graph->Vertices.begin(); childHyperOpItr != graph->Vertices.end(); childHyperOpItr++) {
		if (!(*childHyperOpItr)->isStartHyperOp()) {
			HyperOp* liveStartOfVertex = (*childHyperOpItr)->getImmediateDominator();
			HyperOp* immediatePostDomForDeletion = 0;
			HyperOp* liveEndOfVertex = 0;
			//Non-predicated HyperOp can be deleted by itself
			if (liveStartOfVertex != 0 && (*childHyperOpItr)->isPredicatedHyperOp()) {
				liveEndOfVertex = liveStartOfVertex->getImmediatePostDominator();
			} else {
				liveEndOfVertex = (*childHyperOpItr);
			}
			//Add fdelete instruction from r30
			if (liveEndOfVertex == hyperOp && liveEndOfVertex != (*childHyperOpItr) && !(*childHyperOpItr)->isStartHyperOp()) {
				MachineInstrBuilder fdelete;
				MachineBasicBlock* loopStart, *loopBody, *loopEnd;
				unsigned indVar;
				vector<MachineInstr*> loopBodyInstructions;
				//Find the edge corresponding to the context frame of the child hyperop
				for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = liveEndOfVertex->ParentMap.begin(); parentItr != liveEndOfVertex->ParentMap.end(); parentItr++) {
					if ((parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF) && parentItr->first->getContextFrameAddress() == (*childHyperOpItr)) {
						unsigned virtualReg;
						//Get the slot to read from which translates to a register anyway
						if (parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR) {
							unsigned physicalReg = REDEFINEphysRegs[parentItr->first->getPositionOfContextSlot()];
							if (!MF.getRegInfo().isLiveIn(physicalReg)) {
								//There is no need to check if the register is live-in and have an else block here
								virtualReg = MF.addLiveIn(physicalReg, TRI->getMinimalPhysRegClass(physicalReg));
								for (MachineFunction::iterator bbItr = MF.begin(); bbItr != MF.end(); bbItr++) {
									bbItr->addLiveIn(physicalReg);
								}
								MF.getRegInfo().setRegAllocationHint(virtualReg, 0, physicalReg);
								//Emit copy
								MachineInstrBuilder copy = BuildMI(*lastBB, lastInstruction, dl, TII->get(TargetOpcode::COPY)).addReg(virtualReg, RegState::Define).addReg(physicalReg);
								LIS->getSlotIndexes()->insertMachineInstrInMaps(copy.operator llvm::MachineInstr *());
								allInstructionsOfRegion.push_back(make_pair(copy.operator ->(), make_pair(0, insertPosition++)));
							} else {
								virtualReg = MF.getRegInfo().getLiveInVirtReg(physicalReg);
							}
						} else {
							virtualReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
							MachineInstrBuilder copy = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
							copy.addReg(virtualReg, RegState::Define);
							copy.addReg(REDEFINE::t5);
							unsigned argCount = 0;
							int frameOffset = 0;
							for (int i = 0; i < MF.getFrameInfo()->getNumObjects(); i++) {
								frameOffset += REDEFINEUtils::getSizeOfType(MF.getFrameInfo()->getObjectAllocation(i)->getType());
							}
							copy.addImm(frameOffset + parentItr->first->getMemoryOffsetInTargetFrame());
							LIS->getSlotIndexes()->insertMachineInstrInMaps(copy.operator llvm::MachineInstr *());
							allInstructionsOfRegion.push_back(make_pair(copy.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
						}
						if (parentItr->first->getContextFrameAddress()->getInRange()) {
							Value* locationContainingCount;
							Value* min = parentItr->first->getContextFrameAddress()->getRangeLowerBound();
							Value* max = parentItr->first->getContextFrameAddress()->getRangeUpperBound();
							Value* stride = parentItr->first->getContextFrameAddress()->getStride();

							//Add loop constructs around everything that follows
							loopStart = lastBB->getParent()->CreateMachineBasicBlock();
							lastBB->getParent()->push_back(loopStart);
							lastBB->addSuccessor(loopStart);
							LIS->insertMBBInMaps(loopStart);
							loopBody = lastBB->getParent()->CreateMachineBasicBlock();
							lastBB->getParent()->push_back(loopBody);
							LIS->insertMBBInMaps(loopBody);
							loopStart->addSuccessor(loopBody);
							loopBody->addSuccessor(loopStart);
							vector<MachineInstr*> loopStartInstructions;
							vector<MachineInstr*> loopEndInstructions;

							loopEnd = lastBB->getParent()->CreateMachineBasicBlock();
							lastBB->getParent()->push_back(loopEnd);
							loopStart->addSuccessor(loopEnd);
							LIS->insertMBBInMaps(loopEnd);

							vector<MachineInstr*> startInstr;
							//load trip count from memory location onto a register
							unsigned loopMaxboundReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
							MachineInstrBuilder loopBoundMaxLoadInst;
							if (isa<ConstantInt>(max)) {
								loopBoundMaxLoadInst = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
								loopBoundMaxLoadInst.addReg(loopMaxboundReg, RegState::Define);
								loopBoundMaxLoadInst.addReg(REDEFINE::zero);
								loopBoundMaxLoadInst.addImm(((ConstantInt*) max)->getZExtValue());
							} else {
								int immediate = 0;
								if (!parentModule->getGlobalList().empty()) {
									for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
										const GlobalVariable *globalVar = &*globalArgItr;
										if (globalVar == max) {
											break;
										}
										immediate += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
									}
								}
								string globalAddressString = "ga#";
								globalAddressString.append(itostr(immediate));
								MCSymbol* gaSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(globalAddressString));

								unsigned registerForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
								MachineInstrBuilder movimm = BuildMI(*lastBB, lastBB->end(), BB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR)).addReg(registerForGlobalAddr, RegState::Define).addSym(gaSymbol);
								LIS->InsertMachineInstrInMaps(movimm);
								allInstructionsOfRegion.push_back(make_pair(movimm.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));

								loopBoundMaxLoadInst = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
								loopBoundMaxLoadInst.addReg(loopMaxboundReg, RegState::Define);
								loopBoundMaxLoadInst.addReg(registerForGlobalAddr);
								loopBoundMaxLoadInst.addImm(0);
							}

							LIS->InsertMachineInstrInMaps(loopBoundMaxLoadInst.operator ->());
							allInstructionsOfRegion.push_back(make_pair(loopBoundMaxLoadInst.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
							startInstr.push_back(loopBoundMaxLoadInst.operator ->());

							indVar = REDEFINE::t2;
							MachineInstrBuilder inductionVarInit;
							if (isa<ConstantInt>(min)) {
								inductionVarInit = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
								inductionVarInit.addReg(indVar, RegState::Define);
								inductionVarInit.addReg(virtualReg);
								inductionVarInit.addImm(((ConstantInt*) min)->getZExtValue());
							} else {
								int immediate = 0;
								if (!parentModule->getGlobalList().empty()) {
									for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
										const GlobalVariable *globalVar = &*globalArgItr;
										if (globalVar == min) {
											break;
										}
										immediate += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
									}
								}
								string globalAddressString = "ga#";
								globalAddressString.append(itostr(immediate));
								MCSymbol* gaSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(globalAddressString));

								unsigned registerForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
								MachineInstrBuilder movimm = BuildMI(*lastBB, lastBB->end(), BB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR)).addReg(registerForGlobalAddr, RegState::Define).addSym(gaSymbol);
								LIS->InsertMachineInstrInMaps(movimm);
//								LIS->addLiveRangeToEndOfBlock(registerForGlobalAddr, movimm.operator ->());
								allInstructionsOfRegion.push_back(make_pair(movimm.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));

								inductionVarInit = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
								inductionVarInit.addReg(indVar, RegState::Define);
								inductionVarInit.addReg(registerForGlobalAddr);
								inductionVarInit.addImm(REDEFINE::zero);
							}
							LIS->InsertMachineInstrInMaps(inductionVarInit);
//							LIS->addLiveRangeToEndOfBlock(indVar, inductionVarInit.operator ->());
							allInstructionsOfRegion.push_back(make_pair(inductionVarInit.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));

							unsigned startReg = REDEFINE::t3;
							MachineInstrBuilder startRegInst = BuildMI(*lastBB, lastBB->end(), BB->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI)).addReg(startReg, RegState::Define).addReg(virtualReg).addImm(0);
							LIS->InsertMachineInstrInMaps(startRegInst);
							allInstructionsOfRegion.push_back(make_pair(startRegInst.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));

							//	Add jump from indvar to loopstart
							MachineInstrBuilder jumpInstr = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::JAL));
							jumpInstr.addMBB(loopStart);
							LIS->InsertMachineInstrInMaps(jumpInstr);
							allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));

							for (int i = 1; i < ceCount; i++) {
								//Add jump from indvar to loopstart
								MachineInstrBuilder jumpInstr = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::JAL));
								jumpInstr.addMBB(loopStart);
								LIS->InsertMachineInstrInMaps(jumpInstr);
								allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
								startInstr.push_back(jumpInstr.operator ->());

							}

							if (firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
								firstInstructionsInBB.insert(make_pair(lastBB, startInstr));
							}

							MachineInstrBuilder branchInstr = BuildMI(*loopStart, loopStart->end(), dl, TII->get(REDEFINE::BGE));
							branchInstr.addMBB(loopEnd);
							branchInstr.addReg(indVar);
							branchInstr.addReg(loopMaxboundReg);
							LIS->InsertMachineInstrInMaps(branchInstr);
							allInstructionsOfRegion.push_back(make_pair(branchInstr.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
							loopStartInstructions.push_back(branchInstr.operator ->());

//							MachineInstrBuilder fallback = BuildMI(*loopStart, loopStart->end(), dl, TII->get(REDEFINE::JAL));
//							fallback.addMBB(loopBody);
//							LIS->InsertMachineInstrInMaps(fallback);
//							allInstructionsOfRegion.push_back(make_pair(fallback.operator->(), make_pair(0, insertPosition++)));

							for (int i = 1; i < ceCount; i++) {
								//Add jump from indvar to loopstart
								MachineInstrBuilder jumpInstr = BuildMI(*loopStart, loopStart->end(), dl, TII->get(REDEFINE::JAL));
								jumpInstr.addMBB(loopEnd);
								LIS->InsertMachineInstrInMaps(jumpInstr);
								allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
								loopStartInstructions.push_back(jumpInstr.operator ->());

							}
							firstInstructionsInBB.insert(make_pair(loopStart, loopStartInstructions));

							for (int i = 1; i < ceCount; i++) {
								//Add jump from indvar to loopstart
								MachineInstrBuilder jumpInstr = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::JAL));
								jumpInstr.addMBB(loopStart);
								LIS->InsertMachineInstrInMaps(jumpInstr);
								allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
								loopBodyInstructions[i] = jumpInstr.operator ->();

							}

							lastBB = loopBody;
							lastInstruction = loopBody->end();
							virtualReg = startReg;
						}
						fdelete = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::FDELETE)).addReg(virtualReg);
						if (lastBB != BB && loopBodyInstructions.empty()) {
							loopBodyInstructions.push_back(fdelete.operator ->());
						} else if (firstInstructionOfpHyperOpInRegion[0] == 0) {
							firstInstructionOfpHyperOpInRegion[0] = fdelete.operator llvm::MachineInstr *();
						}

						if (parentItr->first->getContextFrameAddress()->getInRange()) {
							list<MachineInstr*> insertedInstructions;
							MachineInstrBuilder inductionVarIncrement = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::ADDI));
							inductionVarIncrement.addReg(indVar, RegState::Define);
							inductionVarIncrement.addReg(indVar);
							inductionVarIncrement.addImm((((ConstantInt*) parentItr->first->getContextFrameAddress()->getStride())->getZExtValue()));
							LIS->InsertMachineInstrInMaps(inductionVarIncrement);
							insertedInstructions.push_back(inductionVarIncrement.operator ->());

							MachineInstrBuilder regBaseIncrement = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::ADDI));
							regBaseIncrement.addReg(virtualReg, RegState::Define);
							regBaseIncrement.addReg(virtualReg);
							regBaseIncrement.addImm(frameSize * 4);
							LIS->InsertMachineInstrInMaps(regBaseIncrement);
							insertedInstructions.push_back(regBaseIncrement.operator ->());

							MachineInstrBuilder loopbodyJump = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::JAL));
							loopbodyJump.addMBB(loopStart);
							LIS->InsertMachineInstrInMaps(loopbodyJump);
							insertedInstructions.push_back(loopbodyJump.operator ->());

							for (auto insertItr = insertedInstructions.begin(); insertItr != insertedInstructions.end(); insertItr++) {
								allInstructionsOfRegion.push_back(make_pair(*insertItr, make_pair(0, insertPosition++)));
							}
						}

						if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end() && !loopBodyInstructions.empty()) {
							firstInstructionsInBB.insert(make_pair(lastBB, loopBodyInstructions));
						}
						LIS->getSlotIndexes()->insertMachineInstrInMaps(fdelete.operator llvm::MachineInstr *());
						allInstructionsOfRegion.push_back(make_pair(fdelete.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
						break;
					}
				}
			}
			lastBB = &BB->getParent()->back();
			lastInstruction = lastBB->end();
		}
	}

	DEBUG(dbgs() << "Adding localref sw instructions to hyperOp " << hyperOp->asString() << "\n");
	for (map<HyperOpEdge*, HyperOp*>::iterator childItr = hyperOp->ChildMap.begin(); childItr != hyperOp->ChildMap.end(); childItr++) {
		HyperOpEdge* edge = childItr->first;
		HyperOp* consumer = childItr->second;
		list<MachineInstr*> insertedInstructions;
		MachineBasicBlock* loopStart;
		MachineBasicBlock* loopBody;
		MachineBasicBlock* loopEnd;
		unsigned targetCE = 0;
		unsigned indVar;
		vector<MachineInstr*> loopBodyInstructions;
		loopBodyInstructions.reserve(ceCount);
		for (int i = 0; i < ceCount; i++) {
			loopBodyInstructions[i] = NULL;
		}

		if (edge->getType() == HyperOpEdge::LOCAL_REFERENCE) {
			unsigned registerContainingConsumerBase;
			//TODO: Target ce is always set to some value in the next loop snippet because we are talking dependences here in mem form, this would change in reg form
			for (unsigned i = 0; i < ceCount; i++) {
				vector<const Value*> memoryLocationsAccessed = memoryLocationsAccessedInCE[i];
				for (unsigned j = 0; j < memoryLocationsAccessed.size(); j++) {
					if (memoryLocationsAccessed[j] == edge->getValue()) {
						targetCE = i;
						break;
					}
				}
			}

			if (consumer->getImmediateDominator() != hyperOp) {
				//Just use the physical register that's mapped to the consumer hyperOp
				for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = hyperOp->ParentMap.begin(); parentItr != hyperOp->ParentMap.end(); parentItr++) {
					if (parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR && parentItr->first->getContextFrameAddress() == consumer) {
						//Get the slot to read from which translates to a register anyway
						unsigned physicalReg = REDEFINEphysRegs[parentItr->first->getPositionOfContextSlot()];
						if (!MF.getRegInfo().isLiveIn(physicalReg)) {
							registerContainingConsumerBase = MF.addLiveIn(physicalReg, TRI->getMinimalPhysRegClass(physicalReg));
//							lastBB.addLiveIn(physicalReg);
							for (MachineFunction::iterator bbItr = MF.begin(); bbItr != MF.end(); bbItr++) {
								bbItr->addLiveIn(physicalReg);
							}
							MF.getRegInfo().setRegAllocationHint(registerContainingConsumerBase, 0, physicalReg);
							//Emit copy
							MachineInstrBuilder copy = BuildMI(*lastBB, lastInstruction, dl, TII->get(TargetOpcode::COPY)).addReg(registerContainingConsumerBase, RegState::Define).addReg(physicalReg);
							LIS->getSlotIndexes()->insertMachineInstrInMaps(copy.operator llvm::MachineInstr *());
							LIS->addLiveRangeToEndOfBlock(registerContainingConsumerBase, copy.operator ->());
							allInstructionsOfRegion.push_back(make_pair(copy.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
							registerContainingHyperOpFrameAddressAndCEWithFalloc.insert(make_pair(consumer, make_pair(registerContainingConsumerBase, 0)));
							LIS->computeLiveInRegUnits();
						} else {
							registerContainingConsumerBase = MF.getRegInfo().getLiveInVirtReg(physicalReg);
						}
						if (targetCE != 0 && regContainingMemFrameBaseAddress.find(consumer) == regContainingMemFrameBaseAddress.end()) {
							//Add writepm-dreadpm pair
							unsigned sourceCEContainingFrameAddress = 0;
							//Load the base scratchpad address to a register in the producer CE for the first time
							if (registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE] == -1) {
								MachineInstrBuilder sourceLui = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LUI));
								unsigned sourceSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
								sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
								sourceLui.addImm(targetCE);
								LIS->getSlotIndexes()->insertMachineInstrInMaps(sourceLui.operator llvm::MachineInstr *());
								LIS->addLiveRangeToEndOfBlock(sourceSpAddressRegister, sourceLui.operator ->());
								allInstructionsOfRegion.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(sourceCEContainingFrameAddress, insertPosition++)));

								registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE] = (int) sourceSpAddressRegister;
								if (firstInstructionOfpHyperOpInRegion[sourceCEContainingFrameAddress] == 0) {
									firstInstructionOfpHyperOpInRegion[sourceCEContainingFrameAddress] = sourceLui.operator llvm::MachineInstr *();
								}
							}

							//Add a writepm and readpm instruction pair to communicate the address
							MachineInstrBuilder writepm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::WRITEPM));
							writepm.addReg(registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE]);
							writepm.addReg(registerContainingConsumerBase);
							writepm.addImm(faninOfHyperOp[targetCE]);

							allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(sourceCEContainingFrameAddress, insertPosition++)));
							LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());

							//Load the base scratchpad address to a register in the consumer CE the first time
							if (registerContainingBaseAddress[targetCE][targetCE] == -1) {
								MachineInstrBuilder targetLui = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LUI));
								unsigned targetSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
								targetLui.addReg(targetSpAddressRegister, RegState::Define);
								//TODO May need changing later
								targetLui.addImm(targetCE);
								LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());
								allInstructionsOfRegion.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
								registerContainingBaseAddress[targetCE][targetCE] = (int) targetSpAddressRegister;

								if (consumer->getInRange() && loopBodyInstructions[targetCE] == NULL) {
									loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, targetLui.operator llvm::MachineInstr *());
								} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
									firstInstructionOfpHyperOpInRegion[targetCE] = targetLui.operator llvm::MachineInstr *();
								}
							}

							registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
							MachineInstrBuilder readpm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::DREADPM));
							readpm.addReg(registerContainingConsumerBase, RegState::Define);
							readpm.addReg(registerContainingBaseAddress[targetCE][targetCE]);
							readpm.addImm(faninOfHyperOp[targetCE]);

							allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
							pHopInteractionGraph.push_back(make_pair(writepm.operator ->(), readpm.operator ->()));
							LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
							LIS->addLiveRangeToEndOfBlock(registerContainingConsumerBase, readpm.operator ->());
						}
						break;
					}
				}
			} else {
				if (!consumer->isStaticHyperOp()) {
					//Load the base scratchpad address to a register in the consumer CE the first time
//						if (registerContainingBaseAddress[targetCE][spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].first] == -1) {
					MachineInstrBuilder targetLui = 	BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LUI));
					unsigned targetSpAddressRegister = REDEFINE::t3;
					targetLui.addReg(targetSpAddressRegister, RegState::Define);
					//TODO May need changing later
					targetLui.addImm(currentCE);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
//							registerContainingBaseAddress[targetCE][spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].first] = (int) targetSpAddressRegister;
//						}

					if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
						vector<MachineInstr*> startInstructions;
						startInstructions.push_back(targetLui.operator ->());
						firstInstructionsInBB.insert(make_pair(lastBB, startInstructions));
					} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
						firstInstructionOfpHyperOpInRegion[targetCE] = targetLui.operator llvm::MachineInstr *();
					}
					if (!consumer->getInRange()) {
						registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder readpm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::READPM));
						readpm.addReg(registerContainingConsumerBase, RegState::Define);
						readpm.addReg(targetSpAddressRegister);
						readpm.addImm(spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].second);
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
					}else{
						errs()<<"for consumer hop "<<consumer->getFunction()->getName()<<", imm value:"<<(spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].second)<<"\n";
						MachineInstrBuilder readpm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
						readpm.addReg(REDEFINE::t1, RegState::Define);
						readpm.addReg(REDEFINE::zero);
						readpm.addImm(spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].second);
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
					}
				} else {
					registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder addi = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
					addi.addReg(registerContainingConsumerBase, RegState::Define);
					addi.addReg(REDEFINE::zero);
					addi.addImm(REDEFINEUtils::getHyperOpId(consumer));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
					if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
						vector<MachineInstr*> startInstructions;
						startInstructions.push_back(addi.operator ->());
						firstInstructionsInBB.insert(make_pair(lastBB, startInstructions));
					} else if (firstInstructionOfpHyperOpInRegion[currentCE] == 0) {
						firstInstructionOfpHyperOpInRegion[currentCE] = addi.operator llvm::MachineInstr *();
					}
					allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				}
			}

			if (consumer->getInRange()) {
				Value* locationContainingCount;
				//TODO support variables instead of constants
				Value* min = consumer->getRangeLowerBound();
				Value* max = consumer->getRangeUpperBound();
				Value* stride = consumer->getStride();

				//Add loop constructs around everything that follows
				loopStart = lastBB->getParent()->CreateMachineBasicBlock();
				lastBB->getParent()->push_back(loopStart);
				lastBB->addSuccessor(loopStart);
				LIS->insertMBBInMaps(loopStart);
				loopBody = lastBB->getParent()->CreateMachineBasicBlock();
				lastBB->getParent()->push_back(loopBody);
				LIS->insertMBBInMaps(loopBody);
				loopStart->addSuccessor(loopBody);
				loopBody->addSuccessor(loopStart);
				vector<MachineInstr*> loopStartInstructions;
				vector<MachineInstr*> loopEndInstructions;

				loopEnd = lastBB->getParent()->CreateMachineBasicBlock();
				lastBB->getParent()->push_back(loopEnd);
				loopStart->addSuccessor(loopEnd);
				LIS->insertMBBInMaps(loopEnd);

				vector<MachineInstr*> startInstr;

				for (int i = 0; i < targetCE; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopStart);
					LIS->InsertMachineInstrInMaps(jumpInstr);
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
					startInstr.push_back(jumpInstr.operator ->());

				}

				//load trip count from memory location onto a register
				unsigned loopMaxboundReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder loopBoundMaxLoadInst;
				if (isa<ConstantInt>(max)) {
					loopBoundMaxLoadInst = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
					loopBoundMaxLoadInst.addReg(loopMaxboundReg, RegState::Define);
					loopBoundMaxLoadInst.addReg(REDEFINE::zero);
					loopBoundMaxLoadInst.addImm(((ConstantInt*) max)->getZExtValue());
					startInstr.push_back(loopBoundMaxLoadInst.operator ->());
				} else {
					int immediate = 0;
					if (!parentModule->getGlobalList().empty()) {
						for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
							const GlobalVariable *globalVar = &*globalArgItr;
							if (globalVar == max) {
								break;
							}
							immediate += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
						}
					}
					string globalAddressString = "ga#";
					globalAddressString.append(itostr(immediate));
					MCSymbol* gaSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(globalAddressString));

					unsigned registerForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder movimm = BuildMI(*lastBB, lastBB->end(), BB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR)).addReg(registerForGlobalAddr, RegState::Define).addSym(gaSymbol);
					LIS->InsertMachineInstrInMaps(movimm);
					allInstructionsOfRegion.push_back(make_pair(movimm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

					loopBoundMaxLoadInst = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
					loopBoundMaxLoadInst.addReg(loopMaxboundReg, RegState::Define);
					loopBoundMaxLoadInst.addReg(registerForGlobalAddr);
					loopBoundMaxLoadInst.addImm(0);
					startInstr.push_back(movimm.operator ->());
				}

				LIS->InsertMachineInstrInMaps(loopBoundMaxLoadInst.operator ->());
				allInstructionsOfRegion.push_back(make_pair(loopBoundMaxLoadInst.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

				indVar = REDEFINE::t2;
				MachineInstrBuilder inductionVarInit;
				if (isa<ConstantInt>(min)) {
					inductionVarInit = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
					inductionVarInit.addReg(indVar, RegState::Define);
					inductionVarInit.addReg(REDEFINE::zero);
					inductionVarInit.addImm(((ConstantInt*) min)->getZExtValue());
				} else {
					int immediate = 0;
					if (!parentModule->getGlobalList().empty()) {
						for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
							const GlobalVariable *globalVar = &*globalArgItr;
							if (globalVar == min) {
								break;
							}
							immediate += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
						}
					}
					string globalAddressString = "ga#";
					globalAddressString.append(itostr(immediate));
					MCSymbol* gaSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(globalAddressString));

					unsigned registerForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder movimm = BuildMI(*lastBB, lastBB->end(), BB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR)).addReg(registerForGlobalAddr, RegState::Define).addSym(gaSymbol);
					LIS->InsertMachineInstrInMaps(movimm);
//						LIS->addLiveRangeToEndOfBlock(registerForGlobalAddr, movimm.operator ->());
					allInstructionsOfRegion.push_back(make_pair(movimm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

					inductionVarInit = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
					inductionVarInit.addReg(indVar, RegState::Define);
					inductionVarInit.addReg(registerForGlobalAddr);
					inductionVarInit.addImm(0);
				}
				LIS->InsertMachineInstrInMaps(inductionVarInit);
				allInstructionsOfRegion.push_back(make_pair(inductionVarInit.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

				//	Add jump from indvar to loopstart
				MachineInstrBuilder jumpInstr = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::JAL));
				jumpInstr.addMBB(loopStart);
				LIS->InsertMachineInstrInMaps(jumpInstr);
				allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

				for (int i = targetCE + 1; i < ceCount; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopStart);
					LIS->InsertMachineInstrInMaps(jumpInstr);
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
					startInstr.push_back(jumpInstr.operator ->());

				}

				if (firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
					firstInstructionsInBB.insert(make_pair(lastBB, startInstr));
				}

				for (int i = 0; i < targetCE; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*loopStart, loopStart->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopStart);
					LIS->InsertMachineInstrInMaps(jumpInstr);
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));

				}

				MachineInstrBuilder branchInstr = BuildMI(*loopStart, loopStart->end(), dl, TII->get(REDEFINE::BGE));
				branchInstr.addMBB(loopEnd);
				branchInstr.addReg(indVar);
				branchInstr.addReg(loopMaxboundReg);
				LIS->InsertMachineInstrInMaps(branchInstr);
				allInstructionsOfRegion.push_back(make_pair(branchInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				loopStartInstructions.push_back(branchInstr.operator ->());

//				MachineInstrBuilder fallback = BuildMI(*loopStart, loopStart->end(), dl, TII->get(REDEFINE::JAL));
//				fallback.addMBB(loopBody);
//				LIS->InsertMachineInstrInMaps(fallback);
//				allInstructionsOfRegion.push_back(make_pair(fallback.operator->(), make_pair(targetCE, insertPosition++)));

				for (int i = targetCE + 1; i < ceCount; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*loopStart, loopStart->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopEnd);
					LIS->InsertMachineInstrInMaps(jumpInstr);
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
					loopStartInstructions.push_back(jumpInstr.operator ->());

				}
				firstInstructionsInBB.insert(make_pair(loopStart, loopStartInstructions));

				for (int i = 0; i < targetCE; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopStart);
					LIS->InsertMachineInstrInMaps(jumpInstr);
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
					loopBodyInstructions[i] = jumpInstr.operator ->();

				}

				for (int i = targetCE + 1; i < ceCount; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopStart);
					LIS->InsertMachineInstrInMaps(jumpInstr);
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
					loopBodyInstructions[i] = jumpInstr.operator ->();

				}

				lastBB = loopBody;
				lastInstruction = loopBody->end();

				unsigned addInstrReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder addInstr = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADD));
				addInstr.addReg(addInstrReg, RegState::Define).addReg(REDEFINE::t3).addReg(REDEFINE::t1);
				LIS->InsertMachineInstrInMaps(addInstr);
				allInstructionsOfRegion.push_back(make_pair(addInstr.operator->(), make_pair(targetCE, insertPosition++)));

				if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
					loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, addInstr.operator ->());
				} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
					firstInstructionOfpHyperOpInRegion[targetCE] = addInstr.operator ->();
				}

				registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder readpm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::READPM));
				readpm.addReg(registerContainingConsumerBase, RegState::Define);
				readpm.addReg(addInstrReg);
				readpm.addImm(0);
				allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());

			}

			//if local reference, add writes to the local memory of consumer HyperOp and remove the consumer HyperOp's argument
			unsigned frameLocationOfSourceData = 0;
			Type* dataType;
			int arraySize;
			if (isa<AllocaInst>(edge->getValue())) {
				AllocaInst* allocInstr;
				allocInstr = (AllocaInst*) (edge->getValue());
				dataType = allocInstr->getAllocatedType();
				//Get the location of the stack allocated object, starting from 0 because negative offsets from fp contain function arguments
				for (unsigned int i = 0; i < MF.getFrameInfo()->getObjectIndexEnd(); i++) {
					if (MF.getFrameInfo()->getObjectAllocation(i) == allocInstr) {
						break;
					}
					frameLocationOfSourceData += REDEFINEUtils::getSizeOfType(MF.getFrameInfo()->getObjectAllocation(i)->getType());
				}
				arraySize = ((ConstantInt*) allocInstr->getArraySize())->getZExtValue();
			} else if (isa<LoadInst>(edge->getValue())) {
				//Find the alloca instruction that allocates the memory location in the first place
				unsigned argIndex = 0;
				LoadInst* sourceInstr = (LoadInst*) edge->getValue();
				AllocaInst* allocaInstr = (AllocaInst*) hyperOp->loadInstrAndAllocaMap[sourceInstr];
				dataType = allocaInstr->getType();
				arraySize = ((ConstantInt*) allocaInstr->getArraySize())->getZExtValue();
				//Get the location of the stack allocated object in the basic block containing the load instruction and not the alloca instruction because alloca might belong
				//Arguments have negative index and are added in memory locations that succeed the locals of the stack frame
				if (MF.getFrameInfo()->getObjectIndexEnd() > 0) {
					for (unsigned i = 0; i < MF.getFrameInfo()->getObjectIndexEnd(); i++) {
						frameLocationOfSourceData += REDEFINEUtils::getSizeOfType(MF.getFrameInfo()->getObjectAllocation(i)->getType());
					}
				}

				for (auto argItr = MF.getFunction()->arg_begin(); argItr != MF.getFunction()->arg_end(); argItr++) {
					if (argItr == sourceInstr->getOperand(0)) {
						break;
					}
					frameLocationOfSourceData += REDEFINEUtils::getSizeOfType(argItr->getType());
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
			frameLocationOfTargetData += edge->getMemoryOffsetInTargetFrame();
			//Map of primitive data types and their memory locations
			list < pair<Type*, unsigned> > primitiveTypesMap;
			list < pair<Type*, int> > containedTypesForTraversal;
			if (dataType->isArrayTy()) {
				containedTypesForTraversal.push_front(make_pair(dataType, dataType->getArrayNumElements()));
			} else {
				containedTypesForTraversal.push_front(make_pair(dataType, 1));
			}
			unsigned memoryOfType = 0;
			//Find the primitive types of allocatedDataType
			while (!containedTypesForTraversal.empty()) {
				Type* traversingType = containedTypesForTraversal.front().first;
				int typeCount = containedTypesForTraversal.front().second;
				containedTypesForTraversal.pop_front();
				if (!traversingType->isAggregateType()) {
					for (int i = 0; i < typeCount; i++) {
						primitiveTypesMap.push_back(make_pair(traversingType, memoryOfType));
						memoryOfType += (32 / 8);
					}
				} else {
					if (traversingType->isArrayTy()) {
						containedTypesForTraversal.push_back(make_pair(traversingType->getArrayElementType(), traversingType->getArrayNumElements()));
					} else {
						for (unsigned i = 0; i < traversingType->getNumContainedTypes(); i++) {
							containedTypesForTraversal.push_back(make_pair(traversingType->getContainedType(i), 1));
						}
					}
				}
			}

			unsigned registerContainingMemBase;
			//TODO clean up this mess
			unsigned maxGlobalSize = 0;
			if (!parentModule->getGlobalList().empty()) {
				for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
					const GlobalVariable *globalVar = &*globalArgItr;
					maxGlobalSize += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
				}
			}
			string globalAddressString = "ga#";
			globalAddressString.append(itostr(maxGlobalSize));
			MCSymbol* gaSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(globalAddressString));

			string frameSizeString = "fs";
			MCSymbol* frameSizeSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(frameSizeString));

			unsigned registerForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder movimm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::MOVADDR)).addReg(registerForGlobalAddr, RegState::Define).addSym(gaSymbol);
			LIS->getSlotIndexes()->insertMachineInstrInMaps(movimm.operator ->());
			allInstructionsOfRegion.push_back(make_pair(movimm.operator->(), make_pair(targetCE, insertPosition++)));

			if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
				loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, movimm.operator ->());
			} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
				firstInstructionOfpHyperOpInRegion[targetCE] = movimm.operator ->();
			}
			unsigned registerForMulOperand = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder addiForMul = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI)).addReg(registerForMulOperand, RegState::Define).addReg(REDEFINE::zero).addSym(frameSizeSymbol);
			LIS->getSlotIndexes()->insertMachineInstrInMaps(addiForMul.operator ->());
//					LIS->addLiveRangeToEndOfBlock(registerForMulOperand, addiForMul.operator ->());
			allInstructionsOfRegion.push_back(make_pair(addiForMul.operator->(), make_pair(targetCE, insertPosition++)));

			unsigned crId = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder shiftForCRId = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::SRLI)).addReg(crId, RegState::Define).addReg(registerContainingConsumerBase).addImm(SHIFT_FOR_CRID);
			LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForCRId.operator llvm::MachineInstr *());
//					LIS->addLiveRangeToEndOfBlock(crId, shiftForCRId.operator ->());
			allInstructionsOfRegion.push_back(make_pair(shiftForCRId.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

			unsigned shiftedPageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder shiftForPageNumber = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::SRLI)).addReg(shiftedPageNumber, RegState::Define).addReg(registerContainingConsumerBase).addImm(SHIFT_FOR_PAGENUMBER);
			LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForPageNumber.operator llvm::MachineInstr *());
//					LIS->addLiveRangeToEndOfBlock(shiftedPageNumber, shiftForPageNumber.operator ->());
			allInstructionsOfRegion.push_back(make_pair(shiftForPageNumber.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

			unsigned pageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder bitmaskForPageNumber = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ANDI)).addReg(pageNumber, RegState::Define).addReg(shiftedPageNumber).addImm(PAGE_NUMBER_MASK);
			allInstructionsOfRegion.push_back(make_pair(bitmaskForPageNumber.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(bitmaskForPageNumber.operator llvm::MachineInstr *());
			LIS->addLiveRangeToEndOfBlock(pageNumber, bitmaskForPageNumber.operator ->());

			unsigned shiftedFrameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder shiftForFrameNumber = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::SRLI)).addReg(shiftedFrameNumber, RegState::Define).addReg(registerContainingConsumerBase).addImm(SHIFT_FOR_FRAMENUMBER);
			allInstructionsOfRegion.push_back(make_pair(shiftForFrameNumber.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForFrameNumber.operator llvm::MachineInstr *());
//					LIS->addLiveRangeToEndOfBlock(shiftedFrameNumber, shiftForFrameNumber.operator ->());

			unsigned frameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder bitmaskForFrameNumber = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ANDI)).addReg(frameNumber, RegState::Define).addReg(shiftedFrameNumber).addImm(FRAME_NUMBER_MASK);
			allInstructionsOfRegion.push_back(make_pair(bitmaskForFrameNumber.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(bitmaskForFrameNumber.operator llvm::MachineInstr *());
//					LIS->addLiveRangeToEndOfBlock(frameNumber, bitmaskForFrameNumber.operator ->());

			unsigned loadImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder mulOperandForCRBase = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI)).addReg(loadImmInReg, RegState::Define).addReg(REDEFINE::zero).addImm(FRAMES_PER_CR);
			allInstructionsOfRegion.push_back(make_pair(mulOperandForCRBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(mulOperandForCRBase.operator llvm::MachineInstr *());
//					LIS->addLiveRangeToEndOfBlock(loadImmInReg, mulOperandForCRBase.operator ->());

			unsigned crBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder mulForCRBase = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::MUL)).addReg(crBase, RegState::Define).addReg(crId).addReg(loadImmInReg);
			allInstructionsOfRegion.push_back(make_pair(mulForCRBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(mulForCRBase.operator llvm::MachineInstr *());
//					LIS->addLiveRangeToEndOfBlock(crBase, mulForCRBase.operator ->());

			unsigned loadPageImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder mulOperandForPageBase = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI)).addReg(loadPageImmInReg, RegState::Define).addReg(REDEFINE::zero).addImm(FRAMES_PER_PAGE);
			allInstructionsOfRegion.push_back(make_pair(mulOperandForPageBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(mulOperandForPageBase.operator llvm::MachineInstr *());
//					LIS->addLiveRangeToEndOfBlock(loadPageImmInReg, mulOperandForPageBase.operator ->());

			unsigned pageBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder mulForPageBase = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::MUL)).addReg(pageBase, RegState::Define).addReg(pageNumber).addReg(loadPageImmInReg);
			allInstructionsOfRegion.push_back(make_pair(mulForPageBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(mulForPageBase.operator llvm::MachineInstr *());
//					LIS->addLiveRangeToEndOfBlock(pageBase, mulForPageBase.operator ->());

			unsigned frameBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder addForFrameBase = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADD)).addReg(frameBase, RegState::Define).addReg(pageBase).addReg(frameNumber);
			allInstructionsOfRegion.push_back(make_pair(addForFrameBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(addForFrameBase.operator llvm::MachineInstr *());
//					LIS->addLiveRangeToEndOfBlock(frameBase, addForFrameBase.operator ->());

			unsigned crIdBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder addForCRBase = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADD)).addReg(crIdBase, RegState::Define).addReg(frameBase).addReg(crBase);
			allInstructionsOfRegion.push_back(make_pair(addForCRBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(addForCRBase.operator llvm::MachineInstr *());
//					LIS->addLiveRangeToEndOfBlock(crIdBase, addForCRBase.operator ->());

			unsigned mulFrameSize = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder mulFrameSizeInstr = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::MUL)).addReg(mulFrameSize, RegState::Define).addReg(crIdBase).addReg(registerForMulOperand);
			allInstructionsOfRegion.push_back(make_pair(mulFrameSizeInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(mulFrameSizeInstr.operator llvm::MachineInstr *());
//					LIS->addLiveRangeToEndOfBlock(mulFrameSize, mulFrameSizeInstr.operator ->());

			unsigned addFrameSize = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
			MachineInstrBuilder addFrameSizeInstr = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADD)).addReg(addFrameSize, RegState::Define).addReg(mulFrameSize).addReg(registerForGlobalAddr);
			allInstructionsOfRegion.push_back(make_pair(addFrameSizeInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
			LIS->getSlotIndexes()->insertMachineInstrInMaps(addFrameSizeInstr.operator llvm::MachineInstr *());

			registerContainingMemBase = addFrameSize;
			for (unsigned allocatedDataIndex = 0; allocatedDataIndex != arraySize; allocatedDataIndex++) {
				//Add a load instruction from memory and store to the memory frame of the consumer HyperOp
				for (list<pair<Type*, unsigned> >::iterator containedPrimitiveItr = primitiveTypesMap.begin(); containedPrimitiveItr != primitiveTypesMap.end(); containedPrimitiveItr++) {
					Type* containedType = containedPrimitiveItr->first;
					if (containedType->isFloatTy()) {
						unsigned floatingPointRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::f32);
						MachineInstrBuilder lw = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::FLW)).addReg(floatingPointRegister, RegState::Define);
						lw.addReg(virtualRegistersForInstAddr[targetCE].second);
						lw.addImm(allocatedDataIndex * memoryOfType + containedPrimitiveItr->second + frameLocationOfSourceData);
						allInstructionsOfRegion.push_back(make_pair(lw.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(lw.operator llvm::MachineInstr *());
						if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
							loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, lw.operator llvm::MachineInstr *());
						} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
							firstInstructionOfpHyperOpInRegion[targetCE] = lw.operator ->();
						}

						MachineInstrBuilder store = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::FSW));
						store.addReg(floatingPointRegister).addReg(registerContainingMemBase).addImm(allocatedDataIndex * memoryOfType + containedPrimitiveItr->second + frameLocationOfTargetData);
						allInstructionsOfRegion.push_back(make_pair(store.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(store.operator llvm::MachineInstr *());
					} else {
						unsigned integerRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder lw = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
						lw.addReg(integerRegister, RegState::Define);
						lw.addReg(REDEFINE::t5);

						lw.addImm(allocatedDataIndex * memoryOfType + containedPrimitiveItr->second + frameLocationOfSourceData);
						allInstructionsOfRegion.push_back(make_pair(lw.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(lw.operator llvm::MachineInstr *());

						if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
							loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, lw.operator llvm::MachineInstr *());
						} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
							firstInstructionOfpHyperOpInRegion[targetCE] = lw.operator ->();
						}

						MachineInstrBuilder store = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::SW));
						store.addReg(integerRegister).addReg(registerContainingMemBase).addImm(allocatedDataIndex * memoryOfType + containedPrimitiveItr->second + frameLocationOfTargetData);
						//Adding edge source instruction for WCET
						edge->setEdgeSource(store.operator ->());
						allInstructionsOfRegion.push_back(make_pair(store.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(store.operator llvm::MachineInstr *());
					}
				}
			}

			if (consumer->getInRange()) {
				MachineInstrBuilder inductionVarIncrement = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::ADDI));
				inductionVarIncrement.addReg(indVar, RegState::Define);
				inductionVarIncrement.addReg(indVar);
				inductionVarIncrement.addImm((((ConstantInt*) consumer->getStride())->getZExtValue()));
				LIS->InsertMachineInstrInMaps(inductionVarIncrement);
				insertedInstructions.push_back(inductionVarIncrement.operator ->());

				MachineInstrBuilder regBaseIncrement = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::ADDI));
				regBaseIncrement.addReg(REDEFINE::t1, RegState::Define);
				regBaseIncrement.addReg(REDEFINE::t1);
				regBaseIncrement.addImm(-4);
				LIS->InsertMachineInstrInMaps(regBaseIncrement);
				insertedInstructions.push_back(regBaseIncrement.operator ->());

				MachineInstrBuilder loopbodyJump = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::JAL));
				loopbodyJump.addMBB(loopStart);
				LIS->InsertMachineInstrInMaps(loopbodyJump);
				insertedInstructions.push_back(loopbodyJump.operator ->());

				for (auto insertItr = insertedInstructions.begin(); insertItr != insertedInstructions.end(); insertItr++) {
					allInstructionsOfRegion.push_back(make_pair(*insertItr, make_pair(targetCE, insertPosition++)));
				}
			}
			if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end() && !loopBodyInstructions.empty()) {
				firstInstructionsInBB.insert(make_pair(lastBB, loopBodyInstructions));
			}
			lastBB = &BB->getParent()->back();
			lastInstruction = lastBB->end();
		}
	}

	DEBUG(dbgs() << "Adding writecm instructions to hyperOp " << hyperOp->asString() << "\n");

	for (map<HyperOpEdge*, HyperOp*>::iterator childItr = hyperOp->ChildMap.begin(); childItr != hyperOp->ChildMap.end(); childItr++) {
		HyperOpEdge* edge = childItr->first;
		HyperOp* consumer = childItr->second;
		list<MachineInstr*> insertedInstructions;
		MachineBasicBlock* loopStart;
		MachineBasicBlock* loopBody;
		MachineBasicBlock* loopEnd = NULL;
		vector<MachineInstr*> loopBodyInstructions;
		loopBodyInstructions.reserve(ceCount);
		for (int i = 0; i < ceCount; i++) {
			loopBodyInstructions[i] = NULL;
		}

		unsigned targetCE = 0;
		unsigned indVar;
		if (edge->getType() == HyperOpEdge::SCALAR || edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF || edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_SCALAR) {
			unsigned registerContainingConsumerBase;
			unsigned targetCE = 0;
			if (edge->getType() == HyperOpEdge::SCALAR) {
				for (unsigned i = 0; i < ceCount; i++) {
					vector<const Value*> memoryLocationsAccessed = memoryLocationsAccessedInCE[i];
					for (unsigned j = 0; j < memoryLocationsAccessed.size(); j++) {
						if (memoryLocationsAccessed[j] == edge->getValue()) {
							targetCE = i;
							break;
						}
					}
				}
			} else if ((edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF) && edge->getContextFrameAddress()->getImmediateDominator() == hyperOp) {
				targetCE = registerContainingHyperOpFrameAddressAndCEWithFalloc[edge->getContextFrameAddress()].second;
			}
			if (consumer->getImmediateDominator() != hyperOp) {
				//Just use the physical register that's mapped to the consumer hyperOp
				for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = hyperOp->ParentMap.begin(); parentItr != hyperOp->ParentMap.end(); parentItr++) {
					if ((parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF) && parentItr->first->getContextFrameAddress() == consumer) {
						if (parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR) {
							//Get the slot to read from which translates to a register anyway
							unsigned physicalReg = REDEFINEphysRegs[parentItr->first->getPositionOfContextSlot()];
							if (!MF.getRegInfo().isLiveIn(physicalReg)) {
								registerContainingConsumerBase = MF.addLiveIn(physicalReg, TRI->getMinimalPhysRegClass(physicalReg));
//							lastBB.addLiveIn(physicalReg);
								for (MachineFunction::iterator bbItr = MF.begin(); bbItr != MF.end(); bbItr++) {
									bbItr->addLiveIn(physicalReg);
								}
								MF.getRegInfo().setRegAllocationHint(registerContainingConsumerBase, 0, physicalReg);
								//Emit copy
								MachineInstrBuilder copy = BuildMI(*lastBB, lastInstruction, dl, TII->get(TargetOpcode::COPY)).addReg(registerContainingConsumerBase, RegState::Define).addReg(physicalReg);
								LIS->getSlotIndexes()->insertMachineInstrInMaps(copy.operator llvm::MachineInstr *());
								allInstructionsOfRegion.push_back(make_pair(copy.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
//										registerContainingHyperOpFrameAddressAndCEWithFalloc.insert(make_pair(consumer, make_pair(registerContainingConsumerBase, 0)));
								LIS->computeLiveInRegUnits();
								if (lastBB != BB && loopBodyInstructions[0] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
									loopBodyInstructions.insert(loopBodyInstructions.begin(), copy.operator llvm::MachineInstr *());
								} else if (firstInstructionOfpHyperOpInRegion[0] == 0) {
									firstInstructionOfpHyperOpInRegion[0] = copy.operator llvm::MachineInstr *();
								}
							} else {
								registerContainingConsumerBase = MF.getRegInfo().getLiveInVirtReg(physicalReg);
							}
						} else {
							registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
							MachineInstrBuilder copy = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
							copy.addReg(registerContainingConsumerBase, RegState::Define);
							copy.addReg(REDEFINE::t5);
							unsigned argCount = 0;
							int frameOffset = 0;
							for (int i = 0; i < MF.getFrameInfo()->getNumObjects(); i++) {
								frameOffset += REDEFINEUtils::getSizeOfType(MF.getFrameInfo()->getObjectAllocation(i)->getAllocatedType());
							}
							frameOffset += edge->getMemoryOffsetInTargetFrame();
							copy.addImm(frameOffset);
							if (lastBB != BB && loopBodyInstructions[0] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
								loopBodyInstructions.insert(loopBodyInstructions.begin(), copy.operator llvm::MachineInstr *());
							} else if (firstInstructionOfpHyperOpInRegion[0] == 0) {
								firstInstructionOfpHyperOpInRegion[0] = copy.operator llvm::MachineInstr *();
							}
							LIS->getSlotIndexes()->insertMachineInstrInMaps(copy.operator llvm::MachineInstr *());
//									LIS->addLiveRangeToEndOfBlock(registerContainingConsumerBase, copy.operator ->());
							allInstructionsOfRegion.push_back(make_pair(copy.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
						}
						if (targetCE != 0) {
							//Add writepm-dreadpm pair
							unsigned sourceCEContainingFrameAddress = 0;
							//Load the base scratchpad address to a register in the producer CE for the first time
							if (registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE] == -1) {
								MachineInstrBuilder sourceLui = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LUI));
								unsigned sourceSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
								sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
								sourceLui.addImm(targetCE);
								LIS->getSlotIndexes()->insertMachineInstrInMaps(sourceLui.operator llvm::MachineInstr *());
								allInstructionsOfRegion.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(sourceCEContainingFrameAddress, insertPosition++)));
								registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE] = (int) sourceSpAddressRegister;

								if (lastBB != BB && loopBodyInstructions[sourceCEContainingFrameAddress] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
									loopBodyInstructions.insert(loopBodyInstructions.begin(), sourceLui.operator llvm::MachineInstr *());
								} else if (firstInstructionOfpHyperOpInRegion[sourceCEContainingFrameAddress] == 0) {
									firstInstructionOfpHyperOpInRegion[sourceCEContainingFrameAddress] = sourceLui.operator llvm::MachineInstr *();
								}

							}

							//Add a writepm and readpm instruction pair to communicate the address
							MachineInstrBuilder writepm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::WRITEPM));
							writepm.addReg(registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE]);
							writepm.addReg(registerContainingConsumerBase);
							writepm.addImm(faninOfHyperOp[targetCE]);

							allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(sourceCEContainingFrameAddress, insertPosition++)));
							LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());

							//Load the base scratchpad address to a register in the consumer CE the first time
							if (registerContainingBaseAddress[targetCE][targetCE] == -1) {
								MachineInstrBuilder targetLui = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LUI));
								unsigned targetSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
								targetLui.addReg(targetSpAddressRegister, RegState::Define);
								//TODO May need changing later
								targetLui.addImm(targetCE);
								LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());
								allInstructionsOfRegion.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
								registerContainingBaseAddress[targetCE][targetCE] = (int) targetSpAddressRegister;

								if (consumer->getInRange() && loopBodyInstructions[targetCE] == NULL) {
									loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, targetLui.operator llvm::MachineInstr *());
								} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
									firstInstructionOfpHyperOpInRegion[targetCE] = targetLui.operator llvm::MachineInstr *();
								}
							}

							registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
							MachineInstrBuilder readpm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::DREADPM));
							readpm.addReg(registerContainingConsumerBase, RegState::Define);
							readpm.addReg(registerContainingBaseAddress[targetCE][targetCE]);
							readpm.addImm(faninOfHyperOp[targetCE]);

							allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
							LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
						}
						break;
					}
				}
			} else {
				if (!consumer->isStaticHyperOp()) {
					//Load the base scratchpad address to a register in the consumer CE the first time
//							if (registerContainingBaseAddress[targetCE][spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].first] == -1) {
					MachineInstrBuilder targetLui = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LUI));
					unsigned targetSpAddressRegister =  REDEFINE::t3;
					targetLui.addReg(targetSpAddressRegister, RegState::Define);
					//TODO May need changing later
					targetLui.addImm(currentCE);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
//								registerContainingBaseAddress[targetCE][spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].first] = (int) targetSpAddressRegister;
//							}
					if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
						vector<MachineInstr*> startInstructions;
						startInstructions.push_back(targetLui.operator ->());
						firstInstructionsInBB.insert(make_pair(lastBB, startInstructions));
					} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
						firstInstructionOfpHyperOpInRegion[targetCE] = targetLui.operator llvm::MachineInstr *();
					}

					if (!consumer->getInRange()) {
						registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder readpm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::READPM));
						readpm.addReg(registerContainingConsumerBase, RegState::Define);
						readpm.addReg(targetSpAddressRegister);
						readpm.addImm(spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].second);
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
					} else {
						errs()<<"for consumer hop "<<consumer->getFunction()->getName()<<", imm value:"<<(spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].second)<<"\n";
						MachineInstrBuilder readpm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
						readpm.addReg(REDEFINE::t1, RegState::Define);
						readpm.addReg(REDEFINE::zero);
						readpm.addImm(spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].second);
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
					}
				} else {
					registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder addi = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
					addi.addReg(registerContainingConsumerBase, RegState::Define);
					addi.addReg(REDEFINE::zero);
					addi.addImm(REDEFINEUtils::getHyperOpId(consumer));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
					if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
						vector<MachineInstr*> startInstructions;
						startInstructions.push_back(addi.operator ->());
						firstInstructionsInBB.insert(make_pair(lastBB, startInstructions));
					} else if (firstInstructionOfpHyperOpInRegion[currentCE] == 0) {
						firstInstructionOfpHyperOpInRegion[currentCE] = addi.operator llvm::MachineInstr *();
					}
					allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				}
			}

			if (consumer->getInRange()) {
				Value* locationContainingCount;
				//TODO support variables instead of constants
				Value* min = consumer->getRangeLowerBound();
				Value* max = consumer->getRangeUpperBound();
				Value* stride = consumer->getStride();

				//Add loop constructs around everything that follows
				loopStart = lastBB->getParent()->CreateMachineBasicBlock();
				lastBB->getParent()->push_back(loopStart);
				lastBB->addSuccessor(loopStart);
				LIS->insertMBBInMaps(loopStart);
				loopBody = lastBB->getParent()->CreateMachineBasicBlock();
				lastBB->getParent()->push_back(loopBody);
				LIS->insertMBBInMaps(loopBody);
				loopStart->addSuccessor(loopBody);
				loopBody->addSuccessor(loopStart);
				vector<MachineInstr*> loopStartInstructions;
				vector<MachineInstr*> loopEndInstructions;

				loopEnd = lastBB->getParent()->CreateMachineBasicBlock();
				lastBB->getParent()->push_back(loopEnd);
				loopStart->addSuccessor(loopEnd);
				LIS->insertMBBInMaps(loopEnd);

				vector<MachineInstr*> startInstr;

				for (int i = 0; i < targetCE; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopStart);
					LIS->InsertMachineInstrInMaps(jumpInstr);
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
					startInstr.push_back(jumpInstr.operator ->());

				}

				//load trip count from memory location onto a register
				unsigned loopMaxboundReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder loopBoundMaxLoadInst;
				if (isa<ConstantInt>(max)) {
					loopBoundMaxLoadInst = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI)).addReg(loopMaxboundReg, RegState::Define).addReg(REDEFINE::zero).addImm(((ConstantInt*) max)->getZExtValue());
					startInstr.push_back(loopBoundMaxLoadInst.operator ->());
				} else {
					int immediate = 0;
					if (!parentModule->getGlobalList().empty()) {
						for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
							const GlobalVariable *globalVar = &*globalArgItr;
							if (globalVar == max) {
								break;
							}
							immediate += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
						}
					}
					string globalAddressString = "ga#";
					globalAddressString.append(itostr(immediate));
					MCSymbol* gaSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(globalAddressString));

					unsigned registerForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder movimm = BuildMI(*lastBB, lastBB->end(), BB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR)).addReg(registerForGlobalAddr, RegState::Define).addSym(gaSymbol);
					LIS->InsertMachineInstrInMaps(movimm);
//							LIS->addLiveRangeToEndOfBlock(registerForGlobalAddr, movimm.operator ->());
					allInstructionsOfRegion.push_back(make_pair(movimm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

					loopBoundMaxLoadInst = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
					loopBoundMaxLoadInst.addReg(loopMaxboundReg, RegState::Define);
					loopBoundMaxLoadInst.addReg(registerForGlobalAddr);
					loopBoundMaxLoadInst.addImm(0);
					startInstr.push_back(movimm.operator ->());
				}

				LIS->InsertMachineInstrInMaps(loopBoundMaxLoadInst.operator ->());
//						LIS->addLiveRangeToEndOfBlock(loopMaxboundReg, loopBoundMaxLoadInst.operator ->());
				allInstructionsOfRegion.push_back(make_pair(loopBoundMaxLoadInst.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

				indVar = REDEFINE::t2;
				MachineInstrBuilder inductionVarInit;
				if (isa<ConstantInt>(min)) {
					inductionVarInit = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
					inductionVarInit.addReg(indVar, RegState::Define);
					inductionVarInit.addReg(REDEFINE::zero);
					inductionVarInit.addImm(((ConstantInt*) min)->getZExtValue());
				} else {
					int immediate = 0;
					if (!parentModule->getGlobalList().empty()) {
						for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
							const GlobalVariable *globalVar = &*globalArgItr;
							if (globalVar == min) {
								break;
							}
							immediate += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
						}
					}
					string globalAddressString = "ga#";
					globalAddressString.append(itostr(immediate));
					MCSymbol* gaSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(globalAddressString));

					unsigned registerForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder movimm = BuildMI(*lastBB, lastBB->end(), BB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR)).addReg(registerForGlobalAddr, RegState::Define).addSym(gaSymbol);
					LIS->InsertMachineInstrInMaps(movimm);
//							LIS->addLiveRangeToEndOfBlock(registerForGlobalAddr, movimm.operator ->());
					allInstructionsOfRegion.push_back(make_pair(movimm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

					inductionVarInit = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
					inductionVarInit.addReg(indVar, RegState::Define);
					inductionVarInit.addReg(registerForGlobalAddr);
					inductionVarInit.addImm(REDEFINE::zero);
				}
				LIS->InsertMachineInstrInMaps(inductionVarInit);
				allInstructionsOfRegion.push_back(make_pair(inductionVarInit.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

				//	Add jump from indvar to loopstart
				MachineInstrBuilder jumpInstr = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::JAL));
				jumpInstr.addMBB(loopStart);
				LIS->InsertMachineInstrInMaps(jumpInstr);
				allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

				for (int i = targetCE + 1; i < ceCount; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopStart);
					LIS->InsertMachineInstrInMaps(jumpInstr);
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
					startInstr.push_back(jumpInstr.operator ->());

				}

				if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
					firstInstructionsInBB.insert(make_pair(lastBB, startInstr));
				}

				for (int i = 0; i < targetCE; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*loopStart, loopStart->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopStart);
					LIS->InsertMachineInstrInMaps(jumpInstr);
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));

				}

				MachineInstrBuilder branchInstr = BuildMI(*loopStart, loopStart->end(), dl, TII->get(REDEFINE::BGE));
				branchInstr.addMBB(loopEnd);
				branchInstr.addReg(indVar);
				branchInstr.addReg(loopMaxboundReg);
				LIS->InsertMachineInstrInMaps(branchInstr);
				allInstructionsOfRegion.push_back(make_pair(branchInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				loopStartInstructions.push_back(branchInstr.operator ->());

//				MachineInstrBuilder fallback = BuildMI(*loopStart, loopStart->end(), dl, TII->get(REDEFINE::JAL));
//				fallback.addMBB(loopBody);
//				LIS->InsertMachineInstrInMaps(fallback);
//				allInstructionsOfRegion.push_back(make_pair(fallback.operator->(), make_pair(targetCE, insertPosition++)));

				for (int i = targetCE + 1; i < ceCount; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*loopStart, loopStart->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopEnd);
					LIS->InsertMachineInstrInMaps(jumpInstr);
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
					loopStartInstructions.push_back(jumpInstr.operator ->());

				}
				firstInstructionsInBB.insert(make_pair(loopStart, loopStartInstructions));

				for (int i = 0; i < targetCE; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopStart);
					LIS->InsertMachineInstrInMaps(jumpInstr);
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
					loopBodyInstructions[i] = jumpInstr.operator ->();

				}

				for (int i = targetCE + 1; i < ceCount; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopStart);
					LIS->InsertMachineInstrInMaps(jumpInstr);
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
					loopBodyInstructions[i] = jumpInstr.operator ->();

				}

				lastBB = loopBody;
				lastInstruction = loopBody->end();

				unsigned addInstrReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder addInstr = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADD));
				addInstr.addReg(addInstrReg, RegState::Define).addReg(REDEFINE::t3).addReg(REDEFINE::t1);
				LIS->InsertMachineInstrInMaps(addInstr);
				allInstructionsOfRegion.push_back(make_pair(addInstr.operator->(), make_pair(targetCE, insertPosition++)));

				if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
					loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, addInstr.operator ->());
				} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
					firstInstructionOfpHyperOpInRegion[targetCE] = addInstr.operator ->();
				}

				registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder readpm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::READPM));
				readpm.addReg(registerContainingConsumerBase, RegState::Define);
				readpm.addReg(addInstrReg);
				readpm.addImm(0);
				allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
			}

			if (edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF || edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_SCALAR) {
				errs() << "is it because edge is instance type?" << (edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_SCALAR) << ", slot kya hai?" << edge->getPositionOfContextSlot() << "with fname " << edge->getContextFrameAddress()->getFunction()->getName() << "\n";
				//If producer itself is a range hop, consumer cant be a range hop, forward from one of the producer hops
				assert((!hyperOp->getInRange() || !consumer->getInRange()) && "producer and consumer cant be both range hops");
				//Reserve one register for range hops thats r1 for id in the range itself
				unsigned registerContainingData;
				if (edge->getContextFrameAddress()->getImmediateDominator() == hyperOp) {
					//Load the base scratchpad address to a register in the consumer CE the first time
					MachineInstrBuilder targetLui = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LUI));
					unsigned targetSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					targetLui.addReg(targetSpAddressRegister, RegState::Define);
					//TODO May need changing later
					targetLui.addImm(currentCE);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end() && loopBodyInstructions[targetCE] == NULL) {
						vector<MachineInstr*> startInstructions;
						startInstructions.push_back(targetLui.operator ->());
						firstInstructionsInBB.insert(make_pair(lastBB, startInstructions));
					} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
						firstInstructionOfpHyperOpInRegion[targetCE] = targetLui.operator llvm::MachineInstr *();
					}

					registerContainingData = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder readpm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::READPM));
					readpm.addReg(registerContainingData, RegState::Define);
					readpm.addReg(targetSpAddressRegister);
					readpm.addImm(spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].second);
					allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());

				} else {
					//The address was forwarded to the current HyperOp
					for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = hyperOp->ParentMap.begin(); parentItr != hyperOp->ParentMap.end(); parentItr++) {
						if ((parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF) && parentItr->first->getContextFrameAddress() == edge->getContextFrameAddress()) {
							//Get the slot to read from which translates to a register anyway
							if (parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR) {
								//Scalar argument
								unsigned physicalReg = REDEFINEphysRegs[parentItr->first->getPositionOfContextSlot()];
								if (!MF.getRegInfo().isLiveIn(physicalReg)) {
									registerContainingData = MF.addLiveIn(physicalReg, TRI->getMinimalPhysRegClass(physicalReg));
//								lastBB.addLiveIn(physicalReg);
									for (MachineFunction::iterator bbItr = MF.begin(); bbItr != MF.end(); bbItr++) {
										bbItr->addLiveIn(physicalReg);
									}
									MF.getRegInfo().setRegAllocationHint(registerContainingData, 0, physicalReg);
//								MRI.addLiveIn(physicalReg, registerContainingData);
									//Emit copy
									MachineInstrBuilder copy = BuildMI(*lastBB, lastInstruction, dl, TII->get(TargetOpcode::COPY)).addReg(registerContainingData, RegState::Define).addReg(physicalReg);
									LIS->getSlotIndexes()->insertMachineInstrInMaps(copy.operator llvm::MachineInstr *());
									allInstructionsOfRegion.push_back(make_pair(copy.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
//											registerContainingHyperOpFrameAddressAndCEWithFalloc.insert(make_pair(edge->getContextFrameAddress(), make_pair(registerContainingData, 0)));
//											LIS->addLiveRangeToEndOfBlock(registerContainingData, copy.operator ->());
//										LIS->computeLiveInRegUnits();

									if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
										loopBodyInstructions.insert(loopBodyInstructions.begin(), copy.operator ->());
									} else if (firstInstructionOfpHyperOpInRegion[0] == NULL) {
										firstInstructionOfpHyperOpInRegion[0] = copy.operator ->();
									}
								} else {
									registerContainingData = MF.getRegInfo().getLiveInVirtReg(physicalReg);
								}
							} else {
								//Insert a load instruction from memory to copy the address to register
								registerContainingData = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
								MachineInstrBuilder copy = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
								copy.addReg(registerContainingData, RegState::Define);
								copy.addReg(REDEFINE::t5);
								int frameOffset = 0;
								for (int i = 0; i < MF.getFrameInfo()->getNumObjects(); i++) {
									frameOffset += REDEFINEUtils::getSizeOfType(MF.getFrameInfo()->getObjectAllocation(i)->getAllocatedType());
								}
								frameOffset += edge->getMemoryOffsetInTargetFrame();
								copy.addImm(frameOffset);
								LIS->getSlotIndexes()->insertMachineInstrInMaps(copy.operator llvm::MachineInstr *());
//										LIS->addLiveRangeToEndOfBlock(registerContainingData, copy.operator llvm::MachineInstr *());
								allInstructionsOfRegion.push_back(make_pair(copy.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
//										registerContainingHyperOpFrameAddressAndCEWithFalloc.insert(make_pair(edge->getContextFrameAddress(), make_pair(registerContainingData, 0)));
							}
							if (targetCE != registerContainingHyperOpFrameAddressAndCEWithFalloc[edge->getContextFrameAddress()].second) {
								//Add writepm-dreadpm pair
								unsigned sourceCEContainingFrameAddress = registerContainingHyperOpFrameAddressAndCEWithFalloc[edge->getContextFrameAddress()].second;
								//Load the base scratchpad address to a register in the producer CE for the first time
								if (registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE] == -1) {
									MachineInstrBuilder sourceLui = BuildMI(*lastBB, lastInstruction, lastBB->begin()->getDebugLoc(), TII->get(REDEFINE::LUI));
									unsigned sourceSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
									sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
									sourceLui.addImm(targetCE);
									LIS->getSlotIndexes()->insertMachineInstrInMaps(sourceLui.operator llvm::MachineInstr *());
									allInstructionsOfRegion.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(sourceCEContainingFrameAddress, insertPosition++)));
									registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE] = (int) sourceSpAddressRegister;

									if (consumer->getInRange() && loopBodyInstructions[sourceCEContainingFrameAddress] == NULL) {
										loopBodyInstructions.insert(loopBodyInstructions.begin() + sourceCEContainingFrameAddress, sourceLui.operator llvm::MachineInstr *());
									} else if (firstInstructionOfpHyperOpInRegion[sourceCEContainingFrameAddress] == 0) {
										firstInstructionOfpHyperOpInRegion[sourceCEContainingFrameAddress] = sourceLui.operator llvm::MachineInstr *();
									}
								}

								//Add a writepm and readpm instruction pair to communicate the address
								MachineInstrBuilder writepm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::WRITEPM));
								writepm.addReg(registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE]);
								writepm.addReg(registerContainingData);
								writepm.addImm(faninOfHyperOp[targetCE]);

								allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(sourceCEContainingFrameAddress, insertPosition++)));
								LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());

								//Load the base scratchpad address to a register in the consumer CE the first time
								if (registerContainingBaseAddress[targetCE][targetCE] == -1) {
									MachineInstrBuilder targetLui = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LUI));
									unsigned targetSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
									targetLui.addReg(targetSpAddressRegister, RegState::Define);
									//TODO May need changing later
									targetLui.addImm(targetCE);
									LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());
									allInstructionsOfRegion.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
									registerContainingBaseAddress[targetCE][targetCE] = (int) targetSpAddressRegister;

									if (consumer->getInRange() && loopBodyInstructions[targetCE] == NULL) {
										loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, targetLui.operator llvm::MachineInstr *());
									} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
										firstInstructionOfpHyperOpInRegion[targetCE] = targetLui.operator llvm::MachineInstr *();
									}
								}

								registerContainingData = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
								MachineInstrBuilder readpm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::DREADPM));
								readpm.addReg(registerContainingConsumerBase, RegState::Define);
								readpm.addReg(registerContainingBaseAddress[targetCE][targetCE]);
								readpm.addImm(faninOfHyperOp[targetCE]);
								allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

								if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
									loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, readpm.operator ->());
								} else if (firstInstructionOfpHyperOpInRegion[targetCE] == NULL) {
									firstInstructionOfpHyperOpInRegion[targetCE] = readpm.operator ->();
								}

								pHopInteractionGraph.push_back(make_pair(writepm.operator ->(), readpm.operator ->()));
								LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
//										LIS->addLiveRangeToEndOfBlock(registerContainingConsumerBase, readpm.operator llvm::MachineInstr *());
							}
							break;
						}
					}
				}

				MachineInstrBuilder writeToContextFrame;
				if (edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_SCALAR) {
					writeToContextFrame = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::WRITECM));
					writeToContextFrame.addReg(registerContainingConsumerBase);
					writeToContextFrame.addReg(registerContainingData);
					writeToContextFrame.addImm(0);
				} else if (edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF) {
					//Guard by a conditional if the current hyperop is a range hop
					if (hyperOp->getInRange()) {
						lastBB = generateRangeConditionalInstructions(BB, lastBB, lastInstruction, LIS, TM, TII, currentCE, &insertPosition, &allInstructionsOfRegion, loopEnd, ceCount);
						lastInstruction = lastBB->end();
					}
					writeToContextFrame = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::SW));
					writeToContextFrame.addReg(registerContainingData);

					unsigned registerContainingMemBase;
					unsigned ceContainingMemBase;
					if (regContainingMemFrameBaseAddress.find(consumer) != regContainingMemFrameBaseAddress.end()) {
						bool baseAddrInTarget = false;
						for (auto ceContainingRegBase = regContainingMemFrameBaseAddress[consumer].begin(); ceContainingRegBase != regContainingMemFrameBaseAddress[consumer].end(); ceContainingRegBase++) {
							if (ceContainingRegBase->second == targetCE) {
								ceContainingMemBase = ceContainingRegBase->second;
								registerContainingMemBase = ceContainingRegBase->first;
								baseAddrInTarget = true;
								break;
							}
						}
						if (!baseAddrInTarget) {
							registerContainingMemBase = regContainingMemFrameBaseAddress[consumer].begin()->first;
							ceContainingMemBase = regContainingMemFrameBaseAddress[consumer].begin()->second;
						}
					}
					if (regContainingMemFrameBaseAddress.find(consumer) == regContainingMemFrameBaseAddress.end()) {
						//TODO clean up this mess
						unsigned maxGlobalSize = 0;
						if (!parentModule->getGlobalList().empty()) {
							for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
								const GlobalVariable *globalVar = &*globalArgItr;
								maxGlobalSize += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
							}
						}

						string globalAddressString = "ga#";
						globalAddressString.append(itostr(maxGlobalSize));
						MCSymbol* gaSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(globalAddressString));

						string frameSizeString = "fs";
						MCSymbol* frameSizeSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(frameSizeString));

						unsigned registerForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder movimm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::MOVADDR)).addReg(registerForGlobalAddr, RegState::Define).addSym(gaSymbol);
						LIS->getSlotIndexes()->insertMachineInstrInMaps(movimm.operator ->());
						allInstructionsOfRegion.push_back(make_pair(movimm.operator->(), make_pair(targetCE, insertPosition++)));
						if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
							loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, movimm.operator ->());
						} else if (firstInstructionOfpHyperOpInRegion[targetCE] == NULL) {
							firstInstructionOfpHyperOpInRegion[targetCE] = movimm.operator ->();
						}

						unsigned registerForMulOperand = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder addiForMul = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI)).addReg(registerForMulOperand, RegState::Define).addReg(REDEFINE::zero).addSym(frameSizeSymbol);
						LIS->getSlotIndexes()->insertMachineInstrInMaps(addiForMul.operator ->());
						allInstructionsOfRegion.push_back(make_pair(addiForMul.operator->(), make_pair(targetCE, insertPosition++)));

						unsigned crId = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder shiftForCRId = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::SRLI)).addReg(crId, RegState::Define).addReg(registerContainingConsumerBase).addImm(SHIFT_FOR_CRID);
						allInstructionsOfRegion.push_back(make_pair(shiftForCRId.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForCRId.operator llvm::MachineInstr *());

						unsigned shiftedPageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder shiftForPageNumber = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::SRLI)).addReg(shiftedPageNumber, RegState::Define).addReg(registerContainingConsumerBase).addImm(SHIFT_FOR_PAGENUMBER);
						allInstructionsOfRegion.push_back(make_pair(shiftForPageNumber.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForPageNumber.operator llvm::MachineInstr *());

						unsigned pageNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder bitmaskForPageNumber = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ANDI)).addReg(pageNumber, RegState::Define).addReg(shiftedPageNumber).addImm(PAGE_NUMBER_MASK);
						allInstructionsOfRegion.push_back(make_pair(bitmaskForPageNumber.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(bitmaskForPageNumber.operator llvm::MachineInstr *());

						unsigned shiftedFrameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder shiftForFrameNumber = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::SRLI)).addReg(shiftedFrameNumber, RegState::Define).addReg(registerContainingConsumerBase).addImm(SHIFT_FOR_FRAMENUMBER);
						allInstructionsOfRegion.push_back(make_pair(shiftForFrameNumber.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(shiftForFrameNumber.operator llvm::MachineInstr *());

						unsigned frameNumber = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder bitmaskForFrameNumber = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ANDI)).addReg(frameNumber, RegState::Define).addReg(shiftedFrameNumber).addImm(FRAME_NUMBER_MASK);
						allInstructionsOfRegion.push_back(make_pair(bitmaskForFrameNumber.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(bitmaskForFrameNumber.operator llvm::MachineInstr *());

						unsigned loadImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder mulOperandForCRBase = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI)).addReg(loadImmInReg, RegState::Define).addReg(REDEFINE::zero).addImm(FRAMES_PER_CR);
						allInstructionsOfRegion.push_back(make_pair(mulOperandForCRBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(mulOperandForCRBase.operator llvm::MachineInstr *());

						unsigned crBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder mulForCRBase = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::MUL)).addReg(crBase, RegState::Define).addReg(crId).addReg(loadImmInReg);
						allInstructionsOfRegion.push_back(make_pair(mulForCRBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(mulForCRBase.operator llvm::MachineInstr *());

						unsigned loadPageImmInReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder mulOperandForPageBase = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI)).addReg(loadPageImmInReg, RegState::Define).addReg(REDEFINE::zero).addImm(FRAMES_PER_PAGE);
						allInstructionsOfRegion.push_back(make_pair(mulOperandForPageBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(mulOperandForPageBase.operator llvm::MachineInstr *());

						unsigned pageBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder mulForPageBase = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::MUL)).addReg(pageBase, RegState::Define).addReg(pageNumber).addReg(loadPageImmInReg);
						allInstructionsOfRegion.push_back(make_pair(mulForPageBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(mulForPageBase.operator llvm::MachineInstr *());

						unsigned frameBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder addForFrameBase = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADD)).addReg(frameBase, RegState::Define).addReg(pageBase).addReg(frameNumber);
						allInstructionsOfRegion.push_back(make_pair(addForFrameBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(addForFrameBase.operator llvm::MachineInstr *());

						unsigned crIdBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder addForCRBase = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADD)).addReg(crIdBase, RegState::Define).addReg(frameBase).addReg(crBase);
						allInstructionsOfRegion.push_back(make_pair(addForCRBase.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(addForCRBase.operator llvm::MachineInstr *());

						unsigned mulFrameSize = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder mulFrameSizeInstr = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::MUL)).addReg(mulFrameSize, RegState::Define).addReg(crIdBase).addReg(registerForMulOperand);
						allInstructionsOfRegion.push_back(make_pair(mulFrameSizeInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(mulFrameSizeInstr.operator llvm::MachineInstr *());

						unsigned addFrameSize = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder addFrameSizeInstr = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADD)).addReg(addFrameSize, RegState::Define).addReg(mulFrameSize).addReg(registerForGlobalAddr);
						allInstructionsOfRegion.push_back(make_pair(addFrameSizeInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(addFrameSizeInstr.operator llvm::MachineInstr *());

						list < pair<unsigned, unsigned> > prevRegCEPair;
						prevRegCEPair.push_back(make_pair(addFrameSize, targetCE));
						regContainingMemFrameBaseAddress[consumer] = prevRegCEPair;
						registerContainingMemBase = addFrameSize;
					} else if (ceContainingMemBase != targetCE) {
						//Communicate the mem frame base address to the target ce
						unsigned sourceCEContainingFrameAddress = ceContainingMemBase;
						//Load the base scratchpad address to a register in the producer CE for the first time
						if (registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE] == -1) {
							unsigned sourceSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
							MachineInstrBuilder sourceLui = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LUI));
							sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
							sourceLui.addImm(targetCE);
							LIS->getSlotIndexes()->insertMachineInstrInMaps(sourceLui.operator llvm::MachineInstr *());
//									LIS->addLiveRangeToEndOfBlock(sourceSpAddressRegister, sourceLui.operator ->());
							allInstructionsOfRegion.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(sourceCEContainingFrameAddress, insertPosition++)));
							registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE] = (int) sourceSpAddressRegister;
							if (firstInstructionOfpHyperOpInRegion[sourceCEContainingFrameAddress] == 0) {
								firstInstructionOfpHyperOpInRegion[sourceCEContainingFrameAddress] = sourceLui.operator llvm::MachineInstr *();
							}
						}

						//Add a writepm and readpm instruction pair to communicate the address
						MachineInstrBuilder writepm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::WRITEPM));
						writepm.addReg(registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE]);
						writepm.addReg(registerContainingMemBase);
						writepm.addImm(faninOfHyperOp[targetCE]);

						allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(sourceCEContainingFrameAddress, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());

						//Load the base scratchpad address to a register in the consumer CE the first time
						if (registerContainingBaseAddress[targetCE][targetCE] == -1) {
							unsigned targetSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
							MachineInstrBuilder targetLui = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LUI));
							targetLui.addReg(targetSpAddressRegister, RegState::Define);
							//TODO May need changing later
							targetLui.addImm(targetCE);
							LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());

							allInstructionsOfRegion.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
							registerContainingBaseAddress[targetCE][targetCE] = (int) targetSpAddressRegister;

							if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
								loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, targetLui.operator llvm::MachineInstr *());
							} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
								firstInstructionOfpHyperOpInRegion[targetCE] = targetLui.operator llvm::MachineInstr *();
							}
						}

						unsigned addFrameSize = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder readpm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::DREADPM));
						readpm.addReg(addFrameSize, RegState::Define);
						readpm.addReg(registerContainingBaseAddress[targetCE][targetCE]);
						readpm.addImm(faninOfHyperOp[targetCE]);

						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						pHopInteractionGraph.push_back(make_pair(writepm.operator ->(), readpm.operator ->()));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());

						list < pair<unsigned, unsigned> > prevRegCEPair;
						if (regContainingMemFrameBaseAddress.find(consumer) != regContainingMemFrameBaseAddress.end()) {
							prevRegCEPair = regContainingMemFrameBaseAddress[consumer];
							regContainingMemFrameBaseAddress.erase(consumer);
						}
						prevRegCEPair.push_back(make_pair(addFrameSize, targetCE));
						regContainingMemFrameBaseAddress[consumer] = prevRegCEPair;
						registerContainingMemBase = addFrameSize;
					}

					writeToContextFrame.addReg(registerContainingMemBase);
					unsigned offsetInMemory = 0;
					//Add locals in consumer function to offsetInMemory
					for (Function::iterator funcItr = consumer->getFunction()->begin(); funcItr != consumer->getFunction()->end(); funcItr++) {
						for (BasicBlock::iterator bbItr = funcItr->begin(); bbItr != funcItr->end(); bbItr++) {
							if (isa<AllocaInst>(bbItr)) {
								AllocaInst* targetAllocaInst = cast<AllocaInst>(bbItr);
								offsetInMemory += REDEFINEUtils::getSizeOfType(targetAllocaInst->getAllocatedType());
							}
						}
					}
					offsetInMemory += edge->getMemoryOffsetInTargetFrame();
					writeToContextFrame.addImm(offsetInMemory);

					if (hyperOp->getInRange()) {
						lastBB = &BB->getParent()->back();
						lastInstruction = lastBB->end();
					}
				} else {
					writeToContextFrame = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::WRITECM));
					writeToContextFrame.addReg(registerContainingConsumerBase);
					writeToContextFrame.addReg(registerContainingData);
					writeToContextFrame.addImm(edge->getPositionOfContextSlot() * datawidth);
				}

				if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
					loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, writeToContextFrame.operator llvm::MachineInstr *());
					writeToContextFrame.operator ->()->dump();
				} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
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
				edge->setEdgeSource(writeToContextFrame.operator ->());
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
				MachineInstrBuilder loadInstr = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
				loadInstr.addReg(registerContainingData, RegState::Define);
				loadInstr.addReg(REDEFINE::t5);
				loadInstr.addFrameIndex(objectIndex);
				allInstructionsOfRegion.push_back(make_pair(loadInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(loadInstr.operator llvm::MachineInstr *());

				if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
					loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, loadInstr.operator llvm::MachineInstr *());
				} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
					firstInstructionOfpHyperOpInRegion[targetCE] = loadInstr.operator llvm::MachineInstr *();
				}

				MachineInstrBuilder writeToContextFrame = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::WRITECM));
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
				edge->setEdgeSource(writeToContextFrame.operator ->());
			}
			if (consumer->getInRange()) {
				MachineInstrBuilder inductionVarIncrement = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::ADDI));
				inductionVarIncrement.addReg(indVar, RegState::Define);
				inductionVarIncrement.addReg(indVar);
				inductionVarIncrement.addImm((((ConstantInt*) consumer->getStride())->getZExtValue()));
				LIS->InsertMachineInstrInMaps(inductionVarIncrement);
				insertedInstructions.push_back(inductionVarIncrement.operator ->());

				MachineInstrBuilder regBaseIncrement = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::ADDI));
				regBaseIncrement.addReg(REDEFINE::t1, RegState::Define);
				regBaseIncrement.addReg(REDEFINE::t1);
				regBaseIncrement.addImm(-4);
				LIS->InsertMachineInstrInMaps(regBaseIncrement);
				insertedInstructions.push_back(regBaseIncrement.operator ->());

				MachineInstrBuilder loopbodyJump = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::JAL));
				loopbodyJump.addMBB(loopStart);
				LIS->InsertMachineInstrInMaps(loopbodyJump);
				insertedInstructions.push_back(loopbodyJump.operator ->());

				for (auto insertItr = insertedInstructions.begin(); insertItr != insertedInstructions.end(); insertItr++) {
					allInstructionsOfRegion.push_back(make_pair(*insertItr, make_pair(targetCE, insertPosition++)));
				}
			}

			if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end() && !loopBodyInstructions.empty()) {
				firstInstructionsInBB.insert(make_pair(lastBB, loopBodyInstructions));
			}
			lastBB = &BB->getParent()->back();
			lastInstruction = lastBB->end();
		}
	}

	DEBUG(dbgs() << "Adding writecmp and sync instructions\n");
//Writecmp instructions are added after writecm and local reference writes to ensure that local reference writes do go through before a consumer is predicated for execution
	for (map<HyperOpEdge*, HyperOp*>::iterator childItr = hyperOp->ChildMap.begin(); childItr != hyperOp->ChildMap.end(); childItr++) {
		HyperOpEdge* edge = childItr->first;
		HyperOp* consumer = childItr->second;
		list<MachineInstr*> insertedInstructions;
		MachineBasicBlock* loopStart;
		MachineBasicBlock* loopBody;
		MachineBasicBlock* loopEnd;
		unsigned targetCE = 0;
		unsigned indVar;
		vector<MachineInstr*> loopBodyInstructions;
		loopBodyInstructions.reserve(ceCount);
		for (int i = 0; i < ceCount; i++) {
			loopBodyInstructions[i] = NULL;
		}

		unsigned registerContainingConsumerBase;
		targetCE = currentCE;
		if (edge->getType() == HyperOpEdge::PREDICATE) {
			for (unsigned i = 0; i < ceCount; i++) {
				vector<const Value*> memoryLocationsAccessed = memoryLocationsAccessedInCE[i];
				for (unsigned j = 0; j < memoryLocationsAccessed.size(); j++) {
					if (memoryLocationsAccessed[j] == edge->getValue()) {
						targetCE = i;
						break;
					}
				}
			}
		}
		if (edge->getType() == HyperOpEdge::PREDICATE || edge->getType() == HyperOpEdge::SYNC || edge->getType() == HyperOpEdge::ORDERING) {
			if (consumer->getImmediateDominator() != hyperOp) {
				if (consumer->isStaticHyperOp()) {
					registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder addi = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
					addi.addReg(registerContainingConsumerBase, RegState::Define);
					addi.addReg(REDEFINE::zero);
					addi.addImm(REDEFINEUtils::getHyperOpId(consumer));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
					if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
						vector<MachineInstr*> startInstructions;
						startInstructions.push_back(addi.operator ->());
						firstInstructionsInBB.insert(make_pair(lastBB, startInstructions));
					} else if (firstInstructionOfpHyperOpInRegion[currentCE] == 0) {
						firstInstructionOfpHyperOpInRegion[currentCE] = addi.operator llvm::MachineInstr *();
					}
					allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(currentCE, insertPosition++)));

				} else {
					//Just use the physical register that's mapped to the consumer hyperOp
					for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = hyperOp->ParentMap.begin(); parentItr != hyperOp->ParentMap.end(); parentItr++) {
						if ((parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF) && parentItr->first->getContextFrameAddress() == consumer) {
							//Get the slot to read from which translates to a register anyway
							if (parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR) {
								unsigned physicalReg = REDEFINEphysRegs[parentItr->first->getPositionOfContextSlot()];
								if (!MF.getRegInfo().isLiveIn(physicalReg)) {
									registerContainingConsumerBase = MF.addLiveIn(physicalReg, TRI->getMinimalPhysRegClass(physicalReg));
//							lastBB.addLiveIn(physicalReg);
									for (MachineFunction::iterator bbItr = MF.begin(); bbItr != MF.end(); bbItr++) {
										bbItr->addLiveIn(physicalReg);
									}
									MF.getRegInfo().setRegAllocationHint(registerContainingConsumerBase, 0, physicalReg);
//							MRI.addLiveIn(physicalReg, registerContainingConsumerBase);
									//Emit copy
									MachineInstrBuilder copy = BuildMI(*lastBB, lastInstruction, dl, TII->get(TargetOpcode::COPY)).addReg(registerContainingConsumerBase, RegState::Define).addReg(physicalReg);
									LIS->getSlotIndexes()->insertMachineInstrInMaps(copy.operator llvm::MachineInstr *());
//									LIS->addLiveRangeToEndOfBlock(registerContainingConsumerBase, copy.operator ->());
									allInstructionsOfRegion.push_back(make_pair(copy.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
									LIS->computeLiveInRegUnits();
								} else {
									registerContainingConsumerBase = MF.getRegInfo().getLiveInVirtReg(physicalReg);
								}
							} else {
								registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
								MachineInstrBuilder copy = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
								copy.addReg(registerContainingConsumerBase, RegState::Define);
								copy.addReg(REDEFINE::t5);
								int frameOffset = 0;
								for (int i = 0; i < MF.getFrameInfo()->getNumObjects(); i++) {
									frameOffset += REDEFINEUtils::getSizeOfType(MF.getFrameInfo()->getObjectAllocation(i)->getAllocatedType());
								}
								frameOffset += edge->getMemoryOffsetInTargetFrame();
								copy.addImm(frameOffset);
								LIS->getSlotIndexes()->insertMachineInstrInMaps(copy.operator llvm::MachineInstr *());
								allInstructionsOfRegion.push_back(make_pair(copy.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
							}
							if (targetCE != 0) {
								//Add writepm-dreadpm pair
								unsigned sourceCEContainingFrameAddress = 0;
								//Load the base scratchpad address to a register in the producer CE for the first time
								if (registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE] == -1) {
									MachineInstrBuilder sourceLui = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LUI));
									unsigned sourceSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
									sourceLui.addReg(sourceSpAddressRegister, RegState::Define);
									sourceLui.addImm(targetCE);
									LIS->getSlotIndexes()->insertMachineInstrInMaps(sourceLui.operator llvm::MachineInstr *());
									allInstructionsOfRegion.push_back(make_pair(sourceLui.operator llvm::MachineInstr *(), make_pair(sourceCEContainingFrameAddress, insertPosition++)));
									registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE] = (int) sourceSpAddressRegister;
									if (lastBB != BB && loopBodyInstructions[sourceCEContainingFrameAddress] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
										loopBodyInstructions.insert(loopBodyInstructions.begin() + sourceCEContainingFrameAddress, sourceLui.operator llvm::MachineInstr *());
									} else if (firstInstructionOfpHyperOpInRegion[sourceCEContainingFrameAddress] == 0) {
										firstInstructionOfpHyperOpInRegion[targetCE] = sourceLui.operator llvm::MachineInstr *();
									}
								}

								//Add a writepm and readpm instruction pair to communicate the address
								MachineInstrBuilder writepm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::WRITEPM));
								writepm.addReg(registerContainingBaseAddress[sourceCEContainingFrameAddress][targetCE]);
								writepm.addReg(registerContainingConsumerBase);
								writepm.addImm(faninOfHyperOp[targetCE]);

								allInstructionsOfRegion.push_back(make_pair(writepm.operator llvm::MachineInstr *(), make_pair(sourceCEContainingFrameAddress, insertPosition++)));
								LIS->getSlotIndexes()->insertMachineInstrInMaps(writepm.operator llvm::MachineInstr *());

								//Load the base scratchpad address to a register in the consumer CE the first time
								if (registerContainingBaseAddress[targetCE][targetCE] == -1) {
									MachineInstrBuilder targetLui = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LUI));
									unsigned targetSpAddressRegister = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
									targetLui.addReg(targetSpAddressRegister, RegState::Define);
									//TODO May need changing later
									targetLui.addImm(targetCE);
									LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());
									allInstructionsOfRegion.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
									registerContainingBaseAddress[targetCE][targetCE] = (int) targetSpAddressRegister;

									if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
										loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, targetLui.operator llvm::MachineInstr *());
									} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
										firstInstructionOfpHyperOpInRegion[targetCE] = targetLui.operator llvm::MachineInstr *();
									}
								}

								registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
								MachineInstrBuilder readpm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::DREADPM));
								readpm.addReg(registerContainingConsumerBase, RegState::Define);
								readpm.addReg(registerContainingBaseAddress[targetCE][targetCE]);
								readpm.addImm(faninOfHyperOp[targetCE]);

								allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
								pHopInteractionGraph.push_back(make_pair(writepm.operator ->(), readpm.operator ->()));
								LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
							}
							break;
						}
					}
				}
			} else {
				if (!consumer->isStaticHyperOp()) {
					//Load the base scratchpad address to a register in the consumer CE the first time
//						if (registerContainingBaseAddress[targetCE][spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].first] == -1) {
					MachineInstrBuilder targetLui = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LUI));
					unsigned targetSpAddressRegister = REDEFINE::t3;
					targetLui.addReg(targetSpAddressRegister, RegState::Define);
					//TODO May need changing later
					targetLui.addImm(currentCE);
					LIS->getSlotIndexes()->insertMachineInstrInMaps(targetLui.operator llvm::MachineInstr *());
					allInstructionsOfRegion.push_back(make_pair(targetLui.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
//							registerContainingBaseAddress[targetCE][spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].first] = (int) targetSpAddressRegister;
//						}
					if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
						vector<MachineInstr*> startInstructions;
						startInstructions.push_back(targetLui.operator ->());
						firstInstructionsInBB.insert(make_pair(lastBB, startInstructions));
					} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
						firstInstructionOfpHyperOpInRegion[targetCE] = targetLui.operator llvm::MachineInstr *();
					}

					if (!consumer->getInRange()) {
						registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder readpm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::READPM));
						readpm.addReg(registerContainingConsumerBase, RegState::Define);
						readpm.addReg(targetSpAddressRegister);
						readpm.addImm(spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].second);
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
					} else {
						errs()<<"for consumer hop "<<consumer->getFunction()->getName()<<", imm value:"<<(spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].second)<<"\n";
						MachineInstrBuilder readpm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
						readpm.addReg(REDEFINE::t1, RegState::Define);
						readpm.addReg(REDEFINE::zero);
						readpm.addImm(spLocationContainingHyperOpFrameAddressAndCEWithFalloc[consumer].second);
						allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
					}
				} else {
					registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder addi = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
					addi.addReg(registerContainingConsumerBase, RegState::Define);
					addi.addReg(REDEFINE::zero);
					addi.addImm(REDEFINEUtils::getHyperOpId(consumer));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
					if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
						vector<MachineInstr*> startInstructions;
						startInstructions.push_back(addi.operator ->());
						firstInstructionsInBB.insert(make_pair(lastBB, startInstructions));
					} else if (firstInstructionOfpHyperOpInRegion[currentCE] == 0) {
						firstInstructionOfpHyperOpInRegion[currentCE] = addi.operator llvm::MachineInstr *();
					}
					allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				}
			}

			//Insert in range loop here
			if (consumer->getInRange()) {
				Value* locationContainingCount;
				//TODO support variables instead of constants
				Value* min = consumer->getRangeLowerBound();
				Value* max = consumer->getRangeUpperBound();
				Value* stride = consumer->getStride();

				//Add loop constructs around everything that follows
				loopStart = lastBB->getParent()->CreateMachineBasicBlock();
				lastBB->getParent()->push_back(loopStart);
				lastBB->addSuccessor(loopStart);
				LIS->insertMBBInMaps(loopStart);
				loopBody = lastBB->getParent()->CreateMachineBasicBlock();
				lastBB->getParent()->push_back(loopBody);
				LIS->insertMBBInMaps(loopBody);
				loopStart->addSuccessor(loopBody);
				loopBody->addSuccessor(loopStart);
				vector<MachineInstr*> loopStartInstructions;
				vector<MachineInstr*> loopEndInstructions;

				loopEnd = lastBB->getParent()->CreateMachineBasicBlock();
				lastBB->getParent()->push_back(loopEnd);
				loopStart->addSuccessor(loopEnd);
				LIS->insertMBBInMaps(loopEnd);

				vector<MachineInstr*> startInstr;

				for (int i = 0; i < targetCE; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopStart);
					LIS->InsertMachineInstrInMaps(jumpInstr);
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
					startInstr.push_back(jumpInstr.operator ->());

				}

				//load trip count from memory location onto a register
				unsigned loopMaxboundReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder loopBoundMaxLoadInst;
				if (isa<ConstantInt>(max)) {
					loopBoundMaxLoadInst = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
					loopBoundMaxLoadInst.addReg(loopMaxboundReg, RegState::Define);
					loopBoundMaxLoadInst.addReg(REDEFINE::zero);
					loopBoundMaxLoadInst.addImm(((ConstantInt*) max)->getZExtValue());
					startInstr.push_back(loopBoundMaxLoadInst.operator ->());
				} else {
					int immediate = 0;
					if (!parentModule->getGlobalList().empty()) {
						for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
							const GlobalVariable *globalVar = &*globalArgItr;
							if (globalVar == max) {
								break;
							}
							immediate += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
						}
					}
					string globalAddressString = "ga#";
					globalAddressString.append(itostr(immediate));
					MCSymbol* gaSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(globalAddressString));

					unsigned registerForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder movimm = BuildMI(*lastBB, lastBB->end(), BB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR)).addReg(registerForGlobalAddr, RegState::Define).addSym(gaSymbol);
					LIS->InsertMachineInstrInMaps(movimm);
					allInstructionsOfRegion.push_back(make_pair(movimm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

					loopBoundMaxLoadInst = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
					loopBoundMaxLoadInst.addReg(loopMaxboundReg, RegState::Define);
					loopBoundMaxLoadInst.addReg(registerForGlobalAddr);
					loopBoundMaxLoadInst.addImm(0);
					startInstr.push_back(movimm.operator ->());
				}

				LIS->InsertMachineInstrInMaps(loopBoundMaxLoadInst.operator ->());
				allInstructionsOfRegion.push_back(make_pair(loopBoundMaxLoadInst.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

				indVar = REDEFINE::t2;
				MachineInstrBuilder inductionVarInit;
				if (isa<ConstantInt>(min)) {
					inductionVarInit = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
					inductionVarInit.addReg(indVar, RegState::Define);
					inductionVarInit.addReg(REDEFINE::zero);
					inductionVarInit.addImm(((ConstantInt*) min)->getZExtValue());
				} else {
					int immediate = 0;
					if (!parentModule->getGlobalList().empty()) {
						for (Module::const_global_iterator globalArgItr = parentModule->global_begin(); globalArgItr != parentModule->global_end(); globalArgItr++) {
							const GlobalVariable *globalVar = &*globalArgItr;
							if (globalVar == min) {
								break;
							}
							immediate += REDEFINEUtils::getAlignedSizeOfType(globalVar->getType());
						}
					}
					string globalAddressString = "ga#";
					globalAddressString.append(itostr(immediate));
					MCSymbol* gaSymbol = BB->getParent()->getContext().GetOrCreateSymbol(StringRef(globalAddressString));

					unsigned registerForGlobalAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
					MachineInstrBuilder movimm = BuildMI(*lastBB, lastBB->end(), BB->begin()->getDebugLoc(), TII->get(REDEFINE::MOVADDR)).addReg(registerForGlobalAddr, RegState::Define).addSym(gaSymbol);
					LIS->InsertMachineInstrInMaps(movimm);
					allInstructionsOfRegion.push_back(make_pair(movimm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

					inductionVarInit = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
					inductionVarInit.addReg(indVar, RegState::Define);
					inductionVarInit.addReg(registerForGlobalAddr);
					inductionVarInit.addImm(REDEFINE::zero);
				}
				LIS->InsertMachineInstrInMaps(inductionVarInit.operator ->());
				allInstructionsOfRegion.push_back(make_pair(inductionVarInit.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

				//	Add jump from indvar to loopstart
				MachineInstrBuilder jumpInstr = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::JAL));
				jumpInstr.addMBB(loopStart);
				LIS->InsertMachineInstrInMaps(jumpInstr.operator ->());
				allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));

				for (int i = targetCE + 1; i < ceCount; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*lastBB, lastBB->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopStart);
					LIS->InsertMachineInstrInMaps(jumpInstr.operator ->());
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
					startInstr.push_back(jumpInstr.operator ->());

				}

				if (firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
					firstInstructionsInBB.insert(make_pair(lastBB, startInstr));
				}

				for (int i = 0; i < targetCE; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*loopStart, loopStart->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopStart);
					LIS->InsertMachineInstrInMaps(jumpInstr.operator ->());
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
				}

				MachineInstrBuilder branchInstr = BuildMI(*loopStart, loopStart->end(), dl, TII->get(REDEFINE::BGE));
				branchInstr.addMBB(loopEnd);
				branchInstr.addReg(indVar);
				branchInstr.addReg(loopMaxboundReg);
				LIS->InsertMachineInstrInMaps(branchInstr.operator ->());
				allInstructionsOfRegion.push_back(make_pair(branchInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				loopStartInstructions.push_back(branchInstr.operator ->());

//				MachineInstrBuilder fallback = BuildMI(*loopStart, loopStart->end(), dl, TII->get(REDEFINE::JAL));
//				fallback.addMBB(loopBody);
//				LIS->InsertMachineInstrInMaps(fallback);
//				allInstructionsOfRegion.push_back(make_pair(fallback.operator->(), make_pair(targetCE, insertPosition++)));

				for (int i = targetCE + 1; i < ceCount; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*loopStart, loopStart->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopEnd);
					LIS->InsertMachineInstrInMaps(jumpInstr.operator ->());
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
					loopStartInstructions.push_back(jumpInstr.operator ->());

				}
				firstInstructionsInBB.insert(make_pair(loopStart, loopStartInstructions));

				for (int i = 0; i < targetCE; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopStart);
					LIS->InsertMachineInstrInMaps(jumpInstr.operator ->());
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
					loopBodyInstructions[i] = jumpInstr.operator ->();

				}

				for (int i = targetCE + 1; i < ceCount; i++) {
					//Add jump from indvar to loopstart
					MachineInstrBuilder jumpInstr = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::JAL));
					jumpInstr.addMBB(loopStart);
					LIS->InsertMachineInstrInMaps(jumpInstr.operator ->());
					allInstructionsOfRegion.push_back(make_pair(jumpInstr.operator->(), make_pair(i, insertPosition++)));
					loopBodyInstructions[i] = jumpInstr.operator ->();

				}
				lastBB = loopBody;
				lastInstruction = loopBody->end();

				unsigned addInstrReg = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder addInstr = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADD));
				addInstr.addReg(addInstrReg, RegState::Define).addReg(REDEFINE::t3).addReg(REDEFINE::t1);
				LIS->InsertMachineInstrInMaps(addInstr);
				allInstructionsOfRegion.push_back(make_pair(addInstr.operator->(), make_pair(targetCE, insertPosition++)));

				if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
					loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, addInstr.operator ->());
				} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
					firstInstructionOfpHyperOpInRegion[targetCE] = addInstr.operator ->();
				}

				registerContainingConsumerBase = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder readpm = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::READPM));
				readpm.addReg(registerContainingConsumerBase, RegState::Define);
				readpm.addReg(addInstrReg);
				readpm.addImm(0);
				allInstructionsOfRegion.push_back(make_pair(readpm.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(readpm.operator llvm::MachineInstr *());
			}

			if (edge->getType() == HyperOpEdge::PREDICATE || edge->getType() == HyperOpEdge::ORDERING) {
				//predicate can take any offset wrt context frame base, it does not have a dedicated slot
				unsigned contextFrameOffset = edge->getDecrementOperandCount();

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

					MachineInstrBuilder loadInstr = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::LW));
					loadInstr.addReg(registerContainingPredicateData, RegState::Define);
					loadInstr.addReg(REDEFINE::t5);
					loadInstr.addFrameIndex(objectIndex);

					if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
						loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, loadInstr.operator llvm::MachineInstr *());
					} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
						firstInstructionOfpHyperOpInRegion[targetCE] = loadInstr.operator llvm::MachineInstr *();
					}
					allInstructionsOfRegion.push_back(make_pair(loadInstr.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(loadInstr.operator llvm::MachineInstr *());
//						LIS->addLiveRangeToEndOfBlock(registerContainingPredicateData, loadInstr.operator ->());

					if (edge->getPredicateValue()) {
						registerContainingData = registerContainingPredicateData;
					} else {
						registerContainingData = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
						MachineInstrBuilder sltiForPredicate = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::SLTIU));
						sltiForPredicate.addReg(registerContainingData, RegState::Define);
						sltiForPredicate.addReg(registerContainingPredicateData);
						//check use of sltui to find out why inverse of predicate is used
						sltiForPredicate.addImm(!edge->getPredicateValue());

						allInstructionsOfRegion.push_back(make_pair(sltiForPredicate.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
						LIS->getSlotIndexes()->insertMachineInstrInMaps(sltiForPredicate.operator llvm::MachineInstr *());
//							LIS->addLiveRangeToEndOfBlock(registerContainingData, sltiForPredicate.operator ->());
					}
				}
				//No need to access memory for a true predicate, copying via addi will do
				else {
					MachineInstrBuilder addi = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
					addi.addReg(registerContainingData, RegState::Define);
					addi.addReg(REDEFINE::zero);
					addi.addImm(1);
					if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
						loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, addi.operator llvm::MachineInstr *());
					} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
						firstInstructionOfpHyperOpInRegion[targetCE] = addi.operator llvm::MachineInstr *();
					}
					allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
					LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
//						LIS->addLiveRangeToEndOfBlock(registerContainingData, addi.operator ->());
				}

				MachineInstrBuilder offsetAddInst = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
				unsigned registerContainingAddr = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				offsetAddInst.addReg(registerContainingAddr, RegState::Define);
				offsetAddInst.addReg(registerContainingConsumerBase);
				offsetAddInst.addImm(contextFrameOffset);
				allInstructionsOfRegion.push_back(make_pair(offsetAddInst.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(offsetAddInst.operator llvm::MachineInstr *());
//					LIS->addLiveRangeToEndOfBlock(registerContainingAddr, offsetAddInst.operator ->());

				MachineInstrBuilder writeToContextFrame = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::WRITECMP));
				writeToContextFrame.addReg(registerContainingAddr);
				writeToContextFrame.addReg(registerContainingData);

				allInstructionsOfRegion.push_back(make_pair(writeToContextFrame.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				//Add instruction to bundle
				LIS->getSlotIndexes()->insertMachineInstrInMaps(writeToContextFrame.operator llvm::MachineInstr *());
				edge->setEdgeSource(writeToContextFrame.operator ->());
			} else if (edge->getType() == HyperOpEdge::SYNC) {
				//sync always goes to the 15th slot
				unsigned contextFrameOffset = SYNC_SLOT_OFFSET;

				//decrement the barrier by 1
				unsigned registerContainingData = ((REDEFINETargetMachine&) TM).FuncInfo->CreateReg(MVT::i32);
				MachineInstrBuilder addi = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::ADDI));
				addi.addReg(registerContainingData, RegState::Define);
				addi.addReg(REDEFINE::zero);
				int8_t imm = -1;
				addi.addImm(SignExtend8BitNumberTo12Bits(imm));

				if (lastBB != BB && loopBodyInstructions[targetCE] == NULL && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
					loopBodyInstructions.insert(loopBodyInstructions.begin() + targetCE, addi.operator llvm::MachineInstr *());
				} else if (firstInstructionOfpHyperOpInRegion[targetCE] == 0) {
					firstInstructionOfpHyperOpInRegion[targetCE] = addi.operator llvm::MachineInstr *();
				}

				allInstructionsOfRegion.push_back(make_pair(addi.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(addi.operator llvm::MachineInstr *());
//					LIS->addLiveRangeToEndOfBlock(registerContainingData, addi.operator ->());

				MachineInstrBuilder syncInstruction = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::SYNC));
				syncInstruction.addReg(registerContainingConsumerBase);
				syncInstruction.addReg(registerContainingData);
				syncInstruction.addImm(contextFrameOffset);
				allInstructionsOfRegion.push_back(make_pair(syncInstruction.operator llvm::MachineInstr *(), make_pair(targetCE, insertPosition++)));
				LIS->getSlotIndexes()->insertMachineInstrInMaps(syncInstruction.operator llvm::MachineInstr *());
				edge->setEdgeSource(syncInstruction.operator ->());
			}
			if (consumer->getInRange()) {
				MachineInstrBuilder inductionVarIncrement = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::ADDI));
				inductionVarIncrement.addReg(indVar, RegState::Define);
				inductionVarIncrement.addReg(indVar);
				inductionVarIncrement.addImm((((ConstantInt*) consumer->getStride())->getZExtValue()));
				LIS->InsertMachineInstrInMaps(inductionVarIncrement);
				insertedInstructions.push_back(inductionVarIncrement.operator ->());

				MachineInstrBuilder regBaseIncrement = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::ADDI));
				regBaseIncrement.addReg(REDEFINE::t1, RegState::Define);
				regBaseIncrement.addReg(REDEFINE::t1);
				regBaseIncrement.addImm(-4);
				LIS->InsertMachineInstrInMaps(regBaseIncrement);
				insertedInstructions.push_back(regBaseIncrement.operator ->());

				MachineInstrBuilder loopbodyJump = BuildMI(*loopBody, loopBody->end(), dl, TII->get(REDEFINE::JAL));
				loopbodyJump.addMBB(loopStart);
				LIS->InsertMachineInstrInMaps(loopbodyJump);
				insertedInstructions.push_back(loopbodyJump.operator ->());

				for (auto insertItr = insertedInstructions.begin(); insertItr != insertedInstructions.end(); insertItr++) {
					allInstructionsOfRegion.push_back(make_pair(*insertItr, make_pair(targetCE, insertPosition++)));
				}
			}

			if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end() && !loopBodyInstructions.empty()) {
				errs() << "inserting at start of " << loopBody->getNumber() << ":";
				loopBodyInstructions[0]->dump();
				firstInstructionsInBB.insert(make_pair(lastBB, loopBodyInstructions));
			}
			lastBB = &BB->getParent()->back();
			lastInstruction = lastBB->end();
		}
	}

//If a HyperOp is not predicated, it deletes itself
	if (!hyperOp->isPredicatedHyperOp() && !hyperOp->isStartHyperOp()) {
		MachineInstrBuilder fdelete;
//Add an instruction to delete the frame of the HyperOp
		fdelete = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::FDELETE));
		fdelete.addReg(REDEFINE::t4);

		if (lastBB != BB && firstInstructionsInBB.find(lastBB) == firstInstructionsInBB.end()) {
			vector<MachineInstr*> startInstr;
			startInstr.reserve(ceCount);
			startInstr.push_back(fdelete.operator ->());
			firstInstructionsInBB.insert(make_pair(lastBB, startInstr));
			errs() << "added fdelete as the first instruction\n";
			startInstr[0]->dump();
		} else if (firstInstructionOfpHyperOpInRegion[0] == 0) {
			firstInstructionOfpHyperOpInRegion[0] = fdelete.operator llvm::MachineInstr *();
		}
		LIS->getSlotIndexes()->insertMachineInstrInMaps(fdelete.operator llvm::MachineInstr *());
		allInstructionsOfRegion.push_back(make_pair(fdelete.operator llvm::MachineInstr *(), make_pair(0, insertPosition++)));
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
	errs() << "end hop gets added in bb " << lastBB->getName() << "\n";
	lastBB = &BB->getParent()->back();
	lastInstruction = lastBB->end();
//End HyperOp and the two NOPs that follow are kinda like a new region that gets shuffled next
	vector<MachineInstr*> endHyperOpInstructionRegion;
//Add endHyperOp instruction and 2 nops in each pHyperOp
	for (unsigned i = 0; i < ceCount; i++) {
		MachineInstrBuilder endInstruction = BuildMI(*lastBB, lastInstruction, dl, TII->get(REDEFINE::END));
		endInstruction.addImm(0);
		endHyperOpInstructionRegion.push_back(endInstruction.operator ->());
		LIS->getSlotIndexes()->insertMachineInstrInMaps(endInstruction.operator llvm::MachineInstr *());
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

errs() << "fixing live range of loop bbs:";
BB->getParent()->dump();

for (auto bbItr : firstInstructionsInBB) {
	auto bb = bbItr.first;
	errs() << "at bb " << bb->getName() << "\n";
	vector<unsigned> registersUsedInloopBB;
	for (MachineBasicBlock::instr_iterator instItr = bb->instr_begin(); instItr != bb->instr_end(); instItr++) {
		for (unsigned i = 0; i < instItr->getNumOperands(); i++) {
			MachineOperand& operand = instItr->getOperand(i);
			if (operand.isReg() && find(registersUsedInloopBB.begin(), registersUsedInloopBB.end(), operand.getReg()) == registersUsedInloopBB.end()) {
				unsigned operandRegister = operand.getReg();
				bool ignore = false;
				for (unsigned i = 0; i < ceCount; i++) {
					if (virtualRegistersForInstAddr[i].first == operandRegister || virtualRegistersForInstAddr[i].second == operandRegister) {
						ignore = true;
						break;
					}
				}
				if (!ignore) {
					registersUsedInloopBB.push_back(operand.getReg());
				}
			}
		}
	}

	LIS->repairIntervalsInRange(bb, bb->begin(), bb->end(), registersUsedInloopBB);
}

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

errs() << "repairing current BB region " << BB->getName() << "\n";
fflush (stdout);
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
//			continue;
			//pHyperOp is empty, add a nop
			MachineInstrBuilder nopInstruction = BuildMI(*BB, firstInstructionInNextCE, MF.begin()->begin()->getDebugLoc(), TII->get(REDEFINE::ADDI));
			nopInstruction.addReg(REDEFINE::zero, RegState::Define);
			nopInstruction.addReg(REDEFINE::zero);
			nopInstruction.addImm(0);
			LIS->getSlotIndexes()->insertMachineInstrInMaps(nopInstruction.operator llvm::MachineInstr *());
			firstInstructionofCE = nopInstruction.operator ->();
		}

		if (firstInstructionInNextCE != NULL) {
			MIBundleBuilder* bundleBuilder = new MIBundleBuilder(*BB, (MachineBasicBlock::instr_iterator) firstInstructionofCE, (MachineBasicBlock::instr_iterator) firstInstructionInNextCE);
		}
	}
}

//Create instruction bundles corresponding to pHyperOps
for (auto bbItr : firstInstructionsInBB) {
	DEBUG(dbgs() << "Creating pHyperOp bundles for CEs for bb" << bbItr.first->getNumber() << "\n");
	vector<MachineInstr*> firstInstructionInCE = bbItr.second;
	errs() << "is the first instruction in the ce empty?" << (firstInstructionInCE.empty()) << "\n";
	if (!firstInstructionInCE.empty()) {
		for (unsigned i = 0; i < ceCount; i++) {
			MachineInstr* firstInstructionofCE = firstInstructionInCE[i];
			MachineInstr* firstInstructionInNextCE = NULL;
			if (i < ceCount - 1) {
				firstInstructionInNextCE = firstInstructionInCE[i + 1];
			}
			if (firstInstructionInNextCE == NULL && firstInstructionofCE != NULL) {
				firstInstructionInNextCE = firstInstructionofCE->getParent()->end();
			}

			if (firstInstructionofCE != NULL) {
				errs() << "where are we starting bundling:";
				firstInstructionofCE->dump();
				MIBundleBuilder* bundleBuilder = new MIBundleBuilder(*firstInstructionofCE->getParent(), (MachineBasicBlock::instr_iterator) firstInstructionofCE, (MachineBasicBlock::instr_iterator) firstInstructionInNextCE);
			}
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

HyperOpInteractionGraph * graph = ((REDEFINETargetMachine&) TM).HIG;
HyperOp* currentHyperOp = graph->getHyperOp(const_cast<Function*>(MF.getFunction()));
currentHyperOp->setNumCEs(ceCount);

map<unsigned, MachineInstr*> physRegAndFirstMachineOperand;
map<unsigned, list<unsigned> > ceAndLiveInPhysicalRegMap;
list<unsigned> liveInPhysRegisters;

unsigned liveInFrameLocation = 0;
for (MachineRegisterInfo::livein_iterator liveInItr = MF.getRegInfo().livein_begin(); liveInItr != MF.getRegInfo().livein_end(); liveInItr++, liveInFrameLocation++) {
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
for (unsigned ceIndex = 0; ceIndex < ceCount; ceIndex++) {
	if (ceAndLiveInPhysicalRegMap.find(ceIndex) != ceAndLiveInPhysicalRegMap.end()) {
		list<unsigned> liveInRegs = ceAndLiveInPhysicalRegMap[ceIndex];
		currentHyperOp->setNumCEInputs(ceIndex, liveInRegs.size());
		if (!liveInRegs.empty()) {
			for (list<unsigned>::iterator physRegItr = liveInRegs.begin(); physRegItr != liveInRegs.end(); physRegItr++) {
				unsigned physReg = *physRegItr;
				physRegAndContextFrameSlot[physReg] = contextFrameLocation;
				physRegAndLiveIn[physReg] = ceIndex;
				contextFrameLocation++;
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

LIS->computeLiveInRegUnits();

/*
 * This part is for wcet
 */
PHyperOpInteractionGraph pHopDependenceMap;
currentHyperOp->setpHyperOpDependenceMap(pHopDependenceMap);

}

