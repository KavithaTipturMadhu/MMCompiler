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
#include "llvm/Support/CFG.h"
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
#include "llvm/Transforms/Utils/CodeExtractor.h"
#include "llvm/IR/Instruction.def"
#include "llvm/IR/HyperOpMetadataParser.h"
#include "llvm/IR/HyperOpInteractionGraph.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InstIterator.h"
using namespace llvm;

/**
 * Pass to create HyperOps
 */
struct BBToFunctionPass: public ModulePass {
	static char ID; // Pass identification, replacement for typeid
	static char* NEW_NAME;

	BBToFunctionPass() :
			ModulePass(ID) {
	}

	virtual bool runOnModule(Module &M) {
		LLVMContext & ctxt = M.getContext();
		map<Function*, list<BasicBlock*> > originalFunctionToBBListMap;
		map<BasicBlock*, list<pair<Argument*,BasicBlock*> > > newBBArgMap;

		for (Module::iterator funcItr = M.begin(); funcItr != M.end(); funcItr++) {
			Function* function = funcItr;
			 list<BasicBlock*> bbList;
			for(Function::iterator bbItr = function->begin();bbItr!=function->end();bbItr++){
				BasicBlock* bb = bbItr;
				list<pair<Argument*,BasicBlock*> > argBBList;
				errs()<<"Inside block : "<<bb->getName()<<"\n";
				for(BasicBlock::iterator instItr= bb->begin();instItr!= bb->end();instItr++ ){
					Instruction *instr = instItr;

					for(Use* useItr = instr->op_begin();useItr != instr->op_end(); useItr++){
						Value* use = useItr->get();

						errs()<<"Instruction :: "<<*instr<<", use : "<< use->getName().str()<<"\n";
						if (Instruction *Inst = dyn_cast<Instruction>(use)) {
							//errs() << "Instruction used:" << *Inst->getParent()<< "\n";
							BasicBlock* bbParent = Inst->getParent();
							if(bbParent == bb){
								errs() << "same block" << "\n";
							} else {
								errs() << "belongs to block : "<< bbParent->getName().str()<< "\n";
							/*	Type *Ty = use->getType();
								Argument* arg = Argument(Ty,use->getName().str());
								//if (Argument *arg = dyn_cast<Argument>(useItr)) {
								argBBList.push_back(make_pair(arg, bbParent));
//								}*/
							}
						}
					}

				}
				bbList.push_back(bb);
				newBBArgMap[bbItr] = argBBList;
			}
			originalFunctionToBBListMap[function] = bbList;
			function->dump();
		}
		for(Function &Func :M){
				for(BasicBlock &BB : Func){
					list<pair<Argument*,BasicBlock*> > argBBList = newBBArgMap[&BB];
					errs()<<"Arguments for Basic block : "<<BB.getName().str()<<"\n";
					for (auto li = argBBList.begin(); li != argBBList.end(); li++) {
						//list<pair<Argument*, BasicBlock*>> argBB = li;
						errs() << *li->first << ", " << *li->second << "\n";
					}
					/*for(int i=0;i<argBBList.size();i++){

					}*/
				}
			}

		for (Module::iterator funcItr = M.begin(); funcItr != M.end(); funcItr++) {
			map<Value*, Value*> oldToNewValueMap;
			Function* function = funcItr;
			for (list<BasicBlock*>::iterator oldBB = originalFunctionToBBListMap[function].begin();
					oldBB != originalFunctionToBBListMap[function].end(); oldBB++) {
				BasicBlock* bbOld = *oldBB;
				FunctionType *FT = FunctionType::get(Type::getVoidTy(getGlobalContext()), false);
				Function *bbFunction = Function::Create(FT,Function::ExternalLinkage, bbOld->getName(), &M);
				BasicBlock* bbnew = BasicBlock::Create(ctxt, bbOld->getName().str().append(".return"),bbFunction);
				oldToNewValueMap.insert(make_pair(bbOld, bbnew));
			//	bbFunction->dump();
			}

			for(Function::iterator bbItr = function->begin();bbItr!=function->end();bbItr++){
				BasicBlock* oldBB = bbItr;
				//assert(oldToNewValueMap.find(oldBB) != oldToNewValueMap.end() && "Basicblock not cloned before");
				BasicBlock* newBB = (BasicBlock*) oldToNewValueMap[oldBB];
				for (auto instItr = oldBB->begin(); instItr != oldBB->end(); instItr++) {
					Instruction* oldInst = instItr;
					Instruction* newInst = oldInst->clone();
					oldToNewValueMap.insert(make_pair(oldInst, newInst));
					for (int operandIndex = 0; operandIndex < oldInst->getNumOperands(); operandIndex++) {
						Value* oldOperand = oldInst->getOperand(operandIndex);

						for (Value::use_iterator user = oldOperand->use_begin();user != oldOperand->use_end(); oldOperand++) {

						//	errs()<<"oldOperand : "<<oldOperand->getName().str()<<", uses: "<<user->getName().str()<<"\n";
						}
						if (oldToNewValueMap.find(oldOperand) != oldToNewValueMap.end()) {
							newInst->setOperand(operandIndex, oldToNewValueMap[oldOperand]);
						}
						if (isa<PHINode>(newInst)) {
							((PHINode*) newInst)->setIncomingBlock(operandIndex,
									(BasicBlock*) oldToNewValueMap[((PHINode*) newInst)->getIncomingBlock(operandIndex)]);
						}
					}
					newBB->getInstList().insert(newBB->end(), newInst);
				}
				newBB->getParent()->dump();
			}

		}
		/*for (Module::iterator funcItr = M.begin(); funcItr != M.end(); funcItr++) {
			Function* function = funcItr;
			function->dump();
		}*/
		return true;
	}
};
char BBToFunctionPass::ID = 2;
char* BBToFunctionPass::NEW_NAME = "newName";
static RegisterPass<BBToFunctionPass> X("BBToFunctionPass",
		"Pass to create HyperOps");

#define MAKE_GRAPH_STRUCTURED 1
