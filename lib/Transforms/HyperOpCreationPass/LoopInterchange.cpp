//===- LoopInterchange.cpp - Loop interchange pass------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This Pass handles loop interchange transform.
// This pass interchanges loops to provide a more cache-friendly memory access
// patterns.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/CodeMetrics.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpander.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "LoopInterchange.h"

using namespace llvm;

#define DEBUG_TYPE "loop-interchange"

static PHINode *getInductionVariable(Loop *L, ScalarEvolution *SE) {
	PHINode *InnerIndexVar = L->getCanonicalInductionVariable();
	if (InnerIndexVar)
		return InnerIndexVar;
	if (L->getLoopLatch() == nullptr || L->getLoopPredecessor() == nullptr)
		return nullptr;
	for (BasicBlock::iterator I = L->getHeader()->begin(); isa<PHINode>(I); ++I) {
		PHINode *PhiVar = cast<PHINode>(I);
		Type *PhiTy = PhiVar->getType();
		if (!PhiTy->isIntegerTy() && !PhiTy->isFloatingPointTy() && !PhiTy->isPointerTy())
			return nullptr;
		const SCEVAddRecExpr *AddRec = dyn_cast<SCEVAddRecExpr>(SE->getSCEV(PhiVar));
		if (!AddRec || !AddRec->isAffine())
			continue;
		const SCEV *Step = AddRec->getStepRecurrence(*SE);
		if (!isa<SCEVConstant>(Step))
			continue;
		// Found the induction variable.
		// FIXME: Handle loops with more than one induction variable. Note that,
		// currently, legality makes sure we have only one induction variable.
		return PhiVar;
	}
	return nullptr;
}

void LoopInterchangeTransform::splitInnerLoopLatch(Instruction *Inc) {
	BasicBlock *InnerLoopLatch = InnerLoop->getLoopLatch();
	BasicBlock *InnerLoopLatchPred = InnerLoopLatch;
	InnerLoopLatch = SplitBlock(InnerLoopLatchPred, Inc, pass);
	InnerLoop->addBasicBlockToLoop(InnerLoopLatch, LI->getBase());
}

void LoopInterchangeTransform::splitInnerLoopHeader() {
	// Split the inner loop header out. Here make sure that the reduction PHI's
	// stay in the innerloop body.
	BasicBlock *InnerLoopHeader = InnerLoop->getHeader();
	BasicBlock *InnerLoopPreHeader = InnerLoop->getLoopPreheader();
	if (InnerLoopHasReduction) {
		// FIXME: Check if the induction PHI will always be the first PHI.
		BasicBlock *New = InnerLoopHeader->splitBasicBlock(++(InnerLoopHeader->begin()), InnerLoopHeader->getName() + ".split");
		if (LI)
			if (Loop *L = LI->getLoopFor(InnerLoopHeader))
				L->addBasicBlockToLoop(New, LI->getBase());

		// Adjust Reduction PHI's in the block.
		SmallVector<PHINode *, 8> PHIVec;
		for (auto I = New->begin(); isa<PHINode>(I); ++I) {
			PHINode *PHI = dyn_cast<PHINode>(I);
			Value *V = PHI->getIncomingValueForBlock(InnerLoopPreHeader);
			PHI->replaceAllUsesWith(V);
			PHIVec.push_back((PHI));
		}
		for (PHINode *P : PHIVec) {
			P->eraseFromParent();
		}
	} else {
		SplitBlock(InnerLoopHeader, InnerLoopHeader->getFirstNonPHI(), pass);
	}

	DEBUG(dbgs() << "Output of splitInnerLoopHeader InnerLoopHeaderSucc & "
			"InnerLoopHeader\n");
}

void LoopInterchangeTransform::removeChildLoop(Loop *OuterLoop, Loop *InnerLoop) {
	for (Loop::iterator I = OuterLoop->begin(), E = OuterLoop->end(); I != E; ++I) {
		if (*I == InnerLoop) {
			OuterLoop->removeChildLoop(I);
			return;
		}
	}
	llvm_unreachable("Couldn't find loop");
}

