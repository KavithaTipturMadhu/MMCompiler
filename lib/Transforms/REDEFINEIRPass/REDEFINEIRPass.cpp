#include<vector>
#include<string>
#include<list>
#include<map>
#include <algorithm>
#include <sstream>
using namespace std;
#include "llvm/IR/Instructions.h"
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
#include "llvm/IR/HyperopMetadataParser.h"
#include "llvm/IR/HyperopInteractionGraph.h"
using namespace llvm;

#define DEBUG_TYPE "REDEFINEIRPass"

/**
 * Pass to create HyperOps
 */
struct REDEFINEIRPass: public ModulePass {
	static char ID;
	static char* NEW_NAME;

	REDEFINEIRPass() :
		ModulePass(ID) {
	}

	virtual void getAnalysisUsage(AnalysisUsage &AU) const {
		//Mandatory merge return to be invoked on each function
		AU.addRequired<UnifyFunctionExitNodes>();
		AU.addRequired<DependenceAnalysis>();
	}

	virtual bool runOnModule(Module &M) {
		HyperOpInteractionGraph* graph = HyperOpMetadataParser::parseMetadata(&M);
		graph->removeUnreachableHops();
		graph->computeDominatorInfo();
		graph->makeGraphStructured();
		graph->computeDominatorInfo();
		graph->addContextFrameAddressForwardingEdges();
		graph->minimizeControlEdges();
		graph->clusterNodes();
		graph->associateStaticContextFrames();
		graph->verify();
		graph->print(dbgs());
		return true;
	}
};
char REDEFINEIRPass::ID = 3;

char* REDEFINEIRPass::NEW_NAME = "newName";
static RegisterPass<REDEFINEIRPass> X("REDEFINEIRPass", "Pass to create REDEFINE specific IR from Kavitha's metadata based IR");
