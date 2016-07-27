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
#include "llvm/Transforms/IPO/InlinerPass.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/ADT/StringExtras.h"
//#include "llvm/Analysis/CallGraphSCCPass.h";
using namespace llvm;

#define DEBUG_TYPE "HyperOpCreationPass"

/**
 * Pass to create HyperOps
 */
struct HyperOpCreationPass: public ModulePass {
	static char ID; // Pass identification, replacement for typeid
	static char* NEW_NAME;
	const string REDEFINE_ANNOTATIONS = "redefine.annotations";
	const string HYPEROP = "HyperOp";
	const string HYPEROP_CONSUMED_BY = "ConsumedBy";
	const string HYPEROP_CONTROLLED_BY = "ControlledBy";
	const string HYPEROP_AFFINITY = "Affinity";
	const string HYPEROP_START = "Start";
	const string HYPEROP_END = "End";
	const string SCALAR_ARGUMENT = "Scalar";
	const string LOCAL_REFERENCE_ARGUMENT = "LocalReference";

	const unsigned int FRAME_SIZE = 4;

	enum HyperOpArgumentType {
		SCALAR, LOCAL_REFERENCE, GLOBAL_REFERENCE
	};

	//Map of HyperOp arguments with position as the key
	typedef list<pair<list<Value*>, HyperOpArgumentType> > HyperOpArgumentList;

	HyperOpCreationPass() :
			ModulePass(ID) {
	}

	virtual void getAnalysisUsage(AnalysisUsage &AU) const {
		//Mandatory merge return to be invoked on each function
		AU.addRequired<UnifyFunctionExitNodes>();
		AU.addRequired<DependenceAnalysis>();
		AU.addRequired<AliasAnalysis>();
//		AU.addRequired<CallGraphSCCPass>();
	}

	bool pathExistsInCFG(BasicBlock* source, BasicBlock* target, list<BasicBlock*> visitedBasicBlocks) {
		if (source->getParent() == target->getParent()) {
			visitedBasicBlocks.push_back(source);
			for (int i = 0; i < source->getTerminator()->getNumSuccessors(); i++) {
				BasicBlock* succBB = source->getTerminator()->getSuccessor(i);
				if (find(visitedBasicBlocks.begin(), visitedBasicBlocks.end(), succBB) == visitedBasicBlocks.end()) {
					if (succBB == target) {
						return true;
					} else if (pathExistsInCFG(source->getTerminator()->getSuccessor(i), target, visitedBasicBlocks)) {
						return true;
					}
				}
			}
		}
		return false;
	}

	bool isArgInList(Function* function, Value* operand) {
		for (Function::arg_iterator argItr = function->arg_begin(); argItr != function->arg_end(); argItr++) {
			if (argItr == operand) {
				return true;
			}
		}
		return false;
	}

	//Find the definitions of globals reaching the target instruction
	list<pair<Instruction*, list<CallInst*> > > reachingStoreOperationsForGlobals(Value* globalVariable, Instruction* targetInst, list<CallInst*> callSite, map<Function*, list<pair<list<BasicBlock*>, HyperOpArgumentList> > > originalFunctionToHyperOpBBListMap,
			map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> > createdHyperOpAndOriginalBasicBlockAndArgMap) {
		list<pair<Instruction*, list<CallInst*> > > reachingGlobalDefinitionSources;
		list<pair<Instruction*, list<CallInst*> > > reachingGlobalDefToFunctionCall;

		if (!callSite.empty()) {
			//Compute reaching definitions to the callsite
			CallInst* callInst = callSite.back();
			list<CallInst*> tempCallSite;
			std::copy(callSite.begin(), callSite.end(), std::back_inserter(tempCallSite));
			tempCallSite.pop_back();
			reachingGlobalDefToFunctionCall = reachingStoreOperationsForGlobals(globalVariable, callInst, tempCallSite, originalFunctionToHyperOpBBListMap, createdHyperOpAndOriginalBasicBlockAndArgMap);
		}

		list<pair<Instruction*, list<CallInst*> > > reachingDefinitionAndCallSite;
		list<BasicBlock*> accumulatedBasicBlocks;
		Function* originalFunction = targetInst->getParent()->getParent();
		list<pair<list<BasicBlock*>, HyperOpArgumentList> > tempBBList = originalFunctionToHyperOpBBListMap[originalFunction];
		for (list<pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator tempBBListItr = tempBBList.begin(); tempBBListItr != tempBBList.end(); tempBBListItr++) {
			if (find(tempBBListItr->first.begin(), tempBBListItr->first.end(), targetInst->getParent()) != tempBBListItr->first.end()) {
				accumulatedBasicBlocks = tempBBListItr->first;
			}
		}

		map<BasicBlock*, pair<Instruction*, list<CallInst*> > > reachingDefinitions;
		for (Function::iterator originalBBItr = originalFunction->begin(); originalBBItr != originalFunction->end(); originalBBItr++) {
			BasicBlock* originalBB = &*originalBBItr;
			list<BasicBlock*> visitedBasicBlocks;
			if (originalBB == targetInst->getParent() || pathExistsInCFG(originalBB, targetInst->getParent(), visitedBasicBlocks)) {
				bool originalBBHasDefinitionWithSomeUses = false;
				//Reverse iterator so that the last store is encountered first
				for (BasicBlock::reverse_iterator instrItr = originalBB->rbegin(); instrItr != originalBB->rend(); instrItr++) {
					Instruction* instr = &*instrItr;
					if (isa<StoreInst>(instr) && ((StoreInst*) instr)->getOperand(0) == globalVariable && reachingDefinitions.find(originalBB) == reachingDefinitions.end()) {
						//Check if the store instruction is reachable to any of the uses of the argument in the accumulated bb list
						for (Value::use_iterator useItr = globalVariable->use_begin(); useItr != globalVariable->use_end(); useItr++) {
							User* user = *useItr;
							list<BasicBlock*> visitedBasicBlocks;
							if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), ((Instruction*) user)->getParent()) != accumulatedBasicBlocks.end() && pathExistsInCFG(originalBB, ((Instruction*) user)->getParent(), visitedBasicBlocks)) {
								originalBBHasDefinitionWithSomeUses = true;
								break;
							}
						}
						if (originalBBHasDefinitionWithSomeUses) {
							reachingDefinitions[originalBB] = make_pair(instr, callSite);
							break;
						}
					}
				}