void LoopInterchangeTransform::restructureLoops(Loop *InnerLoop, Loop *OuterLoop) {
	Loop *OuterLoopParent = OuterLoop->getParentLoop();
	if (OuterLoopParent) {
		// Remove the loop from its parent loop.
		removeChildLoop(OuterLoopParent, OuterLoop);
		removeChildLoop(OuterLoop, InnerLoop);
		OuterLoopParent->addChildLoop(InnerLoop);
	} else {
		removeChildLoop(OuterLoop, InnerLoop);
		LI->changeTopLevelLoop(OuterLoop, InnerLoop);
	}

	while (!InnerLoop->empty())
		OuterLoop->addChildLoop(InnerLoop->removeChildLoop(InnerLoop->begin()));

	InnerLoop->addChildLoop(OuterLoop);
}

bool LoopInterchangeTransform::transform() {
	bool Transformed = false;
	Instruction *InnerIndexVar;

	errs()<<"whats OuterLoopPreHeader before all this?"<<OuterLoop->getLoopPreheader()->getName()<<"\n";
	if (InnerLoop->getSubLoops().size() == 0) {
		BasicBlock *InnerLoopPreHeader = InnerLoop->getLoopPreheader();
		DEBUG(dbgs() << "Calling Split Inner Loop\n");
		PHINode *InductionPHI = getInductionVariable(InnerLoop, SE);
		if (!InductionPHI) {
			DEBUG(dbgs() << "Failed to find the point to split loop latch \n");
			return false;
		}

		if (InductionPHI->getIncomingBlock(0) == InnerLoopPreHeader)
			InnerIndexVar = dyn_cast<Instruction>(InductionPHI->getIncomingValue(1));
		else
			InnerIndexVar = dyn_cast<Instruction>(InductionPHI->getIncomingValue(0));

		//
		// Split at the place were the induction variable is
		// incremented/decremented.
		// TODO: This splitting logic may not work always. Fix this.
		splitInnerLoopLatch(InnerIndexVar);
		// Splits the inner loops phi nodes out into a separate basic block.
		splitInnerLoopHeader();
		DEBUG(dbgs() << "splitInnerLoopHeader done\n");
	}

	Transformed |= adjustLoopLinks();
	if (!Transformed) {
		DEBUG(dbgs() << "adjustLoopLinks failed\n");
		return false;
	}

	restructureLoops(InnerLoop, OuterLoop);
	return true;
}

