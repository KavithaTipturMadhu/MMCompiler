#include<vector>
#include<string>
#include<list>
#include<map>
#include <algorithm>
#include <sstream>
using namespace std;
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/User.h"
#include "llvm/Pass.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/IR/HyperOpMetadataParser.h"
#include "llvm/IR/HyperOpInteractionGraph.h"
using namespace llvm;

#define DEBUG_TYPE "HyperOpOptimizationPass"

struct HyperOpOptimizationPass: public ModulePass {
	static char ID;
	static char* NEW_NAME;

	HyperOpOptimizationPass() :
			ModulePass(ID) {
	}

	virtual void getAnalysisUsage(AnalysisUsage &AU) const {
		//Mandatory merge return to be invoked on each function
		AU.addRequired<UnifyFunctionExitNodes>();
		AU.addRequired<DependenceAnalysis>();
	}

	virtual bool runOnModule(Module &M) {
		HyperOpInteractionGraph* graph = HyperOpMetadataParser::parseMetadata(
				&M);
		for (auto functionItr = M.begin(); functionItr != M.end();
				functionItr++) {
			Function* func = functionItr;
			unsigned index = 1;
			for (Function::arg_iterator arg_itr =
					func->getArgumentList().begin();
					arg_itr != func->getArgumentList().end() && index <= 17;
					arg_itr++, index++) {
				if (arg_itr->getType()->isIntegerTy()) {
					func->addAttribute(index, Attribute::InReg);
				} else {
					break;
				}
			}
		}
		return true;
	}
};
char HyperOpOptimizationPass::ID = 3;

char* HyperOpOptimizationPass::NEW_NAME = "newName";
static RegisterPass<HyperOpOptimizationPass> X("HyperOpOptimizationPass",
		"Pass to optimize HyperOps");
