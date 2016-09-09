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
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Transforms/IPO/InlinerPass.h"
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
	const string HYPEROP_SYNC = "Sync";
	const string HYPEROP_CONTROLS = "Controls";
	const string HYPEROP_AFFINITY = "Affinity";
	const string SCALAR_ARGUMENT = "Scalar";
	const string LOCAL_REFERENCE_ARGUMENT = "LocalReference";

	const unsigned int FRAME_SIZE = 16;

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
					if (isa < StoreInst > (instr) &&((StoreInst*) instr)->getOperand(0) == globalVariable && reachingDefinitions.find(originalBB) == reachingDefinitions.end()) {
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
				if (isa < StoreInst > (instr) &&((StoreInst*) instr)->getOperand(1) == useInstr && pathExistsInCFG(originalBB, useInstr->getParent(), visitedBasicBlocks)) {
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
		if (argument->getType()->isAggregateType() || argument->getType()->getTypeID() == Type::FloatTyID || argument->getType()->getTypeID() == Type::PointerTyID) {
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
				if (((GetElementPtrInst*) argument)->getPointerOperand() == globalVarItr) {
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

	list<Function*> getFunctionAtCallSite(list<CallInst*> callSite, map<Function*, list<CallInst*> > createdHyperOpAndCallSite) {
		list<Function*> functionList;
		for (map<Function*, list<CallInst*> >::iterator createdHopItr = createdHyperOpAndCallSite.begin(); createdHopItr != createdHyperOpAndCallSite.end(); createdHopItr++) {
			list<CallInst*> createdHopCallSite = createdHopItr->second;
			bool callSiteMatch = true;
			if (createdHopCallSite.size() != callSite.size()) {
				continue;
			}
			list<CallInst*>::iterator callSiteItr = callSite.begin();
			for (list<CallInst*>::iterator createHopCallSiteItr = createdHopCallSite.begin(); createHopCallSiteItr != createdHopCallSite.end() && callSiteItr != callSite.end(); createHopCallSiteItr++, callSiteItr++) {
				if (*createHopCallSiteItr != *callSiteItr) {
					callSiteMatch = false;
					break;
				}
			}

			if (callSiteMatch) {
				functionList.push_back(createdHopItr->first);
			}
		}
		return functionList;
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
			return getClonedArgument(argument, callSite, createdHyperOpAndCallSite, originalToClonedInstructionMap);
		}
		return 0;
	}

	void addInitializationInstructions(GlobalVariable* global, Constant* initializer, vector<Value*> idList, Instruction* insertBefore, Type* type) {
		//TODO Constant cannot be vector or blockaddress
		LLVMContext & ctx = insertBefore->getParent()->getContext();
		if (!type->isAggregateType()) {
			if (isa<ConstantInt>(initializer) || isa<ConstantFP>(initializer) || isa<ConstantExpr>(initializer) || initializer->isZeroValue()) {
				GetElementPtrInst* typeAddrInst = GetElementPtrInst::CreateInBounds(global, idList, "", insertBefore);
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

	//Jump chain is the chain of instructions leading to the target function, with the instruction in front of the list chaining to the next one and so on till the last function is reached
	list<pair<Function*, list<Instruction*> > > getTopmostParentAndJumpChain(Function* function, map<Function*, list<pair<Function*, Instruction*> > > functionAndJumpSources) {
		list<pair<Function*, list<Instruction*> > > topmostParents;
		list<pair<Function*, Instruction*> > parents = functionAndJumpSources[function];
		for (list<pair<Function*, Instruction*> >::iterator parentItr = parents.begin(); parentItr != parents.end(); parentItr++) {
			if (functionAndJumpSources.find(parentItr->first) != functionAndJumpSources.end()) {
				list<pair<Function*, list<Instruction*> > > tempParents = getTopmostParentAndJumpChain(parentItr->first, functionAndJumpSources);
				for (list<pair<Function*, list<Instruction*> > >::iterator tempParentItr = tempParents.begin(); tempParentItr != tempParents.end(); tempParentItr++) {
					tempParentItr->second.push_back(parentItr->second);
				}
				std::copy(tempParents.begin(), tempParents.end(), std::back_inserter(topmostParents));
			} else {
				list<Instruction*> tempList;
				tempList.push_back(parentItr->second);
				topmostParents.push_back(make_pair(parentItr->first, tempList));
			}
		}
		return topmostParents;
	}

	list<list<pair<Function*, CallInst*> > > getCyclesInCallGraph(list<pair<Function*, CallInst*> > functionTraversalList, map<Function*, list<pair<Function*, CallInst*> > > calledFunctionMap) {
		list<list<pair<Function*, CallInst*> > > cyclesInCallGraph;
		Function* function = functionTraversalList.front().first;
		list<pair<Function*, CallInst*> > calledFunctions = calledFunctionMap[function];
		for (list<pair<Function*, CallInst*> >::iterator calledFuncItr = calledFunctions.begin(); calledFuncItr != calledFunctions.end(); calledFuncItr++) {
			bool cycleExit = false;
			for (list<pair<Function*, CallInst*> >::iterator traversedItr = functionTraversalList.begin(); traversedItr != functionTraversalList.end(); traversedItr++) {
				if (traversedItr->first == calledFuncItr->first) {
					list<pair<Function*, CallInst*> > newCycle;
					std::copy(functionTraversalList.begin(), traversedItr, std::front_inserter(newCycle));
					newCycle.push_front(*calledFuncItr);
					cyclesInCallGraph.push_back(newCycle);
					cycleExit = true;
					break;
				}
			}

			if (!cycleExit) {
				list<pair<Function*, CallInst*> > tempTraversalList;
				std::copy(functionTraversalList.begin(), functionTraversalList.end(), std::back_inserter(tempTraversalList));
				tempTraversalList.push_front(*calledFuncItr);
				list<list<pair<Function*, CallInst*> > > tempCyclesInGraph = getCyclesInCallGraph(tempTraversalList, calledFunctionMap);
				if (!tempCyclesInGraph.empty()) {
					std::copy(tempCyclesInGraph.begin(), tempCyclesInGraph.end(), back_inserter(cyclesInCallGraph));
				}
			}
		}
		return cyclesInCallGraph;
	}

	list<list<pair<Function*, CallInst*> > > getCyclesContainingHyperOpInstance(CallInst * callInst, list<list<pair<Function*, CallInst*> > > cyclesInCallGraph) {
		list<list<pair<Function*, CallInst*> > > cyclesContainingCall;
		for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
			list<pair<Function*, CallInst*> > cycle = *cycleItr;
			bool callInCycle = false;
			for (list<pair<Function*, CallInst*> >::iterator funcCallItr = cycle.begin(); funcCallItr != cycle.end(); funcCallItr++) {
				if (funcCallItr->second == callInst) {
					callInCycle = true;
					break;
				}
			}

			if (callInCycle) {
				cyclesContainingCall.push_back(cycle);
			}
		}
		return cyclesContainingCall;
	}

	bool inline isHyperOpInstanceInCycle(CallInst* callInst, list<list<pair<Function*, CallInst*> > > cyclesInCallGraph) {
		map<CallInst*, Function*> functionInstancesToBeCreated;
		for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
			list<pair<Function*, CallInst*> > cycle = *cycleItr;
			for (list<pair<Function*, CallInst*> >::iterator funcCallItr = cycle.begin(); funcCallItr != cycle.end(); funcCallItr++) {
				if (funcCallItr->second == callInst) {
					return true;
				}
			}
		}
		return false;
	}

	bool inline isHyperOpInstanceLastInCycle(CallInst* callInst, list<list<pair<Function*, CallInst*> > > cyclesInCallGraph) {
		map<CallInst*, Function*> functionInstancesToBeCreated;
		for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
			list<pair<Function*, CallInst*> > cycle = *cycleItr;
			if (cycle.front().second == callInst) {
				return true;
			}
		}
		return false;
	}

	enum HYPEROP_TYPE {
		DYNAMIC, STATIC
	};

	virtual bool runOnModule(Module &M) {
		LLVMContext & ctxt = M.getContext();
		//Top level annotation corresponding to all annotations REDEFINE
		NamedMDNode * redefineAnnotationsNode = M.getOrInsertNamedMetadata(REDEFINE_ANNOTATIONS);

		map<Function*, list<pair<list<BasicBlock*>, HyperOpArgumentList> > > originalFunctionToHyperOpBBListMap;

		//Original function may translate to multiple entry functions in case the number of inputs to a function is larger than context frame size
		map<Function*, list<Function*> > originalFunctionAndEntryFunctionsMap;
		map<Function*, Function*> originalFunctionAndExitFunctionMap;

		//Contains all created HyperOp functions and the basic blocks they contain in the original module and HyperOp arguments
		map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> > createdHyperOpAndOriginalBasicBlockAndArgMap;
		map<Function*, list<CallInst*> > createdHyperOpAndCallSite;
		map<Function*, Value*> createdHyperOpAndReturnValue;
		map<Function*, map<Instruction*, vector<unsigned> > > createdHyperOpAndConditionalBranchSources;
		map<Function*, list<Instruction*> > createdHyperOpAndUnconditionalBranchSources;
		map<Function*, list<Instruction*> > createdHyperOpAndReachingDefSources;
		map<Function*, HYPEROP_TYPE> createdHyperOpAndType;

		//Cloned instructions mapped to their original instruction for each created function
		map<Function*, map<Instruction*, Instruction*> > functionOriginalToClonedInstructionMap;

		//Map of the "return void" instruction in each HyperOp function
		map<Function*, Instruction*> retInstMap;

		//Map of created HyperOps and their annotations
		map<Function*, MDNode*> hyperOpAndAnnotationMap;
		map<MDNode*, Function*> annotationAndHyperOpMap;

		map<Value*, Instruction*> allocaInstCreatedForIntermediateValues;

		Function* startHyperOp = 0;
		Function* endHyperOp = 0;

		//Add all the functions to be traversed in a list
		list<Function*> functionList;
		Function* mainFunction = 0;
		map<Function*, list<pair<Function*, CallInst*> > > calledFunctionMap;
		for (Module::iterator funcItr = M.begin(); funcItr != M.end(); funcItr++) {
			Function* function = funcItr;
			if (!function->isIntrinsic()) {
				functionList.push_back(function);
			}
			if (function->getName().compare(REDEFINE_START_FUNCTION) == 0) {
				mainFunction = function;
			}

			list<pair<Function*, CallInst*> > calledFunctions;
			for (Function::iterator bbItr = function->begin(); bbItr != function->end(); bbItr++) {
				for (BasicBlock::iterator instItr = bbItr->begin(); instItr != bbItr->end(); instItr++) {
					Instruction* inst = &*instItr;
					if (isa<CallInst>(inst)) {
						CallInst* callInst = (CallInst*) inst;
						calledFunctions.push_back(make_pair(callInst->getCalledFunction(), callInst));
						if (!callInst->getCalledFunction()->isIntrinsic()) {
							//Replace immediate operands to a call with a memory location
							for (unsigned i = 0; i < callInst->getNumArgOperands(); i++) {
								//TODO what about globals?
								if (isa<Constant>(callInst->getArgOperand(i))) {
//										&&!isa<GlobalVariable>(callInst->getArgOperand(i))) {
									//Place alloc, store and load instructions before call
									AllocaInst* ai = new AllocaInst(callInst->getArgOperand(i)->getType());
									ai->setAlignment(4);
									ai->insertBefore(callInst->getParent()->getParent()->getEntryBlock().getFirstInsertionPt());
									StoreInst* storeInst = new StoreInst(callInst->getArgOperand(i), ai);
									storeInst->insertBefore(callInst);
									LoadInst* loadInst = new LoadInst(ai);
									loadInst->insertBefore(callInst);
									callInst->setArgOperand(i, loadInst);
								}
							}
						}
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

		list<pair<Function*, CallInst*> > traversedFunctions;
		CallInst* callInstToMain = 0;
		traversedFunctions.push_back(make_pair(mainFunction, callInstToMain));
		list<list<pair<Function*, CallInst*> > > cyclesInCallGraph = getCyclesInCallGraph(traversedFunctions, calledFunctionMap);
//		errs() << "found cycles?" << cyclesInCallGraph.size() << "\n";
//		for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
//			errs() << "cycle:";
//			for (list<pair<Function*, CallInst*> >::iterator funcItr = cycleItr->begin(); funcItr != cycleItr->end(); funcItr++) {
//				errs() << funcItr->first->getName() << "(";
//				funcItr->second->dump();
//				errs() << ")\n";
//			}
//			errs() << "\n";
//		}

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
				//If basic block is not the entry block
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
									//call is not the first instruction in the basic block
									if (&*instItr != &bbItr->front()) {
										//Call is the not only instruction here, a separate HyperOp must be created for the call statement
										string firstBBName(NEW_NAME);
										firstBBName.append(itostr(bbIndex));
										bbIndex++;
										bbItr->splitBasicBlock(instItr, firstBBName);
									}
									//If call is not the last instruction
									if (&*instItr == &bbItr->front() && (&*instItr != (Instruction*) bbItr->getTerminator()) && instItr->getNextNode() != (Instruction*) bbItr->getTerminator()) {
										string secondBBName(NEW_NAME);
										secondBBName.append(itostr(bbIndex));
										bbIndex++;
										bbItr->splitBasicBlock(instItr->getNextNode(), secondBBName);
									}
									if (&*instItr == &bbItr->front()) {
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
								if (!isa < Constant > (argument) &&!argument->getType()->isLabelTy()) {
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
										//local references needn't be accounted for when counting context frame args since they are passed through memory directly and not through context frame
										if (type != GLOBAL_REFERENCE && type != LOCAL_REFERENCE) {
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

				//Last basic block
				if (!endOfHyperOp && bbTraverser.empty()) {
					list<BasicBlock*> coveredBBList;
					for (list<pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator funcForCreationItr = hyperOpBBAndArgs.begin(); funcForCreationItr != hyperOpBBAndArgs.end(); funcForCreationItr++) {
						std::copy(funcForCreationItr->first.begin(), funcForCreationItr->first.end(), back_inserter(coveredBBList));
					}
					std::copy(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), back_inserter(coveredBBList));
					bool allCovered = true;
					for (Function::iterator bbItr = function->begin(); bbItr != function->end(); bbItr++) {
						if (find(coveredBBList.begin(), coveredBBList.end(), bbItr) == coveredBBList.end()) {
							allCovered = false;
							break;
						}
					}
					if (allCovered) {
						endOfHyperOp = true;
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

				DEBUG(dbgs() << "Adding basic blocks for traversal in a depth biased order for function " << function->getName() << "\n");
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
		DEBUG(dbgs() << "-----------Creating HyperOps from partitioned functions-----------\n");
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
			bool isStaticHyperOp = true;
			//Create a function using the accumulated basic blocks
			if (isa<CallInst>(accumulatedBasicBlocks.front()->front()) && !((CallInst*) &accumulatedBasicBlocks.front()->front())->getCalledFunction()->isIntrinsic()) {
				//Check if the hyperop instance being created is not in the call cycle
				if (isHyperOpInstanceInCycle((CallInst*) &accumulatedBasicBlocks.front()->front(), cyclesInCallGraph)) {
					CallInst* callInstructionInvokingTheFunction = callSite.back();
					if (isHyperOpInstanceLastInCycle(callInstructionInvokingTheFunction, cyclesInCallGraph)) {
						continue;
					}
				}
				//Create a copy of the called function as a new HyperOp
				list<CallInst*> newCallSite;
				std::copy(callSite.begin(), callSite.end(), std::back_inserter(newCallSite));
				CallInst* callInst = (CallInst*) &accumulatedBasicBlocks.front()->front();
				newCallSite.push_back(callInst);
				Function* calledOriginalFunction = callInst->getCalledFunction();
				//Functions to be created to substitute the called original function need to be pushed to traversal list
				list<pair<list<BasicBlock*>, HyperOpArgumentList> > calledFunctionBBList = originalFunctionToHyperOpBBListMap[calledOriginalFunction];
				//Update the arguments to the HyperOp to be created in place of the callsite
				for (list<pair<list<BasicBlock*>, HyperOpArgumentList> >::reverse_iterator replacementFuncItr = calledFunctionBBList.rbegin(); replacementFuncItr != calledFunctionBBList.rend(); replacementFuncItr++) {
//					traversalList.push_back(make_pair(make_pair(replacementFuncItr->first, replacementFuncItr->second), newCallSite));
					traversalList.push_front(make_pair(make_pair(replacementFuncItr->first, replacementFuncItr->second), newCallSite));
				}
				continue;
			}

			//Check if the hyperop instance being created is not in the call cycle
			/**
			 * (╯°□°)╯︵ ┻━┻
			 */
			CallInst* instanceCallSite = callSite.back();
			if (!callSite.empty() && isa < CallInst > (instanceCallSite) &&isHyperOpInstanceInCycle(instanceCallSite, cyclesInCallGraph)) {
				isStaticHyperOp = false;
			}

			DEBUG(dbgs() << "\n-----------Creating a new HyperOp for function:" << function->getName() << "-----------\n");
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
			list<Function*> instancesForCreation;

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
					if (isa<ReturnInst>(instItr)) {
						clonedInst = BranchInst::Create(retBB, newBB);
						originalToClonedInstMap.insert(std::make_pair(instItr, clonedInst));
						if (((ReturnInst*) &*instItr)->getReturnValue() != 0 && ((ReturnInst*) &*instItr)->getReturnValue()->getType()->getTypeID() != Type::VoidTyID) {
							createdHyperOpAndReturnValue.insert(make_pair(newFunction, instItr->getOperand(0)));
						} else {
							createdHyperOpAndReturnValue.insert(make_pair(newFunction, instItr));
						}
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
			}

			unsigned numCallInstrAdded = 0;
			list<CallInst*> callSiteCopy;
			std::copy(callSite.begin(), callSite.end(), std::back_inserter(callSiteCopy));
			//Add all the call instructions that lead to the current function
			BasicBlock* entryBlock = &function->getEntryBlock();
			list<BasicBlock*> newlyAcquiredBBList;
			newlyAcquiredBBList.push_front(accumulatedBasicBlocks.front());
			while (!callSiteCopy.empty() && entryBlock == newlyAcquiredBBList.front()) {
				CallInst* callInst = callSiteCopy.back();
				callSiteCopy.pop_back();
				newlyAcquiredBBList.push_front(callInst->getParent());
				entryBlock = &callInst->getParent()->getParent()->getEntryBlock();
				numCallInstrAdded++;
			}
			newlyAcquiredBBList.pop_back();
			if (numCallInstrAdded) {
				for(list<BasicBlock*>::iterator acquiredBBItr = newlyAcquiredBBList.begin();acquiredBBItr!=newlyAcquiredBBList.end();acquiredBBItr++){
					accumulatedBasicBlocks.push_back(*acquiredBBItr);
				}
			}

			for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
				//Find out if any basic block is predicated
				BasicBlock* originalBB = *accumulatedBBItr;
				errs()<<"original bb :"<<originalBB->getName()<<"\n";
				//TODO: Predicates are for call sites also and hence, they are not limited to non-entry basic blocks of the original function
				for (pred_iterator predecessorItr = pred_begin(originalBB); predecessorItr != pred_end(originalBB); predecessorItr++) {
					BasicBlock* predecessor = *predecessorItr;
					errs()<<"Pred:"<<predecessor->getName()<<"\n";
					//Control flow edge originates from a predecessor basic block that does not belong to the same HyperOp
					//TODO is the first half of the conditional sufficient?
					if ((find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), predecessor) == accumulatedBasicBlocks.end()||find(newlyAcquiredBBList.begin(), newlyAcquiredBBList.end(), predecessor) == newlyAcquiredBBList.end()) && originalFunctionToHyperOpBBListMap.find(predecessor->getParent()) != originalFunctionToHyperOpBBListMap.end()) {
						TerminatorInst* terminator = predecessor->getTerminator();
						errs() << "which term again?";
						terminator->dump();
						errs() << " parent:" << terminator->getParent()->getName() << "\n";
						//If the terminator instruction has only one successor, its an unconditional jump
						if (terminator->getNumSuccessors() == 1 && terminator->getSuccessor(0) == originalBB) {
							unconditionalBranchSources.push_back(terminator);
							errs() << "Added as unconditional branch source\n";
						} else {
							//Get the first operand of terminator instruction corresponding to a branch
							vector<unsigned> successorBBList;
							for (int i = 0; i < terminator->getNumSuccessors(); i++) {
								if (terminator->getSuccessor(i) == originalBB) {
									//Add only those successors that correspond to a basic block in the current HyperOp
									successorBBList.push_back(i);
								}
							}
							if (successorBBList.size() == terminator->getNumSuccessors()) {
								//All successors of the terminator instruction are in the same target HyperOp, mark the jump as unconditional
								unconditionalBranchSources.push_back(terminator);
							} else {
								if (conditionalBranchSources.find(terminator) != conditionalBranchSources.end()) {
									vector<unsigned> previousSuccessorIndexList = conditionalBranchSources[terminator];
									conditionalBranchSources.erase(terminator);
									std::copy(previousSuccessorIndexList.begin(), previousSuccessorIndexList.end(), back_inserter(successorBBList));
								}
								conditionalBranchSources[terminator] = successorBBList;
								errs() << "conditional branch added:";
								terminator->dump();
								for (unsigned i = 0; i < successorBBList.size(); i++) {
									errs() << "succ:" << successorBBList[i] << ";";
								}
							}
						}
					}
				}
			}

			while (numCallInstrAdded) {
				accumulatedBasicBlocks.pop_back();
				numCallInstrAdded--;
			}

			MDNode *funcAnnotation;
			//Keep the following branch structure
			if (!isStaticHyperOp) {
				Value * values[5];
				values[0] = MDString::get(ctxt, HYPEROP);
				values[1] = newFunction;
				values[2] = MDString::get(ctxt, DYNAMIC_HYPEROP);
				list<CallInst*> parentCallSite;
				std::copy(callSite.begin(), callSite.end(), back_inserter(parentCallSite));
				//pop from call site till you end up with a function thats marked static
				while (!parentCallSite.empty()) {
					//This is a list because the parent function maybe broken down to multiple functions
					list<Function*> parentFunctionList = getFunctionAtCallSite(parentCallSite, createdHyperOpAndCallSite);
					if (createdHyperOpAndType[parentFunctionList.front()] == STATIC) {
						break;
					}
					parentCallSite.pop_back();
				}

				list<Function*> parentFunctionList = getFunctionAtCallSite(parentCallSite, createdHyperOpAndCallSite);
				//The called function doesn't match the callsite of the current hyperop which means that the current HyperOp is not the first in the recursion cycle
				//todo uncomment
//				if (parentCallSite.back()->getCalledFunction() != callSite.back()->getCalledFunction()) {
				values[3] = newFunction;
//				} else {
//					//Find the original function from which the parent function was created
//					for (list<Function*>::iterator parentFuncItr = parentFunctionList.begin(); parentFuncItr != parentFunctionList.end(); parentFuncItr++) {
//						bool matchFound = true;
//						list<BasicBlock*> parentFuncBBList = createdHyperOpAndOriginalBasicBlockAndArgMap[*parentFuncItr].first;
//						for (list<BasicBlock*>::iterator bbItr = parentFuncBBList.begin(); bbItr != parentFuncBBList.end(); bbItr++) {
//							if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), *bbItr) == accumulatedBasicBlocks.end()) {
//								matchFound = false;
//								break;
//							}
//						}
//						if (matchFound) {
//							values[3] = *parentFuncItr;
//							break;
//						}
//					}
//				}
				values[4] = MDString::get(ctxt, "<0>");
				funcAnnotation = MDNode::get(ctxt, values);
			} else {
				Value * values[3];
				values[0] = MDString::get(ctxt, HYPEROP);
				values[1] = newFunction;
				values[2] = MDString::get(ctxt, STATIC_HYPEROP);
				funcAnnotation = MDNode::get(ctxt, values);
			}

			hyperOpAndAnnotationMap.insert(make_pair(newFunction, funcAnnotation));
			annotationAndHyperOpMap.insert(make_pair(funcAnnotation, newFunction));
			redefineAnnotationsNode->addOperand(funcAnnotation);
			bool isKernelEntry = false;
			bool isKernelExit = false;

			if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), &mainFunction->getEntryBlock()) != accumulatedBasicBlocks.end()) {
				isKernelEntry = true;
			}
			//First instruction in redefine_start is a call
			else if (!callSite.empty()) {
				list<CallInst*> callSiteCopy;
				std::copy(callSite.begin(), callSite.end(), std::back_inserter(callSiteCopy));
				//Recursively find out if the call instruction is the first in the whole call chain
				while (!callSiteCopy.empty()) {
					Function* callerFunction = callSiteCopy.front()->getParent()->getParent();
					if (&callerFunction->getEntryBlock().front() == (Instruction*) callSiteCopy.front() && find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), &function->getEntryBlock()) != accumulatedBasicBlocks.end()) {
						isKernelEntry = true;
						break;
					}
					callSiteCopy.pop_front();
				}
			}
			if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), &mainFunction->back()) != accumulatedBasicBlocks.end()) {
				isKernelExit = true;
			}
			//last instruction in redefine_start is a call
			else if (!callSite.empty()) {
				list<CallInst*> callSiteCopy;
				std::copy(callSite.begin(), callSite.end(), std::back_inserter(callSiteCopy));
				//Recursively find out if the call instruction is the first in the whole call chain
				while (!callSiteCopy.empty()) {
					Function* callerFunction = callSiteCopy.front()->getParent()->getParent();
					if (callerFunction->back().getTerminator()->getPrevNode() == callSiteCopy.front() && find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), &function->back()) != accumulatedBasicBlocks.end()) {
						isKernelExit = true;
						break;
					}
					callSiteCopy.pop_front();
				}
			}

			//Is the function an entry node/exit node/intermediate ?
			if (isKernelEntry) {
				Value* hyperOpDescrMDValues[2];
				hyperOpDescrMDValues[0] = MDString::get(ctxt, HYPEROP_ENTRY);
				hyperOpDescrMDValues[1] = funcAnnotation;
				MDNode* hyperOpDescMDNode = MDNode::get(ctxt, hyperOpDescrMDValues);
				redefineAnnotationsNode->addOperand(hyperOpDescMDNode);
				startHyperOp = newFunction;
			}
			if (isKernelExit) {
				Value* hyperOpDescrMDValues[2];
				hyperOpDescrMDValues[0] = MDString::get(ctxt, HYPEROP_EXIT);
				hyperOpDescrMDValues[1] = funcAnnotation;
				MDNode* hyperOpDescMDNode = MDNode::get(ctxt, hyperOpDescrMDValues);
				redefineAnnotationsNode->addOperand(hyperOpDescMDNode);
				endHyperOp = newFunction;
			}
			if (!isKernelEntry && !isKernelExit) {
				Value* hyperOpDescrMDValues[2];
				hyperOpDescrMDValues[0] = MDString::get(ctxt, HYPEROP_INTERMEDIATE);
				hyperOpDescrMDValues[1] = funcAnnotation;
				MDNode* hyperOpDescMDNode = MDNode::get(ctxt, hyperOpDescrMDValues);
				redefineAnnotationsNode->addOperand(hyperOpDescMDNode);
			}

			errs() << "created func:" << newFunction->getName() << "\n----\n";
			createdHyperOpAndOriginalBasicBlockAndArgMap[newFunction] = make_pair(accumulatedBasicBlocks, hyperOpArguments);
			createdHyperOpAndCallSite[newFunction] = callSite;
			createdHyperOpAndConditionalBranchSources[newFunction] = conditionalBranchSources;
			createdHyperOpAndUnconditionalBranchSources[newFunction] = unconditionalBranchSources;
			createdHyperOpAndReachingDefSources[newFunction] = reachingGlobalDefinitionSources;
			functionOriginalToClonedInstructionMap[newFunction] = originalToClonedInstMap;
			createdHyperOpAndType[newFunction] = isStaticHyperOp ? STATIC : DYNAMIC;
		}

		//if there is no end HyperOp because the last instruction is a recursion chain
		if (endHyperOp == 0) {
			//Add a new dummy HyperOp
			FunctionType *FT = FunctionType::get(Type::getVoidTy(getGlobalContext()), false);
			Function *newFunction = Function::Create(FT, Function::ExternalLinkage, "redefine_end", &M);
			//Add a dummy return block
			BasicBlock* retBB = BasicBlock::Create(ctxt, newFunction->getName().str().append(".return"), newFunction);
			Instruction* retInst = ReturnInst::Create(ctxt);
			retBB->getInstList().insert(retBB->getFirstInsertionPt(), retInst);
			Value * values[3];
			values[0] = MDString::get(ctxt, HYPEROP);
			values[1] = newFunction;
			values[2] = MDString::get(ctxt, STATIC_HYPEROP);
			MDNode *funcAnnotation = MDNode::get(ctxt, values);
			redefineAnnotationsNode->addOperand(funcAnnotation);
			hyperOpAndAnnotationMap[newFunction] = funcAnnotation;

			Value* hyperOpDescrMDValues[2];
			hyperOpDescrMDValues[0] = MDString::get(ctxt, HYPEROP_EXIT);
			hyperOpDescrMDValues[1] = funcAnnotation;
			MDNode* hyperOpDescMDNode = MDNode::get(ctxt, hyperOpDescrMDValues);
			redefineAnnotationsNode->addOperand(hyperOpDescMDNode);

			endHyperOp = newFunction;
			list<BasicBlock*> accumulatedBasicBlocks;
			HyperOpArgumentList hyperOpArguments;
			createdHyperOpAndOriginalBasicBlockAndArgMap[newFunction] = make_pair(accumulatedBasicBlocks, hyperOpArguments);
		}
		//End of creation of hyperops

		list<MDNode*> syncMDNodeList;
		DEBUG(dbgs() << "\n----------Adding dependences across created HyperOps----------\n");
		//Add metadata: This code is moved here to ensure that all the functions (corresponding to HyperOps) that need to be created have already been created
		for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHyperOpItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHyperOpItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHyperOpItr++) {
			Function* createdFunction = createdHyperOpItr->first;
			DEBUG(dbgs() << "\n-----------Patching created function " << createdFunction->getName() << "--------------\n");
			list<BasicBlock*> accumulatedOriginalBasicBlocks = createdHyperOpItr->second.first;
			HyperOpArgumentList hyperOpArguments = createdHyperOpItr->second.second;
			map<Instruction*, vector<unsigned> > conditionalBranchSources = createdHyperOpAndConditionalBranchSources[createdFunction];
			list<Instruction*> unconditionalBranchSources = createdHyperOpAndUnconditionalBranchSources[createdFunction];
			list<Instruction*> reachingDefSources = createdHyperOpAndReachingDefSources[createdFunction];
			bool isStaticHyperOp = createdHyperOpAndType[createdFunction];
			list<Function*> addedParentsToCurrentHyperOp;
			MDNode* funcAnnotation = hyperOpAndAnnotationMap.find(createdFunction)->second;
			list<CallInst*> callSite = createdHyperOpAndCallSite[createdFunction];
			list<Function*> localReferenceArgProducers;
			list<Function*> predicateProducers;

			DEBUG(dbgs() << "\n----------Adding consumed by metadata----------\n");
			unsigned hyperOpArgumentIndex = 0;
			//Replace arguments of called functions with the right call arguments or return values
			for (HyperOpArgumentList::iterator hyperOpArgItr = hyperOpArguments.begin(); hyperOpArgItr != hyperOpArguments.end(); hyperOpArgItr++) {
				errs() << "argument being fixed:";
				hyperOpArgItr->first.front()->dump();
				map<Instruction*, Value*> replacementArg;
				HyperOpArgumentType replacementArgType = hyperOpArgItr->second;
				//the argument is a function argument
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
					Value* argOperand = callInst->getArgOperand(positionOfFormalArg);
					Instruction* clonedInst;
					if (isa<Constant>(argOperand)) {
						clonedInst = new AllocaInst(argOperand->getType());
						((AllocaInst*) clonedInst)->setAlignment(4);
						//Alloc instructions need to be inserted in the entry basic block of the function because other allocs are treated as dynamic stack allocs
						clonedInst->insertBefore(callInst);
						StoreInst* storeInst = new StoreInst(argOperand, clonedInst);
						storeInst->setAlignment(4);
						storeInst->insertBefore(callInst);
					} else {
						clonedInst = getClonedArgument(argOperand, callSite, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
					}
					if (isa < LoadInst > (clonedInst) &&isa<AllocaInst>(clonedInst->getOperand(0))) {
						clonedInst = (AllocaInst*) clonedInst->getOperand(0);
						//TODO Is this casting correct?
						replacementArg.insert(make_pair(clonedInst, ((Instruction*) argOperand)->getOperand(0)));
					} else {
						replacementArg.insert(make_pair(clonedInst, argOperand));
					}
					callSite.push_back(callInst);
				} else if (isa<CallInst>(hyperOpArgItr->first.front())) {
					CallInst* callInst = (CallInst*) hyperOpArgItr->first.front();
					Function* calledFunction = callInst->getCalledFunction();
					list<CallInst*> callSiteCopy;

					std::copy(callSite.begin(), callSite.end(), back_inserter(callSiteCopy));
					callSiteCopy.push_back(callInst);
					Function* replicatedCalledFunction = 0;
					Value* returnValue = 0;

					while (true) {
						CallInst* appendCall = 0;
						//TODO refactor
						for (map<Function*, list<CallInst*> >::iterator createdHopItr = createdHyperOpAndCallSite.begin(); createdHopItr != createdHyperOpAndCallSite.end(); createdHopItr++) {
							list<BasicBlock*> createdHopAccumulatedBlocks = createdHyperOpAndOriginalBasicBlockAndArgMap[createdHopItr->first].first;
							BasicBlock* lastBB = &createdHopAccumulatedBlocks.front()->getParent()->back();
							bool callSiteMatch = true;
							list<CallInst*> createdHopCallSite = createdHopItr->second;
							if (createdHopCallSite.size() != callSiteCopy.size()) {
								continue;
							}
							list<CallInst*>::iterator callSiteItr = callSiteCopy.begin();
							for (list<CallInst*>::iterator createHopCallSiteItr = createdHopCallSite.begin(); createHopCallSiteItr != createdHopCallSite.end() && callSiteItr != callSiteCopy.end(); createHopCallSiteItr++, callSiteItr++) {
								(*callSiteItr)->dump();
								if (*createHopCallSiteItr != *callSiteItr) {
									callSiteMatch = false;
									break;
								}
							}
							if (callSiteMatch) {
								if (find(createdHopAccumulatedBlocks.begin(), createdHopAccumulatedBlocks.end(), lastBB) != createdHopAccumulatedBlocks.end() || (isa<CallInst>(lastBB->front()) && isa<ReturnInst>(lastBB->front().getNextNode()))) {
									replicatedCalledFunction = createdHopItr->first;
									appendCall = (CallInst*) &lastBB->front();
									break;
								}
							}
						}
						if (createdHyperOpAndReturnValue.find(replicatedCalledFunction) != createdHyperOpAndReturnValue.end()) {
							returnValue = createdHyperOpAndReturnValue[replicatedCalledFunction];
						}
						if (returnValue == 0) {
							if (appendCall == 0) {
								break;
							}
							callSiteCopy.push_back(appendCall);
						} else {
							if (isa<CallInst>(returnValue)) {
								callSiteCopy.push_back((CallInst*) returnValue);
							} else {
								break;
							}
						}
					}
					if (returnValue == 0) {
						continue;
					}
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
						replacementArg.insert(make_pair(clonedInst, returnValue));
					} else {
						clonedInst = getClonedArgument(returnValue, callSiteCopy, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
						replacementArg.insert(make_pair(clonedInst, returnValue));
					}
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
								replacementArg.insert(make_pair(getClonedArgument(reachingDefInstr, callSite, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap), reachingDefInstr));
							}
						}
					}
				}

				if (hyperOpArgItr->second != GLOBAL_REFERENCE) {
					for (map<Instruction*, Value*>::iterator clonedReachingDefItr = replacementArg.begin(); clonedReachingDefItr != replacementArg.end(); clonedReachingDefItr++) {
						Instruction* clonedReachingDefInst = clonedReachingDefItr->first;
						list<Instruction*> clonedInstructionsToBeLabeled;
						clonedInstructionsToBeLabeled.push_back(clonedReachingDefInst);
						Function* producerFunction = ((Instruction*) clonedReachingDefItr->second)->getParent()->getParent();
						//Is it static?
						//TODO
						bool isProducerStatic = createdHyperOpAndType[clonedReachingDefInst->getParent()->getParent()];
						//Find the replicas of the consumer HyperOp which also need to be annotated, hence populating a list
						if (isProducerStatic && !isStaticHyperOp) {
							//replicate the meta data in the last instance of the HyperOp corresponding to the last HyperOp in the cycle
							list<list<pair<Function*, CallInst*> > > cyclesContainingCall = getCyclesContainingHyperOpInstance(callSite.back(), cyclesInCallGraph);
							list<CallInst*> callChain;
							std::copy(callSite.begin(), callSite.end(), std::back_inserter(callChain));
							for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
								list<pair<Function*, CallInst*> > cycle = *cycleItr;
								if (cycle.front().first == producerFunction) {
									for (list<pair<Function*, CallInst*> >::iterator cycleItr = cycle.begin(); cycleItr != cycle.end(); cycleItr++) {
										callChain.push_back(cycleItr->second);
									}
									callChain.pop_back();
									break;
								}
							}
							//Find the function corresponding to the callChain
							Instruction* clonedInstInstance = getClonedArgument(clonedReachingDefItr->second, callChain, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
							clonedInstructionsToBeLabeled.push_back(clonedInstInstance);
						}
						for (list<Instruction*>::iterator clonedInstItr = clonedInstructionsToBeLabeled.begin(); clonedInstItr != clonedInstructionsToBeLabeled.end(); clonedInstItr++) {
							Instruction* clonedDefInst = *clonedInstItr;
							errs() << "adding metadata to communicate ";
							clonedDefInst->dump();
							//Is the producer static?
							MDNode * consumedByMetadata;
							//TODO this isn't enough for nested recursion cycles
							if ((isProducerStatic && isStaticHyperOp) || (!isProducerStatic && !isStaticHyperOp)) {
								errs() << "adding static md\n";
								//Add "consumedby" metadata to the function locals that need to be passed to other HyperOps
								Value * values[3];
								values[0] = funcAnnotation;
								if (replacementArgType == SCALAR) {
									values[1] = MDString::get(ctxt, SCALAR_ARGUMENT);
								} else if (replacementArgType == LOCAL_REFERENCE) {
									values[1] = MDString::get(ctxt, LOCAL_REFERENCE_ARGUMENT);
								}
								//Local reference args don't need a context slot but we are adding them here anyway since they tail the arguments of a function and are required during metadata parsing
								values[2] = ConstantInt::get(ctxt, APInt(32, hyperOpArgumentIndex));
								consumedByMetadata = MDNode::get(ctxt, values);
							} else {
								errs() << "adding dynamic md\n";
								//Add "consumedby" metadata to the function locals that need to be passed to other HyperOps
								Value * values[4];
								values[0] = funcAnnotation;
								if (replacementArgType == SCALAR) {
									values[1] = MDString::get(ctxt, SCALAR_ARGUMENT);
								} else if (replacementArgType == LOCAL_REFERENCE) {
									values[1] = MDString::get(ctxt, LOCAL_REFERENCE_ARGUMENT);
								}
								//Local reference args don't need a context slot but we are adding them here anyway since they tail the arguments of a function and are required during metadata parsing
								values[2] = ConstantInt::get(ctxt, APInt(32, hyperOpArgumentIndex));
								if (isProducerStatic && !isStaticHyperOp) {
									values[3] = MDString::get(ctxt, "<id,0>");
								} else {
									values[3] = MDString::get(ctxt, "<prefixId>");
								}
								consumedByMetadata = MDNode::get(ctxt, values);
							}
							Instruction* metadataHost = 0;
							if (isa<AllocaInst>(clonedDefInst)) {
								metadataHost = clonedDefInst;
							} else if (isa < LoadInst > (clonedDefInst) &&isArgInList(clonedDefInst->getParent()->getParent(), clonedDefInst->getOperand(0))) {
								//function argument is passed on to another HyperOp, find the first load instruction from the memory location and add metadata to it
								for (Function::iterator bbItr = clonedDefInst->getParent()->getParent()->begin(); bbItr != clonedDefInst->getParent()->getParent()->end(); bbItr++) {
									for (BasicBlock::iterator instrItr = bbItr->begin(); instrItr != bbItr->end(); instrItr++) {
										if (isa < LoadInst > (instrItr) &&((LoadInst*) &instrItr)->getOperand(0) == clonedDefInst->getOperand(0)) {
											metadataHost = instrItr;
											break;
										}
									}
									if (metadataHost != 0) {
										break;
									}
								}
							} else if (allocaInstCreatedForIntermediateValues.find(clonedDefInst) != allocaInstCreatedForIntermediateValues.end()) {
								//Get the equivalent alloca inserted before
								metadataHost = allocaInstCreatedForIntermediateValues[clonedDefInst];
							}
							if (metadataHost == 0) {
								//Temporary data, add an alloca and a store instruction after the argument and label the alloca instruction with metadata
								AllocaInst* ai = new AllocaInst(clonedDefInst->getType());
								ai->setAlignment(4);
								//Alloc instructions need to be inserted in the entry basic block of the function because other allocs are treated as dynamic stack allocs
								ai->insertBefore(clonedDefInst->getParent()->getParent()->getEntryBlock().getFirstInsertionPt());
								StoreInst* storeInst = new StoreInst(clonedDefInst, ai);
								storeInst->setAlignment(4);
								storeInst->insertAfter(clonedDefInst);
								allocaInstCreatedForIntermediateValues.insert(make_pair(clonedDefInst, ai));
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
							if (isProducerStatic && find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), metadataHost->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()) {
								addedParentsToCurrentHyperOp.push_back(metadataHost->getParent()->getParent());
							}
							if (replacementArgType == LOCAL_REFERENCE && find(localReferenceArgProducers.begin(), localReferenceArgProducers.end(), metadataHost->getParent()->getParent()) == localReferenceArgProducers.end()) {
								localReferenceArgProducers.push_back(metadataHost->getParent()->getParent());
							}
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

							list<Instruction*> clonedInstructionsToBeLabeled;
							clonedInstructionsToBeLabeled.push_back(clonedInstr);
							Function* producerFunction = reachingDefInstr->getParent()->getParent();
							bool isProducerStatic = createdHyperOpAndType[clonedInstr->getParent()->getParent()];
							//Find the replicas of the consumer HyperOp which also need to be annotated, hence populating a list
							if (isProducerStatic && !isStaticHyperOp) {
								//replicate the meta data in the last instance of the HyperOp corresponding to the last HyperOp in the cycle
								list<list<pair<Function*, CallInst*> > > cyclesContainingCall = getCyclesContainingHyperOpInstance(callSite.back(), cyclesInCallGraph);
								list<CallInst*> callChain;
								std::copy(callSite.begin(), callSite.end(), std::back_inserter(callChain));
								for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
									list<pair<Function*, CallInst*> > cycle = *cycleItr;
									if (cycle.front().first == producerFunction) {
										for (list<pair<Function*, CallInst*> >::iterator cycleItr = cycle.begin(); cycleItr != cycle.end(); cycleItr++) {
											callChain.push_back(cycleItr->second);
										}
										callChain.pop_back();
										break;
									}
								}
								//Find the function corresponding to the callChain
								Instruction* clonedInstInstance = getClonedArgument(reachingDefInstr, callChain, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
								clonedInstructionsToBeLabeled.push_back(clonedInstInstance);
							}
							for (list<Instruction*>::iterator clonedInstItr = clonedInstructionsToBeLabeled.begin(); clonedInstItr != clonedInstructionsToBeLabeled.end(); clonedInstItr++) {
								Instruction* clonedInstr = *clonedInstItr;
								vector<Value*> metadataList;
								Instruction* metadataHost = 0;
								if (allocaInstCreatedForIntermediateValues.find(clonedInstr) != allocaInstCreatedForIntermediateValues.end()) {
									//Get the equivalent alloca inserted before
									metadataHost = allocaInstCreatedForIntermediateValues[clonedInstr];
									MDNode* predicateMetadata = metadataHost->getMetadata(HYPEROP_CONTROLS);
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
								MDNode* newPredicateMetadata;
								//TODO this isn't enough for nested recursion cycles
								if ((isProducerStatic && isStaticHyperOp) || (!isProducerStatic && !isStaticHyperOp)) {
									Value * values[1];
									values[0] = funcAnnotation;
									newPredicateMetadata = MDNode::get(ctxt, values);
								} else {
									Value * values[1];
									values[0] = funcAnnotation;
									if (isProducerStatic && !isStaticHyperOp) {
										values[1] = MDString::get(ctxt, "<id,0>");
									} else {
										values[1] = MDString::get(ctxt, "<prefixId>");
									}
									newPredicateMetadata = MDNode::get(ctxt, values);
								}
								metadataList.push_back(newPredicateMetadata);
								ArrayRef<Value*> metadataRef(metadataList);
								MDNode * predicatesRelation = MDNode::get(ctxt, metadataRef);
								metadataHost->setMetadata(HYPEROP_CONTROLS, predicatesRelation);

								//Parent function buffered to ensure that unnecessary control dependences need not exist
								if (isProducerStatic && find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), metadataHost->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()) {
									addedParentsToCurrentHyperOp.push_back(metadataHost->getParent()->getParent());
								}
								if (find(predicateProducers.begin(), predicateProducers.end(), metadataHost->getParent()->getParent()) == predicateProducers.end()) {
									predicateProducers.push_back(metadataHost->getParent()->getParent());
								}
							}

						}
					}
				}
			}

			DEBUG(dbgs() << "\n----------Dealing with conditional branches from other HyperOps----------\n");