bool LoopInterchangeTransform::adjustLoopBranches() {

	DEBUG(dbgs() << "adjustLoopBranches called\n");
	// Adjust the loop preheader
	BasicBlock *InnerLoopHeader = InnerLoop->getHeader();
	BasicBlock *OuterLoopHeader = OuterLoop->getHeader();
	BasicBlock *InnerLoopLatch = InnerLoop->getLoopLatch();
	BasicBlock *OuterLoopLatch = OuterLoop->getLoopLatch();
	BasicBlock *OuterLoopPreHeader = OuterLoop->getLoopPreheader();
	BasicBlock *InnerLoopPreHeader = InnerLoop->getLoopPreheader();
	BasicBlock *OuterLoopPredecessor = NULL;
//	if (OuterLoopPreHeader != NULL && OuterLoopPreHeader->getUniquePredecessor() != NULL) {
	errs()<<"how many preds does outerlooppreheader have?:"<<OuterLoopPreHeader->getName()<<"\n";
	for(pred_iterator predItr = pred_begin(OuterLoopPreHeader);predItr!=pred_end(OuterLoopPreHeader);predItr++){
		errs()<<(*predItr)->getName()<<",";
	}
		OuterLoopPredecessor = OuterLoopPreHeader->getUniquePredecessor();
//	}
	BasicBlock *InnerLoopLatchPredecessor = NULL;
	if (InnerLoopLatch != NULL && InnerLoopLatch->getUniquePredecessor() != NULL) {
		InnerLoopLatchPredecessor = InnerLoopLatch->getUniquePredecessor();
	}
	BasicBlock *InnerLoopLatchSuccessor;
	BasicBlock *OuterLoopLatchSuccessor;

	BranchInst *OuterLoopLatchBI = NULL;
	BranchInst *InnerLoopLatchBI = NULL;
	BranchInst *OuterLoopHeaderBI = NULL;
	BranchInst *InnerLoopHeaderBI = NULL;
	if (InnerLoopLatch != NULL) {
		InnerLoopLatchBI = dyn_cast<BranchInst>(InnerLoopLatch->getTerminator());
	}
	if (OuterLoopLatch != NULL) {
		OuterLoopLatchBI = dyn_cast<BranchInst>(OuterLoopLatch->getTerminator());
	}

	OuterLoopHeaderBI = dyn_cast<BranchInst>(OuterLoopHeader->getTerminator());
	InnerLoopHeaderBI = dyn_cast<BranchInst>(InnerLoopHeader->getTerminator());

	if (!OuterLoopPredecessor || !InnerLoopLatchPredecessor || !OuterLoopLatchBI || !InnerLoopLatchBI || !OuterLoopHeaderBI || !InnerLoopHeaderBI){
		errs()<<"whats null?"<<(!OuterLoopPredecessor )<<","<< (!InnerLoopLatchPredecessor )<<","<< (!OuterLoopLatchBI )<<","<< (!InnerLoopLatchBI )<<","<< (!OuterLoopHeaderBI )<<","<< (!InnerLoopHeaderBI)<<"\n";
		return false;
	}

	BranchInst *InnerLoopLatchPredecessorBI = dyn_cast<BranchInst>(InnerLoopLatchPredecessor->getTerminator());
	BranchInst *OuterLoopPredecessorBI = dyn_cast<BranchInst>(OuterLoopPredecessor->getTerminator());

	if (!OuterLoopPredecessorBI || !InnerLoopLatchPredecessorBI)
		return false;
	BasicBlock *InnerLoopHeaderSuccessor = InnerLoopHeader->getTerminator()->getSuccessor(0);
	if (!InnerLoopHeaderSuccessor)
		return false;

	// Adjust Loop Preheader and headers

	unsigned NumSucc = OuterLoopPredecessorBI->getNumSuccessors();
	for (unsigned i = 0; i < NumSucc; ++i) {
		if (OuterLoopPredecessorBI->getSuccessor(i) == OuterLoopPreHeader)
			OuterLoopPredecessorBI->setSuccessor(i, InnerLoopPreHeader);
	}

	NumSucc = OuterLoopHeaderBI->getNumSuccessors();
	for (unsigned i = 0; i < NumSucc; ++i) {
		if (OuterLoopHeaderBI->getSuccessor(i) == OuterLoopLatch)
			OuterLoopHeaderBI->setSuccessor(i, LoopExit);
		else if (OuterLoopHeaderBI->getSuccessor(i) == InnerLoopPreHeader)
			OuterLoopHeaderBI->setSuccessor(i, InnerLoopHeaderSuccessor);
	}

	// Adjust reduction PHI's now that the incoming block has changed.
	updateIncomingBlock(InnerLoopHeaderSuccessor, InnerLoopHeader, OuterLoopHeader);

	BranchInst::Create(OuterLoopPreHeader, InnerLoopHeaderBI);
	InnerLoopHeaderBI->eraseFromParent();

	errs()<<"InnerLoopLatchBI num succ:"<<InnerLoopLatchBI->getNumSuccessors()<<"\n";
	errs()<<"InnerLoopLatch BI:";
	InnerLoopLatch->dump();
	// -------------Adjust loop latches-----------
//	if (InnerLoopLatchBI->getSuccessor(0) == InnerLoopHeader)
//		InnerLoopLatchSuccessor = InnerLoopLatchBI->getSuccessor(1);
//	else
		InnerLoopLatchSuccessor = InnerLoopLatchBI->getSuccessor(0);

	NumSucc = InnerLoopLatchPredecessorBI->getNumSuccessors();
	for (unsigned i = 0; i < NumSucc; ++i) {
		if (InnerLoopLatchPredecessorBI->getSuccessor(i) == InnerLoopLatch)
			InnerLoopLatchPredecessorBI->setSuccessor(i, InnerLoopLatchSuccessor);
	}

	// Adjust PHI nodes in InnerLoopLatchSuccessor. Update all uses of PHI with
	// the value and remove this PHI node from inner loop.
	SmallVector<PHINode *, 8> LcssaVec;
	for (auto I = InnerLoopLatchSuccessor->begin(); isa<PHINode>(I); ++I) {
		PHINode *LcssaPhi = cast<PHINode>(I);
		LcssaVec.push_back(LcssaPhi);
	}
	for (PHINode *P : LcssaVec) {
		Value *Incoming = P->getIncomingValueForBlock(InnerLoopLatch);
		P->replaceAllUsesWith(Incoming);
		P->eraseFromParent();
	}

//	if (OuterLoopLatchBI->getSuccessor(0) == OuterLoopHeader)
//		OuterLoopLatchSuccessor = OuterLoopLatchBI->getSuccessor(1);
//	else
		OuterLoopLatchSuccessor = OuterLoopLatchBI->getSuccessor(0);

//	if (InnerLoopLatchBI->getSuccessor(1) == InnerLoopLatchSuccessor)
//		InnerLoopLatchBI->setSuccessor(1, OuterLoopLatchSuccessor);
//	else
		InnerLoopLatchBI->setSuccessor(0, OuterLoopLatchSuccessor);

	updateIncomingBlock(OuterLoopLatchSuccessor, OuterLoopLatch, InnerLoopLatch);

	if (OuterLoopLatchBI->getSuccessor(0) == OuterLoopLatchSuccessor) {
		OuterLoopLatchBI->setSuccessor(0, InnerLoopLatch);
	}
//	else {
//		OuterLoopLatchBI->setSuccessor(1, InnerLoopLatch);
//	}

	return true;
}