				if (!originalBBHasDefinitionWithSomeUses && !reachingGlobalDefToFunctionCall.empty()) {
					for (list<pair<Instruction*, list<CallInst*> > >::iterator reachingGlobalDefItr = reachingGlobalDefToFunctionCall.begin(); reachingGlobalDefItr != reachingGlobalDefToFunctionCall.end(); reachingGlobalDefItr++) {
						reachingDefinitions[originalBB] = *reachingGlobalDefItr;
					}
				}
			}
		}

		list<BasicBlock*> discardList;
		for (map<BasicBlock*, pair<Instruction*, list<CallInst*> > >::iterator defBBItr = reachingDefinitions.begin(); defBBItr != reachingDefinitions.end(); defBBItr++) {
			BasicBlock* defBB = defBBItr->first;
			for (map<BasicBlock*, pair<Instruction*, list<CallInst*> > >::iterator secDefBBItr = reachingDefinitions.begin(); secDefBBItr != reachingDefinitions.end(); secDefBBItr++) {
				BasicBlock* secDefBB = secDefBBItr->first;
				list<BasicBlock*> visitedBasicBlocks;
				if (secDefBB != defBB && (pathExistsInCFG(defBB, secDefBB, visitedBasicBlocks) || find(discardList.begin(), discardList.end(), secDefBB) != discardList.end())) {
					discardList.push_back(defBB);
				}
			}
		}

		for (map<BasicBlock*, pair<Instruction*, list<CallInst*> > >::iterator defBBItr = reachingDefinitions.begin(); defBBItr != reachingDefinitions.end(); defBBItr++) {
			if (find(discardList.begin(), discardList.end(), defBBItr->first) == discardList.end()) {
				reachingGlobalDefinitionSources.push_back(make_pair(defBBItr->second.first, defBBItr->second.second));
			}
		}
		return reachingGlobalDefinitionSources;
	}

	//List of basic blocks containing store instructions that reach currentInstr
	map<BasicBlock*, Instruction*> reachingStoreOperations(Instruction* useInstr, Function* originalFunction, list<BasicBlock*> accumulatedBasicBlocks) {
		map<BasicBlock*, Instruction*> reachingDefinitions;
		//Find uses of allocInstr in the basic blocks of created functions
		map<BasicBlock*, Instruction*> basicBlocksWithDefinitions;
		basicBlocksWithDefinitions.insert(make_pair(useInstr->getParent(), useInstr));
		for (Function::iterator originalBBItr = originalFunction->begin(); originalBBItr != originalFunction->end(); originalBBItr++) {
			BasicBlock* originalBB = &*originalBBItr;
			for (BasicBlock::iterator instrItr = originalBB->begin(); instrItr != originalBB->end(); instrItr++) {
				Instruction* instr = instrItr;
				//Check the uses in BasicBlocks that are predecessors and use allocInstr
				list<BasicBlock*> visitedBasicBlocks;
				if (isa<StoreInst>(instr) && ((StoreInst*) instr)->getOperand(1) == useInstr && pathExistsInCFG(originalBB, useInstr->getParent(), visitedBasicBlocks)) {
					//A previous store to the same memory location exists, we need to consider the latest definition
					if (basicBlocksWithDefinitions.find(originalBB) != basicBlocksWithDefinitions.end()) {
						basicBlocksWithDefinitions.erase(originalBB);
					}
					basicBlocksWithDefinitions.insert(make_pair(originalBB, instr));
					break;
				}
			}

			//Eliminate all transitive paths and retain the last one
			list<BasicBlock*> discardList;
			for (map<BasicBlock*, Instruction*>::iterator defBBItr = basicBlocksWithDefinitions.begin(); defBBItr != basicBlocksWithDefinitions.end(); defBBItr++) {
				BasicBlock* defBB = defBBItr->first;
				for (map<BasicBlock*, Instruction*>::iterator secDefBBItr = basicBlocksWithDefinitions.begin(); secDefBBItr != basicBlocksWithDefinitions.end(); secDefBBItr++) {
					BasicBlock* secDefBB = secDefBBItr->first;
					list<BasicBlock*> visitedBasicBlocks;
					if (secDefBB != defBB && (pathExistsInCFG(defBB, secDefBB, visitedBasicBlocks) || find(discardList.begin(), discardList.end(), secDefBB) != discardList.end())) {
						discardList.push_back(defBB);
					}
				}
			}

			for (map<BasicBlock*, Instruction*>::iterator defBBItr = basicBlocksWithDefinitions.begin(); defBBItr != basicBlocksWithDefinitions.end(); defBBItr++) {
				//If the store instr is reachable i.e., not in discard list
				if (find(discardList.begin(), discardList.end(), defBBItr->first) == discardList.end()) {
					for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
						list<BasicBlock*> visitedBasicBlocks;
						if (pathExistsInCFG(defBBItr->first, *accumulatedBBItr, visitedBasicBlocks) && reachingDefinitions.find(defBBItr->first) == reachingDefinitions.end()) {
							reachingDefinitions.insert(*defBBItr);
						}
					}
				}
			}
		}
		return reachingDefinitions;
	}

	HyperOpArgumentType supportedArgType(Value* argument, Module &m) {
		if (argument->getType()->getTypeID() == Type::FloatTyID) {
			return LOCAL_REFERENCE;
		}

		if (isa<AllocaInst>(argument)) {
			if (((AllocaInst*) argument)->getType()->getPointerElementType()->getTypeID() == Type::IntegerTyID) {
				return SCALAR;
			}
			return LOCAL_REFERENCE;
		}

		if (isa<GetElementPtrInst>(argument)) {
			for (Module::global_iterator globalVarItr = m.global_begin(); globalVarItr != m.global_end(); globalVarItr++) {
				if (((GetElementPtrInst*) argument)->getPointerOperand()->getName().equals(globalVarItr->getName())) {
					return GLOBAL_REFERENCE;
				}
			}
			return LOCAL_REFERENCE;
		}

		return SCALAR;
	}

	unsigned distanceToExitBlock(BasicBlock* basicBlock, list<BasicBlock*> visitedBasicBlocks) {
		//Merge return assumed here
		unsigned depthOfSuccessor = 0;
		bool first = true;
		visitedBasicBlocks.push_back(basicBlock);
		for (unsigned i = 0; i < basicBlock->getTerminator()->getNumSuccessors(); i++) {
			BasicBlock* succBB = basicBlock->getTerminator()->getSuccessor(i);
			if (find(visitedBasicBlocks.begin(), visitedBasicBlocks.end(), succBB) == visitedBasicBlocks.end() && (first || depthOfSuccessor > distanceToExitBlock(succBB, visitedBasicBlocks))) {
				depthOfSuccessor = distanceToExitBlock(succBB, visitedBasicBlocks);
				first = false;
			}
		}
		return 1 + depthOfSuccessor;
	}

	Instruction* getClonedArgument(Value* argument, list<CallInst*> callSite, map<Function*, list<CallInst*> > createdHyperOpAndCallSite, map<Function*, map<Instruction*, Instruction*> > originalToClonedInstructionMap) {
		if (isa<Instruction>(argument)) {
			for (map<Function*, list<CallInst*> >::iterator createdHopItr = createdHyperOpAndCallSite.begin(); createdHopItr != createdHyperOpAndCallSite.end(); createdHopItr++) {
				list<CallInst*> createdHopCallSite = createdHopItr->second;
				bool callSiteMatch = true;
				if (createdHopCallSite.size() != callSite.size()) {
					callSiteMatch = false;
				}
				if (callSiteMatch) {
					list<CallInst*>::iterator callSiteItr = callSite.begin();
					for (list<CallInst*>::iterator createHopCallSiteItr = createdHopCallSite.begin(); createHopCallSiteItr != createdHopCallSite.end() && callSiteItr != callSite.end(); createHopCallSiteItr++, callSiteItr++) {
						if (*createHopCallSiteItr != *callSiteItr) {
							callSiteMatch = false;
							break;
						}
					}
				}

				if (callSiteMatch) {
					Function* createdFunctionOfCallSite = createdHopItr->first;
					//TODO is this true for phi operands as well?
					if (originalToClonedInstructionMap[createdFunctionOfCallSite].find((Instruction*) argument) != originalToClonedInstructionMap[createdFunctionOfCallSite].end()) {
						return originalToClonedInstructionMap[createdFunctionOfCallSite][(Instruction*) argument];
					}
				}
			}
		}
		if (!callSite.empty()) {
			callSite.pop_back();
		}

		return getClonedArgument(argument, callSite, createdHyperOpAndCallSite, originalToClonedInstructionMap);
	}

	void addInitializationInstructions(GlobalVariable* global, Constant* initializer, vector<Value*> idList, Instruction* insertBefore, Type* type) {
		//TODO Constant cannot be vector or blockaddress
		LLVMContext & ctx = insertBefore->getParent()->getContext();
		if (!type->isAggregateType()) {
			if (isa<ConstantInt>(initializer) || isa<ConstantFP>(initializer) || isa<ConstantExpr>(initializer) || initializer->isZeroValue()) {
				GetElementPtrInst* typeAddrInst = GetElementPtrInst::CreateInBounds(global, idList, "", insertBefore);
				typeAddrInst->dump();
				if (initializer->isZeroValue()) {
					Value* zero = ConstantInt::get(ctx, APInt(32, 0));
					StoreInst* storeInst = new StoreInst(zero, typeAddrInst, insertBefore);
					storeInst->setAlignment(4);
				} else {
					StoreInst* storeInst = new StoreInst(initializer, typeAddrInst, insertBefore);
					storeInst->setAlignment(4);
				}
			}
		} else if (type->isArrayTy()) {
			for (unsigned i = 0; i < type->getArrayNumElements(); i++) {
				Constant* subTypeInitializer;
				idList.push_back(ConstantInt::get(ctx, APInt(32, i)));
				//Find the subtype's initializer
				if (ConstantDataSequential *CDS = dyn_cast<ConstantDataSequential>(initializer)) {
					subTypeInitializer = CDS->getElementAsConstant(i);
				} else if (initializer->getNumOperands() > i) {
					subTypeInitializer = cast<Constant>(initializer->getOperand(i));
				} else {
					subTypeInitializer = initializer;
				}
				addInitializationInstructions(global, subTypeInitializer, idList, insertBefore, type->getArrayElementType());
				idList.pop_back();
			}
		} else {
			for (unsigned subTypeIndex = 0; subTypeIndex < type->getNumContainedTypes(); subTypeIndex++) {
				Type* subType = type->getContainedType(subTypeIndex);
				Constant* subTypeInitializer;
				idList.push_back(ConstantInt::get(ctx, APInt(32, subTypeIndex)));
				//Find the subtype's initializer
				if (ConstantDataSequential *CDS = dyn_cast<ConstantDataSequential>(initializer)) {
					subTypeInitializer = CDS->getElementAsConstant(subTypeIndex);
				} else if (initializer->getNumOperands() > subTypeIndex) {
					subTypeInitializer = cast<Constant>(initializer->getOperand(subTypeIndex));
				} else {
					subTypeInitializer = initializer;
				}
				addInitializationInstructions(global, subTypeInitializer, idList, insertBefore, subType);
				idList.pop_back();
			}
		}
	}

	list<pair<Function*, Instruction*> > getTopmostParentInJumpChain(Function* function, map<Function*, list<pair<Function*, Instruction*> > > functionAndJumpSources) {
		list<pair<Function*, Instruction*> > topmostParents;
		list<pair<Function*, Instruction*> > parents = functionAndJumpSources[function];
		for (list<pair<Function*, Instruction*> >::iterator parentItr = parents.begin(); parentItr != parents.end(); parentItr++) {
			if (functionAndJumpSources.find(parentItr->first) != functionAndJumpSources.end()) {
				list<pair<Function*, Instruction*> > tempParents = getTopmostParentInJumpChain(parentItr->first, functionAndJumpSources);
				std::copy(tempParents.begin(), tempParents.end(), std::back_inserter(topmostParents));
			} else {
				topmostParents.push_back(make_pair(parentItr->first, parentItr->second));
			}
		}
		return topmostParents;
	}

	virtual bool runOnModule(Module &M) {
		LLVMContext & ctxt = M.getContext();
		//Top level annotation corresponding to all annotations REDEFINE
		NamedMDNode * redefineAnnotationsNode = M.getOrInsertNamedMetadata(REDEFINE_ANNOTATIONS);

		map<Function*, list<pair<list<BasicBlock*>, HyperOpArgumentList> > > originalFunctionToHyperOpBBListMap;

		//Contains all created HyperOp functions and the basic blocks they contain in the original module and HyperOp arguments
		map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> > createdHyperOpAndOriginalBasicBlockAndArgMap;
		map<Function*, list<CallInst*> > createdHyperOpAndCallSite;
		map<Function*, Value*> createdHyperOpAndReturnValue;
		map<Function*, map<Instruction*, vector<unsigned> > > createdHyperOpAndConditionalBranchSources;
		map<Function*, list<Instruction*> > createdHyperOpAndUnconditionalBranchSources;
		map<Function*, list<Instruction*> > createdHyperOpAndReachingDefSources;

		//Cloned instructions mapped to their original instruction for each created function
		map<Function*, map<Instruction*, Instruction*> > functionOriginalToClonedInstructionMap;

		//Map of the "return void" instruction in each HyperOp function
		map<Function*, Instruction*> retInstMap;

		//Map of created HyperOps and their annotations
		map<Function*, MDNode*> hyperOpAndAnnotationMap;

		map<Value*, Instruction*> allocaInstCreatedForIntermediateValues;

		//Add all the functions to be traversed in a list
		list<Function*> functionList;
		Function* mainFunction = 0;
		map<Function*, list<Function*> > calledFunctionMap;
		for (Module::iterator funcItr = M.begin(); funcItr != M.end(); funcItr++) {
			Function* function = funcItr;
			if (!function->isIntrinsic()) {
				functionList.push_back(function);
			}
			if (function->getName().compare(REDEFINE_START_FUNCTION) == 0) {
				mainFunction = function;
			}

			list<Function*> calledFunctions;
			for (Function::iterator bbItr = function->begin(); bbItr != function->end(); bbItr++) {
				for (BasicBlock::iterator instItr = bbItr->begin(); instItr != bbItr->end(); instItr++) {
					Instruction* inst = &*instItr;
					if (isa<CallInst>(inst)) {
						calledFunctions.push_back(((CallInst*) inst)->getCalledFunction());
					}
				}
			}
			calledFunctionMap[function] = calledFunctions;
		}

		if (mainFunction == 0) {
			DEBUG(errs() << "REDEFINE kernel's entry point i.e., redefine_start function missing, aborting\n");
			return false;
		}

		if (!mainFunction->getArgumentList().empty()) {
			DEBUG(errs() << "REDEFINE kernel's entry point i.e., redefine_start function takes arguments, aborting\n");
			return false;
		}

		if (mainFunction->getReturnType()->getTypeID() != Type::VoidTyID) {
			DEBUG(errs() << "REDEFINE kernel's entry point i.e., redefine_start function returning a non-void value, aborting\n");
			return false;
		}

		while (!functionList.empty()) {
			Function* function = functionList.front();
			functionList.pop_front();
			StringRef name = function->getName();
			if (function == mainFunction) {
				Instruction* startOfBB = function->begin()->begin();
				//Add initializers to globals that are not redefine inputs or outputs
				for (Module::global_iterator globalVarItr = M.global_begin(); globalVarItr != M.global_end(); globalVarItr++) {
					if (!globalVarItr->getName().startswith(REDEFINE_INPUT_PREFIX) && !globalVarItr->getName().startswith(REDEFINE_OUTPUT_PREFIX) && !globalVarItr->getName().startswith(REDEFINE_INOUT_PREFIX)) {
						//Externs are not allowed as of now and hence, there is no need to check if the global var has an initializer at all or otherwise
						Constant * initializer = globalVarItr->getInitializer();
						vector<Value*> idList;
						idList.push_back(ConstantInt::get(ctxt, APInt(32, 0)));
						addInitializationInstructions(globalVarItr, initializer, idList, startOfBB, initializer->getType());
					}
				}
			}

			list<pair<list<BasicBlock*>, HyperOpArgumentList> > hyperOpBBAndArgs;
			list<BasicBlock*> accumulatedBasicBlocks;
			HyperOpArgumentList hyperOpArguments;
			bool endOfHyperOp = false;
			list<BasicBlock*> traversedBasicBlocks;
			list<BasicBlock*> bbTraverser;
			unsigned hyperOpArgCount = 0;

			bbTraverser.push_back(function->begin());
			while (!bbTraverser.empty()) {
				BasicBlock* bbItr = bbTraverser.front();
				bbTraverser.pop_front();
				bool canAcquireBBItr = true;
				//If basic block does not have a unique predecessor and basic block is not the entry block
				if (bbItr != &(function->getEntryBlock())) {
					//Check if all the parent nodes of the basic block are in the same HyperOp
					//While this should in principle be done to all producers of data as well, since we are choosing one basic-block after another from a CFG to form a HyperOp, immediate predecessors should suffice
					list<BasicBlock*> predecessorsFromSameFunction;
					for (pred_iterator predecessorItr = pred_begin(bbItr); predecessorItr != pred_end(bbItr); predecessorItr++) {
						BasicBlock* predecessor = *predecessorItr;
						if (predecessor->getParent() == function) {
							predecessorsFromSameFunction.push_back(predecessor);
						}
					}

					//More than one predecessor
					if (predecessorsFromSameFunction.size() > 1) {
						for (list<BasicBlock*>::iterator predecessorItr = predecessorsFromSameFunction.begin(); predecessorItr != predecessorsFromSameFunction.end(); predecessorItr++) {
							BasicBlock* dependenceSource = *predecessorItr;
							for (list<BasicBlock*>::iterator secondPredecessorItr = predecessorsFromSameFunction.begin(); secondPredecessorItr != predecessorItr && secondPredecessorItr != predecessorsFromSameFunction.end(); secondPredecessorItr++) {
								BasicBlock *dependenceTarget = *secondPredecessorItr;
								//If dependence target does not belong to the same HyperOp
								list<BasicBlock*> visitedBasicBlocks;
								if (pathExistsInCFG(dependenceSource, dependenceTarget, visitedBasicBlocks)
										&& (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), dependenceSource) != accumulatedBasicBlocks.end() && find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), dependenceTarget) == accumulatedBasicBlocks.end())) {
									canAcquireBBItr = false;
									break;
								}
							}
							if (!canAcquireBBItr) {
								break;
							}
						}
					}

					//Check if the basic block's inclusion results introduces multiple entry points to the same HyperOp
					if (canAcquireBBItr && !accumulatedBasicBlocks.empty()) {
						//Find the basic blocks that jump to the basic block being acquired
						for (Function::iterator sourceBBItr = function->begin(); sourceBBItr != function->end(); sourceBBItr++) {
							for (unsigned i = 0; i < sourceBBItr->getTerminator()->getNumSuccessors(); i++) {
								BasicBlock* successor = sourceBBItr->getTerminator()->getSuccessor(i);
								if (successor == bbItr && find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), sourceBBItr) == accumulatedBasicBlocks.end()) {
									//Predicate to the basic block in the current set that comes from outside the basic block set accumulated for the current HyperOp
									canAcquireBBItr = false;
									break;
								}
							}
							if (!canAcquireBBItr) {
								break;
							}
						}
					}
				}

				if (!canAcquireBBItr) {
					//Create a HyperOp at this boundary
					endOfHyperOp = true;
					//Place the bbItr back in its place
					bbTraverser.push_front(bbItr);
				} else {
					accumulatedBasicBlocks.push_back(bbItr);
					unsigned bbIndex = 0;
					for (BasicBlock::iterator instItr = bbItr->begin(); instItr != bbItr->end(); instItr++) {
						errs() << "dealing with instruction:";
						instItr->dump();
						if (!(isa<AllocaInst>(instItr) || isa<ReturnInst>(instItr))) {
							//Function calls that couldn't be inlined when generating the IR
							if (isa<CallInst>(instItr) || isa<InvokeInst>(instItr)) {
								Function * calledFunction;
								if (isa<CallInst>(instItr)) {
									calledFunction = ((CallInst*) ((Instruction*) instItr))->getCalledFunction();
								} else if (isa<InvokeInst>(instItr)) {
									calledFunction = ((InvokeInst*) ((Instruction*) instItr))->getCalledFunction();
								}
								//An intrinsic function translates to an instruction in the backend, don't treat it like a function call
								//Otherwise, create a function boundary as follows
								if (!calledFunction->isIntrinsic()) {
									//Size here must be >2 because if there is another instruction in the bb other than call, it is a jump anyway
									if (bbItr->getInstList().size() > 2) {
										//Call is the not only instruction here, a separate HyperOp must be created for the call statement
										string firstBBName(NEW_NAME);
										firstBBName.append(itostr(bbIndex));
										bbIndex++;
										BasicBlock* newFunctionCallBlock = bbItr->splitBasicBlock(instItr, firstBBName);
										Instruction* nonTerminalInstruction = instItr;
										if (nonTerminalInstruction != (Instruction*) bbItr->getTerminator()) {
											Instruction* instructionAfterCall = newFunctionCallBlock->begin()->getNextNode();
											string secondBBName(NEW_NAME);
											secondBBName.append(itostr(bbIndex));
											bbIndex++;
											newFunctionCallBlock->splitBasicBlock(instructionAfterCall, secondBBName);
										}
									} else {
										CallInst* callInst = (CallInst*) (&*instItr);
										for (unsigned int i = 0; i < callInst->getNumArgOperands(); i++) {
											Value * argument = callInst->getArgOperand(i);
											list<Value*> argumentList;
											argumentList.push_back(argument);
											//Find out if the argument is stored into in a predecessor HyperOp
											HyperOpArgumentType argType = supportedArgType(argument, M);
											if (argType != GLOBAL_REFERENCE) {
												hyperOpArgCount++;
											}
											hyperOpArguments.push_back(make_pair(argumentList, argType));
										}
									}
									endOfHyperOp = true;
									break;
								}
							}

							list<Value*> newHyperOpArguments;
							for (unsigned int i = 0; i < instItr->getNumOperands(); i++) {
								Value * argument = instItr->getOperand(i);
								if (!isa<Constant>(argument) && !argument->getType()->isLabelTy()) {
									//Find the reaching definition of the argument; alloca instruction maybe followed by store instructions to the memory location, we need to identify the set of store instructions to the memory location that reach the current use of the memory location
									if (isa<Instruction>(argument)) {
										if (isa<AllocaInst>(argument)) {
											bool hasReachingDefinitionFromAnotherHyperOp = false;
											map<BasicBlock*, Instruction*> reachingDefinitions = reachingStoreOperations((Instruction*) argument, function, accumulatedBasicBlocks);
											//Reaching definition to the memory location is in the same HyperOp, need not be added to hyperOp argument list
											for (map<BasicBlock*, Instruction*>::iterator reachingDefItr = reachingDefinitions.begin(); reachingDefItr != reachingDefinitions.end(); reachingDefItr++) {
												BasicBlock* reachingDefBB = reachingDefItr->first;
												if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), reachingDefBB) == accumulatedBasicBlocks.end()) {
													//Reaching definition is from a different basic block not accumulated currently
													hasReachingDefinitionFromAnotherHyperOp = true;
													break;
												}
											}
											//Reaching definition is in the accumulated basic block set
											if (!hasReachingDefinitionFromAnotherHyperOp) {
												continue;
											}
										} else if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), ((Instruction*) argument)->getParent()) != accumulatedBasicBlocks.end()) {
											continue;
										}
									}

									bool argAddedPreviously = false;
									//Check if same argument is used multiple times in the same instruction
									//We need to track the arguments in the instruction separately
									if (find(newHyperOpArguments.begin(), newHyperOpArguments.end(), argument) != newHyperOpArguments.end()) {
										argAddedPreviously = true;
									} else {
										for (HyperOpArgumentList::iterator previousArgsItr = hyperOpArguments.begin(); previousArgsItr != hyperOpArguments.end(); previousArgsItr++) {
											if (find(previousArgsItr->first.begin(), previousArgsItr->first.end(), argument) != previousArgsItr->first.end()) {
												argAddedPreviously = true;
												break;
											}
										}
									}
									if (!argAddedPreviously) {
										newHyperOpArguments.push_back(argument);
									}
								}
							}
							unsigned numArgs = newHyperOpArguments.size();
							//Phi in principle translates to only one argument at runtime
							if (isa<PHINode>(instItr)) {
								numArgs = 1;
							}

							if (numArgs + hyperOpArgCount > FRAME_SIZE) {
								//Break the basic block here
								stringstream newString("");
								newString << instItr->getParent()->getName().str();
								newString << bbIndex;
								bbItr->splitBasicBlock(instItr, newString.str());
								bbIndex++;
								endOfHyperOp = true;
								break;
							} else if (!newHyperOpArguments.empty()) {
								if (numArgs == newHyperOpArguments.size()) {
									for (list<Value*>::iterator newArgItr = newHyperOpArguments.begin(); newArgItr != newHyperOpArguments.end(); newArgItr++) {
										list<Value*> newArg;
										newArg.push_back(*newArgItr);
										HyperOpArgumentType type = supportedArgType(*newArgItr, M);
										if (type != GLOBAL_REFERENCE) {
											hyperOpArgCount++;
										}
										hyperOpArguments.push_back(make_pair(newArg, type));
									}
								} else {
									//Phi instruction's arguments correspond to only one argument to a HyperOp
									hyperOpArguments.push_back(make_pair(newHyperOpArguments, supportedArgType(newHyperOpArguments.front(), M)));
								}
							}
						}

						if (endOfHyperOp) {
							break;
						}
					}
				}

				if (!endOfHyperOp && bbTraverser.empty()) {
					bool allBBCovered = true;
					for (Function::iterator functionBBItr = function->begin(); functionBBItr != function->end(); functionBBItr++) {
						BasicBlock* originalBB = &*functionBBItr;
						bool isBBCovered = false;
						if (originalFunctionToHyperOpBBListMap.find(function) != originalFunctionToHyperOpBBListMap.end()) {
							//Check if the bb has been covered in a function created previously
							for (list<pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator funcForCreationItr = originalFunctionToHyperOpBBListMap[function].begin(); funcForCreationItr != originalFunctionToHyperOpBBListMap[function].end(); funcForCreationItr++) {
								list<BasicBlock*> secondSet = funcForCreationItr->first;
								if (find(secondSet.begin(), secondSet.end(), originalBB) != secondSet.end()) {
									isBBCovered = true;
									break;
								}
							}

							if (!isBBCovered && find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), originalBB) != accumulatedBasicBlocks.end()) {
								isBBCovered = true;
							}

							if (!isBBCovered) {
								//Atleast one basic block yet to be traversed
								allBBCovered = false;
								break;
							}
						}

						if (allBBCovered) {
							endOfHyperOp = true;
						}
					}
				}

				//Create a new HyperOp
				if (endOfHyperOp) {
					//Shuffle the arguments to the HyperOp such that scalar arguments are positioned first and the rest of the arguments are positioned after
					HyperOpArgumentList tempHyperOpArguments;
					for (HyperOpArgumentList::iterator hyperOpArgumentItr = hyperOpArguments.begin(); hyperOpArgumentItr != hyperOpArguments.end(); hyperOpArgumentItr++) {
						if (hyperOpArgumentItr->second == SCALAR) {
							tempHyperOpArguments.push_back(*hyperOpArgumentItr);
						}
					}

					for (HyperOpArgumentList::iterator hyperOpArgumentItr = hyperOpArguments.begin(); hyperOpArgumentItr != hyperOpArguments.end(); hyperOpArgumentItr++) {
						if (hyperOpArgumentItr->second != SCALAR) {
							tempHyperOpArguments.push_back(*hyperOpArgumentItr);
						}
					}

					hyperOpArguments.clear();
					if (!isa<CallInst>(accumulatedBasicBlocks.front()->front())) {
						//end of shuffling HyperOp arguments
						//Add consumedby meta data from source HyperOp to the HyperOp being created
						vector<HyperOpArgumentList::iterator> deleteItrList;
						for (HyperOpArgumentList::iterator hyperOpArgumentItr = tempHyperOpArguments.begin(); hyperOpArgumentItr != tempHyperOpArguments.end(); hyperOpArgumentItr++) {
							list<Value*> individualArguments = hyperOpArgumentItr->first;
							individualArguments.front()->dump();
							HyperOpArgumentType argumentType = hyperOpArgumentItr->second;
							if (argumentType == GLOBAL_REFERENCE) {
								continue;
							}

							bool argUsedElsewhere = false;
							for (list<Value*>::iterator individualArgItr = individualArguments.begin(); individualArgItr != individualArguments.end(); individualArgItr++) {
								Value* argument = *individualArgItr;
								if (isa<AllocaInst>(argument)) {
									for (Function::iterator useBBItr = function->begin(); useBBItr != function->end(); useBBItr++) {
										if (argument->isUsedInBasicBlock(useBBItr) && find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), useBBItr) == accumulatedBasicBlocks.end()) {
											argUsedElsewhere = true;
											break;
										}
									}
								} else {
									argUsedElsewhere = true;
								}
								if (argUsedElsewhere) {
									break;
								}
							}
							if (!argUsedElsewhere) {
								deleteItrList.push_back(hyperOpArgumentItr);
								for (list<Value*>::iterator individualArgItr = individualArguments.begin(); individualArgItr != individualArguments.end(); individualArgItr++) {
									Value* argument = *individualArgItr;
									((Instruction*) argument)->moveBefore(accumulatedBasicBlocks.front()->getFirstInsertionPt());
								}
							}
						}
						for (unsigned i = 0; i < deleteItrList.size(); i++) {
							tempHyperOpArguments.erase(deleteItrList[i]);
						}
					}

					list<BasicBlock*> tempBBList;
					//Couldn't use splice here since it clears away accumulatedBasicBlocks list
					for (list<BasicBlock*>::iterator accumulatedItr = accumulatedBasicBlocks.begin(); accumulatedItr != accumulatedBasicBlocks.end(); accumulatedItr++) {
						traversedBasicBlocks.push_back(*accumulatedItr);
						tempBBList.push_back(*accumulatedItr);
					}

					hyperOpBBAndArgs.push_back(make_pair(tempBBList, tempHyperOpArguments));
					accumulatedBasicBlocks.clear();
					hyperOpArguments.clear();
					hyperOpArgCount = 0;
					endOfHyperOp = false;
				}

				DEBUG(dbgs() << "Adding basic blocks for traversal in a depth biased order\n");
				vector<unsigned> distanceToExit;
				map<unsigned, list<BasicBlock*> > untraversedBasicBlocks;
				for (unsigned succIndex = 0; succIndex < bbItr->getTerminator()->getNumSuccessors(); succIndex++) {
					BasicBlock* succBB = bbItr->getTerminator()->getSuccessor(succIndex);
					if (find(traversedBasicBlocks.begin(), traversedBasicBlocks.end(), succBB) == traversedBasicBlocks.end() && find(bbTraverser.begin(), bbTraverser.end(), succBB) == bbTraverser.end()) {
						list<BasicBlock*> visitedBasicBlockList;
						unsigned distanceFromExit = distanceToExitBlock(succBB, visitedBasicBlockList);
						list<BasicBlock*> basicBlockList;
						if (untraversedBasicBlocks.find(distanceFromExit) != untraversedBasicBlocks.end()) {
							basicBlockList = untraversedBasicBlocks.find(distanceFromExit)->second;
							untraversedBasicBlocks.erase(distanceFromExit);
						}
						basicBlockList.push_back(succBB);
						untraversedBasicBlocks.insert(make_pair(distanceFromExit, basicBlockList));
						if (distanceToExit.empty() || find(distanceToExit.begin(), distanceToExit.end(), distanceFromExit) == distanceToExit.end()) {
							distanceToExit.push_back(distanceFromExit);
						}
					}
				}

				//Sort the basic blocks in descending order of their distance to the exit basic block to ensure that the basic block with more children gets traversed sooner, is this required though?
				list<BasicBlock*> sortedSuccBasicBlockList;
				if (distanceToExit.size() > 1) {
					for (unsigned i = 0; i < distanceToExit.size() - 1; i++) {
						unsigned min = distanceToExit[i];
						for (unsigned j = i + 1; j < distanceToExit.size(); j++) {
							if (min < distanceToExit[j]) {
								unsigned temp = distanceToExit[j];
								distanceToExit[i] = temp;
								distanceToExit[j] = min;
								min = temp;
							}
						}
					}
				}

				for (unsigned i = 0; i < distanceToExit.size(); i++) {
					list<BasicBlock*> succBBList = untraversedBasicBlocks.find(distanceToExit[i])->second;
					for (list<BasicBlock*>::iterator succBBItr = succBBList.begin(); succBBItr != succBBList.end(); succBBItr++) {
						bbTraverser.push_back(*succBBItr);
					}
				}
			}
			originalFunctionToHyperOpBBListMap[function] = hyperOpBBAndArgs;
		}

		//Done partitioning basic blocks of all functions into multiple HyperOps

		DEBUG(dbgs() << "Creating HyperOps from partitioned functions\n");
		list<pair<pair<list<BasicBlock*>, HyperOpArgumentList>, list<CallInst*> > > traversalList;

		for (list<pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator mainItr = originalFunctionToHyperOpBBListMap[mainFunction].begin(); mainItr != originalFunctionToHyperOpBBListMap[mainFunction].end(); mainItr++) {
			list<CallInst*> emptyCallList;
			traversalList.push_back(make_pair(*mainItr, emptyCallList));
		}
		//Create hyperops
		unsigned index = 0;
		while (!traversalList.empty()) {
			pair<list<BasicBlock*>, HyperOpArgumentList> functionToBeCreated = traversalList.front().first;
			//Fully qualified call site i.e., call sequence from the main function
			list<CallInst*> callSite = traversalList.front().second;
			traversalList.pop_front();

			list<BasicBlock*> accumulatedBasicBlocks = functionToBeCreated.first;
			Function* function = accumulatedBasicBlocks.front()->getParent();
			HyperOpArgumentList hyperOpArguments = functionToBeCreated.second;

			//Create a function using the accumulated basic blocks
			if (isa<CallInst>(accumulatedBasicBlocks.front()->front())) {
				CallInst* callInst = (CallInst*) &accumulatedBasicBlocks.front()->front();
				Function* calledOriginalFunction = callInst->getCalledFunction();
				//Functions to be created to substitute the called original function need to be pushed to traversal list
				list<pair<list<BasicBlock*>, HyperOpArgumentList> > calledFunctionBBList = originalFunctionToHyperOpBBListMap[calledOriginalFunction];
				//Update the arguments to the HyperOp to be created in place of the callsite
				for (list<pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator replacementFuncItr = calledFunctionBBList.begin(); replacementFuncItr != calledFunctionBBList.end(); replacementFuncItr++) {
					callSite.push_back(callInst);
					traversalList.push_front(make_pair(make_pair(replacementFuncItr->first, replacementFuncItr->second), callSite));
				}
			} else {
				DEBUG(dbgs() << "\n===============Creating a new HyperOp for function:" << function->getName() << "=============\n");
				map<Instruction*, Instruction*> originalToClonedInstMap;
				map<BasicBlock*, BasicBlock*> originalToClonedBasicBlockMap;
				vector<Type*> argsList;
				for (HyperOpArgumentList::iterator hyperOpArgumentItr = hyperOpArguments.begin(); hyperOpArgumentItr != hyperOpArguments.end(); hyperOpArgumentItr++) {
					//Set type of each argument of the HyperOp
					Value* argument = hyperOpArgumentItr->first.front();
					if (hyperOpArgumentItr->second == GLOBAL_REFERENCE) {
						continue;
					}
					argsList.push_back(argument->getType());
				}
				FunctionType *FT = FunctionType::get(Type::getVoidTy(getGlobalContext()), argsList, false);
				Function *newFunction = Function::Create(FT, Function::ExternalLinkage, function->getName(), &M);
				//Mark HyperOp function arguments which are not global references or local references as inReg to ease register allocation
				int functionArgumentIndex = 1;
				for (HyperOpArgumentList::iterator hyperOpArgItr = hyperOpArguments.begin(); hyperOpArgItr != hyperOpArguments.end(); hyperOpArgItr++) {
					HyperOpArgumentType type = hyperOpArgItr->second;
					switch (type) {
					case GLOBAL_REFERENCE:
						continue;
					case SCALAR:
						newFunction->addAttribute(functionArgumentIndex, Attribute::InReg);
						break;
					}
					functionArgumentIndex++;
				}

				//Compute branch sources
				map<Instruction*, vector<unsigned> > conditionalBranchSources;
				list<Instruction*> unconditionalBranchSources;
				list<Instruction*> reachingGlobalDefinitionSources;

				//Add a basic block with a dummy return instruction as the single point of exit for a HyperOp
				BasicBlock* retBB = BasicBlock::Create(ctxt, newFunction->getName().str().append(".return"), newFunction);
				Instruction* retInst = ReturnInst::Create(ctxt);
				retInstMap.insert(make_pair(newFunction, retInst));
				retBB->getInstList().insert(retBB->getFirstInsertionPt(), retInst);

				for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
					string bbName = "";
					bbName.append(newFunction->getName()).append(".");
					bbName.append((*accumulatedBBItr)->getName());
					BasicBlock *newBB = BasicBlock::Create(getGlobalContext(), bbName, newFunction, retBB);
					originalToClonedBasicBlockMap.insert(make_pair(*accumulatedBBItr, newBB));
					//Cloning instructions in the reverse order so that the user instructions are cloned before the definition instructions
					for (BasicBlock::iterator instItr = (*accumulatedBBItr)->begin(); instItr != (*accumulatedBBItr)->end(); instItr++) {
						Instruction* clonedInst;
						if (isa<ReturnInst>(&*instItr) && ((ReturnInst*) &*instItr)->getReturnValue() != 0 && ((ReturnInst*) &*instItr)->getReturnValue()->getType()->getTypeID() != Type::VoidTyID) {
							clonedInst = BranchInst::Create(retBB, newBB);
							createdHyperOpAndReturnValue.insert(make_pair(newFunction, instItr->getOperand(0)));
							originalToClonedInstMap.insert(std::make_pair(instItr, clonedInst));
						} else {
							clonedInst = instItr->clone();
							Instruction * originalInstruction = instItr;
							originalToClonedInstMap.insert(std::make_pair(originalInstruction, clonedInst));
							newBB->getInstList().insert(newBB->end(), clonedInst);
							for (unsigned operandIndex = 0; operandIndex < clonedInst->getNumOperands(); operandIndex++) {
								Value* operandToBeReplaced = clonedInst->getOperand(operandIndex);
								//If the instruction operand is an argument to the HyperOp
								unsigned hyperOpArgIndex = 0;
								bool argUpdated = false;
								for (HyperOpArgumentList::iterator argumentItr = hyperOpArguments.begin(); argumentItr != hyperOpArguments.end(); argumentItr++) {
									//If the argument is a scalar or a local reference
									if (argumentItr->second != GLOBAL_REFERENCE) {
										list<Value*> individualArguments = argumentItr->first;
										for (list<Value*>::iterator argumentValueItr = individualArguments.begin(); argumentValueItr != individualArguments.end(); argumentValueItr++) {
											if (*argumentValueItr == operandToBeReplaced) {
												//Get Value object of the newly created function's argument corresponding to the replacement
												for (Function::arg_iterator argItr = newFunction->arg_begin(); argItr != newFunction->arg_end(); argItr++) {
													if ((*argItr).getArgNo() == hyperOpArgIndex) {
														clonedInst->setOperand(operandIndex, argItr);
														argUpdated = true;
														break;
													}
												}
												if (argUpdated) {
													break;
												}
											}
										}
									}
									hyperOpArgIndex++;
								}

								//Find the definitions added previously which reach the use
								if (!argUpdated) {
									//If the original operand is an instruction that was cloned previously which belongs to the list of accumulated HyperOps
									for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
										for (BasicBlock::iterator accumulatedInstItr = (*accumulatedBBItr)->begin(); accumulatedInstItr != (*accumulatedBBItr)->end(); accumulatedInstItr++) {
											for (Value::use_iterator useItr = accumulatedInstItr->use_begin(); useItr != accumulatedInstItr->use_end(); useItr++) {
												if (*useItr == instItr && ((Instruction*) instItr->getOperand(operandIndex)) == accumulatedInstItr) {
													Instruction* clonedSourceInstr = (Instruction*) originalToClonedInstMap.find(accumulatedInstItr)->second;
													clonedInst->setOperand(operandIndex, clonedSourceInstr);
												}
											}
										}
									}
								}
							}
						}
					}
				}

				if (!callSite.empty()) {
					CallInst* callInst = callSite.back();
					accumulatedBasicBlocks.push_back(callInst->getParent());
				}
				//Update the branch instruction targets to point to the basic blocks in the cloned set
				for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
					for (list<BasicBlock*>::iterator bbItr = accumulatedBasicBlocks.begin(); bbItr != accumulatedBasicBlocks.end(); bbItr++) {
						TerminatorInst* terminator = (*bbItr)->getTerminator();
						for (unsigned i = 0; i < terminator->getNumSuccessors(); i++) {
							if (terminator->getSuccessor(i) == (*accumulatedBBItr)) {
								((TerminatorInst*) originalToClonedInstMap[terminator])->setSuccessor(i, originalToClonedBasicBlockMap[*accumulatedBBItr]);
							}
						}
					}
					//Find out if any basic block is predicated
					BasicBlock* originalBB = *accumulatedBBItr;
					//Predicates are only for non-entry basic blocks
					if (originalBB != &(*accumulatedBBItr)->getParent()->getEntryBlock()) {
						for (pred_iterator predecessorItr = pred_begin(originalBB); predecessorItr != pred_end(originalBB); predecessorItr++) {
							BasicBlock* predecessor = *predecessorItr;
							//Control flow edge originates from a predecessor basic block that does not belong to the same HyperOp
							if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), predecessor) == accumulatedBasicBlocks.end()) {
								for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHopItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHopItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHopItr++) {
									list<BasicBlock*> containedBasicBlocks = createdHopItr->second.first;
									if (find(containedBasicBlocks.begin(), containedBasicBlocks.end(), predecessor) != containedBasicBlocks.end()) {
										TerminatorInst* terminator = predecessor->getTerminator();
										//If the terminator instruction has only one successor
										if (terminator->getNumSuccessors() == 1 && terminator->getSuccessor(0) == originalBB) {
											//Terminator has no operands, its an unconditional jump
											//Add the instruction before the unconditional jump to contain the metadata
											unconditionalBranchSources.push_back(terminator);
										} else {
											//Get the first operand of terminator instruction corresponding to a branch
											vector<unsigned> successorBBList;
											for (int i = 0; i < terminator->getNumSuccessors(); i++) {
												if (terminator->getSuccessor(i) == originalBB) {
													//Add only those successors that correspond to a basic block in the current HyperOp
													successorBBList.push_back(i + 1);
												}
											}
											if (successorBBList.size() == terminator->getNumSuccessors()) {
												errs()<<"unconditional jump source added:";
												terminator->dump();
												//All successors of the terminator instruction are in the same target HyperOp, mark the jump as unconditional
												unconditionalBranchSources.push_back(terminator);
											} else {
												if (conditionalBranchSources.find(terminator) != conditionalBranchSources.end()) {
													vector<unsigned> previousSuccessorIndexList = conditionalBranchSources[terminator];
													conditionalBranchSources.erase(terminator);
													std::copy(previousSuccessorIndexList.begin(), previousSuccessorIndexList.end(), back_inserter(successorBBList));
												}
												conditionalBranchSources[terminator] = successorBBList;
											}
										}
									}
								}
							}
						}
					}
				}

				if (!callSite.empty()) {
					accumulatedBasicBlocks.pop_back();
				}

				Value * values[2];
				values[0] = MDString::get(ctxt, HYPEROP);
				values[1] = newFunction;
				MDNode *funcAnnotation = MDNode::get(ctxt, values);
				hyperOpAndAnnotationMap.insert(make_pair(newFunction, funcAnnotation));
				redefineAnnotationsNode->addOperand(funcAnnotation);

				bool isEntry = false;
				bool isExit = false;
				if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), &mainFunction->getEntryBlock()) != accumulatedBasicBlocks.end()) {
					isEntry = true;
				}
				//First instruction in redefine_start is a call
				else if (!callSite.empty() && (&mainFunction->getEntryBlock().front()) == callSite.back() && find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), &function->getEntryBlock()) != accumulatedBasicBlocks.end()) {
					isEntry = true;
				}

				if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), &mainFunction->back()) != accumulatedBasicBlocks.end()) {
					isExit = true;
				}
				//last instruction in redefine_start is a call
				else if (!callSite.empty() && (mainFunction->back().getTerminator()->getPrevNode()) == callSite.back() && find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), &function->back()) != accumulatedBasicBlocks.end()) {
					isExit = true;
				}

				//Is the function an entry node/exit node/intermediate ?
				if (isEntry) {
					Value* hyperOpDescrMDValues[2];
					hyperOpDescrMDValues[0] = MDString::get(ctxt, HYPEROP_ENTRY);
					hyperOpDescrMDValues[1] = funcAnnotation;
					MDNode* hyperOpDescMDNode = MDNode::get(ctxt, hyperOpDescrMDValues);
					redefineAnnotationsNode->addOperand(hyperOpDescMDNode);
				}
				if (isExit) {
					Value* hyperOpDescrMDValues[2];
					hyperOpDescrMDValues[0] = MDString::get(ctxt, HYPEROP_EXIT);
					hyperOpDescrMDValues[1] = funcAnnotation;
					MDNode* hyperOpDescMDNode = MDNode::get(ctxt, hyperOpDescrMDValues);
					redefineAnnotationsNode->addOperand(hyperOpDescMDNode);
				}
				if (!isEntry && !isExit) {
					Value* hyperOpDescrMDValues[2];
					hyperOpDescrMDValues[0] = MDString::get(ctxt, HYPEROP_INTERMEDIATE);
					hyperOpDescrMDValues[1] = funcAnnotation;
					MDNode* hyperOpDescMDNode = MDNode::get(ctxt, hyperOpDescrMDValues);
					redefineAnnotationsNode->addOperand(hyperOpDescMDNode);
				}

				createdHyperOpAndOriginalBasicBlockAndArgMap[newFunction] = make_pair(accumulatedBasicBlocks, hyperOpArguments);
				createdHyperOpAndCallSite[newFunction] = callSite;
				createdHyperOpAndConditionalBranchSources[newFunction] = conditionalBranchSources;
				createdHyperOpAndUnconditionalBranchSources[newFunction] = unconditionalBranchSources;
				createdHyperOpAndReachingDefSources[newFunction] = reachingGlobalDefinitionSources;
				functionOriginalToClonedInstructionMap[newFunction] = originalToClonedInstMap;
			}
		}
		//End of creation of hyperops

		errs() << "\n=============module before patching===============\n";
		M.dump();
		DEBUG(dbgs() << "Adding dependences across created HyperOps\n");
		map<Function*, list<pair<Function*, Instruction*> > > createdFunctionAndUnconditionalJumpSources;
		//Add metadata: This code is moved here to ensure that all the functions (corresponding to HyperOps) that need to be created have already been created
		for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHyperOpItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHyperOpItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHyperOpItr++) {
			Function* createdFunction = createdHyperOpItr->first;
			DEBUG(dbgs() << "\n-----------Patching created function " << createdFunction->getName() << "--------------\n");
			list<BasicBlock*> accumulatedOriginalBasicBlocks = createdHyperOpItr->second.first;
			HyperOpArgumentList hyperOpArguments = createdHyperOpItr->second.second;
			map<Instruction*, vector<unsigned> > conditionalBranchSources = createdHyperOpAndConditionalBranchSources[createdFunction];
			list<Instruction*> unconditionalBranchSources = createdHyperOpAndUnconditionalBranchSources[createdFunction];
			list<Instruction*> reachingDefSources = createdHyperOpAndReachingDefSources[createdFunction];

			list<Function*> addedParentsToCurrentHyperOp;
			MDNode* funcAnnotation = hyperOpAndAnnotationMap.find(createdFunction)->second;
			list<CallInst*> callSite = createdHyperOpAndCallSite[createdFunction];

			DEBUG(dbgs() << "Adding consumed by metadata\n");
			unsigned hyperOpArgumentIndex = 0;
			//Replace arguments of called functions with the right call arguments or return values
			for (HyperOpArgumentList::iterator hyperOpArgItr = hyperOpArguments.begin(); hyperOpArgItr != hyperOpArguments.end(); hyperOpArgItr++) {
				errs() << "Patching argument ";
				hyperOpArgItr->first.front()->dump();
				list<Instruction*> replacementArg;
				HyperOpArgumentType replacementArgType = hyperOpArgItr->second;
				//Add consumedby meta data from source HyperOp to the HyperOp being created
				//the argument is a function argument
				//Local arguments that come from within the original function will not be considered here
				if (hyperOpArgItr->second != GLOBAL_REFERENCE && !isa<Instruction>(hyperOpArgItr->first.front())) {
					assert(!callSite.empty() && "Function without a callsite marked as a hyperop!");
					unsigned positionOfFormalArg = 0;
					Function* originalFunction = accumulatedOriginalBasicBlocks.front()->getParent();
					for (Function::arg_iterator originalArgItr = originalFunction->arg_begin(); originalArgItr != originalFunction->arg_end(); originalArgItr++, positionOfFormalArg++) {
						if (originalArgItr == hyperOpArgItr->first.front()) {
							break;
						}
					}
					CallInst* callInst = callSite.back();
					callSite.pop_back();
					Instruction* clonedInst = getClonedArgument(callInst->getArgOperand(positionOfFormalArg), callSite, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
					if (isa<LoadInst>(clonedInst) && isa<AllocaInst>(clonedInst->getOperand(0))) {
						clonedInst = (AllocaInst*) clonedInst->getOperand(0);
					}
					replacementArg.push_back(clonedInst);
					callSite.push_back(callInst);
				} else if (isa<CallInst>(hyperOpArgItr->first.front())) {
					CallInst* callInst = (CallInst*) hyperOpArgItr->first.front();
					Function* calledFunction = callInst->getCalledFunction();
					callSite.push_back(callInst);
					Function* replicatedCalledFunction;
					for (map<Function*, list<CallInst*> >::iterator createdHopItr = createdHyperOpAndCallSite.begin(); createdHopItr != createdHyperOpAndCallSite.end(); createdHopItr++) {
						bool callSiteMatch = true;
						list<CallInst*> createdHopCallSite = createdHopItr->second;
						if (createdHopCallSite.size() != callSite.size()) {
							callSiteMatch = false;
						}
						if (callSiteMatch) {
							list<CallInst*>::iterator callSiteItr = callSite.begin();
							for (list<CallInst*>::iterator createHopCallSiteItr = createdHopCallSite.begin(); createHopCallSiteItr != createdHopCallSite.end() && callSiteItr != callSite.end(); createHopCallSiteItr++, callSiteItr++) {
								if (*createHopCallSiteItr != *callSiteItr) {
									callSiteMatch = false;
									break;
								}
							}
						}
						if (callSiteMatch) {
							replicatedCalledFunction = createdHopItr->first;
							break;
						}
					}

					Value* returnValue = createdHyperOpAndReturnValue[replicatedCalledFunction];
					Instruction* clonedInst;
					if (isa<Constant>(returnValue)) {
						//Immediate value
						clonedInst = new AllocaInst(returnValue->getType());
						((AllocaInst*) clonedInst)->setAlignment(4);
						//Alloc instructions need to be inserted in the entry basic block of the function because other allocs are treated as dynamic stack allocs
						clonedInst->insertBefore(replicatedCalledFunction->getEntryBlock().getFirstInsertionPt());
						StoreInst* storeInst = new StoreInst(returnValue, clonedInst);
						storeInst->setAlignment(4);
						storeInst->insertBefore(retInstMap[replicatedCalledFunction]);
					} else {
						clonedInst = getClonedArgument(returnValue, callSite, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
					}
					replacementArg.push_back(clonedInst);
					callSite.pop_back();
				} else if (hyperOpArgItr->second != GLOBAL_REFERENCE) {
					for (list<Value*>::iterator individualArgItr = hyperOpArgItr->first.begin(); individualArgItr != hyperOpArgItr->first.end(); individualArgItr++) {
						Value* argument = *individualArgItr;
						if (isa<Instruction>(argument)) {
							Function* originalFunction = createdHyperOpAndOriginalBasicBlockAndArgMap[createdFunction].first.front()->getParent();
							//Get Reaching definitions of the argument to the accumulated basic block list
							map<BasicBlock*, Instruction*> reachingDefBasicBlocks;
							if (isa<AllocaInst>(argument)) {
								//Reaching definitions are only for memory instructions
								reachingDefBasicBlocks = reachingStoreOperations((Instruction*) argument, originalFunction, accumulatedOriginalBasicBlocks);
							} else {
								reachingDefBasicBlocks.insert(make_pair(((Instruction*) argument)->getParent(), (Instruction*) argument));
							}
							//Find the HyperOp with the reaching definition
							for (map<BasicBlock*, Instruction*>::iterator reachingDefItr = reachingDefBasicBlocks.begin(); reachingDefItr != reachingDefBasicBlocks.end(); reachingDefItr++) {
								Instruction* reachingDefInstr = reachingDefItr->second;
								replacementArg.push_back(getClonedArgument(reachingDefInstr, callSite, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap));
							}
						}
					}
				}

				if (hyperOpArgItr->second != GLOBAL_REFERENCE) {
					for (list<Instruction*>::iterator clonedReachingDefItr = replacementArg.begin(); clonedReachingDefItr != replacementArg.end(); clonedReachingDefItr++) {
						Instruction* clonedReachingDefInst = *clonedReachingDefItr;
						//Add "consumedby" metadata to the function locals that need to be passed to other HyperOps
						Value * values[3];
						values[0] = funcAnnotation;
						if (replacementArgType == SCALAR) {
							values[1] = MDString::get(ctxt, SCALAR_ARGUMENT);
						} else if (replacementArgType == LOCAL_REFERENCE) {
							values[1] = MDString::get(ctxt, LOCAL_REFERENCE_ARGUMENT);
						}
						values[2] = ConstantInt::get(ctxt, APInt(32, hyperOpArgumentIndex));
						MDNode * consumedByMetadata = MDNode::get(ctxt, values);
						Instruction* metadataHost = 0;
						if (isa<AllocaInst>(clonedReachingDefInst)) {
							metadataHost = clonedReachingDefInst;
						} else if (isa<LoadInst>(clonedReachingDefInst) && isArgInList(clonedReachingDefInst->getParent()->getParent(), clonedReachingDefInst->getOperand(1))) {
							//function argument is passed on to another HyperOp, find the first load instruction from the memory location and add metadata to it
							for (Function::iterator bbItr = clonedReachingDefInst->getParent()->getParent()->begin(); bbItr != clonedReachingDefInst->getParent()->getParent()->end(); bbItr++) {
								for (BasicBlock::iterator instrItr = bbItr->begin(); instrItr != bbItr->end(); instrItr++) {
									if (isa<LoadInst>(instrItr) && ((LoadInst*) &instrItr)->getOperand(0) == clonedReachingDefInst->getOperand(0)) {
										metadataHost = instrItr;
										break;
									}
								}
								if (metadataHost != 0) {
									break;
								}
							}
						} else if (allocaInstCreatedForIntermediateValues.find(clonedReachingDefInst) != allocaInstCreatedForIntermediateValues.end()) {
							//Get the equivalent alloca inserted before
							metadataHost = allocaInstCreatedForIntermediateValues[clonedReachingDefInst];
						}
						if (metadataHost == 0) {
							//Temporary data, add an alloca and a store instruction after the argument and label the alloca instruction with metadata
							AllocaInst* ai = new AllocaInst(clonedReachingDefInst->getType());
							ai->setAlignment(4);
							//Alloc instructions need to be inserted in the entry basic block of the function because other allocs are treated as dynamic stack allocs
							ai->insertBefore(clonedReachingDefInst->getParent()->getParent()->getEntryBlock().getFirstInsertionPt());
							StoreInst* storeInst = new StoreInst(clonedReachingDefInst, ai);
							storeInst->setAlignment(4);
							storeInst->insertAfter(clonedReachingDefInst);
							allocaInstCreatedForIntermediateValues.insert(make_pair(clonedReachingDefInst, ai));
							metadataHost = ai;
						}
						MDNode* currentMetadataOfInstruction = metadataHost->getMetadata(HYPEROP_CONSUMED_BY);
						vector<Value*> newMDNodeValues;
						//Same data maybe required by multiple HyperOps
						if (currentMetadataOfInstruction != 0) {
							for (unsigned i = 0; i < currentMetadataOfInstruction->getNumOperands(); i++) {
								newMDNodeValues.push_back(currentMetadataOfInstruction->getOperand(i));
							}
						}
						newMDNodeValues.push_back(consumedByMetadata);
						ArrayRef<Value*> mdNodeArrayRef(newMDNodeValues);
						MDNode* newMDNode = MDNode::get(ctxt, mdNodeArrayRef);

						metadataHost->setMetadata(HYPEROP_CONSUMED_BY, newMDNode);
						//Parent function buffered to ensure that unnecessary control dependences need not exist
						if (find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), metadataHost->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()) {
							addedParentsToCurrentHyperOp.push_back(metadataHost->getParent()->getParent());
						}
					}
					hyperOpArgumentIndex++;
				} else {
					for (list<Value*>::iterator individualArgItr = hyperOpArgItr->first.begin(); individualArgItr != hyperOpArgItr->first.end(); individualArgItr++) {
						Value* globalArgument = *individualArgItr;
						Function* originalFunction = createdHyperOpAndOriginalBasicBlockAndArgMap[createdFunction].first.front()->getParent();
						//Get Reaching definitions of the global to the accumulated basic block list
						list<pair<Instruction*, list<CallInst*> > > reachingDefBasicBlocks = reachingStoreOperationsForGlobals(globalArgument, originalFunction->front().begin(), callSite, originalFunctionToHyperOpBBListMap, createdHyperOpAndOriginalBasicBlockAndArgMap);
						//Find the HyperOp with the reaching definition
						for (list<pair<Instruction*, list<CallInst*> > >::iterator reachingDefItr = reachingDefBasicBlocks.begin(); reachingDefItr != reachingDefBasicBlocks.end(); reachingDefItr++) {
							Instruction* reachingDefInstr = reachingDefItr->first;
							list<CallInst*> reachingDefCallSite = reachingDefItr->second;
							Instruction* clonedInstr = getClonedArgument(reachingDefInstr, reachingDefCallSite, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
							Function* sourceParentFunction = clonedInstr->getParent()->getParent();
							Instruction* retInstOfProducer = retInstMap.find(sourceParentFunction)->second;

							//No need to add unconditional jump if there is data being produced already
							if (find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), sourceParentFunction) != addedParentsToCurrentHyperOp.end()) {
								continue;
							}
							vector<Value*> metadataList;
							Instruction* metadataHost = 0;
							if (allocaInstCreatedForIntermediateValues.find(clonedInstr) != allocaInstCreatedForIntermediateValues.end()) {
								//Get the equivalent alloca inserted before
								metadataHost = allocaInstCreatedForIntermediateValues[clonedInstr];
								MDNode* predicateMetadata = metadataHost->getMetadata(HYPEROP_CONTROLLED_BY);
								if (predicateMetadata != 0) {
									for (unsigned i = 0; i < predicateMetadata->getNumOperands(); i++) {
										metadataList.push_back(predicateMetadata->getOperand(i));
									}
								}
							}

							if (metadataHost == 0) {
								AllocaInst* ai = new AllocaInst(Type::getInt1Ty(ctxt));
								ai->setAlignment(4);
								ai->insertBefore(sourceParentFunction->getEntryBlock().getFirstInsertionPt());
								StoreInst* storeInst = new StoreInst(ConstantInt::get(ctxt, APInt(1, 1)), ai);
								storeInst->setAlignment(4);
								storeInst->insertBefore(retInstOfProducer);
								allocaInstCreatedForIntermediateValues.insert(make_pair(clonedInstr, ai));
								metadataHost = ai;
							}
							//Label the instruction with predicates metadata
							Value * values[1];
							values[0] = funcAnnotation;
							MDNode* newPredicateMetadata = MDNode::get(ctxt, values);
							metadataList.push_back(newPredicateMetadata);
							ArrayRef<Value*> metadataRef(metadataList);
							MDNode * predicatesRelation = MDNode::get(ctxt, metadataRef);
							metadataHost->setMetadata(HYPEROP_CONTROLLED_BY, predicatesRelation);

							//Parent function buffered to ensure that unnecessary control dependences need not exist
							if (find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), metadataHost->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()) {
								addedParentsToCurrentHyperOp.push_back(metadataHost->getParent()->getParent());
							}
						}
					}
				}
			}

			DEBUG(dbgs() << "Dealing with conditional branches from other HyperOps\n");
