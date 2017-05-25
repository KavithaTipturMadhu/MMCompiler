/*
 * LoopInterchange.h
 *
 *  Created on: 23-May-2017
 *      Author: kavitha
 */

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

using namespace llvm;

#define DEBUG_TYPE "loop-interchange"

/// LoopInterchangeTransform interchanges the loop.
class LoopInterchangeTransform {
public:
  LoopInterchangeTransform(Loop *Outer, Loop *Inner, ScalarEvolution *SE,
                           LoopInfo *LI, DominatorTree *DT,
                           BasicBlock *LoopNestExit,
                           bool InnerLoopContainsReductions, Pass* pass)
      : OuterLoop(Outer), InnerLoop(Inner), SE(SE), LI(LI), DT(DT),
        LoopExit(LoopNestExit),pass(pass),
        InnerLoopHasReduction(InnerLoopContainsReductions) {}

  /// Interchange OuterLoop and InnerLoop.
  bool transform();
  void restructureLoops(Loop *InnerLoop, Loop *OuterLoop);
  void removeChildLoop(Loop *OuterLoop, Loop *InnerLoop);

private:
  void splitInnerLoopLatch(Instruction *);
  void splitInnerLoopHeader();
  bool adjustLoopLinks();
  void adjustLoopPreheaders();
  bool adjustLoopBranches();
  void updateIncomingBlock(BasicBlock *CurrBlock, BasicBlock *OldPred,
                           BasicBlock *NewPred);

  Loop *OuterLoop;
  Loop *InnerLoop;

  /// Scev analysis.
  ScalarEvolution *SE;
  LoopInfo *LI;
  DominatorTree *DT;
  BasicBlock *LoopExit;
  bool InnerLoopHasReduction;
  Pass* pass;
};