/// \brief Move all instructions except the terminator from FromBB right before
/// InsertBefore
static void moveBBContents(BasicBlock *FromBB, Instruction *InsertBefore) {
	auto &ToList = InsertBefore->getParent()->getInstList();
	auto &FromList = FromBB->getInstList();

	ToList.splice(InsertBefore, FromList, FromList.begin(), FromBB->getTerminator());
}

void LoopInterchangeTransform::adjustLoopPreheaders() {

	// We have interchanged the preheaders so we need to interchange the data in
	// the preheader as well.
	// This is because the content of inner preheader was previously executed
	// inside the outer loop.
	BasicBlock *OuterLoopPreHeader = OuterLoop->getLoopPreheader();
	BasicBlock *InnerLoopPreHeader = InnerLoop->getLoopPreheader();
	BasicBlock *OuterLoopHeader = OuterLoop->getHeader();
	BranchInst *InnerTermBI = cast<BranchInst>(InnerLoopPreHeader->getTerminator());

	// These instructions should now be executed inside the loop.
	// Move instruction into a new block after outer header.
	moveBBContents(InnerLoopPreHeader, OuterLoopHeader->getTerminator());
	// These instructions were not executed previously in the loop so move them to
	// the older inner loop preheader.
	moveBBContents(OuterLoopPreHeader, InnerTermBI);
}

bool LoopInterchangeTransform::adjustLoopLinks() {

	// Adjust all branches in the inner and outer loop.
	bool Changed = adjustLoopBranches();
	if (Changed)
		adjustLoopPreheaders();
	return Changed;
}

void LoopInterchangeTransform::updateIncomingBlock(BasicBlock *CurrBlock,
                                                   BasicBlock *OldPred,
                                                   BasicBlock *NewPred) {
  for (auto I = CurrBlock->begin(); isa<PHINode>(I); ++I) {
    PHINode *PHI = cast<PHINode>(I);
    unsigned Num = PHI->getNumIncomingValues();
    for (unsigned i = 0; i < Num; ++i) {
      if (PHI->getIncomingBlock(i) == OldPred)
        PHI->setIncomingBlock(i, NewPred);
    }
  }
}
