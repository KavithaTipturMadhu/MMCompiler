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
		/* Add falloc instructions */
		for(auto vertexItr:graph->Vertices){
			HyperOp* vertex = vertexItr;
			Function* vertexFunction = vertex->getFunction();
			for(auto childItr : vertex->getChildList()){
				HyperOp* child = childItr;
				if (child->getImmediateDominator() == vertex && !child->isStaticHyperOp()) {
					BasicBlock* insertInBB = vertexFunction->end();
					BasicBlock * loopBegin, *loopBody, *loopEnd;
					LoadInst* loadInst;
					if (child->getInRange()) {
						/* Add loop constructs */
						string loopBeginNameSR=child->getFunction()->getName().str();
						loopBeginNameSR.append("_create_begin");
						Twine loopBeginName(loopBeginNameSR);
						loopBegin = BasicBlock::Create(M.getContext(), loopBeginName, vertexFunction);
						loopBody = BasicBlock::Create(M.getContext(), loopBeginName, vertexFunction);
						loopEnd = BasicBlock::Create(M.getContext(), loopBeginName, vertexFunction);

						AllocaInst* allocItrInst = new AllocaInst(Type::getInt32Ty(M.getContext()));
						allocItrInst->setAlignment(4);
						allocItrInst->insertBefore(insertInBB->getFirstInsertionPt());
						Value* zero = ConstantInt::get(M.getContext(), APInt(32, 0));

						StoreInst* storeItrInst = new StoreInst(zero, allocItrInst, insertInBB->end());
						storeItrInst->setAlignment(4);

						BranchInst* loopBodyJump = BranchInst::Create(loopBegin,insertInBB->end());
						loadInst = new LoadInst(allocItrInst,"falloc_itr", loopBegin->end());
						CmpInst* cmpInst = CmpInst::Create(Instruction::ICmp, llvm::CmpInst::ICMP_UGE, loadInst, child->getRangeUpperBound(),"cmpinst", loopBegin->end());
						BranchInst* bgeItrInst = BranchInst::Create(loopEnd, loopBody,
								cmpInst, loopBegin->end());
						insertInBB = loopBody;
					}

					Value *Args[] = {ConstantInt::get(M.getContext(), APInt(32, 0))};
					Value *F = Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID)Intrinsic::falloc, 0);
					CallInst* fallocCallInst = CallInst::Create(F, Args);
					/* Add falloc and fbind instructions */
					if (child->getInRange()) {
						BranchInst* loopBodyJump = BranchInst::Create(loopEnd,insertInBB->end());
						BinaryOperator* incItr = BinaryOperator::CreateNSWAdd(loadInst, ConstantInt::get(M.getContext(), APInt(32, 1)), "", loopEnd->end());
						BranchInst* loopEndJump = BranchInst::Create(loopBegin,loopEnd->end());
					}
				}
			}
		}
		return true;
	}
};
char REDEFINEIRPass::ID = 3;

char* REDEFINEIRPass::NEW_NAME = "newName";
static RegisterPass<REDEFINEIRPass> X("REDEFINEIRPass", "Pass to create REDEFINE specific IR from Kavitha's metadata based IR");