//Find out if there exist any branch instructions leading to the HyperOp, add metadata to the instruction
			for (map<Instruction*, vector<unsigned> >::iterator conditionalBranchSourceItr = conditionalBranchSources.begin(); conditionalBranchSourceItr != conditionalBranchSources.end(); conditionalBranchSourceItr++) {
				Instruction* conditionalBranchInst = conditionalBranchSourceItr->first;
				vector<unsigned> branchOperands = conditionalBranchSourceItr->second;
				BasicBlock* conditionalTargetBB = ((BranchInst*) conditionalBranchInst)->getSuccessor(branchOperands[0]);
				Value* predicateOperand = conditionalBranchInst->getOperand(0);
				Instruction* clonedInstruction;
				if (isa<CallInst>(&conditionalTargetBB->front())) {
					//Current function is a call
					callSite.pop_back();
				}
				clonedInstruction = getClonedArgument(predicateOperand, callSite, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
				//No need to add unconditional jump if there is data being produced already
				if (find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), clonedInstruction->getParent()->getParent()) != addedParentsToCurrentHyperOp.end()) {
					continue;
				}

				//Branch instruction's first operand
				vector<Value*> metadataList;
				Instruction* metadataHost = 0;
				if (isa<AllocaInst>(predicateOperand) || allocaInstCreatedForIntermediateValues.find(predicateOperand) == allocaInstCreatedForIntermediateValues.end()) {
					metadataHost = (AllocaInst*) predicateOperand;
				} else {
					//Get the equivalent alloca inserted before
					metadataHost = allocaInstCreatedForIntermediateValues[predicateOperand];
					MDNode* predicateMetadata = metadataHost->getMetadata(HYPEROP_CONTROLLED_BY);
					if (predicateMetadata != 0) {
						for (unsigned i = 0; i < predicateMetadata->getNumOperands(); i++) {
							metadataList.push_back(predicateMetadata->getOperand(i));
						}
					}
				}

				//Label the instruction with predicates metadata
				Value * values[1];
				values[0] = funcAnnotation;
				MDNode* newPredicateMetadata = MDNode::get(ctxt, values);
				metadataList.push_back(newPredicateMetadata);
				ArrayRef<Value*> metadataRef(metadataList);
				MDNode * predicatesRelation = MDNode::get(ctxt, metadataRef);

				if (!isa<AllocaInst>(metadataHost)) {
					//Since the temporary does not have a memory location associated with it, add an alloca and a store instruction after the argument and label the alloca instruction with metadata
					AllocaInst* ai = new AllocaInst(predicateOperand->getType());
					ai->setAlignment(4);
					ai->insertBefore(clonedInstruction->getParent()->getParent()->getEntryBlock().getFirstInsertionPt());
					StoreInst* storeInst = new StoreInst(predicateOperand, ai);
					storeInst->setAlignment(4);
					storeInst->insertBefore(clonedInstruction);
					allocaInstCreatedForIntermediateValues[predicateOperand] = ai;
					metadataHost = ai;
				}
				metadataHost->setMetadata(HYPEROP_CONTROLLED_BY, predicatesRelation);

				for (unsigned branchOperandIndex = 0; branchOperandIndex != conditionalBranchSourceItr->second.size(); branchOperandIndex++) {
					//Update the cloned conditional branch instruction with the right target
					unsigned positionToBeUpdated = ConstantInt::get(ctxt, APInt(32, conditionalBranchSourceItr->second[branchOperandIndex]))->getZExtValue();
					Instruction* retInstOfProducer = retInstMap.find(clonedInstruction->getParent()->getParent())->second;
					clonedInstruction->setOperand(positionToBeUpdated, retInstOfProducer->getParent());
				}

				list<BasicBlock*> branchTargets;
				for (unsigned successorIndex = 0; successorIndex != ((BranchInst*) clonedInstruction)->getNumSuccessors(); successorIndex++) {
					if (find(branchTargets.begin(), branchTargets.end(), ((BranchInst*) clonedInstruction)->getSuccessor(successorIndex)) == branchTargets.end()) {
						branchTargets.push_back(((BranchInst*) clonedInstruction)->getSuccessor(successorIndex));
					}
				}

				//Branch instruction points to the same target, replace with unconditional branch
				if (branchTargets.size() == 1) {
					BranchInst* unconditionalBranch = BranchInst::Create(((BranchInst*) clonedInstruction)->getSuccessor(0));
					unconditionalBranch->insertBefore(clonedInstruction);
					clonedInstruction->eraseFromParent();
				}

				if (find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), metadataHost->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()) {
					addedParentsToCurrentHyperOp.push_back(metadataHost->getParent()->getParent());
				}
			}

			DEBUG(dbgs() << "Dealing with unconditional branches from other HyperOps\n");
			//Remove unconditional branch instruction, add the annotation to the alloca instruction of the branch
			for (list<Instruction*>::iterator unconditionalBranchSourceItr = unconditionalBranchSources.begin(); unconditionalBranchSourceItr != unconditionalBranchSources.end(); unconditionalBranchSourceItr++) {
				Instruction* unconditionalBranchInstr = *unconditionalBranchSourceItr;
				BasicBlock* targetBB = ((BranchInst*) unconditionalBranchInstr)->getSuccessor(0);
				list<pair<Function*, Instruction*> > addedJumpSources;
				Instruction* clonedInstr;
				if (isa<CallInst>(&targetBB->front())) {
					//Current function is a call
					callSite.pop_back();
				}
				clonedInstr = getClonedArgument(unconditionalBranchInstr, callSite, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);

				Function* sourceParentFunction = clonedInstr->getParent()->getParent();
				Instruction* retInstOfProducer = retInstMap.find(clonedInstr->getParent()->getParent())->second;
				//Update the branch instruction to jump to the return basic block
				clonedInstr->setOperand(0, retInstOfProducer->getParent());

				//No need to add unconditional jump if there is data being produced already
				if (find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), sourceParentFunction) != addedParentsToCurrentHyperOp.end()) {
					continue;
				}
				vector<Value*> metadataList;
				Instruction* metadataHost = 0;
				if (allocaInstCreatedForIntermediateValues.find(clonedInstr) != allocaInstCreatedForIntermediateValues.end()) {
					//Get the equivalent alloca inserted before
					metadataHost = allocaInstCreatedForIntermediateValues[clonedInstr];
					MDNode* predicateMetadata = metadataHost->getMetadata(HYPEROP_CONTROLLED_BY);
					if (predicateMetadata != 0) {
						for (unsigned i = 0; i < predicateMetadata->getNumOperands(); i++) {
							metadataList.push_back(predicateMetadata->getOperand(i));
						}
					}
				}

				if (metadataHost == 0) {
					AllocaInst* ai = new AllocaInst(Type::getInt1Ty(ctxt));
					ai->setAlignment(4);
					ai->insertBefore(retInstOfProducer->getParent()->getParent()->getEntryBlock().getFirstInsertionPt());
					StoreInst* storeInst = new StoreInst(ConstantInt::get(ctxt, APInt(1, 1)), ai);
					storeInst->setAlignment(4);
					storeInst->insertBefore(retInstOfProducer);
					allocaInstCreatedForIntermediateValues.insert(make_pair(clonedInstr, ai));
					metadataHost = ai;
					addedJumpSources.push_back(make_pair(ai->getParent()->getParent(), ai));
				}
				//Label the instruction with predicates metadata
				Value * values[1];
				values[0] = funcAnnotation;
				MDNode* newPredicateMetadata = MDNode::get(ctxt, values);
				metadataList.push_back(newPredicateMetadata);
				ArrayRef<Value*> metadataRef(metadataList);
				MDNode * predicatesRelation = MDNode::get(ctxt, metadataRef);
				metadataHost->setMetadata(HYPEROP_CONTROLLED_BY, predicatesRelation);
				addedParentsToCurrentHyperOp.push_back(sourceParentFunction);
				createdFunctionAndUnconditionalJumpSources[createdFunction] = addedJumpSources;
			}

		}
		//Compute unconditional jump chains across HyperOps and short-circuit them to jump straight to the target of the chain
		for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHyperOpItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHyperOpItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHyperOpItr++) {
			Function* createdFunction = createdHyperOpItr->first;
			if (createdFunctionAndUnconditionalJumpSources.find(createdFunction) != createdFunctionAndUnconditionalJumpSources.end()) {
				list<pair<Function*, Instruction*> > topmostParents = getTopmostParentInJumpChain(createdFunction, createdFunctionAndUnconditionalJumpSources);
				//Re-route the unconditional jumps straight from topmostParents to createdFunction
				for (list<pair<Function*, Instruction*> >::iterator parentItr = topmostParents.begin(); parentItr != topmostParents.end(); parentItr++) {
					Function* parentFunction = parentItr->first;
					Instruction* jumpInstruction = parentItr->second;
					MDNode* controlledByMDNode = jumpInstruction->getMetadata(HYPEROP_CONTROLLED_BY);
					for (unsigned i = 0; i < controlledByMDNode->getNumOperands(); i++) {
						MDNode* consumerMDNode = (MDNode*) controlledByMDNode->getOperand(i);
						MDNode* consumerFunctionMDNode = (MDNode*) consumerMDNode->getOperand(0);
						Function* consumerFunction = (Function *) consumerFunctionMDNode->getOperand(1);

						list<pair<Function*, Instruction*> > parentFunctionMap = createdFunctionAndUnconditionalJumpSources[createdFunction];
						for (list<pair<Function*, Instruction*> >::iterator parentItr = parentFunctionMap.begin(); parentItr != parentFunctionMap.end(); parentItr++) {
							if (parentItr->first == consumerFunction) {
								//Replace the operand with the current function's MDNode
								consumerFunctionMDNode->replaceOperandWith(1, hyperOpAndAnnotationMap[createdFunction]);
								break;
							}
						}
					}
				}
			}

			//Short-circuit unconditional jump chains within a function
			for (Function::iterator bbItr = createdFunction->begin(); bbItr != createdFunction->end(); bbItr++) {
				BasicBlock* originOfJumpChain = bbItr;
				while (true) {
					if (isa<BranchInst>(originOfJumpChain->getTerminator()) && ((BranchInst*) originOfJumpChain->getTerminator())->isUnconditional()) {
						//Unconditional jump would only have one successor
						originOfJumpChain = originOfJumpChain->getTerminator()->getSuccessor(0);
					} else {
						break;
					}
				}

				if (originOfJumpChain != bbItr) {
					bbItr->getTerminator()->setSuccessor(0, originOfJumpChain);
				}
			}

		}

		//Remove unreachable basic blocks from created functions
		for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHyperOpItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHyperOpItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHyperOpItr++) {
			Function* newFunction = createdHyperOpItr->first;
			list<BasicBlock*> bbForDelete;
			for (Function::iterator bbItr = newFunction->begin(); bbItr != newFunction->end(); bbItr++) {
				//If bbItr is not the entry block and has no predecessors or multiple predecessors (this check is in place to avoid considering basic blocks which have a single predecessor, an optimization)
				if (bbItr->getName().compare(newFunction->getEntryBlock().getName()) && bbItr->getSinglePredecessor() == 0) {
					BasicBlock* bb = &*bbItr;
					bool hasPredecessor = false;
					for (unsigned i = 0; i < bb->getTerminator()->getNumSuccessors(); i++) {
						BasicBlock* successor = bb->getTerminator()->getSuccessor(i);
						if (successor == bb) {
							hasPredecessor = true;
							break;
						}
					}
					if (!hasPredecessor) {
						bbForDelete.push_back(bb);
					}
				}
			}

			for (list<BasicBlock*>::iterator deleteItr = bbForDelete.begin(); deleteItr != bbForDelete.end(); deleteItr++) {
				(*deleteItr)->eraseFromParent();
			}
		}
		DEBUG(dbgs() << "\n---------------Deleting unused functions---------------\n");
		errs() << "what's in the module?";
		M.dump();

		//Workaround for deleting unused functions, deletion doesn't work unless in topological order but what about recursion?
		list<Function*> functionsForDeletion;
		for (Module::iterator functionItr = M.begin(); functionItr != M.end(); functionItr++) {
			//Remove old functions from module
			if (createdHyperOpAndOriginalBasicBlockAndArgMap.find(functionItr) == createdHyperOpAndOriginalBasicBlockAndArgMap.end()) {
				errs() << "deleting function:" << functionItr->getName() << "\n";
				functionItr->deleteBody();
				functionsForDeletion.push_back(functionItr);
			}
		}

		while (!functionsForDeletion.empty()) {
			Function* function = functionsForDeletion.front();
			functionsForDeletion.pop_front();
			function->eraseFromParent();
		}
		DEBUG(dbgs() << "Final module contents:");
		M.dump();
		return true;
	}

private:
	const char* REDEFINE_START_FUNCTION = "redefine_start";
	const char* HYPEROP_ENTRY = "Entry";
	const char* HYPEROP_EXIT = "Exit";
	const char* HYPEROP_INTERMEDIATE = "Intermediate";
	const char* REDEFINE_INPUT_PREFIX = "redefine_in";
	const char* REDEFINE_OUTPUT_PREFIX = "redefine_out";
	const char* REDEFINE_INOUT_PREFIX = "redefine_inout";
}
;
char HyperOpCreationPass::ID = 2;
char* HyperOpCreationPass::NEW_NAME = "newName";
static RegisterPass<HyperOpCreationPass> X("HyperOpCreationPass", "Pass to create HyperOps");
