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
#include "../lib/Transforms/HyperOpCreationPass/HyperOpCreationPass.cpp"
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
		AU.addRequired<HyperOpCreationPass>();
	}

	void addRangeLoopConstructs(HyperOp* child, Function* vertexFunction, const Module &M, BasicBlock** loopBegin, BasicBlock** loopBody, BasicBlock** loopEnd, BasicBlock** insertInBB, LoadInst** loadInst) {
		/* Add loop constructs */
		string loopBeginNameSR = child->getFunction()->getName().str();
		loopBeginNameSR.append("_create_begin");
		Twine loopBeginName(loopBeginNameSR);
		*loopBegin = BasicBlock::Create(M.getContext(), loopBeginName, vertexFunction);

		string loopBodyNameSR = child->getFunction()->getName().str();
		loopBodyNameSR.append("_create_body");
		Twine loopBodyName(loopBodyNameSR);
		*loopBody = BasicBlock::Create(M.getContext(), loopBodyName, vertexFunction);

		string loopEndNameSR = child->getFunction()->getName().str();
		loopEndNameSR.append("_create_end");
		Twine loopEndName(loopEndNameSR);
		*loopEnd = BasicBlock::Create(M.getContext(), loopEndName, vertexFunction);

		AllocaInst* allocItrInst = new AllocaInst(Type::getInt32Ty(M.getContext()));
		allocItrInst->setAlignment(4);
		allocItrInst->insertBefore((*insertInBB)->getFirstInsertionPt());

//		Value* zero = ConstantInt::get(M.getContext(), APInt(32, 0));
		StoreInst* storeItrInst = new StoreInst(child->getRangeLowerBound(), allocItrInst, (*insertInBB)->getTerminator());
		storeItrInst->setAlignment(4);

		*loadInst = new LoadInst(allocItrInst, "falloc_itr", (*insertInBB)->getTerminator());
		BranchInst* loopBodyJump = BranchInst::Create(*loopBegin, (*insertInBB)->getTerminator());
		(*insertInBB)->getTerminator()->removeFromParent();

		CmpInst* cmpInst = CmpInst::Create(Instruction::ICmp, llvm::CmpInst::ICMP_UGE, *loadInst, child->getRangeUpperBound(), "cmpinst", *loopBegin);
		BranchInst* bgeItrInst = BranchInst::Create(*loopEnd, *loopBody, cmpInst, *loopBegin);
		BranchInst* loopEndJump = BranchInst::Create(*loopBegin, *loopBody);
		ReturnInst* ret = ReturnInst::Create(M.getContext(), *loopEnd);
	}

	virtual bool runOnModule(Module &M) {
		HyperOpInteractionGraph* graph = HyperOpMetadataParser::parseMetadata(&M);
		graph->print(dbgs());
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
		map<Function*, unsigned> functionAndIndexMap;

		unsigned index = 0;
		for (auto functionItr = M.begin(); functionItr != M.end(); functionItr++) {
			Function* func = functionItr;
			functionAndIndexMap.insert(make_pair(func, index));
			index++;
		}

		for (auto vertexItr : graph->Vertices) {
			HyperOp* vertex = vertexItr;
			Function* vertexFunction = vertex->getFunction();
			BasicBlock* insertInBB = &vertexFunction->back();

			DEBUG(dbgs() << "Adding falloc, fbind instructions to module\n");
			for (auto childItr : vertex->getChildList()) {
				HyperOp* child = childItr;
				if (child->getImmediateDominator() == vertex && !child->isStaticHyperOp()) {
					BasicBlock * loopBegin, *loopBody, *loopEnd;
					LoadInst* loadInst;
					CallInst* fallocCallInst, *fbindInst;
					Value *fallocArgs[1];
					Value* baseAddress;
					if (child->getInRange()) {
						addRangeLoopConstructs(child, vertexFunction, M, &loopBegin, &loopBody, &loopEnd, &insertInBB, &loadInst);
						fallocArgs[0] = child->getRangeUpperBound();
					} else {
						fallocArgs[0] = ConstantInt::get(M.getContext(), APInt(32, 0));
						baseAddress = fallocCallInst;
					}

					fallocCallInst = CallInst::Create((Value*) Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID) Intrinsic::falloc, 0), fallocArgs, "falloc_reg", &insertInBB->back());

					if (child->getInRange()) {
						insertInBB = loopBody;
						baseAddress = BinaryOperator::CreateNSWAdd(fallocCallInst, ConstantInt::get(M.getContext(), APInt(32, 64)), "base_addr", &loopBody->back());
					}

					Value *fbindArgs[] = { baseAddress, ConstantInt::get(M.getContext(), APInt(32, functionAndIndexMap[child->getFunction()])) };
					fbindInst = CallInst::Create((Value*) Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID) Intrinsic::fbind, 0), fbindArgs, "", &insertInBB->back());

					/* Add falloc and fbind instructions */
					if (child->getInRange()) {
						BinaryOperator* incItr = BinaryOperator::CreateNSWAdd(loadInst, ConstantInt::get(M.getContext(), APInt(32, 1)), "", &loopBody->back());
						insertInBB = loopEnd;
					}
				}
			}

			DEBUG(dbgs() << "Adding fdelete instructions to module\n");
			for (auto hyperOpItr : vertex->getParentGraph()->Vertices) {
				HyperOp* child = hyperOpItr;
				if (child != vertex && !child->isPredicatedHyperOp() && child->getImmediateDominator() != NULL && child->getImmediateDominator()->getImmediatePostDominator() == vertex) {
					HyperOpEdge* contextFrameAddressEdge = NULL;
					for(auto incomingEdgeItr:child->ParentMap){
						HyperOpEdge* edge = incomingEdgeItr.first;
						if(edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR && edge->getContextFrameAddress() == child){
							contextFrameAddressEdge = edge;
							break;
						}
					}
					assert((contextFrameAddressEdge!=NULL)&&"");
					unsigned argSlot = contextFrameAddressEdge->getPositionOfContextSlot();
					Value* argContainingAddress = 0;
					unsigned index = 0;
					for (auto argItr = vertexFunction->getArgumentList().begin(); argItr != vertexFunction->getArgumentList().end(); argItr++, index++) {
						if (argSlot == index) {
							argContainingAddress = argItr;
							break;
						}
					}
					BasicBlock * loopBegin, *loopBody, *loopEnd;
					LoadInst* loadInst;
					if (child->getInRange()) {
						addRangeLoopConstructs(child, vertexFunction, M, &loopBegin, &loopBody, &loopEnd, &insertInBB, &loadInst);
						insertInBB = loopBody;
					}

					CallInst* fdeleteInst;
					Value *fdeleteArgs[] = { argContainingAddress };
					fdeleteInst = CallInst::Create((Value*) Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID) Intrinsic::fdelete, 0), fdeleteArgs, "", &insertInBB->back());
					/* Add falloc and fbind instructions */
					if (child->getInRange()) {
						BinaryOperator* incItr = BinaryOperator::CreateNSWAdd(loadInst, ConstantInt::get(M.getContext(), APInt(32, 1)), "", &loopBody->back());
						insertInBB = loopEnd;
					}
				}
			}
//
//			DEBUG(dbgs() << "Adding fdelete self instruction\n");
//			if(!vertex->isPredicatedHyperOp()){
//				CallInst* fdeleteInst;
//				assert(vertexFunction->getArgumentList().size()>=1);
//				Value *fdeleteArgs[] = { &vertexFunction->getArgumentList().front()};
//				fdeleteInst = CallInst::Create((Value*) Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID) Intrinsic::fdelete, 0), fdeleteArgs, "", &insertInBB->back());
//			}

		}
		return true;
	}
};
char REDEFINEIRPass::ID = 3;

char* REDEFINEIRPass::NEW_NAME = "newName";
static RegisterPass<REDEFINEIRPass> Y("REDEFINEIRPass", "Pass to create REDEFINE specific IR from Kavitha's metadata based IR");