//Find out if there exist any branch instructions leading to the HyperOp, add metadata to the instruction
			for (map<Instruction*, vector<unsigned> >::iterator conditionalBranchSourceItr = conditionalBranchSources.begin(); conditionalBranchSourceItr != conditionalBranchSources.end(); conditionalBranchSourceItr++) {
				Instruction* conditionalBranchInst = conditionalBranchSourceItr->first;
				errs() << "conditional branch:";
				conditionalBranchInst->dump();
				errs() << "from parent func:" << conditionalBranchInst->getParent()->getParent()->getName() << "\n";
				vector<unsigned> branchOperands = conditionalBranchSourceItr->second;
				BasicBlock* conditionalTargetBB = ((BranchInst*) conditionalBranchInst)->getSuccessor(branchOperands[0]);
				Value* predicateOperand = conditionalBranchInst->getOperand(0);
				Instruction* clonedInstr;
				Value* clonedPredicateOperand;
				list<CallInst*> newCallSite;
				std::copy(callSite.begin(), callSite.end(), std::back_inserter(newCallSite));

				if (isa<CallInst>(&conditionalTargetBB->front())) {
					//Current function is a call
					newCallSite.pop_back();
				}
				Function* replicatedCalledFunction = 0;
				if (isa<CallInst>(predicateOperand)) {
					newCallSite.push_back((CallInst*) predicateOperand);
					while (true) {
						CallInst* appendCall = 0;
						for (map<Function*, list<CallInst*> >::iterator createdHopItr = createdHyperOpAndCallSite.begin(); createdHopItr != createdHyperOpAndCallSite.end(); createdHopItr++) {
							bool callSiteMatch = true;
							list<CallInst*> createdHopCallSite = createdHopItr->second;
							if (createdHopCallSite.size() != newCallSite.size()) {
								callSiteMatch = false;
							}
							if (callSiteMatch) {
								list<CallInst*>::iterator callSiteItr = newCallSite.begin();
								for (list<CallInst*>::iterator createHopCallSiteItr = createdHopCallSite.begin(); createHopCallSiteItr != createdHopCallSite.end() && callSiteItr != newCallSite.end(); createHopCallSiteItr++, callSiteItr++) {
									if (*createHopCallSiteItr != *callSiteItr) {
										callSiteMatch = false;
										break;
									}
								}
							}
							if (callSiteMatch) {
								Function* callSiteFunc = createdHopItr->first;
								list<BasicBlock*> accumulatedBlocks = createdHyperOpAndOriginalBasicBlockAndArgMap[callSiteFunc].first;
								BasicBlock* lastBB = &accumulatedBlocks.front()->getParent()->back();
								if (find(accumulatedBlocks.begin(), accumulatedBlocks.end(), lastBB) != accumulatedBlocks.end() || (isa<CallInst>(lastBB->front()) && isa<ReturnInst>(lastBB->front().getNextNode()))) {
									replicatedCalledFunction = createdHopItr->first;
									appendCall = (CallInst*) &lastBB->front();
									break;
								}
							}
						}
						if (createdHyperOpAndReturnValue.find(replicatedCalledFunction) != createdHyperOpAndReturnValue.end()) {
							predicateOperand = createdHyperOpAndReturnValue[replicatedCalledFunction];
						}
						if (predicateOperand == 0) {
							if (appendCall == 0) {
								break;
							}
							newCallSite.push_back(appendCall);
						} else {
							if (isa<CallInst>(predicateOperand)) {
								newCallSite.push_back((CallInst*) predicateOperand);
							} else {
								break;
							}
						}
					}

				}
				if (isa < CallInst > (predicateOperand) &&predicateOperand == conditionalBranchInst->getOperand(0)) {
					continue;
				}
				if (isa<Constant>(predicateOperand)) {
					//Immediate value
					clonedInstr = new AllocaInst(predicateOperand->getType());
					((AllocaInst*) clonedInstr)->setAlignment(4);
					//Alloc instructions need to be inserted in the entry basic block of the function because other allocs are treated as dynamic stack allocs
					clonedInstr->insertBefore(replicatedCalledFunction->getEntryBlock().getFirstInsertionPt());
					StoreInst* storeInst = new StoreInst(predicateOperand, clonedInstr);
					storeInst->setAlignment(4);
					storeInst->insertBefore(retInstMap[replicatedCalledFunction]);
				} else {
					clonedPredicateOperand = getClonedArgument(predicateOperand, newCallSite, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
					clonedInstr = getClonedArgument(conditionalBranchInst, newCallSite, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
				}

				list<pair<Instruction*, Value*> > clonedInstructionsToBeLabeled;
				clonedInstructionsToBeLabeled.push_back(make_pair(clonedInstr, clonedPredicateOperand));
				Function* producerFunction = conditionalBranchInst->getParent()->getParent();
				bool isProducerStatic = createdHyperOpAndType[clonedInstr->getParent()->getParent()];
				//Find the replicas of the consumer HyperOp which also need to be annotated, hence populating a list
				if (isProducerStatic && !isStaticHyperOp) {
					//replicate the meta data in the last instance of the HyperOp corresponding to the last HyperOp in the cycle
					list<list<pair<Function*, CallInst*> > > cyclesContainingCall = getCyclesContainingHyperOpInstance(callSite.back(), cyclesInCallGraph);
					list<CallInst*> callChain;
					std::copy(callSite.begin(), callSite.end(), std::back_inserter(callChain));
					for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
						list<pair<Function*, CallInst*> > cycle = *cycleItr;
						if (cycle.front().first == producerFunction) {
							for (list<pair<Function*, CallInst*> >::iterator cycleItr = cycle.begin(); cycleItr != cycle.end(); cycleItr++) {
								callChain.push_back(cycleItr->second);
							}
							callChain.pop_back();
							break;
						}
					}
					//Find the function corresponding to the callChain
					Value* clonedPredicateOperand = getClonedArgument(predicateOperand, callChain, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
					Instruction* clonedInstInstance = getClonedArgument(conditionalBranchInst, callChain, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
					clonedInstructionsToBeLabeled.push_back(make_pair(clonedInstInstance, clonedPredicateOperand));
				}

				errs() << "how many instructions do we update?" << clonedInstructionsToBeLabeled.size() << "\n";
				for (list<pair<Instruction*, Value*> >::iterator clonedInstItr = clonedInstructionsToBeLabeled.begin(); clonedInstItr != clonedInstructionsToBeLabeled.end(); clonedInstItr++) {
					Instruction* clonedInstr = clonedInstItr->first;
					Value* predicateOperand = clonedInstItr->second;
					errs() << "updating instr:";
					clonedInstr->dump();
					errs() << "with predicate:";
					predicateOperand->dump();
					//Branch instruction's first operand
					vector<Value*> metadataList;
					Instruction* metadataHost = 0;
					if (isa<AllocaInst>(predicateOperand)) {
						metadataHost = (AllocaInst*) predicateOperand;
					} else if (allocaInstCreatedForIntermediateValues.find(predicateOperand) != allocaInstCreatedForIntermediateValues.end()) {
						//Get the equivalent alloca inserted before
						metadataHost = allocaInstCreatedForIntermediateValues[predicateOperand];
						MDNode* predicateMetadata = metadataHost->getMetadata(HYPEROP_CONTROLS);
						if (predicateMetadata != 0) {
							for (unsigned i = 0; i < predicateMetadata->getNumOperands(); i++) {
								metadataList.push_back(predicateMetadata->getOperand(i));
							}
						}
					}
					if (metadataHost == 0) {
						//Since the temporary does not have a memory location associated with it, add an alloca and a store instruction after the argument and label the alloca instruction with metadata
						AllocaInst* ai = new AllocaInst(predicateOperand->getType());
						ai->setAlignment(4);
						ai->insertBefore(clonedInstr->getParent()->getParent()->getEntryBlock().getFirstInsertionPt());
						StoreInst* storeInst = new StoreInst(predicateOperand, ai);
						storeInst->setAlignment(4);
						storeInst->insertBefore(clonedInstr);
						allocaInstCreatedForIntermediateValues[predicateOperand] = ai;
						metadataHost = ai;
					}
					//Label the instruction with predicates metadata
					MDNode* newPredicateMetadata;
					//TODO this isn't enough for nested recursion cycles
					if ((isProducerStatic && isStaticHyperOp) || (!isProducerStatic && !isStaticHyperOp)) {
						Value * values[1];
						values[0] = funcAnnotation;
						newPredicateMetadata = MDNode::get(ctxt, values);
					} else {
						Value * values[2];
						values[0] = funcAnnotation;
						if (isProducerStatic && !isStaticHyperOp) {
							values[1] = MDString::get(ctxt, "<id,0>");
						} else {
							values[1] = MDString::get(ctxt, "<prefixId>");
						}
						newPredicateMetadata = MDNode::get(ctxt, values);
					}
					metadataList.push_back(newPredicateMetadata);
					MDNode * predicatesRelation = MDNode::get(ctxt, metadataList);
					metadataHost->setMetadata(HYPEROP_CONTROLS, predicatesRelation);

					for (unsigned branchOperandIndex = 0; branchOperandIndex != conditionalBranchSourceItr->second.size(); branchOperandIndex++) {
						//Update the cloned conditional branch instruction with the right target
						unsigned positionToBeUpdated = ConstantInt::get(ctxt, APInt(32, conditionalBranchSourceItr->second[branchOperandIndex]))->getZExtValue();
						errs() << "updating position " << positionToBeUpdated << "\n";
						Instruction* retInstOfProducer = retInstMap.find(clonedInstr->getParent()->getParent())->second;
						((BranchInst*) clonedInstr)->setSuccessor(positionToBeUpdated, retInstOfProducer->getParent());
					}
					errs() << "instruction updated to ";
					clonedInstr->dump();

					list<BasicBlock*> branchTargets;
					for (unsigned successorIndex = 0; successorIndex != ((BranchInst*) clonedInstr)->getNumSuccessors(); successorIndex++) {
						if (find(branchTargets.begin(), branchTargets.end(), ((BranchInst*) clonedInstr)->getSuccessor(successorIndex)) == branchTargets.end()) {
							branchTargets.push_back(((BranchInst*) clonedInstr)->getSuccessor(successorIndex));
						}
					}

					//Branch instruction points to the same target, replace with unconditional branch
					if (branchTargets.size() == 1) {
						BranchInst* unconditionalBranch = BranchInst::Create(((BranchInst*) clonedInstr)->getSuccessor(0));
						unconditionalBranch->insertBefore(clonedInstr);
						clonedInstr->eraseFromParent();
					}
					if (isProducerStatic) {
						if (find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), metadataHost->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()) {
							addedParentsToCurrentHyperOp.push_back(metadataHost->getParent()->getParent());
						}
					}
					if (find(predicateProducers.begin(), predicateProducers.end(), metadataHost->getParent()->getParent()) == predicateProducers.end()) {
						predicateProducers.push_back(metadataHost->getParent()->getParent());
					}
				}

			}

			DEBUG(dbgs() << "\n----------Dealing with unconditional branches from other HyperOps----------\n");
			//Remove unconditional branch instruction, add the annotation to the alloca instruction of the branch
			for (list<Instruction*>::iterator unconditionalBranchSourceItr = unconditionalBranchSources.begin(); unconditionalBranchSourceItr != unconditionalBranchSources.end(); unconditionalBranchSourceItr++) {
				Value* unconditionalBranchInstr = *unconditionalBranchSourceItr;
				errs()<<"unconditional branch:";
				unconditionalBranchInstr->dump();
				Value* originalUnconditionalBranchInstr = unconditionalBranchInstr;
				BasicBlock* targetBB = ((BranchInst*) unconditionalBranchInstr)->getSuccessor(0);
				list<pair<Function*, Instruction*> > addedJumpSources;
				Instruction* clonedInstr;
				Function* replicatedCalledFunction = 0;
				list<BasicBlock*> parentBBList;

				list<CallInst*> newCallSite;
				std::copy(callSite.begin(), callSite.end(), std::back_inserter(newCallSite));
				if (isa<CallInst>(&targetBB->front())) {
					//						&& isStaticHyperOp) {
					//Current function is a call
					newCallSite.pop_back();
				}

				for (list<pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator bbItr = originalFunctionToHyperOpBBListMap[((Instruction*) unconditionalBranchInstr)->getParent()->getParent()].begin();
						bbItr != originalFunctionToHyperOpBBListMap[((Instruction*) unconditionalBranchInstr)->getParent()->getParent()].end(); bbItr++) {
					if (find(bbItr->first.begin(), bbItr->first.end(), ((Instruction*) originalUnconditionalBranchInstr)->getParent()) != bbItr->first.end()) {
						parentBBList = bbItr->first;
						break;
					}
				}
				list<TerminatorInst*> terminatorInst;
				terminatorInst.push_back((TerminatorInst*) originalUnconditionalBranchInstr);
				while (terminatorInst.size() > 0) {
					TerminatorInst* tempTerminator = terminatorInst.front();
					terminatorInst.pop_front();
					//Check if the terminator itself is conditionally executed
					for (pred_iterator terminatorPredItr = pred_begin(tempTerminator->getParent()); terminatorPredItr != pred_end(tempTerminator->getParent()); terminatorPredItr++) {
						BasicBlock* terminatorPredecessor = *terminatorPredItr;
						//Check if terminatorPredecessor and predecessor are both in the same function to be created
						if (find(parentBBList.begin(), parentBBList.end(), terminatorPredecessor) != parentBBList.end()) {
							if (terminatorPredecessor->getTerminator()->getNumSuccessors() > 1) {
								unconditionalBranchInstr = terminatorPredecessor->getTerminator();
								unconditionalBranchInstr->dump();
								break;
							} else {
								terminatorInst.push_front(terminatorPredecessor->getTerminator());
							}
						}
					}
				}
				//Conditional branch exists in the same function which jumps to the unconditional terminator's basic block
				//TODO duplicate code, need to clean this up
				if (unconditionalBranchInstr != originalUnconditionalBranchInstr) {
					Value* predicateOperand = ((Instruction*) unconditionalBranchInstr)->getOperand(0);
					Instruction* clonedInstr;
					Value* clonedPredicateOperand;
					Function* replicatedCalledFunction = 0;
					if (isa<CallInst>(predicateOperand)) {
						newCallSite.push_back((CallInst*) predicateOperand);
						while (true) {
							CallInst* appendCall = 0;
							for (map<Function*, list<CallInst*> >::iterator createdHopItr = createdHyperOpAndCallSite.begin(); createdHopItr != createdHyperOpAndCallSite.end(); createdHopItr++) {
								bool callSiteMatch = true;
								list<CallInst*> createdHopCallSite = createdHopItr->second;
								if (createdHopCallSite.size() != newCallSite.size()) {
									callSiteMatch = false;
								}
								if (callSiteMatch) {
									list<CallInst*>::iterator callSiteItr = newCallSite.begin();
									for (list<CallInst*>::iterator createHopCallSiteItr = createdHopCallSite.begin(); createHopCallSiteItr != createdHopCallSite.end() && callSiteItr != newCallSite.end(); createHopCallSiteItr++, callSiteItr++) {
										if (*createHopCallSiteItr != *callSiteItr) {
											callSiteMatch = false;
											break;
										}
									}
								}
								if (callSiteMatch) {
									Function* callSiteFunc = createdHopItr->first;
									list<BasicBlock*> accumulatedBlocks = createdHyperOpAndOriginalBasicBlockAndArgMap[callSiteFunc].first;
									BasicBlock* lastBB = &accumulatedBlocks.front()->getParent()->back();
									if (find(accumulatedBlocks.begin(), accumulatedBlocks.end(), lastBB) != accumulatedBlocks.end() || (isa<CallInst>(lastBB->front()) && isa<ReturnInst>(lastBB->front().getNextNode()))) {
										replicatedCalledFunction = createdHopItr->first;
										appendCall = (CallInst*) &lastBB->front();
										break;
									}
								}
							}
							if (createdHyperOpAndReturnValue.find(replicatedCalledFunction) != createdHyperOpAndReturnValue.end()) {
								predicateOperand = createdHyperOpAndReturnValue[replicatedCalledFunction];
							}

							if (predicateOperand == 0) {
								if (appendCall == 0) {
									break;
								}
								newCallSite.push_back(appendCall);
							} else {
								if (isa<CallInst>(predicateOperand)) {
									newCallSite.push_back((CallInst*) predicateOperand);
								} else {
									break;
								}
							}
						}
					}
					if (isa < CallInst > (predicateOperand) &&predicateOperand == ((Instruction*) unconditionalBranchInstr)->getOperand(0)) {
						continue;
					}
					if (isa<Constant>(predicateOperand)) {
						//Immediate value
						clonedInstr = new AllocaInst(predicateOperand->getType());
						((AllocaInst*) clonedInstr)->setAlignment(4);
						//Alloc instructions need to be inserted in the entry basic block of the function because other allocs are treated as dynamic stack allocs
						clonedInstr->insertBefore(replicatedCalledFunction->getEntryBlock().getFirstInsertionPt());
						StoreInst* storeInst = new StoreInst(predicateOperand, clonedInstr);
						storeInst->setAlignment(4);
						storeInst->insertBefore(retInstMap[replicatedCalledFunction]);
					} else {
						clonedPredicateOperand = getClonedArgument(predicateOperand, newCallSite, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
						clonedInstr = getClonedArgument(originalUnconditionalBranchInstr, newCallSite, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
					}

					list<pair<Instruction*, Value*> > clonedInstructionsToBeLabeled;
					clonedInstructionsToBeLabeled.push_back(make_pair(clonedInstr, clonedPredicateOperand));
					Function* producerFunction = ((Instruction*) unconditionalBranchInstr)->getParent()->getParent();
					bool isProducerStatic = createdHyperOpAndType[clonedInstr->getParent()->getParent()];
					//Find the replicas of the consumer HyperOp which also need to be annotated, hence populating a list
					if (isProducerStatic && !isStaticHyperOp) {
						//replicate the meta data in the last instance of the HyperOp corresponding to the last HyperOp in the cycle
						list<list<pair<Function*, CallInst*> > > cyclesContainingCall = getCyclesContainingHyperOpInstance(callSite.back(), cyclesInCallGraph);
						list<CallInst*> callChain;
						std::copy(callSite.begin(), callSite.end(), std::back_inserter(callChain));
						for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
							list<pair<Function*, CallInst*> > cycle = *cycleItr;
							if (cycle.front().first == producerFunction) {
								for (list<pair<Function*, CallInst*> >::iterator cycleItr = cycle.begin(); cycleItr != cycle.end(); cycleItr++) {
									callChain.push_back(cycleItr->second);
								}
								callChain.pop_back();
								break;
							}
						}
						//Find the function corresponding to the callChain
						Value* clonedPredicate = getClonedArgument(predicateOperand, callChain, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
						Instruction* clonedOriginalInstr = getClonedArgument(originalUnconditionalBranchInstr, callChain, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
						clonedInstructionsToBeLabeled.push_back(make_pair((Instruction*) clonedOriginalInstr, clonedPredicate));
					}
					for (list<pair<Instruction*, Value*> >::iterator clonedInstItr = clonedInstructionsToBeLabeled.begin(); clonedInstItr != clonedInstructionsToBeLabeled.end(); clonedInstItr++) {
						Instruction* clonedInstr = clonedInstItr->first;
						Value* predicateOperand = clonedInstItr->second;

						//Update the unconditional branch instruction's first operand
						vector<Value*> metadataList;
						Instruction* metadataHost = 0;
						if (isa<AllocaInst>(predicateOperand)) {
							metadataHost = (AllocaInst*) predicateOperand;
						} else if (allocaInstCreatedForIntermediateValues.find(predicateOperand) != allocaInstCreatedForIntermediateValues.end()) {
							//Get the equivalent alloca inserted before
							metadataHost = allocaInstCreatedForIntermediateValues[predicateOperand];
							MDNode* predicateMetadata = metadataHost->getMetadata(HYPEROP_CONTROLS);
							if (predicateMetadata != 0) {
								for (unsigned i = 0; i < predicateMetadata->getNumOperands(); i++) {
									metadataList.push_back(predicateMetadata->getOperand(i));
								}
							}
						}
						if (metadataHost == 0) {
							//Since the temporary does not have a memory location associated with it, add an alloca and a store instruction after the argument and label the alloca instruction with metadata
							AllocaInst* ai = new AllocaInst(predicateOperand->getType());
							ai->setAlignment(4);
							ai->insertBefore(clonedInstr->getParent()->getParent()->getEntryBlock().getFirstInsertionPt());
							StoreInst* storeInst = new StoreInst(predicateOperand, ai);
							storeInst->setAlignment(4);
							storeInst->insertBefore(clonedInstr);
							allocaInstCreatedForIntermediateValues[predicateOperand] = ai;
							metadataHost = ai;
						}
						//Label the instruction with predicates metadata
						MDNode* newPredicateMetadata;
						//TODO this isn't enough for nested recursion cycles
						if ((isProducerStatic && isStaticHyperOp) || (!isProducerStatic && !isStaticHyperOp)) {
							Value * values[1];
							values[0] = funcAnnotation;
							newPredicateMetadata = MDNode::get(ctxt, values);
						} else {
							Value * values[2];
							values[0] = funcAnnotation;
							if (isProducerStatic && !isStaticHyperOp) {
								values[1] = MDString::get(ctxt, "<id,0>");
							} else {
								values[1] = MDString::get(ctxt, "<prefixId>");
							}
							newPredicateMetadata = MDNode::get(ctxt, values);
						}
						metadataList.push_back(newPredicateMetadata);
						MDNode * predicatesRelation = MDNode::get(ctxt, metadataList);
						metadataHost->setMetadata(HYPEROP_CONTROLS, predicatesRelation);

						//Update the unconditional branch instruction to jump to the retInst of its function(HyperOp)
						clonedInstr->setOperand(0, retInstMap[clonedInstr->getParent()->getParent()]->getParent());

						if (isProducerStatic) {
							if (find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), metadataHost->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()) {
								addedParentsToCurrentHyperOp.push_back(metadataHost->getParent()->getParent());
							}
						}
						if (find(predicateProducers.begin(), predicateProducers.end(), metadataHost->getParent()->getParent()) == predicateProducers.end()) {
							predicateProducers.push_back(metadataHost->getParent()->getParent());
						}
					}
				} else {
					//Unconditional branch's target is a call instruction
					if (isa<CallInst>(&(*unconditionalBranchSourceItr)->getParent()->front())) {
//						&& createdHyperOpAndType.find((*unconditionalBranchSourceItr)->getParent()->getParent()) != createdHyperOpAndType.end() && createdHyperOpAndType[(*unconditionalBranchSourceItr)->getParent()->getParent()]) {
						newCallSite.push_back((CallInst*) &(*unconditionalBranchSourceItr)->getParent()->front());
						while (true) {
							CallInst* appendCall = 0;
							for (map<Function*, list<CallInst*> >::iterator createdHopItr = createdHyperOpAndCallSite.begin(); createdHopItr != createdHyperOpAndCallSite.end(); createdHopItr++) {
								bool callSiteMatch = true;
								list<CallInst*> createdHopCallSite = createdHopItr->second;
								if (createdHopCallSite.size() != newCallSite.size()) {
									callSiteMatch = false;
								}
								if (callSiteMatch) {
									list<CallInst*>::iterator callSiteItr = newCallSite.begin();
									for (list<CallInst*>::iterator createHopCallSiteItr = createdHopCallSite.begin(); createHopCallSiteItr != createdHopCallSite.end() && callSiteItr != newCallSite.end(); createHopCallSiteItr++, callSiteItr++) {
										if (*createHopCallSiteItr != *callSiteItr) {
											callSiteMatch = false;
											break;
										}
									}
								}
								if (callSiteMatch) {
									Function* callSiteFunc = createdHopItr->first;
									list<BasicBlock*> accumulatedBlocks = createdHyperOpAndOriginalBasicBlockAndArgMap[callSiteFunc].first;
									BasicBlock* lastBB = &accumulatedBlocks.front()->getParent()->back();
									if (find(accumulatedBlocks.begin(), accumulatedBlocks.end(), lastBB) != accumulatedBlocks.end() || (isa<CallInst>(lastBB->front()) && isa<ReturnInst>(lastBB->front().getNextNode()))) {
										replicatedCalledFunction = createdHopItr->first;
										appendCall = (CallInst*) &lastBB->front();
										break;
									}
								}
							}
							if (createdHyperOpAndReturnValue.find(replicatedCalledFunction) != createdHyperOpAndReturnValue.end()) {
								unconditionalBranchInstr = createdHyperOpAndReturnValue[replicatedCalledFunction];
							}

							if (unconditionalBranchInstr == 0) {
								if (appendCall == 0) {
									break;
								}
								newCallSite.push_back(appendCall);
							} else {
								if (isa<CallInst>(unconditionalBranchInstr)) {
//									&& createdHyperOpAndType[(*unconditionalBranchSourceItr)->getParent()->getParent()]) {
									newCallSite.push_back((CallInst*) unconditionalBranchInstr);
								} else {
									break;
								}
							}
						}
					}
					if (unconditionalBranchInstr == *unconditionalBranchSourceItr && isa<CallInst>(&(*unconditionalBranchSourceItr)->getParent()->front())) {
						continue;
					}
					if (isa<Constant>(unconditionalBranchInstr)) {
						//Immediate value
						clonedInstr = new AllocaInst(unconditionalBranchInstr->getType());
						((AllocaInst*) clonedInstr)->setAlignment(4);
						//Alloc instructions need to be inserted in the entry basic block of the function because other allocs are treated as dynamic stack allocs
						clonedInstr->insertBefore(replicatedCalledFunction->getEntryBlock().getFirstInsertionPt());
						StoreInst* storeInst = new StoreInst(unconditionalBranchInstr, clonedInstr);
						storeInst->setAlignment(4);
						storeInst->insertBefore(retInstMap[replicatedCalledFunction]);
					} else {
						clonedInstr = getClonedArgument(unconditionalBranchInstr, newCallSite, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
					}
					list<Instruction*> clonedInstructionsToBeLabeled;
					clonedInstructionsToBeLabeled.push_back(clonedInstr);
					Function* producerFunction = ((Instruction*) unconditionalBranchInstr)->getParent()->getParent();
					bool isProducerStatic = createdHyperOpAndType[clonedInstr->getParent()->getParent()];
					//Find the replicas of the consumer HyperOp which also need to be annotated, hence populating a list
					if (isProducerStatic && !isStaticHyperOp) {
						//replicate the meta data in the last instance of the HyperOp corresponding to the last HyperOp in the cycle
						list<list<pair<Function*, CallInst*> > > cyclesContainingCall = getCyclesContainingHyperOpInstance(callSite.back(), cyclesInCallGraph);
						list<CallInst*> callChain;
						std::copy(callSite.begin(), callSite.end(), std::back_inserter(callChain));
						for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
							list<pair<Function*, CallInst*> > cycle = *cycleItr;
							if (cycle.front().first == producerFunction) {
								for (list<pair<Function*, CallInst*> >::iterator cycleItr = cycle.begin(); cycleItr != cycle.end(); cycleItr++) {
									callChain.push_back(cycleItr->second);
								}
								callChain.pop_back();
								break;
							}
						}
						//Find the function corresponding to the callChain
						Instruction* clonedInstInstance = getClonedArgument(unconditionalBranchInstr, callChain, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
						clonedInstructionsToBeLabeled.push_back(clonedInstInstance);
					}
					for (list<Instruction*>::iterator clonedInstItr = clonedInstructionsToBeLabeled.begin(); clonedInstItr != clonedInstructionsToBeLabeled.end(); clonedInstItr++) {
						Instruction* clonedInstr = *clonedInstItr;
						Instruction* retInstOfProducer = retInstMap.find(clonedInstr->getParent()->getParent())->second;
						if (isa<BranchInst>(clonedInstr)) {
							//Update the branch instruction to jump to the return basic block
							clonedInstr->setOperand(0, retInstOfProducer->getParent());
						}
					}
				}
			}

			DEBUG(dbgs() << "\n----------Adding a predicate from entry hyperop if there are no incoming edges to the hyperop----------\n");

			//Check if a dynamic HyperOp has added parents
			if (addedParentsToCurrentHyperOp.empty() && createdHyperOpAndType[createdFunction] == DYNAMIC) {
				for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHyperOpItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHyperOpItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHyperOpItr++) {
					for (Function::iterator bbItr = createdHyperOpItr->first->begin(); bbItr != createdHyperOpItr->first->end(); bbItr++) {
						for (BasicBlock::iterator instItr = bbItr->begin(); instItr != bbItr->end(); instItr++) {
							MDNode* consumedByMDNode = instItr->getMetadata(HYPEROP_CONSUMED_BY);
							if (consumedByMDNode != 0) {
								for (unsigned i = 0; i < consumedByMDNode->getNumOperands(); i++) {
									MDNode* consumer = (MDNode*) ((MDNode*) (consumedByMDNode->getOperand(i)))->getOperand(0);
									if (funcAnnotation == consumer) {
										addedParentsToCurrentHyperOp.push_back(createdHyperOpItr->first);
										break;
									}
								}
							}
							if (addedParentsToCurrentHyperOp.empty()) {
								MDNode* controlledByMDNode = instItr->getMetadata(HYPEROP_CONTROLS);
								if (controlledByMDNode != 0) {
									for (unsigned i = 0; i < controlledByMDNode->getNumOperands(); i++) {
										MDNode* consumer = (MDNode*) ((MDNode*) (controlledByMDNode->getOperand(i)))->getOperand(0);
										if (funcAnnotation == consumer) {
											addedParentsToCurrentHyperOp.push_back(createdHyperOpItr->first);
											break;
										}
									}
								}
							}
						}
					}
				}
			}
			if (addedParentsToCurrentHyperOp.empty() && startHyperOp != createdFunction) {
				//There are no incoming edge to the hyperop, add a predicate edge from the entry HyperOp
				vector<Value*> nodeList;
				Value* values[1];
				values[0] = hyperOpAndAnnotationMap[createdFunction];
				MDNode* newPredicateMetadata = MDNode::get(ctxt, values);
				nodeList.push_back(newPredicateMetadata);
				MDNode* node = MDNode::get(ctxt, nodeList);
				AllocaInst* ai = new AllocaInst(Type::getInt1Ty(ctxt));
				ai->setAlignment(4);
				ai->insertBefore(startHyperOp->begin()->getFirstInsertionPt());
				StoreInst* storeInst = new StoreInst(ConstantInt::get(ctxt, APInt(1, 1)), ai);
				storeInst->setAlignment(4);
				storeInst->insertBefore(retInstMap[startHyperOp]);
				ai->setMetadata(HYPEROP_SYNC, node);
				list<Instruction*> incomingEdgesToHop;
				incomingEdgesToHop.push_back(ai);
				if (find(predicateProducers.begin(), predicateProducers.end(), ai->getParent()->getParent()) == predicateProducers.end()) {
					predicateProducers.push_back(ai->getParent()->getParent());
				}
			}

			DEBUG(dbgs() << "\n----------Adding sync edges to HyperOps that only take local reference inputs----------\n");
			for (list<Function*>::iterator localRefItr = localReferenceArgProducers.begin(); localRefItr != localReferenceArgProducers.end(); localRefItr++) {
				Function* localRefProducer = *localRefItr;
				if (predicateProducers.empty() || find(predicateProducers.begin(), predicateProducers.end(), localRefProducer) == predicateProducers.end()) {
					//Add a sync edge from local ref producer and the consumer HyperOp
					Value* values[1];
					values[0] = hyperOpAndAnnotationMap[createdFunction];
					MDNode* newPredicateMetadata = MDNode::get(ctxt, values);
					Instruction* metadataHost = &(localRefProducer->begin()->front());
					MDNode* currentMetadataOfInstruction = metadataHost->getMetadata(HYPEROP_SYNC);
					vector<Value*> newMDNodeValues;
					//Same data maybe required by multiple HyperOps
					if (currentMetadataOfInstruction != 0) {
						for (unsigned i = 0; i < currentMetadataOfInstruction->getNumOperands(); i++) {
							newMDNodeValues.push_back(currentMetadataOfInstruction->getOperand(i));
						}
					}
					newMDNodeValues.push_back(newPredicateMetadata);
					MDNode* mdNode = MDNode::get(ctxt, newMDNodeValues);
					//Create a sync edge between the current HyperOp and the last HyperOp
					//We use sync edge here because adding a predicate to the end hyperop will increase the number of predicates
					metadataHost->setMetadata(HYPEROP_SYNC, mdNode);
					syncMDNodeList.push_back(newPredicateMetadata);
				}
			}
			errs() << "after patching function " << createdFunction->getName() << ", module:";
			M.dump();
		}

		DEBUG(dbgs() << "\n----------Adding sync edges to dangling HyperOps----------\n");
		for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHyperOpItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHyperOpItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHyperOpItr++) {
			Function* createdFunction = createdHyperOpItr->first;
			if (createdFunction != endHyperOp) {
				bool hasOutgoingEdges = false;
				for (Function::iterator bbItr = createdFunction->begin(); bbItr != createdFunction->end(); bbItr++) {
					for (BasicBlock::iterator instItr = bbItr->begin(); instItr != bbItr->end(); instItr++) {
						//check if any of the instructions have any metadata associated
						if (instItr->getMetadata(HYPEROP_CONSUMED_BY) != 0 || instItr->getMetadata(HYPEROP_CONTROLS) != 0 || instItr->getMetadata(HYPEROP_SYNC) != 0) {
							hasOutgoingEdges = true;
							break;
						}
					}
				}
				if (!hasOutgoingEdges) {
					//Add a sync edge to end HyperOp
					vector<Value*> nodeList;
					Value* values[1];
					values[0] = hyperOpAndAnnotationMap[endHyperOp];
					MDNode* newPredicateMetadata = MDNode::get(ctxt, values);
					nodeList.push_back(newPredicateMetadata);
					MDNode* mdNode = MDNode::get(ctxt, nodeList);
					//Create a sync edge between the current HyperOp and the last HyperOp
					//We use sync edge here because adding a predicate to the end hyperop
					createdFunction->begin()->front().setMetadata(HYPEROP_SYNC, mdNode);
					syncMDNodeList.push_back(newPredicateMetadata);
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
		errs() << "before deleting:";
		M.dump();

		DEBUG(dbgs() << "\n-----------Re-routing sync edges from end hyperop to the newly created end HyperOp-----------\n");
		//If the end hyperop has 16 arguments, create another end hyperop so that sync (if any) can go to the last HyperOp
		unsigned numRegArgsToEnd = 0;
		for (unsigned i = 0; i < endHyperOp->getArgumentList().size(); i++) {
			if (endHyperOp->getAttributes().hasAttribute(i, Attribute::InReg)) {
				numRegArgsToEnd++;
			}
		}

		if (numRegArgsToEnd == FRAME_SIZE && !syncMDNodeList.empty()) {
			//Add a new dummy HyperOp
			FunctionType *FT = FunctionType::get(Type::getVoidTy(getGlobalContext()), false);
			Function *newFunction = Function::Create(FT, Function::ExternalLinkage, "redefine_end", &M);
			//Add a dummy return block
			BasicBlock* retBB = BasicBlock::Create(ctxt, newFunction->getName().str().append(".return"), newFunction);
			Instruction* retInst = ReturnInst::Create(ctxt);
			retBB->getInstList().insert(retBB->getFirstInsertionPt(), retInst);

			Value * values[3];
			values[0] = MDString::get(ctxt, HYPEROP);
			values[1] = newFunction;
			values[2] = MDString::get(ctxt, STATIC_HYPEROP);
			MDNode *funcAnnotation = MDNode::get(ctxt, values);
			redefineAnnotationsNode->addOperand(funcAnnotation);
			hyperOpAndAnnotationMap[newFunction] = funcAnnotation;

			Value* hyperOpDescrMDValues[2];
			hyperOpDescrMDValues[0] = MDString::get(ctxt, HYPEROP_EXIT);
			hyperOpDescrMDValues[1] = funcAnnotation;
			MDNode* hyperOpDescMDNode = MDNode::get(ctxt, hyperOpDescrMDValues);
			redefineAnnotationsNode->addOperand(hyperOpDescMDNode);

			Instruction* endHopRetInst = retInstMap[endHyperOp];
			AllocaInst* ai = new AllocaInst(Type::getInt1Ty(ctxt));
			ai->setAlignment(4);
			ai->insertBefore(endHopRetInst->getParent()->getParent()->getEntryBlock().getFirstInsertionPt());
			StoreInst* storeInst = new StoreInst(ConstantInt::get(ctxt, APInt(1, 1)), ai);
			storeInst->setAlignment(4);
			storeInst->insertBefore(endHopRetInst);

			Value * annotationValues[1];
			annotationValues[0] = hyperOpAndAnnotationMap[endHyperOp];
			MDNode* newPredicateMetadata = MDNode::get(ctxt, annotationValues);
			vector<Value*> metadataList;
			metadataList.push_back(newPredicateMetadata);
			MDNode * predicatesRelation = MDNode::get(ctxt, metadataList);
			ai->setMetadata(HYPEROP_SYNC, predicatesRelation);

			for (list<MDNode*>::iterator syncSourceItr = syncMDNodeList.begin(); syncSourceItr != syncMDNodeList.end(); syncSourceItr++) {
				MDNode* source = *syncSourceItr;
				source->replaceOperandWith(0, funcAnnotation);
			}

			list<BasicBlock*> accumulatedBasicBlocks;
			HyperOpArgumentList hyperOpArguments;
			createdHyperOpAndOriginalBasicBlockAndArgMap[newFunction] = make_pair(accumulatedBasicBlocks, hyperOpArguments);
		}

		DEBUG(dbgs() << "\n-----------Removing unreachable basic blocks from created functions-----------\n");
		for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHyperOpItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHyperOpItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHyperOpItr++) {
			Function* newFunction = createdHyperOpItr->first;
			list<BasicBlock*> bbForDelete;
			for (Function::iterator bbItr = newFunction->begin(); bbItr != newFunction->end(); bbItr++) {
				//If bbItr is not the entry block and has no predecessors or multiple predecessors (this check is in place to avoid considering basic blocks which have a single predecessor, an optimization)
				if (&*bbItr != &newFunction->getEntryBlock()) {
					BasicBlock* bb = &*bbItr;
					bool hasPredecessor = false;
					for (Function::iterator secondBBItr = newFunction->begin(); secondBBItr != newFunction->end(); secondBBItr++) {
						for (unsigned i = 0; i < secondBBItr->getTerminator()->getNumSuccessors(); i++) {
							BasicBlock* successor = secondBBItr->getTerminator()->getSuccessor(i);
							if (successor == bb) {
								hasPredecessor = true;
								break;
							}
						}
						if (hasPredecessor) {
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

		DEBUG(dbgs() << "\n-----------Adding entry bb-----------\n");
		for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHyperOpItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHyperOpItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHyperOpItr++) {
			Function* newFunction = createdHyperOpItr->first;
			//If the entry basic block has multiple incoming edges, create a new HyperOp which has an unconditional jump to it
			unsigned predCount = 0;
			for (pred_iterator predecessorItr = pred_begin(&newFunction->getEntryBlock()); predecessorItr != pred_end(&newFunction->getEntryBlock()); predecessorItr++, predCount++) {
			}
			if (predCount > 0) {
				//Create a new bb as the first bb
				BasicBlock* oldBB = &newFunction->getEntryBlock();
				BasicBlock* newEntryBB = BasicBlock::Create(ctxt, newFunction->getName().str().append(".entry"), newFunction, oldBB);
				Instruction* branchInst = BranchInst::Create(oldBB);
				newEntryBB->getInstList().insert(newEntryBB->getFirstInsertionPt(), branchInst);
			}
		}

		DEBUG(dbgs() << "\n-----------Deleting unused functions-----------\n");
		//Workaround for deleting unused functions, deletion doesn't work unless in topological order but what about recursion?
		list<Function*> functionsForDeletion;
		for (Module::iterator functionItr = M.begin(); functionItr != M.end(); functionItr++) {
			//Remove old functions from module
			if (createdHyperOpAndOriginalBasicBlockAndArgMap.find(functionItr) == createdHyperOpAndOriginalBasicBlockAndArgMap.end() && !functionItr->isIntrinsic()) {
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
	const char* STATIC_HYPEROP = "Static";
	const char* DYNAMIC_HYPEROP = "Dynamic";
}
;
char HyperOpCreationPass::ID = 2;
char* HyperOpCreationPass::NEW_NAME = "newName";
static RegisterPass<HyperOpCreationPass> X("HyperOpCreationPass", "Pass to create HyperOps");
