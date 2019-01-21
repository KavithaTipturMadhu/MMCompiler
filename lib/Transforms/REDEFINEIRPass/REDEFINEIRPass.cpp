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
//		graph->minimizeControlEdges();
		graph->clusterNodes();
		graph->associateStaticContextFrames();
		graph->verify();
		graph->print(dbgs());
		DEBUG(dbgs() << "Adding falloc instructions to module\n");
		/* Add falloc instructions */
		for (auto vertexItr : graph->Vertices) {
			HyperOp* vertex = vertexItr;
			Function* vertexFunction = vertex->getFunction();
			BasicBlock* insertInBB = &vertexFunction->back();
			for (auto childItr : vertex->getChildList()) {
				HyperOp* child = childItr;
				if (child->getImmediateDominator() == vertex && !child->isStaticHyperOp()) {
					BasicBlock * loopBegin, *loopBody, *loopEnd;
					LoadInst* loadInst;
					if (child->getInRange()) {
						/* Add loop constructs */
						string loopBeginNameSR = child->getFunction()->getName().str();
						loopBeginNameSR.append("_create_begin");
						Twine loopBeginName(loopBeginNameSR);
						loopBegin = BasicBlock::Create(M.getContext(), loopBeginName, vertexFunction);

						string loopBodyNameSR = child->getFunction()->getName().str();
						loopBodyNameSR.append("_create_body");
						Twine loopBodyName(loopBodyNameSR);
						loopBody = BasicBlock::Create(M.getContext(), loopBodyName, vertexFunction);

						string loopEndNameSR = child->getFunction()->getName().str();
						loopEndNameSR.append("_create_end");
						Twine loopEndName(loopEndNameSR);
						loopEnd = BasicBlock::Create(M.getContext(), loopEndName, vertexFunction);

						AllocaInst* allocItrInst = new AllocaInst(Type::getInt32Ty(M.getContext()));
						allocItrInst->setAlignment(4);
						allocItrInst->insertBefore(insertInBB->getFirstInsertionPt());
						Value* zero = ConstantInt::get(M.getContext(), APInt(32, 0));
						StoreInst* storeItrInst = new StoreInst(zero, allocItrInst, insertInBB->getTerminator());
						storeItrInst->setAlignment(4);

						BranchInst* loopBodyJump = BranchInst::Create(loopBegin, insertInBB->getTerminator());
						insertInBB->getTerminator()->removeFromParent();
						loadInst = new LoadInst(allocItrInst, "falloc_itr", loopBegin);
						CmpInst* cmpInst = CmpInst::Create(Instruction::ICmp, llvm::CmpInst::ICMP_UGE, loadInst, child->getRangeUpperBound(), "cmpinst", loopBegin);
						BranchInst* bgeItrInst = BranchInst::Create(loopEnd, loopBody, cmpInst, loopBegin);
						insertInBB = loopBody;
					}

					Value *Args[] = { ConstantInt::get(M.getContext(), APInt(32, 0)) };
					Value *F = Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID) Intrinsic::falloc, 0);
					if(insertInBB->empty()){
						CallInst* fallocCallInst = CallInst::Create(F, Args, "falloc_reg", insertInBB);
					}else{
						CallInst* fallocCallInst = CallInst::Create(F, Args, "falloc_reg", &insertInBB->back());
					}
					/* Add falloc and fbind instructions */
					if (child->getInRange()) {
						BinaryOperator* incItr = BinaryOperator::CreateNSWAdd(loadInst, ConstantInt::get(M.getContext(), APInt(32, 1)), "", loopBody);
						BranchInst* loopEndJump = BranchInst::Create(loopBegin, loopBody);
						ReturnInst* ret = ReturnInst::Create(M.getContext(), loopEnd);
						insertInBB = loopEnd;
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
