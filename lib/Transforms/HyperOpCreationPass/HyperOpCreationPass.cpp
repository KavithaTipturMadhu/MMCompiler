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
#include "LoopInterchange.h"
#include "llvm/Transforms/Utils/CodeExtractor.h"
#include "llvm/IR/Instruction.def"
#include "llvm/IR/HyperOpMetadataParser.h"
#include "llvm/IR/HyperOpInteractionGraph.h"
#include "llvm/Support/CommandLine.h"
using namespace llvm;

#define DEBUG_TYPE "HyperOpCreationPass"
#define INLINE_FUNCTION_CALLS 0
#define BASICBLOCK_HYPEROP 0

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
		SCALAR, GLOBAL_REFERENCE, ADDRESS
	};

	class LoopIV {
	public:
		typedef enum Type {
			CONSTANT, VARIABLE
		} boundType;

		boundType upperBoundType, lowerBoundType;

		union {
			unsigned constantLowerbound;
			Value* variableLowerBound;
		} lowerBound;

		union {
			unsigned constantUpperbound;
			Value* variableUpperbound;
		} upperBound;

		PHINode* inductionVar;
		Value* stride;
		unsigned operation;
		const char* operationName;

		LoopIV(Value* stride, PHINode* inductionVar, unsigned operation, const char* operationName) {
			this->inductionVar = inductionVar;
			this->stride = stride;
			this->operation = operation;
			this->operationName = operationName;
			this->lowerBoundType = CONSTANT;
			this->upperBoundType = CONSTANT;
		}

		Type getLowerBoundType() {
			return lowerBoundType;
		}
		Type getUpperBoundType() {
			return upperBoundType;
		}

		void setConstantUpperBound(unsigned upperBound) {
			this->upperBound.constantUpperbound = upperBound;
			this->upperBoundType = CONSTANT;
		}

		unsigned getConstantUpperBound() {
			return this->upperBound.constantUpperbound;
		}
		void setVariableUpperBound(Value* upperBound) {
			this->upperBound.variableUpperbound = upperBound;
			this->upperBoundType = VARIABLE;
		}
		Value* getVariableUpperBound() {
			return this->upperBound.variableUpperbound;
		}

		void setConstantLowerBound(unsigned lowerBound) {
			this->lowerBound.constantLowerbound = lowerBound;
			this->lowerBoundType = CONSTANT;
		}
		unsigned getConstantLowerBound() {
			return this->lowerBound.constantLowerbound;
		}
		void setVariableLowerBound(Value* lowerBound) {
			this->lowerBound.variableLowerBound = lowerBound;
			this->lowerBoundType = VARIABLE;
		}

		Value* getVariableLowerBound() {
			return this->lowerBound.variableLowerBound;
		}

		PHINode* getInductionVar() {
			return inductionVar;
		}

		Value* getStride() {
			return stride;
		}
		const char* getIncOperation() {
			return this->operationName;
		}
	};

	//Map of HyperOp arguments with position as the key
	typedef list<pair<list<Value*>, HyperOpArgumentType> > HyperOpArgumentList;

	HyperOpCreationPass() :
			ModulePass(ID) {
	}

	virtual void getAnalysisUsage(AnalysisUsage &AU) const {
		//Mandatory merge return to be invoked on each function
		AU.addRequired<UnifyFunctionExitNodes>();
		AU.addRequired<ScalarEvolution>();
		AU.addRequired<DependenceAnalysis>();
		AU.addRequired<PostDominatorTree>();
	}

	bool pathExistsInCFG(BasicBlock* source, BasicBlock* target, list<BasicBlock*> visitedBasicBlocks) {
		if (source->getParent() == target->getParent()) {
			visitedBasicBlocks.push_back(source);
			for (unsigned i = 0; i < source->getTerminator()->getNumSuccessors(); i++) {
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

	//Duplicate string parser, can move it from here and metadata parser later to utils
	list<StringRef> parseInstanceIdString(StringRef instanceTag, char seperator = ',') {
		list<StringRef> instanceId;
		//Parse string to get a list of identifiers
		instanceTag = instanceTag.drop_back();
		instanceTag = instanceTag.drop_front();
		StringRef tempString = instanceTag;
		while (!tempString.empty()) {
			pair<StringRef, StringRef> tokens = tempString.split(seperator);
			StringRef idPart = tokens.first;
			instanceId.push_back(idPart);
			tempString = tokens.second;
		}
		return instanceId;
	}

	bool isArgInList(Function* function, Value* operand) {
		for (Function::arg_iterator argItr = function->arg_begin(); argItr != function->arg_end(); argItr++) {
			if (argItr == operand) {
				return true;
			}
		}
		return false;
	}

	BasicBlock* getExitBlock(Function* function) {
		for (auto bbItr = function->begin(); bbItr != function->end(); bbItr++) {
			unsigned succCount = 0;
			for (auto succItr = succ_begin(bbItr); succItr != succ_end(bbItr); succItr++, succCount++)
				;
			if (succCount == 0) {
				return bbItr;
			}
		}
		return NULL;
	}

	inline list<BasicBlock*> getBBListContainingBasicBlock(BasicBlock * targetBB, map<Function*, list<pair<list<BasicBlock*>, HyperOpArgumentList> > > originalFunctionToHyperOpBBListMap) {
		list<BasicBlock*> retVal;
		//Get the parent basic block containing the unconditional jump
		for (list<pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator bbItr = originalFunctionToHyperOpBBListMap[targetBB->getParent()].begin(); bbItr != originalFunctionToHyperOpBBListMap[targetBB->getParent()].end(); bbItr++) {
			if (find(bbItr->first.begin(), bbItr->first.end(), targetBB) != bbItr->first.end()) {
				return bbItr->first;
			}
		}
		return retVal;
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

//List of basic blocks containing store instructions that reach useInstr
	map<BasicBlock*, Instruction*> reachingStoreOperations(AllocaInst* useInstr, Function* originalFunction, list<BasicBlock*> accumulatedBasicBlocks) {
		map<BasicBlock*, Instruction*> reachingDefinitions;
		//Find uses of allocInstr in the basic blocks of created functions
		map<BasicBlock*, Instruction*> basicBlocksWithDefinitions;
		for (Function::iterator originalBBItr = originalFunction->begin(); originalBBItr != originalFunction->end(); originalBBItr++) {
			BasicBlock* originalBB = &*originalBBItr;
			if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), originalBB) == accumulatedBasicBlocks.end()) {
				for (BasicBlock::iterator instrItr = originalBB->begin(); instrItr != originalBB->end(); instrItr++) {
					Instruction* instr = instrItr;
					//Check the uses in BasicBlocks that are predecessors and use allocInstr
					list<BasicBlock*> visitedBasicBlocks;
					if (isa<StoreInst>(instr) && ((StoreInst*) instr)->getOperand(1) == useInstr && (pathExistsInCFG(useInstr->getParent(), originalBB, visitedBasicBlocks) || useInstr->getParent() == originalBB)) {
						//Check if there is a path from the current BB to any of the accumulated bbs
						bool pathExistsToAccumulatedBB = false;
						for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
							list<BasicBlock*> visitedBasicBlocks;
							if (pathExistsInCFG(originalBB, *accumulatedBBItr, visitedBasicBlocks)) {
								pathExistsToAccumulatedBB = true;
								break;
							}
						}

						if (pathExistsToAccumulatedBB) {
							//A previous store to the same memory location exists, we need to consider the latest definition
							if (basicBlocksWithDefinitions.find(originalBB) != basicBlocksWithDefinitions.end()) {
								basicBlocksWithDefinitions.erase(originalBB);
							}
							basicBlocksWithDefinitions.insert(make_pair(originalBB, instr));
							break;
						}
					}
				}
			}
		}

		//Eliminate all transitive paths and retain the last one
		list<BasicBlock*> discardList;
		for (map<BasicBlock*, Instruction*>::iterator defBBItr = basicBlocksWithDefinitions.begin(); defBBItr != basicBlocksWithDefinitions.end(); defBBItr++) {
			BasicBlock* defBB = defBBItr->first;
			map<BasicBlock*, Instruction*>::iterator lastEntry = basicBlocksWithDefinitions.end();
			lastEntry--;
			if (defBBItr != lastEntry) {
				map<BasicBlock*, Instruction*>::iterator secDefBBItr = defBBItr;
				secDefBBItr++;
				for (; secDefBBItr != basicBlocksWithDefinitions.end(); secDefBBItr++) {
					BasicBlock* secDefBB = secDefBBItr->first;
					list<BasicBlock*> visitedBasicBlocks;
					if (pathExistsInCFG(defBB, secDefBB, visitedBasicBlocks) && find(discardList.begin(), discardList.end(), secDefBB) == discardList.end()) {
						discardList.push_back(defBB);
					}
				}
			}
		}

		for (map<BasicBlock*, Instruction*>::iterator defBBItr = basicBlocksWithDefinitions.begin(); defBBItr != basicBlocksWithDefinitions.end(); defBBItr++) {
			//If the store instr is reachable i.e., not in discard list
			if (find(discardList.begin(), discardList.end(), defBBItr->first) == discardList.end()) {
				reachingDefinitions.insert(*defBBItr);
			}
		}

		return reachingDefinitions;
	}

	HyperOpArgumentType supportedArgType(Value* argument, Module &m, Value* parentInstruction) {
		//Memory location needs to be passed as a different type since all its uses need to be replaced with a different type, but it won't see realization in metadata
		if ((parentInstruction!=NULL && isa<StoreInst>(parentInstruction) && ((StoreInst*) parentInstruction)->getOperand(1) == argument)|| argument->getType()->isPointerTy()) {
			return ADDRESS;
		}
		if (isa<GetElementPtrInst>(argument)) {
			for (Module::global_iterator globalVarItr = m.global_begin(); globalVarItr != m.global_end(); globalVarItr++) {
				if (((GetElementPtrInst*) argument)->getPointerOperand() == globalVarItr) {
					return GLOBAL_REFERENCE;
				}
			}
		}

		return SCALAR;
	}

	unsigned distanceToExitBlock(BasicBlock* basicBlock, list<BasicBlock*> visitedBlocks, DominatorTree & tree, map<BasicBlock*, int>* distanceMap) {
		//Merge return assumed here
		unsigned depthOfSuccessor = 0;
		bool first = true;
		list<BasicBlock*> backedgePreds;
		visitedBlocks.push_back(basicBlock);
		if (!isa<ReturnInst>(basicBlock->getTerminator())) {
			for (succ_iterator successorItr = succ_begin(basicBlock); successorItr != succ_end(basicBlock); successorItr++) {
				BasicBlock* successor = *successorItr;
				BasicBlock* successorDom = 0;
				bool isBackEdge = false;
				BasicBlock* tempSuccessor = basicBlock;
				while (true) {
					if (tree.getNode(tempSuccessor)->getIDom() == NULL) {
						break;
					}
					successorDom = tree.getNode(tempSuccessor)->getIDom()->getBlock();
					if (successorDom == successor) {
						isBackEdge = true;
						break;
					}
					tempSuccessor = successorDom;
				}

				if (isBackEdge) {
					backedgePreds.push_back(successor);
				}
			}

			for (succ_iterator succBBItr = succ_begin(basicBlock); succBBItr != succ_end(basicBlock); succBBItr++) {
				BasicBlock* succBB = *succBBItr;
				if (find(visitedBlocks.begin(), visitedBlocks.end(), succBB) == visitedBlocks.end() && find(backedgePreds.begin(), backedgePreds.end(), succBB) == backedgePreds.end()) {
					unsigned tempDepth;
					if (distanceMap->find(succBB) == distanceMap->end()) {
						tempDepth = distanceToExitBlock(succBB, visitedBlocks, tree, distanceMap);
						distanceMap->insert(make_pair(succBB, tempDepth));
					} else {
						tempDepth = distanceMap->find(succBB)->second;
					}
					if (first || tempDepth > depthOfSuccessor) {
						depthOfSuccessor = tempDepth;
						first = false;
					}
				}
			}
		}
		return depthOfSuccessor + 1;
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
				if (!callSiteMatch) {
					continue;
				}
				list<CallInst*>::iterator callSiteItr = callSite.begin();
				for (list<CallInst*>::iterator createHopCallSiteItr = createdHopCallSite.begin(); createHopCallSiteItr != createdHopCallSite.end() && callSiteItr != callSite.end(); createHopCallSiteItr++, callSiteItr++) {
					if (*createHopCallSiteItr != *callSiteItr) {
						callSiteMatch = false;
						break;
					}
				}

				if (!callSiteMatch) {
					continue;
				}
				Function* createdFunctionOfCallSite = createdHopItr->first;
				if (originalToClonedInstructionMap[createdFunctionOfCallSite].find((Instruction*) argument) != originalToClonedInstructionMap[createdFunctionOfCallSite].end()) {
					return originalToClonedInstructionMap[createdFunctionOfCallSite].find((Instruction*) argument)->second;
				}
			}
		}
		if (!callSite.empty()) {
			CallInst* funcCall = callSite.back();
			callSite.pop_back();
			unsigned positionOfFormalArg = 0;
			Function* originalFunction = funcCall->getCalledFunction();
			for (Function::arg_iterator originalArgItr = originalFunction->arg_begin(); originalArgItr != originalFunction->arg_end(); originalArgItr++, positionOfFormalArg++) {
				if (originalArgItr == argument) {
					break;
				}
			}
			Value* argOperand = funcCall->getArgOperand(positionOfFormalArg);
			return getClonedArgument(argOperand, callSite, createdHyperOpAndCallSite, originalToClonedInstructionMap);
		}
		return 0;
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

//	list<list<pair<Function*, CallInst*> > > getCyclesContainingHyperOpInstance(CallInst * callInst, list<list<pair<Function*, CallInst*> > > cyclesInCallGraph) {
//		errs() << "cycles containing call:";
//		callInst->dump();
//		list<list<pair<Function*, CallInst*> > > cyclesContainingCall;
//		for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
//			list<pair<Function*, CallInst*> > cycle = *cycleItr;
//			bool callInCycle = false;
//			for (list<pair<Function*, CallInst*> >::iterator funcCallItr = cycle.begin(); funcCallItr != cycle.end(); funcCallItr++) {
//				if (funcCallItr->second == callInst) {
//					callInCycle = true;
//					break;
//				}
//			}
//
//			if (callInCycle) {
//				cyclesContainingCall.push_back(cycle);
//			}
//		}
//		return cyclesContainingCall;
//	}

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

	list<unsigned> getHyperOpInstanceTag(list<CallInst*> callSite, Function* newFunction, map<Function*, list<CallInst*> > createdHyperOpAndCallSite, map<Function*, list<unsigned> > createdHyperOpAndUniqueId, list<BasicBlock*> accumulatedBasiBlocks,
			map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> > createdHyperOpsAndAccumulatedBB) {
		list<unsigned> largestTag;
		for (map<Function*, list<CallInst*> >::iterator createdHopItr = createdHyperOpAndCallSite.begin(); createdHopItr != createdHyperOpAndCallSite.end(); createdHopItr++) {
			list<CallInst*> callSiteOfCreatedFunc = createdHopItr->second;
			if (!callSiteOfCreatedFunc.empty() && callSiteOfCreatedFunc.back()->getCalledFunction() == callSite.back()->getCalledFunction() && callSite.size() == callSiteOfCreatedFunc.size()) {
				//Check if the prefix of the callsites match
				bool callSitesMatch = true;
				list<CallInst*>::iterator newFunctionCallSiteItr = callSite.begin();
				for (list<CallInst*>::iterator callSiteItr = callSiteOfCreatedFunc.begin(); callSiteItr != callSiteOfCreatedFunc.end() && newFunctionCallSiteItr != callSite.end(); callSiteItr++, newFunctionCallSiteItr++) {
					if (*callSiteItr != callSiteOfCreatedFunc.back() && *newFunctionCallSiteItr != callSite.back() && *callSiteItr != *newFunctionCallSiteItr) {
						callSitesMatch = false;
						break;
					}
				}

				if (callSitesMatch && createdHyperOpAndUniqueId.find(createdHopItr->first) != createdHyperOpAndUniqueId.end()) {
					//Check if the new HyperOp is an instance of the previously created one
					list<BasicBlock*> createdHopAccumulatedBB = createdHyperOpsAndAccumulatedBB[createdHopItr->first].first;
					list<BasicBlock*> unionOfLists;
					std::set_union(createdHopAccumulatedBB.begin(), createdHopAccumulatedBB.end(), accumulatedBasiBlocks.begin(), accumulatedBasiBlocks.end(), std::back_inserter(unionOfLists));
					if (unionOfLists.size() == createdHopAccumulatedBB.size() && unionOfLists.size() == accumulatedBasiBlocks.size()) {
						//Get the id of the previously added function
						list<unsigned> tempTag = createdHyperOpAndUniqueId[createdHopItr->first];
						if (largestTag.empty() || largestTag.back() < tempTag.back()) {
							largestTag = tempTag;
						}
					}
				}
			}
		}
		if (largestTag.empty()) {
			largestTag.push_back(0);
		} else {
			//Increment the last entry in the tag
			unsigned lastEntry = largestTag.back();
			largestTag.pop_back();
			lastEntry++;
			largestTag.push_back(lastEntry);
		}
		return largestTag;
	}

	virtual bool runOnModule(Module &M) {
		LLVMContext & ctxt = M.getContext();
		//Top level annotation corresponding to all annotations REDEFINE
		NamedMDNode * redefineAnnotationsNode = M.getOrInsertNamedMetadata(REDEFINE_ANNOTATIONS);

		map<Function*, list<pair<list<BasicBlock*>, HyperOpArgumentList> > > originalFunctionToHyperOpBBListMap;

		//Contains all created HyperOp functions and the basic blocks they contain in the original module and HyperOp arguments
		map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> > createdHyperOpAndOriginalBasicBlockAndArgMap;
		map<Function*, list<CallInst*> > createdHyperOpAndCallSite;
		map<Function*, list<unsigned> > createdHyperOpAndUniqueId;
		map<Function*, Value*> createdHyperOpAndReturnValue;
		map<Function*, map<Instruction*, vector<pair<BasicBlock*, int> > > > createdHyperOpAndConditionalBranchSources;
		map<Function*, map<Instruction*, vector<pair<BasicBlock*, int> > > > createdHyperOpAndUnconditionalBranchSources;
		map<Function*, list<Instruction*> > createdHyperOpAndReachingDefSources;
		map<Function*, HYPEROP_TYPE> createdHyperOpAndType;
		list<BasicBlock*> originalParallelLatchBB;
		list<BasicBlock*> originalLatchBB;
		list<BasicBlock*> originalHeaderBB;
		list<BasicBlock*> originalExitBB;

		//Cloned instructions mapped to their original instruction for each created function
		map<Function*, map<Instruction*, Instruction*> > functionOriginalToClonedInstructionMap;
		map<Function*, map<BasicBlock*, BasicBlock*> > functionOriginalToClonedBBMap;

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
			//Replace immediate operands to a function call with arguments stored in a memory location
			for (Function::iterator bbItr = function->begin(); bbItr != function->end(); bbItr++) {
				for (BasicBlock::iterator instItr = bbItr->begin(); instItr != bbItr->end(); instItr++) {
					Instruction* inst = &*instItr;
					if (isa<CallInst>(inst)) {
						CallInst* callInst = (CallInst*) inst;
						calledFunctions.push_back(make_pair(callInst->getCalledFunction(), callInst));
						if (!callInst->getCalledFunction()->isIntrinsic()) {
							for (unsigned i = 0; i < callInst->getNumArgOperands(); i++) {
								//TODO what about globals?
								if (isa<Constant>(callInst->getArgOperand(i))) {
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

		//Sanity checks on input file
		assert(mainFunction != 0 && "REDEFINE kernel's entry point i.e., redefine_start function missing, aborting\n");
		assert(mainFunction->getArgumentList().empty() && "REDEFINE kernel's entry point i.e., redefine_start function takes arguments, aborting\n");
		assert((mainFunction->getReturnType()->getTypeID() == Type::VoidTyID) && "REDEFINE kernel's entry point i.e., redefine_start function returning a non-void value, aborting\n");

		//TODO replace the following with callgraph analysis
		list<pair<Function*, CallInst*> > traversedFunctions;
		CallInst* callInstToMain = 0;
		traversedFunctions.push_back(make_pair(mainFunction, callInstToMain));
		list<list<pair<Function*, CallInst*> > > cyclesInCallGraph = getCyclesInCallGraph(traversedFunctions, calledFunctionMap);
		DEBUG(dbgs() << "Found cycles?" << cyclesInCallGraph.size() << "\n");
		for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
			DEBUG(dbgs() << "cycle of size " << cycleItr->size() << ":");
			for (list<pair<Function*, CallInst*> >::iterator funcItr = cycleItr->begin(); funcItr != cycleItr->end(); funcItr++) {
				errs() << funcItr->first->getName() << "(";
				funcItr->second->dump();
				DEBUG(dbgs() << ")\n");
			}
			DEBUG(dbgs() << "\n");
		}

		//Inline calls that are not in cycles
		list<CallInst*> callsToInline;
		for (auto func = M.begin(); func != M.end(); func++) {
			for (auto bb = func->begin(); bb != func->end(); bb++) {
				for (auto instr = bb->begin(); instr != bb->end(); instr++) {
					Instruction* instruction = instr;
					if (isa<CallInst>(instruction)) {
						bool callInCycle = false;
						for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
							for (list<pair<Function*, CallInst*> >::iterator funcItr = cycleItr->begin(); funcItr != cycleItr->end(); funcItr++) {
								if (funcItr->second == instruction) {
									callInCycle = true;
									break;
								}
							}
						}
						if (!callInCycle) {
							callsToInline.push_back((CallInst*) instruction);
						}
					}
				}
			}
		}

		const char* isHyperOpInline = std::getenv("HYPEROP_INLINE");
		/*if (INLINE_FUNCTION_CALLS) {*/
		if (isHyperOpInline != NULL && strcmp(isHyperOpInline, "true") == 0) {
			for (CallInst* callInst : callsToInline) {
				DEBUG(dbgs() << "inling function call:" << callInst);
				InlineFunctionInfo info;
				InlineFunction(callInst, info);
			}
		}

		//Processing loops for parallelism detection
		list<pair<list<BasicBlock*>, LoopIV*> > originalParallelLoopBB;
		list<pair<list<BasicBlock*>, LoopIV*> > originalSerialLoopBB;
		//List of all functions created for parallel loops
		list<Function*> parallelLoopFunctionList;
		list<LoopInfo*> loopCache;
		for (Module::iterator func = M.begin(); func != M.end(); func++) {
			DEBUG(dbgs() << "Finding dependences of function:" << func->getName() << "\n");
			if (!func->isIntrinsic()) {
				DependenceAnalysis& depAnalysis = getAnalysis<DependenceAnalysis>(*func);
				LoopInfo& LI = getAnalysis<LoopInfo>(*func);
				loopCache.push_back(&LI);

				//Lets check if there are loop carried dependences
				int count = 0;
				for (LoopInfo::iterator loopItr = LI.begin(); loopItr != LI.end(); loopItr++, count++) {
				}
				int loopCountSecond = 0;
				for (LoopInfo::iterator loopItr = LI.begin(); loopItr != LI.end(); loopItr++, loopCountSecond++) {
					if (loopCountSecond == count) {
						break;
					}
					Function* loopParent = (*loopItr)->getHeader()->getParent();
					if (find(parallelLoopFunctionList.begin(), parallelLoopFunctionList.end(), loopParent) != parallelLoopFunctionList.end()) {
						continue;
					}
					list<vector<int> > distanceVectorsList;
					int distanceVectorArray[10][10];
					int tempDistanceVectorArray[10][10];
					ScalarEvolution& SE = getAnalysis<ScalarEvolution>(*func);
					DominatorTree & tree = getAnalysis<DominatorTree>(*func);
					Loop* loop = *loopItr;
					unsigned maxLevels = 0;
					list<Loop*> tempLoopList;
					tempLoopList.push_back(loop);
					while (!tempLoopList.empty()) {
						Loop* front = tempLoopList.front();
						tempLoopList.pop_front();
						if (maxLevels < front->getLoopDepth()) {
							maxLevels = front->getLoopDepth();
						}
						for (auto subLoopItr : front->getSubLoopsVector()) {
							tempLoopList.push_back(subLoopItr);
						}
					}

					vector<BasicBlock*> basicBlocksList = loop->getBlocks();
					for (auto bbItr = basicBlocksList.begin(); bbItr != basicBlocksList.end(); bbItr++) {
						BasicBlock* basicBlock = *bbItr;
						for (auto SrcI = basicBlock->begin(), SrcE = basicBlock->end(); SrcI != SrcE; ++SrcI) {
							if (isa<StoreInst>(*SrcI) || isa<LoadInst>(*SrcI)) {
								for (auto DstI = basicBlock->begin(), DstE = basicBlock->end(); DstI != DstE; ++DstI) {
									if (isa<StoreInst>(*DstI) || isa<LoadInst>(*DstI)) {
										if (isa<LoadInst>(*SrcI) && isa<LoadInst>(*DstI)) {
											//Read after read maybe ignored
											continue;
										}
										errs() << "\n----\nda analyze - ";
										SrcI->print(dbgs());
										DstI->print(dbgs());
										if (Dependence *D = depAnalysis.depends(&*SrcI, &*DstI, true)) {
											if (D->isConfused()) {
												errs() << "confused dependence\n";
											}
											if (!D->isInput() && !D->isLoopIndependent()) {
												DEBUG(dbgs() << "Dependence between ");
												SrcI->print(dbgs());
												DEBUG(dbgs() << " and ");
												DstI->print(dbgs());
												DEBUG(dbgs() << "\n");
												D->dump(dbgs());
												vector<int> distanceVector;
												for (unsigned i = 1; i <= D->getLevels(); i++) {
													unsigned Direction = D->getDirection(i);
													if (Direction == Dependence::DVEntry::ALL) {
														distanceVector.push_back(2);
													} else {
														if (Direction & Dependence::DVEntry::LT) {
															distanceVector.push_back(1);
														} else if (Direction & Dependence::DVEntry::EQ) {
															distanceVector.push_back(0);
														} else if (Direction & Dependence::DVEntry::GT) {
															distanceVector.push_back(-1);
														} else {
															distanceVector.push_back(0);
														}
													}
//													if (D->getDistance(i) != NULL && isa<SCEVConstant>(D->getDistance(i))) {
//														((SCEVConstant*) (D->getDistance(i)))->getValue()->getZExtValue());
//													}
												}
												if (distanceVector.size() == D->getLevels()) {
													//Check if the distance vector has a leading negative value
													bool leadingNegativeValue = false;
													for (auto distanceVectorItr = distanceVector.begin(); distanceVectorItr != distanceVector.end(); distanceVectorItr++) {
														if ((*distanceVectorItr) > 0) {
															break;
														}
														if ((*distanceVectorItr) < 0) {
															leadingNegativeValue = true;
															break;
														}
													}

													if (!leadingNegativeValue) {
														distanceVectorsList.push_back(distanceVector);
													}
												}
											}
//											delete D;
										}
									}
								}
							}
						}
					}

					unsigned i = 0;
					unsigned numVectors = distanceVectorsList.size();
					for (auto vectorItr = distanceVectorsList.begin(); vectorItr != distanceVectorsList.end(); vectorItr++, i++) {
						vector<int> vector = *vectorItr;
						//Get the distance vectors in array form
						for (unsigned j = 0; j < maxLevels - vector.size(); j++) {
							distanceVectorArray[i][j] = 0;
						}
						int k = maxLevels - vector.size();
						for (unsigned j = 0; j < vector.size(); j++) {
							distanceVectorArray[i][j + k] = vector[j];
						}
					}

					//Shuffle the dependence vectors such that the levels carrying most dependences may be moved to the outer levels
					//Identify the positive only columns and move them in the order of the number of positive entries
					list<unsigned> positiveColumnsOrder;
					list<unsigned> eliminatedRows;
					while (true) {
						bool firstIteration = true;
						list<unsigned> maxPositiveEntryRows;
						unsigned maxColumn;
						for (i = 0; i < maxLevels; i++) {
							list<unsigned> positiveEntryRows;
							bool hasUneleminatedNegativeEntries = false;
							for (unsigned j = 0; j < numVectors; j++) {
								if (distanceVectorArray[j][i] > 0 && (eliminatedRows.empty() || find(eliminatedRows.begin(), eliminatedRows.end(), j) == eliminatedRows.end())) {
									positiveEntryRows.push_back(j);
								} else if (distanceVectorArray[j][i] < 0 && (eliminatedRows.empty() || find(eliminatedRows.begin(), eliminatedRows.end(), j) == eliminatedRows.end())) {
									hasUneleminatedNegativeEntries = true;
									break;
								}
							}

							if (!hasUneleminatedNegativeEntries) {
								if (firstIteration || maxPositiveEntryRows.size() < positiveEntryRows.size()) {
									firstIteration = false;
									maxPositiveEntryRows = positiveEntryRows;
									maxColumn = i;
								}
							}
						}

						if (maxPositiveEntryRows.empty()) {
							break;
						} else {
							positiveColumnsOrder.push_back(maxColumn);
							std::copy(maxPositiveEntryRows.begin(), maxPositiveEntryRows.end(), std::back_inserter(eliminatedRows));
						}
					}

					for (unsigned i = 0; i < maxLevels; i++) {
						if (find(positiveColumnsOrder.begin(), positiveColumnsOrder.end(), i) == positiveColumnsOrder.end()) {
							positiveColumnsOrder.push_back(i);
						}
					}

					//Find the swaps required to shuffle the vectors into their correct places
					vector<unsigned> originalVectorFinalLocation;
					for (i = 0; i < maxLevels; i++) {
						unsigned finalVectorPosition = 0;
						for (auto orderItr = positiveColumnsOrder.begin(); orderItr != positiveColumnsOrder.end(); orderItr++, finalVectorPosition++) {
							if (*orderItr == i) {
								break;
							}
						}
						originalVectorFinalLocation.push_back(finalVectorPosition);
					}

					//Record swaps
					list<pair<unsigned, unsigned> > vectorSwapList;
					if (!originalVectorFinalLocation.empty()) {
						for (unsigned i = 0; i < originalVectorFinalLocation.size() - 1; i++) {
							for (unsigned j = 0; j < originalVectorFinalLocation.size() - 1; j++) {
								if (originalVectorFinalLocation[j] > originalVectorFinalLocation[j + 1]) {
									int temp = originalVectorFinalLocation[j];
									originalVectorFinalLocation[j] = originalVectorFinalLocation[j + 1];
									originalVectorFinalLocation[j + 1] = temp;
									vectorSwapList.push_back(make_pair(j, j + 1));
								}
							}
						}

						//Perform swaps
						for (auto swapItr = vectorSwapList.begin(); swapItr != vectorSwapList.end(); swapItr++) {
							//depth of loop
							map<unsigned, list<Loop*> > nestedLoopDepth;
							list<Loop*> loopQueue;
							loopQueue.push_back(loop);
							while (!loopQueue.empty()) {
								Loop* currentLoop = loopQueue.front();
								unsigned currentLoopDepth = currentLoop->getLoopDepth();
								loopQueue.pop_front();
								list<Loop*> loopsAtDepth;
								if (nestedLoopDepth.find(currentLoopDepth) != nestedLoopDepth.end()) {
									loopsAtDepth = nestedLoopDepth[currentLoopDepth];
									nestedLoopDepth.erase(currentLoopDepth);
								}
								loopsAtDepth.push_back(currentLoop);
								nestedLoopDepth.insert(make_pair(currentLoopDepth - 1, loopsAtDepth));

								for (auto subLoopItr = currentLoop->getSubLoops().begin(); subLoopItr != currentLoop->getSubLoops().end(); subLoopItr++) {
									loopQueue.push_back(*subLoopItr);
								}
							}

							unsigned sourceRow = swapItr->first;
							unsigned targetRow = swapItr->second;
							Loop* sourceLoop = nestedLoopDepth[sourceRow].front();
							Loop* targetLoop = nestedLoopDepth[targetRow].front();
							//TODO Uncomment this after making sure interchange works for sure
//							LoopInterchangeTransform* interchange = new LoopInterchangeTransform(sourceLoop, targetLoop, &SE, &LI, &tree, loop->getExitBlock(), false, this);
//							errs()<<"inner loop:";
//							targetLoop->dump();
//							errs()<<"\nhow could inner loop's latch be null?"<<(targetLoop->getLoopLatch()->getName())<<"\n";
//							errs() << "swapped loops:" << interchange->transform() << "\n";
						}
					}

					//move positive entry rows to the front of dependence array
					for (auto columnItr = positiveColumnsOrder.begin(); columnItr != positiveColumnsOrder.end(); columnItr++) {
						for (i = 0; i < numVectors; i++) {
							tempDistanceVectorArray[i][*columnItr] = distanceVectorArray[i][*columnItr];
						}
					}

					//Find the column corresponding to a level with maximum number of zeros which has not been eliminated yet
					eliminatedRows.clear();
					enum ExecutionMode {
						SERIAL, PARALLEL
					};
					map<unsigned, ExecutionMode> executionOrder;
					//Traversing each column
					for (i = 0; i < maxLevels; i++) {
						list<unsigned> positiveEntryRows;
						for (unsigned j = 0; j < numVectors; j++) {
//							if (distanceVectorArray[j][i] > 0 && (eliminatedRows.empty() || find(eliminatedRows.begin(), eliminatedRows.end(), j) == eliminatedRows.end())) {
							if (tempDistanceVectorArray[j][i] > 0 && (eliminatedRows.empty() || find(eliminatedRows.begin(), eliminatedRows.end(), j) == eliminatedRows.end())) {
								positiveEntryRows.push_back(j);
							}
						}

						if (positiveEntryRows.empty()) {
							executionOrder.insert(make_pair(i, PARALLEL));
						} else {
							std::copy(positiveEntryRows.begin(), positiveEntryRows.end(), std::back_inserter(eliminatedRows));
							executionOrder.insert(make_pair(i, SERIAL));
						}
					}

					//depth of loop
					map<unsigned, list<Loop*> > nestedLoopDepth;
					list<Loop*> loopQueue;
					loopQueue.push_back(loop);
					while (!loopQueue.empty()) {
						Loop* currentLoop = loopQueue.front();
						unsigned currentLoopDepth = currentLoop->getLoopDepth();
						loopQueue.pop_front();
						list<Loop*> loopsAtDepth;
						if (nestedLoopDepth.find(currentLoopDepth) != nestedLoopDepth.end()) {
							loopsAtDepth = nestedLoopDepth[currentLoopDepth];
							nestedLoopDepth.erase(currentLoopDepth);
						}
						loopsAtDepth.push_back(currentLoop);
						nestedLoopDepth.insert(make_pair(currentLoopDepth, loopsAtDepth));
						for (auto subLoopItr = currentLoop->getSubLoops().begin(); subLoopItr != currentLoop->getSubLoops().end(); subLoopItr++) {
							loopQueue.push_back(*subLoopItr);
						}
					}

					for (i = 0; i < maxLevels; i++) {
						if (nestedLoopDepth.find(i + 1) == nestedLoopDepth.end()) {
							continue;
						}
						list<Loop*> loopsAtDepth = nestedLoopDepth[i + 1];
						Loop* currentLoop = loopsAtDepth.front();
						currentLoop->dump();
						LoopIV* loopIVObject = NULL;
						//Assuming no overflows in integer, because there's an add 1 in the method invoked in the next line for god knows what reason
						PHINode* inductionVariable = currentLoop->getCanonicalInductionVariable();
						unsigned upperBound = SE.getSmallConstantTripCount(currentLoop, currentLoop->getExitBlock());
						unsigned lowerBound = 0;
						Value* stride;
						unsigned strideUpdateOperation;
						const char* strideUpdateOperationName;
						LoopIV::Type type;
						//Compute stride over the induction variable
						map<BasicBlock*, Value*> entryBBToInductionVarPHI;
						for (auto phiOperandItr = 0; phiOperandItr != inductionVariable->getNumIncomingValues(); phiOperandItr++) {
							BasicBlock* incomingBlock = inductionVariable->getIncomingBlock(phiOperandItr);
							if (find(currentLoop->getBlocks().begin(), currentLoop->getBlocks().end(), incomingBlock) != currentLoop->getBlocks().end()) {
								entryBBToInductionVarPHI.insert(make_pair(incomingBlock, inductionVariable->getIncomingValue(phiOperandItr)));
							}
						}
						assert(entryBBToInductionVarPHI.size() == 1 && "Multiple exit block loop unsupported");
						//We can't support multiple strides for parallelism or support any operation other than binary or shift operations
						for (auto entryBBItr = entryBBToInductionVarPHI.begin(); entryBBItr != entryBBToInductionVarPHI.end(); entryBBItr++) {
							BasicBlock* incomingBlock = entryBBItr->first;
							Value* incomingValue = entryBBItr->second;
							//Check what the increment on incomingValue is

							for (unsigned i = 0; i < ((Instruction*) incomingValue)->getNumOperands(); i++) {
								Value* operand = ((Instruction*) incomingValue)->getOperand(i);
								if (operand != inductionVariable && (((Instruction*) incomingValue)->isBinaryOp() || ((Instruction*) incomingValue)->isShift())) {
									strideUpdateOperation = ((Instruction*) incomingValue)->getOpcode();
									strideUpdateOperationName = ((Instruction*) incomingValue)->getOpcodeName();
									stride = operand;
									break;
								}
							}

						}
						bool useTripCount = true;
						if (isa<SCEVConstant>(SE.getBackedgeTakenCount(currentLoop))) {
							ConstantInt* Result = ((SCEVConstant*) SE.getBackedgeTakenCount(currentLoop))->getValue();
							if (!Result || Result->getValue().getActiveBits() > 32 || Result->getValue().getActiveBits() == 0) {
								useTripCount = false;
							}
						} else {
							useTripCount = false;
						}
						Instruction* loopBound = NULL;
						Value* loopBoundValue = NULL;
						if (!useTripCount) {
							for (auto instItr = currentLoop->getHeader()->begin(); instItr != currentLoop->getHeader()->end(); instItr++) {
								if (!isa<CmpInst>(instItr)) {
									continue;
								}
								bool isBoundCompareInst = false;
								///Check if the use of this comparison instruction translates to a backedge in the loop
								for (auto useItr = instItr->use_begin(); useItr != instItr->use_end(); useItr++) {
									if (isa<BranchInst>(*useItr)) {
										for (unsigned i = 0; i < ((BranchInst*) *useItr)->getNumSuccessors(); i++) {
											if (((BranchInst*) *useItr)->getSuccessor(i) == currentLoop->getExitBlock()) {
												isBoundCompareInst = true;
												break;
											}
										}
										if (isBoundCompareInst) {
											break;
										}
									}
								}

								if (isBoundCompareInst) {
									//Find the operand being compared with
									for (int i = 0; i < instItr->getNumOperands(); i++) {
										Value* operand = instItr->getOperand(i);
										while (true) {
											if (isa<CastInst>(operand)) {
												operand = ((CastInst*) operand)->getOperand(0);
											} else {
												break;
											}
										}

										if (operand != inductionVariable) {
											loopBound = instItr;
											loopBoundValue = operand;
											break;
										}
									}
									break;
								}
							}

							assert(loopBoundValue!=NULL&&"Loop bound can't be null\n");
							//Check if loopbound is undefined recursively
							bool undefVal = false;
							list<Value*> traverseOperands;
							traverseOperands.push_back(loopBoundValue);
							while (!traverseOperands.empty()) {
								Value* val = traverseOperands.front();
								traverseOperands.pop_front();
								if (val->getValueID() == Value::UndefValueVal) {
									undefVal = true;
									break;
								}

								if (isa<Instruction>(val)) {
									for (auto operandCount = 0; operandCount != ((Instruction*) val)->getNumOperands(); operandCount++) {
										traverseOperands.push_back(((Instruction*) val)->getOperand(operandCount));
									}
								}
							}
							assert(!undefVal && "Loop bound wasn't initialized to a valid value, throwing an error here is against ansi c standards, but it's incorrect logically anyway\n");
							type = LoopIV::VARIABLE;
						} else {
							type = LoopIV::CONSTANT;
						}
						loopIVObject = new LoopIV(stride, inductionVariable, strideUpdateOperation, strideUpdateOperationName);
						loopIVObject->setConstantLowerBound(lowerBound);
						if (type == LoopIV::CONSTANT) {
							loopIVObject->setConstantUpperBound(upperBound - 1);
						} else {
							loopIVObject->setVariableUpperBound(loopBoundValue);
						}
						list<BasicBlock*> loopBBList;
						std::copy(currentLoop->getBlocks().begin(), currentLoop->getBlocks().end(), std::back_inserter(loopBBList));
						bool nestedParallelLoop = false;
						if (executionOrder[i] == SERIAL) {
							for (unsigned j = i + 1; j < maxLevels; j++) {
								if (executionOrder[j] == PARALLEL) {
									nestedParallelLoop = true;
									//remove those basic blocks in the inner parallel loops
									list<Loop*> innerLoops = nestedLoopDepth[j + 1];
									for (auto loopItr = innerLoops.begin(); loopItr != innerLoops.end(); loopItr++) {
										for (auto bbItr : (*loopItr)->getBlocks()) {
											loopBBList.remove(bbItr);
										}
									}
									loopBBList.push_back(currentLoop->getExitBlock());
									originalSerialLoopBB.push_back(make_pair(loopBBList, loopIVObject));
									break;
								}
							}
						}

						if (executionOrder[i] == SERIAL && !nestedParallelLoop) {
							continue;
						}
						//Find the bound of the loop at that level
						else if (executionOrder[i] == PARALLEL) {
							originalParallelLoopBB.push_back(make_pair(loopBBList, loopIVObject));
						}

						vector<BasicBlock*> containedBlocks;
						bool containsLoop = false;

						list<BasicBlock*> prevSuccessorsOfHeader;
						for (unsigned i = 0; i < currentLoop->getHeader()->getTerminator()->getNumSuccessors(); i++) {
							prevSuccessorsOfHeader.push_back(currentLoop->getHeader()->getTerminator()->getSuccessor(i));
						}
						if (nestedLoopDepth.find(i + 2) != nestedLoopDepth.end()) {
							Loop* innerLoop = nestedLoopDepth[i + 2].front();
							std::copy(innerLoop->getBlocks().begin(), innerLoop->getBlocks().end(), std::back_inserter(containedBlocks));
							containsLoop = true;
						} else {
							for (auto bbItr : currentLoop->getBlocksVector()) {
								errs() << bbItr->getName() << ":" << (bbItr == currentLoop->getExitBlock() && bbItr == currentLoop->getHeader()) << "\n";
								if (bbItr != currentLoop->getLoopLatch() && bbItr != currentLoop->getExitBlock() && bbItr != currentLoop->getHeader()) {
									containedBlocks.push_back(bbItr);
								}
							}

							//TODO HACK HACK HACK!!!!
							if (containedBlocks.empty()) {
								containedBlocks.push_back(currentLoop->getLoopLatch());
							}
						}
						CodeExtractor* extractor = new CodeExtractor(containedBlocks);
						Function* innerLoopFunction = extractor->extractCodeRegion();
						BasicBlock* newSucc;
						for (unsigned i = 0; i < currentLoop->getHeader()->getTerminator()->getNumSuccessors(); i++) {
							if (find(prevSuccessorsOfHeader.begin(), prevSuccessorsOfHeader.end(), currentLoop->getHeader()->getTerminator()->getSuccessor(i)) == prevSuccessorsOfHeader.end()) {
								newSucc = currentLoop->getHeader()->getTerminator()->getSuccessor(i);
								if (isa<CallInst>(newSucc->front()) && ((CallInst*) &newSucc->front())->getCalledFunction()->getName().compare(innerLoopFunction->getName()) == 0) {
									break;
								}
							}
						}

						for (auto bbListItr = originalSerialLoopBB.begin(); bbListItr != originalSerialLoopBB.end(); bbListItr++) {
							if (find(bbListItr->first.begin(), bbListItr->first.end(), currentLoop->getHeader()) != bbListItr->first.end()) {
								list<BasicBlock*> newList;
								LoopIV* oldIV = bbListItr->second;
								std::copy(bbListItr->first.begin(), bbListItr->first.end(), std::back_inserter(newList));
								newList.push_back(newSucc);
								originalSerialLoopBB.erase(bbListItr);
								originalSerialLoopBB.push_back(make_pair(newList, oldIV));
								break;
							}
						}
						for (auto bbListItr = originalParallelLoopBB.begin(); bbListItr != originalParallelLoopBB.end(); bbListItr++) {
							if (find(bbListItr->first.begin(), bbListItr->first.end(), currentLoop->getHeader()) != bbListItr->first.end()) {
								list<BasicBlock*> newList;
								LoopIV* oldIV = bbListItr->second;
								std::copy(bbListItr->first.begin(), bbListItr->first.end(), std::back_inserter(newList));
								newList.push_back(newSucc);
								originalParallelLoopBB.erase(bbListItr);
								originalParallelLoopBB.push_back(make_pair(newList, oldIV));
								break;
							}
						}

						if (executionOrder[i] == PARALLEL) {
							parallelLoopFunctionList.push_back(innerLoopFunction);
						}
						functionList.push_back(innerLoopFunction);
						LoopInfo& innerLoopInfo = getAnalysis<LoopInfo>(*innerLoopFunction);
						list<BasicBlock*> currentBBList;
						for (auto bbItr = innerLoopFunction->getBasicBlockList().begin(); bbItr != innerLoopFunction->getBasicBlockList().end(); bbItr++) {
							currentBBList.push_back(bbItr);
						}

						if (containsLoop) {
							for (auto innerLoopItr = innerLoopInfo.begin(); innerLoopItr != innerLoopInfo.end(); innerLoopItr++) {
								Loop* innerLoop = *innerLoopItr;
								map<unsigned, list<Loop*> > tempNestedLoopDepth;
								list<Loop*> loopQueue;
								loopQueue.push_back(innerLoop);
								while (!loopQueue.empty()) {
									Loop* currentLoop = loopQueue.front();
									loopQueue.pop_front();
									unsigned currentLoopDepth = currentLoop->getLoopDepth();
									list<Loop*> loopsAtDepth;
									if (tempNestedLoopDepth.find(currentLoopDepth) != tempNestedLoopDepth.end()) {
										loopsAtDepth = tempNestedLoopDepth[currentLoopDepth];
										tempNestedLoopDepth.erase(currentLoopDepth);
									}
									loopsAtDepth.push_back(currentLoop);
									tempNestedLoopDepth.insert(make_pair(currentLoopDepth, loopsAtDepth));

									for (auto subLoopItr = currentLoop->getSubLoops().begin(); subLoopItr != currentLoop->getSubLoops().end(); subLoopItr++) {
										loopQueue.push_back(*subLoopItr);
									}
								}

								//Replace whatever is contained in the nestedLoopDepth map with the new loops
								for (auto nestedLoopItr = nestedLoopDepth.begin(); nestedLoopItr != nestedLoopDepth.end(); nestedLoopItr++) {
									unsigned key = nestedLoopItr->first;
									if (key >= i + 2) {
										nestedLoopDepth[key] = tempNestedLoopDepth[key - i - 1];
									}
								}
							}
						}
					}
				}
			}
		}

		while (!functionList.empty()) {
			Function* function = functionList.front();
			functionList.pop_front();
			StringRef name = function->getName();
			errs() << "\n-----------\nPartitioning function:" << name << "\n";

			list<pair<list<BasicBlock*>, HyperOpArgumentList> > hyperOpBBAndArgs;
			list<BasicBlock*> accumulatedBasicBlocks;
			HyperOpArgumentList hyperOpArguments;
			bool endOfHyperOp = false;
			list<BasicBlock*> traversedBasicBlocks;
			list<BasicBlock*> bbTraverser;

			bbTraverser.push_back(function->begin());
			bool functionHasCalls = false;

			while (!bbTraverser.empty()) {
				BasicBlock* bbItr = bbTraverser.front();
				bbTraverser.pop_front();
				if(bbItr == &function->front()){
					/* Add all args to the function as arguments to the current hyperop */
					for(auto argItr = function->arg_begin(); argItr!=function->arg_end(); argItr++){
						list<Value*> tempArgs;
						tempArgs.push_back(argItr);
						hyperOpArguments.push_back(make_pair(tempArgs, supportedArgType(argItr, M, argItr)));
					}
				}
				BasicBlock* originalBBItr = bbItr;
				//Check if basic block is the header or exit block of a loop
				bool parallelLatchBlock = false;
				list<Loop*> loopsOfFunction;
				LoopIV* loopIV;
				LoopInfo& loopInfo = getAnalysis<LoopInfo>(*function);
				for (auto loopItr = loopInfo.begin(); loopItr != loopInfo.end(); loopItr++) {
					list<Loop*> loopList;
					loopList.push_back(*loopItr);
					while (!loopList.empty()) {
						Loop* currentLoop = loopList.front();
						loopsOfFunction.push_back(currentLoop);
						loopList.pop_front();
						for (auto subLoopItr : currentLoop->getSubLoopsVector()) {
							loopList.push_back(subLoopItr);
						}
					}
				}
				for (auto loopItr = loopsOfFunction.begin(); loopItr != loopsOfFunction.end(); loopItr++) {
					Loop* loop = *loopItr;
					for (auto parallelLevelItr = originalParallelLoopBB.begin(); parallelLevelItr != originalParallelLoopBB.end(); parallelLevelItr++) {
						if (find(parallelLevelItr->first.begin(), parallelLevelItr->first.end(), bbItr) != parallelLevelItr->first.end()) {
							if (bbItr == loop->getLoopLatch()) {
								parallelLatchBlock = true;
								originalParallelLatchBB.push_back(bbItr);
								errs() << bbItr->getName() << ",";
								loopIV = parallelLevelItr->second;
							}
						}
					}
					if (bbItr == loop->getHeader()) {
						originalHeaderBB.push_back(bbItr);
					}
					if (bbItr == loop->getExitBlock()) {
						originalExitBB.push_back(bbItr);
					}
					if (bbItr == loop->getLoopLatch()) {
						originalLatchBB.push_back(bbItr);
					}
				}

				bool canAcquireBBItr = true;
				list<BasicBlock*> backEdgePredecessors;
				//If basic block is not the entry block
				if (bbItr != &(function->getEntryBlock())) {
					//Check if all the parent nodes of the basic block are in the same HyperOp
					//While this should in principle be done to all producers of data as well, since we are choosing one basic-block after another from a CFG to form a HyperOp, immediate predecessors should suffice
					bool noParentAcquired = true;
					int numParentsAcquired = 0;
					int numPredecessors = 0;
					//Check if dependence leads to a back-edge
					DominatorTree & dom = getAnalysis<DominatorTree>(*function);
					for (pred_iterator predecessorItr = pred_begin(bbItr); predecessorItr != pred_end(bbItr); predecessorItr++) {
						BasicBlock* predecessor = *predecessorItr;
//						errs() << "considering pred " << predecessor->getName() << " for bb " << bbItr->getName() << "\n";
						BasicBlock* predecessorDom = 0;
						bool isBackEdge = false;
						BasicBlock* tempPredecessor = predecessor;
						while (true) {
							if (dom.getNode(tempPredecessor)->getIDom() == NULL) {
								break;
							}
							predecessorDom = dom.getNode(tempPredecessor)->getIDom()->getBlock();
							if (predecessorDom == bbItr) {
								isBackEdge = true;
								break;
							}
							tempPredecessor = predecessorDom;
						}

						if (isBackEdge) {
							backEdgePredecessors.push_back(predecessor);
							continue;
						}
						if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), predecessor) != accumulatedBasicBlocks.end()) {
							noParentAcquired = false;
							numParentsAcquired++;
						}
						numPredecessors++;
					}

					bool allParentsAcquired = (numParentsAcquired == numPredecessors);
					if (!(noParentAcquired || allParentsAcquired)) {
						canAcquireBBItr = false;
					}
					//Check if the basic block's inclusion results introduces multiple entry points to the same HyperOp
					if (canAcquireBBItr && !accumulatedBasicBlocks.empty()) {
						//Find the basic blocks that jump to the basic block being acquired
						for (Function::iterator sourceBBItr = function->begin(); sourceBBItr != function->end(); sourceBBItr++) {
							for (auto successorItr = succ_begin(sourceBBItr); successorItr != succ_end(sourceBBItr); successorItr++) {
								BasicBlock* successor = *successorItr;
								if (successor == bbItr) {
									if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), sourceBBItr) == accumulatedBasicBlocks.end() && find(backEdgePredecessors.begin(), backEdgePredecessors.end(), sourceBBItr) == backEdgePredecessors.end()) {
										//Predicate to the basic block in the current set that comes from outside the basic block set accumulated for the current HyperOp
										canAcquireBBItr = false;
										break;
									}
								}
								if (!canAcquireBBItr) {
									break;
								}
							}
							if (!canAcquireBBItr) {
								break;
							}
						}

						if (canAcquireBBItr) {
							//Check if bb is the header of a serial or a parallel loop
							list<pair<list<BasicBlock*>, LoopIV*> > tempLoopBlocks;
							std::copy(originalParallelLoopBB.begin(), originalParallelLoopBB.end(), std::back_inserter(tempLoopBlocks));
							std::copy(originalSerialLoopBB.begin(), originalSerialLoopBB.end(), std::back_inserter(tempLoopBlocks));
							for (auto loopItr = tempLoopBlocks.begin(); loopItr != tempLoopBlocks.end(); loopItr++) {
								if (find(loopItr->first.begin(), loopItr->first.end(), bbItr) != loopItr->first.end() && (find(originalHeaderBB.begin(), originalHeaderBB.end(), bbItr) != originalHeaderBB.end() || find(originalExitBB.begin(), originalExitBB.end(), bbItr) != originalExitBB.end())) {
									canAcquireBBItr = false;
									break;
								}
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
								//An intrinsic function translates to an instruction in the back end, don't treat it like a function call
								//Otherwise, create a function boundary as follows
								if (!calledFunction->isIntrinsic()) {
									//call is not the first instruction in the basic block
									if (&*instItr != &bbItr->front()) {
										//Call is the not only instruction here, a separate HyperOp must be created for the call statement
										string firstBBName(NEW_NAME);
										firstBBName.append(itostr(bbIndex));
										bbIndex++;
										BasicBlock* newSucc = bbItr->splitBasicBlock(instItr, firstBBName);
										instItr = newSucc->begin();
										bbItr = newSucc;
										for (auto bbListItr = originalSerialLoopBB.begin(); bbListItr != originalSerialLoopBB.end(); bbListItr++) {
											if (find(bbListItr->first.begin(), bbListItr->first.end(), bbItr) != bbListItr->first.end()) {
												list<BasicBlock*> newList;
												LoopIV* oldIV = bbListItr->second;
												std::copy(bbListItr->first.begin(), bbListItr->first.end(), std::back_inserter(newList));
												newList.push_back(newSucc);
												originalSerialLoopBB.erase(bbListItr);
												originalSerialLoopBB.push_back(make_pair(newList, oldIV));
												break;
											}
										}
										for (auto bbListItr = originalParallelLoopBB.begin(); bbListItr != originalParallelLoopBB.end(); bbListItr++) {
											if (find(bbListItr->first.begin(), bbListItr->first.end(), bbItr) != bbListItr->first.end()) {
												list<BasicBlock*> newList;
												LoopIV* oldIV = bbListItr->second;
												std::copy(bbListItr->first.begin(), bbListItr->first.end(), std::back_inserter(newList));
												newList.push_back(newSucc);
												originalParallelLoopBB.erase(bbListItr);
												originalParallelLoopBB.push_back(make_pair(newList, oldIV));
												break;
											}
										}
									}
									//If call is not the last instruction
									if ((&*instItr != (Instruction*) bbItr->getTerminator()) && instItr->getNextNode() != (Instruction*) bbItr->getTerminator()) {
										string secondBBName(NEW_NAME);
										secondBBName.append(itostr(bbIndex));
										bbIndex++;
										BasicBlock* newSucc = bbItr->splitBasicBlock(instItr->getNextNode(), secondBBName);
										for (auto bbListItr = originalSerialLoopBB.begin(); bbListItr != originalSerialLoopBB.end(); bbListItr++) {
											if (find(bbListItr->first.begin(), bbListItr->first.end(), bbItr) != bbListItr->first.end()) {
												list<BasicBlock*> newList;
												LoopIV* oldIV = bbListItr->second;
												std::copy(bbListItr->first.begin(), bbListItr->first.end(), std::back_inserter(newList));
												newList.push_back(newSucc);
												originalSerialLoopBB.erase(bbListItr);
												originalSerialLoopBB.push_back(make_pair(newList, oldIV));
												break;
											}
										}
										for (auto bbListItr = originalParallelLoopBB.begin(); bbListItr != originalParallelLoopBB.end(); bbListItr++) {
											if (find(bbListItr->first.begin(), bbListItr->first.end(), bbItr) != bbListItr->first.end()) {
												list<BasicBlock*> newList;
												LoopIV* oldIV = bbListItr->second;
												std::copy(bbListItr->first.begin(), bbListItr->first.end(), std::back_inserter(newList));
												newList.push_back(newSucc);
												originalParallelLoopBB.erase(bbListItr);
												originalParallelLoopBB.push_back(make_pair(newList, oldIV));
												break;
											}
										}

									}
									if (&*instItr == &bbItr->front()) {
										if (accumulatedBasicBlocks.size() == 1) {
											CallInst* callInst = (CallInst*) (&*instItr);
											for (unsigned int i = 0; i < callInst->getNumArgOperands(); i++) {
												Value * argument = callInst->getArgOperand(i);
												list<Value*> argumentList;
												argumentList.push_back(argument);
												//Find out if the argument is stored into in a predecessor HyperOp
												HyperOpArgumentType argType = supportedArgType(argument, M, instItr);
												hyperOpArguments.push_back(make_pair(argumentList, argType));
											}

										} else {
											accumulatedBasicBlocks.remove(bbItr);
											bbTraverser.push_front(bbItr);
										}
									}
									endOfHyperOp = true;
									break;
								}
							}

							list<Value*> newHyperOpArguments;
							for (unsigned int i = 0; i < instItr->getNumOperands(); i++) {
								Value * argument = instItr->getOperand(i);
								if (isa<BasicBlock>(argument)) {
									continue;
								}
								//Ignore the induction variable
								if (parallelLatchBlock) {
									if (loopIV->getInductionVar() == argument) {
										continue;
									}
									PHINode* iv = loopIV->getInductionVar();
									bool ivInstrArg = false;
									for (unsigned i = 0; i < iv->getNumIncomingValues(); i++) {
										if (iv->getIncomingValue(i) == argument) {
											ivInstrArg = true;
											break;
										}
									}
									if (ivInstrArg) {
										continue;
									}
								}

								bool isPHIPartOfParallelLoop = false;
								if (isa<PHINode>(instItr)) {
									for (auto parallelLoopItr = originalParallelLoopBB.begin(); parallelLoopItr != originalParallelLoopBB.end(); parallelLoopItr++) {
										if (find(parallelLoopItr->first.begin(), parallelLoopItr->first.end(), bbItr) != parallelLoopItr->first.end()) {
											isPHIPartOfParallelLoop = true;
											break;
										}
									}
								}
								if (!argument->getType()->isLabelTy() && ((isa<Constant>(argument) && isa<PHINode>(instItr) && !isPHIPartOfParallelLoop) || (!isa<Constant>(argument) && !isPHIPartOfParallelLoop))) {
									//Find the reaching definition of the argument; alloca instruction maybe followed by store instructions to the memory location, we need to identify the set of store instructions to the memory location that reach the current use of the memory location
									if (isa<Instruction>(argument) && find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), ((Instruction*) argument)->getParent()) != accumulatedBasicBlocks.end()) {
										continue;
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
										Instruction* instruction = instItr;
										if (isa<PHINode>(instruction) && isa<Constant>(argument)) {
											//Convert constant args of phi to temporaries
											//Add a new instruction to store in a memory location in the parent hyperOp producing the data
											BasicBlock* incomingBlock = ((PHINode*) instruction)->getIncomingBlock(i);
											AllocaInst* newalloc = new AllocaInst(((Constant*) argument)->getType());
											newalloc->setAlignment(4);
											newalloc->insertBefore(incomingBlock->getFirstInsertionPt());
											StoreInst* storeInst = new StoreInst(argument, newalloc);
											storeInst->setAlignment(4);
											storeInst->insertAfter(newalloc);
											argument = newalloc;
											LoadInst* newLoadInst = new LoadInst(newalloc);
											newLoadInst->insertAfter(storeInst);
											((PHINode*) instruction)->setIncomingValue(i, newLoadInst);
											argument = newLoadInst;
										}
										newHyperOpArguments.push_back(argument);
									}
								}
							}
							unsigned numArgs = newHyperOpArguments.size();
							//Phi translates to only one argument at runtime
							if (isa<PHINode>(instItr) && !newHyperOpArguments.empty()) {
								numArgs = 1;
							}
							if (numArgs) {
								if (numArgs == newHyperOpArguments.size()) {
									for (list<Value*>::iterator newArgItr = newHyperOpArguments.begin(); newArgItr != newHyperOpArguments.end(); newArgItr++) {
										list<Value*> newArg;
										newArg.push_back(*newArgItr);
										HyperOpArgumentType type = supportedArgType(*newArgItr, M, instItr);
										hyperOpArguments.push_back(make_pair(newArg, type));
									}
								} else {
									//Phi instruction's arguments correspond to only one argument to a HyperOp
									hyperOpArguments.push_back(make_pair(newHyperOpArguments, supportedArgType(newHyperOpArguments.front(), M, instItr)));
								}
							}
						}

						/* Hack to create 1 hyperop per basic block */
						if (accumulatedBasicBlocks.size() >= 1 && BASICBLOCK_HYPEROP) {
							endOfHyperOp = true;
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
					//First remove args due to back edges
					list<HyperOpArgumentList::iterator> deleteList;

					for (auto newArgItr = hyperOpArguments.begin(); newArgItr != hyperOpArguments.end(); newArgItr++) {
						list<Value*> phiArgs = (*newArgItr).first;
						for (auto argItr = phiArgs.begin(); argItr != phiArgs.end(); argItr++) {
							Value* argument = *argItr;
							if (isa<Instruction>(argument)) {
								if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), ((Instruction*) argument)->getParent()) != accumulatedBasicBlocks.end()) {
									deleteList.push_back(newArgItr);
									break;
								}

								BasicBlock* parentBB = ((Instruction*) argument)->getParent();
								list<pair<list<BasicBlock*>, LoopIV*> > tempLoopBlocks;
								std::copy(originalParallelLoopBB.begin(), originalParallelLoopBB.end(), std::back_inserter(tempLoopBlocks));
								for (auto loopItr = tempLoopBlocks.begin(); loopItr != tempLoopBlocks.end(); loopItr++) {
									auto loopList = loopItr->first;
									if (find(loopList.begin(), loopList.end(), parentBB) != loopList.end()) {
										deleteList.push_back(newArgItr);
										break;
									}
								}
							}
						}
					}

					for (auto deleteItr = deleteList.begin(); deleteItr != deleteList.end(); deleteItr++) {
						hyperOpArguments.erase(*deleteItr);
					}

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

					list<BasicBlock*> tempBBList;
					//Couldn't use splice here since it clears away accumulatedBasicBlocks list
					for (list<BasicBlock*>::iterator accumulatedItr = accumulatedBasicBlocks.begin(); accumulatedItr != accumulatedBasicBlocks.end(); accumulatedItr++) {
						traversedBasicBlocks.push_back(*accumulatedItr);
						tempBBList.push_back(*accumulatedItr);
					}

					hyperOpBBAndArgs.push_back(make_pair(tempBBList, tempHyperOpArguments));

					accumulatedBasicBlocks.clear();
					hyperOpArguments.clear();
					endOfHyperOp = false;
				}

				vector<unsigned> distanceToExit;
				map<unsigned, list<BasicBlock*> > untraversedBasicBlocks;
				list<BasicBlock*> tempTraversalList;
				std::copy(bbTraverser.begin(), bbTraverser.end(), std::back_inserter(tempTraversalList));
				for (unsigned succIndex = 0; succIndex < originalBBItr->getTerminator()->getNumSuccessors(); succIndex++) {
					BasicBlock* succBB = originalBBItr->getTerminator()->getSuccessor(succIndex);
					if (find(traversedBasicBlocks.begin(), traversedBasicBlocks.end(), succBB) == traversedBasicBlocks.end() && find(bbTraverser.begin(), bbTraverser.end(), succBB) == bbTraverser.end()
							&& find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), succBB) == accumulatedBasicBlocks.end()) {
						tempTraversalList.push_back(succBB);
					}
				}
				bbTraverser.clear();
				for (auto successorTraverser = tempTraversalList.begin(); successorTraverser != tempTraversalList.end(); successorTraverser++) {
					BasicBlock* succBB = *successorTraverser;
					if (find(traversedBasicBlocks.begin(), traversedBasicBlocks.end(), succBB) == traversedBasicBlocks.end() && find(bbTraverser.begin(), bbTraverser.end(), succBB) == bbTraverser.end()) {
						list<BasicBlock*> visitedBlocks;
						DominatorTree& tree = getAnalysis<DominatorTree>(*succBB->getParent());
						map<BasicBlock*, int> distanceMap;
						unsigned distanceFromExitBlock = distanceToExitBlock(succBB, visitedBlocks, tree, &distanceMap);
						list<BasicBlock*> basicBlockList;
						if (untraversedBasicBlocks.find(distanceFromExitBlock) != untraversedBasicBlocks.end()) {
							basicBlockList = untraversedBasicBlocks.find(distanceFromExitBlock)->second;
							untraversedBasicBlocks.erase(distanceFromExitBlock);
						}
						basicBlockList.push_back(succBB);
						untraversedBasicBlocks.insert(make_pair(distanceFromExitBlock, basicBlockList));
						if (distanceToExit.empty() || find(distanceToExit.begin(), distanceToExit.end(), distanceFromExitBlock) == distanceToExit.end()) {
							distanceToExit.push_back(distanceFromExitBlock);
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

		for (auto functionListItr : functionList) {
			free(functionListItr);
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

			//Check if the hyperop instance being created is not in the call cycle
			if (isa<CallInst>(accumulatedBasicBlocks.front()->front()) && !((CallInst*) &accumulatedBasicBlocks.front()->front())->getCalledFunction()->isIntrinsic()) {
				if (isHyperOpInstanceInCycle((CallInst*) &accumulatedBasicBlocks.front()->front(), cyclesInCallGraph)) {
					CallInst* callInstructionInvokingTheFunction = callSite.back();
					if (isHyperOpInstanceLastInCycle(callInstructionInvokingTheFunction, cyclesInCallGraph)) {
						continue;
					}
				}
				//Create a copy of the called function as a new HyperOp, by adding it to traversal list
				list<CallInst*> newCallSite;
				std::copy(callSite.begin(), callSite.end(), std::back_inserter(newCallSite));
				CallInst* callInst = (CallInst*) &accumulatedBasicBlocks.front()->front();
				newCallSite.push_back(callInst);
				Function* calledOriginalFunction = callInst->getCalledFunction();
				//Functions to be created to substitute the called original function need to be pushed to traversal list
				list<pair<list<BasicBlock*>, HyperOpArgumentList> > calledFunctionBBList = originalFunctionToHyperOpBBListMap[calledOriginalFunction];
				//Update the arguments to the HyperOp to be created in place of the callsite
				//TODO check why this is in reverse order
				for (list<pair<list<BasicBlock*>, HyperOpArgumentList> >::reverse_iterator replacementFuncItr = calledFunctionBBList.rbegin(); replacementFuncItr != calledFunctionBBList.rend(); replacementFuncItr++) {
					traversalList.push_back(make_pair(make_pair(replacementFuncItr->first, replacementFuncItr->second), newCallSite));
				}
				continue;
			}

			DEBUG(dbgs() << "\n-----------Creating a new HyperOp for function:" << function->getName() << "-----------\n");
			CallInst* instanceCallSite = callSite.back();
			Function* accumulatedFunc = accumulatedBasicBlocks.front()->getParent();
			if ((!callSite.empty() && isa<CallInst>(instanceCallSite) && ((isHyperOpInstanceInCycle(instanceCallSite, cyclesInCallGraph)) || find(parallelLoopFunctionList.begin(), parallelLoopFunctionList.end(), instanceCallSite->getCalledFunction()) != parallelLoopFunctionList.end()))) {
				isStaticHyperOp = false;
			} else {
				for (auto nestedLoopItr = originalSerialLoopBB.begin(); nestedLoopItr != originalSerialLoopBB.end(); nestedLoopItr++) {
					list<BasicBlock*> loopBB = nestedLoopItr->first;
					for (auto accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
						if (find(loopBB.begin(), loopBB.end(), *accumulatedBBItr) != loopBB.end()) {
							isStaticHyperOp = false;
							break;
						}
					}
					if (!isStaticHyperOp) {
						break;
					}
				}
			}

			bool parallelLatchBB = false;
			for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
				LoopIV* parallelLoopIV;
				if (find(originalParallelLatchBB.begin(), originalParallelLatchBB.end(), *accumulatedBBItr) != originalParallelLatchBB.end()) {
					parallelLatchBB = true;
					break;
				}
			}
			if (parallelLatchBB) {
				continue;
			}

			map<Instruction*, Instruction*> originalToClonedInstMap;
			map<BasicBlock*, BasicBlock*> originalToClonedBasicBlockMap;
			list<Type*> argsList;
			list<unsigned> filteredAddressArgs;
			map<unsigned, AllocaInst*> filteredLocalRefAllocaInst;
			list<unsigned> localRefReplacementArgIndex;
			map<unsigned, Value*> localRefReplacementArgMap;
			unsigned argIndex = 0;

			for (HyperOpArgumentList::iterator hyperOpArgumentItr = hyperOpArguments.begin(); hyperOpArgumentItr != hyperOpArguments.end(); hyperOpArgumentItr++) {
				//Set type of each argument of the HyperOp
				Value* argument = hyperOpArgumentItr->first.front();
				switch (hyperOpArgumentItr->second) {
				case GLOBAL_REFERENCE:
					break;
				case ADDRESS:
					filteredAddressArgs.push_back(argIndex);
					argIndex++;
					break;
				case SCALAR:
					argIndex++;
					argsList.push_back(argument->getType());
					break;
				}
			}

			vector<Type*> funcArgsList;
			std::copy(argsList.begin(), argsList.end(), std::back_inserter(funcArgsList));
			FunctionType *FT = FunctionType::get(Type::getVoidTy(getGlobalContext()), funcArgsList, false);
			Function *newFunction = Function::Create(FT, Function::ExternalLinkage, function->getName(), &M);
			//Mark HyperOp function arguments which are not global references or local references as inReg to ease register allocation
			unsigned originalIndex = 0;
			map<unsigned, unsigned> originalIndexAndfuncArgIndexMap;
			unsigned functionArgIndex = 0;

			for (HyperOpArgumentList::iterator hyperOpArgItr = hyperOpArguments.begin(); hyperOpArgItr != hyperOpArguments.end(); hyperOpArgItr++) {
				HyperOpArgumentType type = hyperOpArgItr->second;
				Value* argument = hyperOpArgItr->first.front();
				switch (type) {
				case GLOBAL_REFERENCE:
					break;
				case ADDRESS:
					originalIndex++;
					break;
				case SCALAR:
					originalIndexAndfuncArgIndexMap.insert(make_pair(originalIndex, functionArgIndex));
					if (argument->getType()->isPointerTy()) {
						localRefReplacementArgIndex.push_back(functionArgIndex);
					}
					originalIndex++;
					functionArgIndex++;
					break;
				}
			}

			//Compute branch sources
			map<Instruction*, vector<pair<BasicBlock*, int> > > conditionalBranchSources;
			map<Instruction*, vector<pair<BasicBlock*, int> > > unconditionalBranchSources;
			list<Instruction*> reachingGlobalDefinitionSources;
			list<Function*> instancesForCreation;

			//Add a basic block with a dummy return instruction as the single point of exit for a HyperOp
			BasicBlock* retBB = BasicBlock::Create(ctxt, newFunction->getName().str().append(".return"), newFunction);
			Instruction* retInst = ReturnInst::Create(ctxt);
			retInstMap.insert(make_pair(newFunction, retInst));
			retBB->getInstList().insert(retBB->getFirstInsertionPt(), retInst);

			list<Instruction*> ignoredInstructions;
			for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
				bool parallelLatchBB = false;
				LoopIV* parallelLoopIV;
				if (find(originalParallelLatchBB.begin(), originalParallelLatchBB.end(), *accumulatedBBItr) != originalParallelLatchBB.end()) {
					parallelLatchBB = true;
				}

				list<pair<list<BasicBlock*>, LoopIV*> > tempLoopBlocks;
				std::copy(originalParallelLoopBB.begin(), originalParallelLoopBB.end(), std::back_inserter(tempLoopBlocks));
				for (auto parallelLoopItr = tempLoopBlocks.begin(); parallelLoopItr != tempLoopBlocks.end(); parallelLoopItr++) {
					list<BasicBlock*> bbList = parallelLoopItr->first;
					if (find(bbList.begin(), bbList.end(), *accumulatedBBItr) != bbList.end()) {
						parallelLoopIV = parallelLoopItr->second;
						break;
					}
				}

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
						//Check if the instruction is updating the contents of the induction variable
						bool cloneRequired = true;
						if (parallelLatchBB) {
							for (unsigned phiNodeOperandIndex = 0; phiNodeOperandIndex < parallelLoopIV->getInductionVar()->getNumIncomingValues(); phiNodeOperandIndex++) {
								if (parallelLoopIV->getInductionVar()->getIncomingValue(phiNodeOperandIndex) == instItr) {
									cloneRequired = false;
									break;
								}
							}
						}
						//There is no need to clone this instruction
						if (!cloneRequired) {
							ignoredInstructions.push_back(instItr);
							continue;
						}
						clonedInst = instItr->clone();
						Instruction * originalInstruction = instItr;
						originalToClonedInstMap.insert(std::make_pair(originalInstruction, clonedInst));
						newBB->getInstList().insert(newBB->end(), clonedInst);
					}
				}
			}

			//Instructions have to be patched for operands after all are cloned to ensure that phi nodes are handled correctly
			for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
				for (BasicBlock::iterator instItr = (*accumulatedBBItr)->begin(); instItr != (*accumulatedBBItr)->end(); instItr++) {
					if (find(ignoredInstructions.begin(), ignoredInstructions.end(), instItr) != ignoredInstructions.end()) {
						continue;
					}
					if (!isa<ReturnInst>(instItr)) {
						Instruction* clonedInst = (Instruction*) originalToClonedInstMap.find(instItr)->second;
						for (unsigned operandIndex = 0; operandIndex < clonedInst->getNumOperands(); operandIndex++) {
							Value* operandToBeReplaced = clonedInst->getOperand(operandIndex);
							//If the instruction operand is an argument to the HyperOp
							unsigned hyperOpArgIndex = 0;
							bool argUpdated = false;
							for (HyperOpArgumentList::iterator argumentItr = hyperOpArguments.begin(); argumentItr != hyperOpArguments.end(); argumentItr++) {
								if (argumentItr->second != GLOBAL_REFERENCE) {
									list<Value*> individualArguments = argumentItr->first;
									if (argumentItr->second == ADDRESS && find(filteredAddressArgs.begin(), filteredAddressArgs.end(), hyperOpArgIndex) != filteredAddressArgs.end() && individualArguments.front() == operandToBeReplaced) {
										if (filteredLocalRefAllocaInst.find(hyperOpArgIndex) == filteredLocalRefAllocaInst.end()) {
											//Create an alloca instruction for the local reference in the current HyperOp
											AllocaInst* localAllocaInst = new AllocaInst(((AllocaInst*) individualArguments.front())->getAllocatedType());
											localAllocaInst->setAlignment(4);
											//Alloc instructions need to be inserted in the entry basic block of the function because other allocs are treated as dynamic stack allocs
											localAllocaInst->insertBefore(newFunction->front().begin());
											filteredLocalRefAllocaInst[hyperOpArgIndex] = localAllocaInst;
										}
										AllocaInst* localAllocaInst = filteredLocalRefAllocaInst[hyperOpArgIndex];
										//Replace uses of the alloca variable with the newly allocated variable
										clonedInst->setOperand(operandIndex, localAllocaInst);
										if (isa<PHINode>(clonedInst)) {
											//Find the corresponding branch that leads to phi and patch
											BasicBlock* sourceBB = ((PHINode*) clonedInst)->getIncomingBlock(operandIndex);
											((PHINode*) clonedInst)->setIncomingBlock(operandIndex, originalToClonedBasicBlockMap[sourceBB]);
										}
									} else {
										for (list<Value*>::iterator argumentValueItr = individualArguments.begin(); argumentValueItr != individualArguments.end(); argumentValueItr++) {
											if (*argumentValueItr == operandToBeReplaced) {
												unsigned localHyperOpArgIndex = originalIndexAndfuncArgIndexMap[hyperOpArgIndex];
												if (find(localRefReplacementArgIndex.begin(), localRefReplacementArgIndex.end(), localHyperOpArgIndex) != localRefReplacementArgIndex.end()) {
													//Check if a local load instruction has already been added
													if (localRefReplacementArgMap.find(localHyperOpArgIndex) == localRefReplacementArgMap.end()) {
														//Add a new load instruction in the first basic block of the function being created now
														for (Function::arg_iterator argItr = newFunction->arg_begin(); argItr != newFunction->arg_end(); argItr++) {
															if ((*argItr).getArgNo() == localHyperOpArgIndex) {
																LoadInst* loadFromRefInst = new LoadInst(argItr);
																loadFromRefInst->insertBefore(newFunction->front().begin());
																clonedInst->setOperand(operandIndex, loadFromRefInst);
																if (isa<PHINode>(clonedInst)) {
																	//Find the corresponding branch that leads to phi and patch
																	BasicBlock* sourceBB = ((PHINode*) clonedInst)->getIncomingBlock(operandIndex);
																	((PHINode*) clonedInst)->setIncomingBlock(operandIndex, originalToClonedBasicBlockMap[sourceBB]);
																}
																localRefReplacementArgMap[localHyperOpArgIndex] = (Value*) loadFromRefInst;
																argUpdated = true;
																break;
															}
														}
													} else {
														Value* replacementArg = localRefReplacementArgMap[localHyperOpArgIndex];
														clonedInst->setOperand(operandIndex, replacementArg);
														if (isa<PHINode>(clonedInst)) {
															//Find the corresponding branch that leads to phi and patch
															BasicBlock* sourceBB = ((PHINode*) clonedInst)->getIncomingBlock(operandIndex);
															((PHINode*) clonedInst)->setIncomingBlock(operandIndex, originalToClonedBasicBlockMap[sourceBB]);
														}
														argUpdated = true;
													}

												} else {
													//Get Value object of the newly created function's argument corresponding to the replacement
													for (Function::arg_iterator argItr = newFunction->arg_begin(); argItr != newFunction->arg_end(); argItr++) {
														if ((*argItr).getArgNo() == localHyperOpArgIndex) {
															clonedInst->setOperand(operandIndex, argItr);
															argUpdated = true;
															break;
														}
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
							}

							//Find the definitions added previously which reach the use
							if (!argUpdated) {
								if (isa<Constant>(operandToBeReplaced) && isa<PHINode>(instItr)) {
									BasicBlock* sourceBB = ((PHINode*) &*instItr)->getIncomingBlock(operandIndex);
									((PHINode*) clonedInst)->setIncomingBlock(operandIndex, originalToClonedBasicBlockMap[sourceBB]);
								}
								//If the original operand is an instruction that was cloned previously which belongs to the list of accumulated HyperOps
								for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
									for (BasicBlock::iterator accumulatedInstItr = (*accumulatedBBItr)->begin(); accumulatedInstItr != (*accumulatedBBItr)->end(); accumulatedInstItr++) {
										for (Value::use_iterator useItr = accumulatedInstItr->use_begin(); useItr != accumulatedInstItr->use_end(); useItr++) {
											if (*useItr == instItr && ((Instruction*) instItr->getOperand(operandIndex)) == accumulatedInstItr) {
												Instruction* clonedSourceInstr = (Instruction*) originalToClonedInstMap.find(accumulatedInstItr)->second;
												if (clonedSourceInstr != NULL) {
													clonedInst->setOperand(operandIndex, clonedSourceInstr);
													if (isa<PHINode>(clonedInst)) {
														//Find the corresponding branch that leads to phi and patch
														BasicBlock* sourceBB = ((PHINode*) clonedInst)->getIncomingBlock(operandIndex);
														((PHINode*) clonedInst)->setIncomingBlock(operandIndex, originalToClonedBasicBlockMap[sourceBB]);
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}

			//Patch phi instructions
			for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
				for (BasicBlock::iterator instItr = (*accumulatedBBItr)->begin(); instItr != (*accumulatedBBItr)->end(); instItr++) {
					if (isa<PHINode>(instItr)) {
						Instruction* clonedInst = (Instruction*) originalToClonedInstMap.find(instItr)->second;
						list<int> patchIncomingValues;
						for (int i = 0; i < ((PHINode*) &*instItr)->getNumIncomingValues(); i++) {
							BasicBlock* incomingBlock = ((PHINode*) &*instItr)->getIncomingBlock(i);
							if (find(originalParallelLatchBB.begin(), originalParallelLatchBB.end(), incomingBlock) != originalParallelLatchBB.end() || find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), incomingBlock) == accumulatedBasicBlocks.end()) {
								patchIncomingValues.push_back(i);
							}
						}

						Value* setValue = ((PHINode*) clonedInst)->getIncomingValue(0);
						((PHINode*) clonedInst)->setIncomingValue(0, setValue);
						for (auto incomingValItr : patchIncomingValues) {
							if (incomingValItr) {
								((PHINode*) clonedInst)->removeIncomingValue(((PHINode*) clonedInst)->getIncomingBlock(incomingValItr));
							} else {
								((PHINode*) clonedInst)->setIncomingBlock(incomingValItr, NULL);
							}
						}
						originalToClonedInstMap.find(instItr)->second->dump();
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
						} else {
							//Check if the target is the latch of a parallel loop
							for (auto parallelLoopLatchBB : originalParallelLatchBB) {
								if (terminator->getSuccessor(i) == parallelLoopLatchBB) {
									((TerminatorInst*) originalToClonedInstMap[terminator])->setSuccessor(i, originalToClonedBasicBlockMap[*accumulatedBBItr]);
								}
							}
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
				errs() << callInst->getParent()->getName() << ",";
				entryBlock = &callInst->getParent()->getParent()->getEntryBlock();
				numCallInstrAdded++;
			}
			newlyAcquiredBBList.pop_back();
			if (numCallInstrAdded) {
				for (list<BasicBlock*>::iterator acquiredBBItr = newlyAcquiredBBList.begin(); acquiredBBItr != newlyAcquiredBBList.end(); acquiredBBItr++) {
					accumulatedBasicBlocks.push_back(*acquiredBBItr);
				}
			}

			for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
				//Find out if any basic block is predicated
				BasicBlock* originalBB = *accumulatedBBItr;
				//check if the original bb is a part of a loop, don't bother adding conditional jumps unless the bb is a loop header
				//Find the immediate dominator
				DominatorTree& idomTree = getAnalysis<DominatorTree>(*originalBB->getParent());
				PostDominatorTree& postdomTree = getAnalysis<PostDominatorTree>(*originalBB->getParent());
				if (idomTree.getNode(originalBB) != NULL && idomTree.getNode(originalBB)->getIDom() != NULL) {
					//Find the parent idom till one source of branch emerges somewhere
					BasicBlock* currentBB = originalBB;
					BasicBlock* idom = idomTree.getNode(originalBB)->getIDom()->getBlock();
					BasicBlock* originalIdom = idom;
					bool idomPredecessor = false;
					while (true) {
						idomPredecessor = false;
						for (auto predItr = pred_begin(currentBB); predItr != pred_end(currentBB); predItr++) {
							BasicBlock* predecessor = *predItr;
							if (predecessor == idom && (predecessor->getTerminator()->getNumSuccessors() > 1 || (isa<BranchInst>(predecessor->getTerminator()) && ((BranchInst*) predecessor->getTerminator())->isConditional()))) {
								bool isConditionalJumpFromLoop = false;
								for (LoopInfo* loopInfoItr : loopCache) {
									for (LoopInfo::iterator loopItr = loopInfoItr->begin(); loopItr != loopInfoItr->end(); loopItr++) {
										vector<BasicBlock*> loopBBList = (*loopItr)->getBlocks();
										if ((*loopItr)->getHeader() == idom && find(loopBBList.begin(), loopBBList.end(), currentBB) == loopBBList.end()) {
											isConditionalJumpFromLoop = true;
											break;
										}
									}
								}
								if (!isConditionalJumpFromLoop) {
									idomPredecessor = true;
									break;
								}
							}
						}

						if (idomPredecessor || idomTree.getNode(idom) == NULL || idomTree.getNode(idom)->getIDom() == NULL) {
							break;
						}

						currentBB = idom;
						idom = idomTree.getNode(idom)->getIDom()->getBlock();
					}

					//if the idom is not in accumulated list, add it as a branch source
					if (idom != NULL) {
						if (idomPredecessor && (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), idom) == accumulatedBasicBlocks.end() || find(newlyAcquiredBBList.begin(), newlyAcquiredBBList.end(), originalBB) != newlyAcquiredBBList.end())) {
							//Find the terminator of idom that leads to bbItr
							vector<pair<BasicBlock*, int> > successorBBList;
							TerminatorInst* terminator = idom->getTerminator();
							bool parallelLoopLatchSource = false;
							for (auto parallelLoopItr : originalParallelLatchBB) {
								if (parallelLoopItr == idom) {
									parallelLoopLatchSource = true;
									break;
								}
							}
							if (parallelLoopLatchSource) {
								continue;
							}

							int numSuccessorsCovered = 0;
							for (unsigned i = 0; i < terminator->getNumSuccessors(); i++) {
								BasicBlock* postIdomOfSucc = NULL;
								if (postdomTree.getNode(terminator->getSuccessor(i))->getIDom() != NULL) {
									postIdomOfSucc = postdomTree.getNode(terminator->getSuccessor(i))->getIDom()->getBlock();
								}
								if (terminator->getSuccessor(i) == originalBB || terminator->getSuccessor(i) == currentBB || postIdomOfSucc == currentBB || postIdomOfSucc == originalBB) {
									//Add only those successors that correspond to a basic block in the current HyperOp
									successorBBList.push_back(make_pair(originalBB, i));
									numSuccessorsCovered++;
								}
							}
							if (numSuccessorsCovered == terminator->getNumSuccessors()) {
								//All successors of the terminator instruction are in the same target HyperOp, mark the jump as unconditional
								if (unconditionalBranchSources.find(terminator) == unconditionalBranchSources.end()) {
									unconditionalBranchSources[terminator] = successorBBList;
								}
							} else {
								if (conditionalBranchSources.find(terminator) != conditionalBranchSources.end()) {
									vector<pair<BasicBlock*, int> > previousSuccessorIndexList = conditionalBranchSources[terminator];
									conditionalBranchSources.erase(terminator);
									std::copy(previousSuccessorIndexList.begin(), previousSuccessorIndexList.end(), back_inserter(successorBBList));
								}
								conditionalBranchSources[terminator] = successorBBList;
							}
						}

						//Add all the jump sources of the basic block to point to their return blocks
						for (auto predItr = pred_begin(originalBB); predItr != pred_end(originalBB); predItr++) {
							BasicBlock* pred = *predItr;
							bool parallelLoopLatchSource = false;
							for (auto parallelLoopItr : originalParallelLatchBB) {
								if (parallelLoopItr == pred) {
									parallelLoopLatchSource = true;
									break;
								}
							}
							if (parallelLoopLatchSource) {
								continue;
							}
							if ((pred != idom || !idomPredecessor) && originalFunctionToHyperOpBBListMap.find(pred->getParent()) != originalFunctionToHyperOpBBListMap.end()
									&& (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), pred) == accumulatedBasicBlocks.end() || find(newlyAcquiredBBList.begin(), newlyAcquiredBBList.end(), pred->getTerminator()->getSuccessor(0)) != newlyAcquiredBBList.end())
									&& originalFunctionToHyperOpBBListMap.find(pred->getParent()) != originalFunctionToHyperOpBBListMap.end()) {
								vector<pair<BasicBlock*, int> > successorBBList;
								for (int i = 0; i < pred->getTerminator()->getNumSuccessors(); i++) {
									if (pred->getTerminator()->getSuccessor(i) == originalBB) {
										successorBBList.push_back(make_pair(originalBB, i));
									}
								}
								if (unconditionalBranchSources.find(pred->getTerminator()) == unconditionalBranchSources.end()) {
									unconditionalBranchSources[pred->getTerminator()] = successorBBList;
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
					if (isStaticHyperOp) {
						if ((&callerFunction->getEntryBlock().front() == (Instruction*) callSiteCopy.front()) && find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), &function->getEntryBlock()) != accumulatedBasicBlocks.end()) {
							isKernelEntry = true;
						}
						break;
					}
					callSiteCopy.pop_front();
				}
			}

			//last instruction in redefine_start is a call
			else if (!callSite.empty()) {
				list<CallInst*> callSiteCopy;
				std::copy(callSite.begin(), callSite.end(), std::back_inserter(callSiteCopy));
				//Recursively find out if the call instruction is the first in the whole call chain
				while (!callSiteCopy.empty()) {
					Function* callerFunction = callSiteCopy.front()->getParent()->getParent();
					if (callerFunction->back().getInstList().size() > 1 && callerFunction->back().getTerminator()->getPrevNode() != NULL && callerFunction->back().getTerminator()->getPrevNode() == callSiteCopy.front()) {
						if (isStaticHyperOp && find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), &function->back()) != accumulatedBasicBlocks.end()) {
							//Not the last basic block in the function
							isKernelExit = true;
							break;
						} else {
							callSiteCopy.pop_front();
						}
					} else {
						break;
					}
				}
			}

			if (isKernelEntry) {
				isStaticHyperOp = true;
			}

			MDNode *funcAnnotation;
			//Keep the following branch structure
			if (!isStaticHyperOp) {
				vector<Value*> values;
				values.push_back(MDString::get(ctxt, HYPEROP));
				values.push_back(newFunction);
				values.push_back(MDString::get(ctxt, DYNAMIC_HYPEROP));
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
				if (parentCallSite.empty() || callSite.empty() || parentCallSite.back()->getCalledFunction() != callSite.back()->getCalledFunction()) {
					values.push_back(newFunction);
				} else {
					//Find the original function from which the parent function was created
					for (list<Function*>::iterator parentFuncItr = parentFunctionList.begin(); parentFuncItr != parentFunctionList.end(); parentFuncItr++) {
						bool matchFound = true;
						list<BasicBlock*> parentFuncBBList = createdHyperOpAndOriginalBasicBlockAndArgMap[*parentFuncItr].first;
						for (list<BasicBlock*>::iterator bbItr = parentFuncBBList.begin(); bbItr != parentFuncBBList.end(); bbItr++) {
							if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), *bbItr) == accumulatedBasicBlocks.end()) {
								matchFound = false;
								break;
							}
						}
						if (matchFound) {
							values.push_back(*parentFuncItr);
							break;
						}
					}
				}
				//Convert the id to a tag string
				list<unsigned> uniqueIdInCallTree = getHyperOpInstanceTag(callSite, newFunction, createdHyperOpAndCallSite, createdHyperOpAndUniqueId, accumulatedBasicBlocks, createdHyperOpAndOriginalBasicBlockAndArgMap);
				string tag = "<";
				for (list<unsigned>::iterator tagItr = uniqueIdInCallTree.begin(); tagItr != uniqueIdInCallTree.end(); tagItr++) {
					tag.append(itostr(*tagItr));
					if (*tagItr != uniqueIdInCallTree.back()) {
						tag.append(",");
					}
				}
				tag.append(">");
				values.push_back(MDString::get(ctxt, tag));
				//Check if the accumulated bbs are supposed to be a range of HyperOps
				LoopIV* loopIV = NULL;
				bool mismatch = false;

				for (auto accumulatedbbItr = accumulatedBasicBlocks.begin(); accumulatedbbItr != accumulatedBasicBlocks.end(); accumulatedbbItr++) {
					//Only loop header needs range
					bool accumulatedBBInParallelLoop = false;
					loopIV = NULL;
					for (auto parallelLoopItr = parallelLoopFunctionList.begin(); parallelLoopItr != parallelLoopFunctionList.end(); parallelLoopItr++) {
						bool bbInList = false;
						for (auto bbItr = (*parallelLoopItr)->begin(); bbItr != (*parallelLoopItr)->end(); bbItr++) {
							BasicBlock* bb = bbItr;
							if (bb == *accumulatedbbItr) {
								bbInList = true;
								break;
							}
						}

						if (bbInList) {
							BasicBlock* clonedBB = &((*parallelLoopItr)->front());
							StringRef originalParallelLoopItrName = (*parallelLoopItr)->getName();
							for (auto originalParallelLoopItr : originalParallelLoopBB) {
								list<BasicBlock*> loopBBList = originalParallelLoopItr.first;
								for (auto bbItr : loopBBList) {
									if (isa<CallInst>(bbItr->front()) && ((CallInst*) (&bbItr->front()))->getCalledFunction()->getName().compare(originalParallelLoopItrName) == 0) {
										loopIV = originalParallelLoopItr.second;
										break;
									}
								}
								if (loopIV) {
									break;
								}
							}
							break;
						}
					}
					if (loopIV == NULL && find(originalHeaderBB.begin(), originalHeaderBB.end(), *accumulatedbbItr) == originalHeaderBB.end()) {
						mismatch = true;
						break;
					}
				}

				if (loopIV != NULL && !mismatch) {
					if (loopIV->getLowerBoundType() == LoopIV::CONSTANT) {
						values.push_back(MDString::get(ctxt, itostr(loopIV->getConstantLowerBound())));
					} else {
						Value* lowerBound = loopIV->getVariableLowerBound();
						//Now the bounds to be a global because I need to think of how to support variable bounds
						//Global value, use as is
						assert(isa<LoadInst>(lowerBound) && "Non global bounds not supported currently");
						values.push_back(((Instruction*) lowerBound)->getOperand(0));
					}

					if (loopIV->getUpperBoundType() == LoopIV::CONSTANT) {
						values.push_back(MDString::get(ctxt, itostr(loopIV->getConstantUpperBound())));
					} else {
						Value* upperBound = loopIV->getVariableUpperBound();
						upperBound->dump();
						assert(isa<LoadInst>(upperBound) && "Non global bounds not supported currently");
						values.push_back(((Instruction*) upperBound)->getOperand(0));
					}

					values.push_back(MDString::get(ctxt, StringRef(loopIV->getIncOperation())));
					values.push_back(loopIV->getStride());
				}
				ArrayRef<Value*> valueArray(values);
				funcAnnotation = MDNode::get(ctxt, valueArray);
				createdHyperOpAndUniqueId[newFunction] = uniqueIdInCallTree;
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

			//Is the function an entry node/exit node/intermediate ?
			if (isKernelEntry) {
				Value* hyperOpDescrMDValues[2];
				hyperOpDescrMDValues[0] = MDString::get(ctxt, HYPEROP_ENTRY);
				hyperOpDescrMDValues[1] = funcAnnotation;
				MDNode* startHyperOpMDNode = MDNode::get(ctxt, hyperOpDescrMDValues);
				redefineAnnotationsNode->addOperand(startHyperOpMDNode);
				assert(startHyperOp==NULL&&"HIG can only have one start HyperOp\n");
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

			newFunction->dump();
			createdHyperOpAndOriginalBasicBlockAndArgMap[newFunction] = make_pair(accumulatedBasicBlocks, hyperOpArguments);
			createdHyperOpAndCallSite[newFunction] = callSite;
			createdHyperOpAndConditionalBranchSources[newFunction] = conditionalBranchSources;
			createdHyperOpAndUnconditionalBranchSources[newFunction] = unconditionalBranchSources;
			createdHyperOpAndReachingDefSources[newFunction] = reachingGlobalDefinitionSources;
			functionOriginalToClonedInstructionMap[newFunction] = originalToClonedInstMap;
			functionOriginalToClonedBBMap[newFunction] = originalToClonedBasicBlockMap;
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
			string tag = "<0>";
			Value * values[5];
			values[0] = MDString::get(ctxt, HYPEROP);
			values[1] = newFunction;
			values[2] = MDString::get(ctxt, DYNAMIC_HYPEROP);
			values[3] = newFunction;
			values[4] = MDString::get(ctxt, tag);
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
		map<Function*, list<Function*> > addedPredicateSourcesMap;
		map<Function*, list<Function*> > addedDataSourcesMap;
		DEBUG(dbgs() << "\n============Adding dependences across created HyperOps=======\n");
		//Add metadata: This code is moved here to ensure that all the functions (corresponding to HyperOps) that need to be created have already been created
		for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHyperOpItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHyperOpItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHyperOpItr++) {
			Function* createdFunction = createdHyperOpItr->first;
			DEBUG(dbgs() << "\n-----------Patching created function " << createdFunction->getName() << "--------------\n");
			list<BasicBlock*> accumulatedOriginalBasicBlocks = createdHyperOpItr->second.first;
			HyperOpArgumentList hyperOpArguments = createdHyperOpItr->second.second;
			map<Instruction*, vector<pair<BasicBlock*, int> > > conditionalBranchSources = createdHyperOpAndConditionalBranchSources[createdFunction];
			map<Instruction*, vector<pair<BasicBlock*, int> > > unconditionalBranchSources = createdHyperOpAndUnconditionalBranchSources[createdFunction];
			list<Instruction*> reachingDefSources = createdHyperOpAndReachingDefSources[createdFunction];
			bool isStaticHyperOp = createdHyperOpAndType[createdFunction];

			//Includes sync and data sources
			list<Function*> addedParentsToCurrentHyperOp;

			MDNode* funcAnnotation = hyperOpAndAnnotationMap.find(createdFunction)->second;
			list<CallInst*> callSite = createdHyperOpAndCallSite[createdFunction];
			list<Function*> scalarProducers;
			list<Function*> predicateProducers;
			//Shouldve merged with the previous list, but would work for now
			map<Function*, Value*> predicateProducerValueMap;

			DEBUG(dbgs() << "\n----------Adding consumed by metadata----------\n");
			unsigned hyperOpArgumentIndex = 0;
			//Replace arguments of called functions with the right call arguments or return values
			for (HyperOpArgumentList::iterator hyperOpArgItr = hyperOpArguments.begin(); hyperOpArgItr != hyperOpArguments.end(); hyperOpArgItr++) {
				map<Value*, Value*> replacementArg;
				HyperOpArgumentType replacementArgType = hyperOpArgItr->second;
				//the argument is a function argument
				//second half hack for constant phi args
				list<CallInst*> callSiteCopy;
				std::copy(callSite.begin(), callSite.end(), back_inserter(callSiteCopy));

				if (hyperOpArgItr->second != GLOBAL_REFERENCE && !isa<Instruction>(hyperOpArgItr->first.front()) && !isa<Constant>(hyperOpArgItr->first.front())) {
					errs()<<"whats the arg?";
					hyperOpArgItr->first.front()->dump();
					unsigned positionOfFormalArg = 0;
					Function* originalFunction = accumulatedOriginalBasicBlocks.front()->getParent();
					for (Function::arg_iterator originalArgItr = originalFunction->arg_begin(); originalArgItr != originalFunction->arg_end(); originalArgItr++, positionOfFormalArg++) {
						if (originalArgItr == hyperOpArgItr->first.front()) {
							break;
						}
					}

					CallInst* callInst = callSiteCopy.back();
					Value* argOperand = callInst->getArgOperand(positionOfFormalArg);
					BasicBlock* lookForBB = NULL;
					bool isEntryOfNewFunction = find(accumulatedOriginalBasicBlocks.begin(), accumulatedOriginalBasicBlocks.end(), &callInst->getParent()->getParent()->getEntryBlock()) != accumulatedOriginalBasicBlocks.end();

					/* If the current hyperOp has the entry basic block to the current hyperOp's function, we need to skip the last call in the call site to add an edge from the caller to the current HyperOp */
					if(isEntryOfNewFunction){
						callSiteCopy.pop_back();
						list<BasicBlock*> preds;
						for(auto bbItr =  callInst->getParent()->getParent()->begin(); bbItr!=callInst->getParent()->getParent()->end(); bbItr++){
							if(bbItr->getNextNode() == callInst->getParent()){
								preds.push_back(bbItr);
							}
						}
						assert(preds.size() == 1 && "Cant have more than 1 pred from the same function\n");
						lookForBB = preds.front();
					}else{
						lookForBB = &callInst->getCalledFunction()->getEntryBlock();
					}
					Function * sourceFunction = NULL;
					/* Get the newly created function's argument matching the arg operand */
					for (map<Function*, list<CallInst*> >::iterator createdHopItr = createdHyperOpAndCallSite.begin(); createdHopItr != createdHyperOpAndCallSite.end(); createdHopItr++) {
						list<CallInst*> createdHopCallSite = createdHopItr->second;
						if (createdHopCallSite.size() != callSiteCopy.size()) {
							continue;
						}
						bool callSiteMatch = true;
						list<CallInst*>::iterator callSiteItr = callSiteCopy.begin();
						for (list<CallInst*>::iterator createHopCallSiteItr = createdHopCallSite.begin(); createHopCallSiteItr != createdHopCallSite.end() && callSiteItr != callSiteCopy.end(); createHopCallSiteItr++, callSiteItr++) {
							if (*createHopCallSiteItr != *callSiteItr) {
								callSiteMatch = false;
								break;
							}
						}

						if (callSiteMatch) {
							list<BasicBlock*> createdHopAccumulatedBlocks = createdHyperOpAndOriginalBasicBlockAndArgMap[createdHopItr->first].first;
							if (find(createdHopAccumulatedBlocks.begin(), createdHopAccumulatedBlocks.end(), lookForBB) != createdHopAccumulatedBlocks.end()) {
								sourceFunction = createdHopItr->first;
								break;
							}
						}
					}

					assert(sourceFunction!=NULL && "Source of the argument can't be NULL\n");
					int index = 0;
					Value* clonedInst = NULL;
					if (!isEntryOfNewFunction) {
						for (auto argItr = sourceFunction->arg_begin(); argItr != sourceFunction->arg_end(); argItr++, index++) {
							if (index == positionOfFormalArg) {
								clonedInst = argItr;
								break;
							}
						}
					}else{
						clonedInst = getClonedArgument(argOperand, callSiteCopy, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
					}
					assert(clonedInst!=NULL && "clone of the operand can't be NULL\n");
					replacementArg.insert(make_pair(clonedInst, argOperand));
				}/* return value of a function call is an argument */
				else if (isa<CallInst>(hyperOpArgItr->first.front())) {
					CallInst* callInst = (CallInst*) hyperOpArgItr->first.front();
					callSiteCopy.push_back(callInst);

					Value* returnValue = 0;
					BasicBlock* lookForBB = &callInst->getCalledFunction()->back();
					Function * sourceFunction = NULL;
					/* Get the newly created function's argument matching the arg operand */
					for (map<Function*, list<CallInst*> >::iterator createdHopItr = createdHyperOpAndCallSite.begin(); createdHopItr != createdHyperOpAndCallSite.end(); createdHopItr++) {
						list<CallInst*> createdHopCallSite = createdHopItr->second;
						if (createdHopCallSite.size() != callSiteCopy.size()) {
							continue;
						}
						bool callSiteMatch = true;
						list<CallInst*>::iterator callSiteItr = callSiteCopy.begin();
						for (list<CallInst*>::iterator createHopCallSiteItr = createdHopCallSite.begin(); createHopCallSiteItr != createdHopCallSite.end() && callSiteItr != callSiteCopy.end(); createHopCallSiteItr++, callSiteItr++) {
							if (*createHopCallSiteItr != *callSiteItr) {
								callSiteMatch = false;
								break;
							}
						}

						if (callSiteMatch) {
							list<BasicBlock*> createdHopAccumulatedBlocks = createdHyperOpAndOriginalBasicBlockAndArgMap[createdHopItr->first].first;
							if (find(createdHopAccumulatedBlocks.begin(), createdHopAccumulatedBlocks.end(), lookForBB) != createdHopAccumulatedBlocks.end()) {
								sourceFunction = createdHopItr->first;
								break;
							}
						}
					}
					/* This indicates that the source function is an unrolled instance and we need not add metadata for this communication */
					if(sourceFunction == NULL){
						continue;
					}
					assert(createdHyperOpAndReturnValue.find(sourceFunction) != createdHyperOpAndReturnValue.end() && "A function for return value of called function was never created\n");
					returnValue = createdHyperOpAndReturnValue[sourceFunction];
					assert(returnValue!=NULL && "Return hyperOp not attached to the producer function\n");
					Instruction* clonedInst;
					if (isa<Constant>(returnValue)) {
						//Immediate value
						clonedInst = new AllocaInst(returnValue->getType());
						((AllocaInst*) clonedInst)->setAlignment(4);
						//Alloc instructions need to be inserted in the entry basic block of the function because other allocs are treated as dynamic stack allocs
						clonedInst->insertBefore(sourceFunction->getEntryBlock().getFirstInsertionPt());
						StoreInst* storeInst = new StoreInst(returnValue, clonedInst);
						storeInst->setAlignment(4);
						storeInst->insertBefore(retInstMap[sourceFunction]);
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
								reachingDefBasicBlocks = reachingStoreOperations((AllocaInst*) argument, originalFunction, accumulatedOriginalBasicBlocks);
							} else {
								reachingDefBasicBlocks.insert(make_pair(((Instruction*) argument)->getParent(), (Instruction*) argument));
							}

							list<Function*> addedFunctions;
							//Find the HyperOp with the reaching definition
							for (map<BasicBlock*, Instruction*>::iterator reachingDefItr = reachingDefBasicBlocks.begin(); reachingDefItr != reachingDefBasicBlocks.end(); reachingDefItr++) {
								Instruction* reachingDefInstr = reachingDefItr->second;
								Instruction* clonedReachingDef = getClonedArgument(reachingDefInstr, callSite, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
								if (find(addedFunctions.begin(), addedFunctions.end(), clonedReachingDef->getParent()->getParent()) == addedFunctions.end()) {
									replacementArg.insert(make_pair(clonedReachingDef, reachingDefInstr));
									addedFunctions.push_back(clonedReachingDef->getParent()->getParent());
								}
							}
						}
					}
				}
				if (hyperOpArgItr->second != GLOBAL_REFERENCE && hyperOpArgItr->second != ADDRESS) {
					for (map<Value*, Value*>::iterator clonedReachingDefItr = replacementArg.begin(); clonedReachingDefItr != replacementArg.end(); clonedReachingDefItr++) {
						Value* clonedReachingDefInst = clonedReachingDefItr->first;
						list<Value*> clonedInstructionsToBeLabeled;
						clonedInstructionsToBeLabeled.push_back(clonedReachingDefInst);
						Function* producerFunction;
						if(isa<Instruction>(clonedReachingDefInst)){
							producerFunction = ((Instruction*)clonedReachingDefInst)->getParent()->getParent();
						}else{
							//Check if its an argument
							assert(isa<Argument>(clonedReachingDefInst) && "Replacement arg neither an instruction nor an argument\n");
							producerFunction = ((Argument*) clonedReachingDefInst)->getParent();
						}
						//TODO
						bool isProducerStatic = createdHyperOpAndType[producerFunction];
						//Find the replicas of the consumer HyperOp which also need to be annotated, hence populating a list
						if (isProducerStatic && !isStaticHyperOp) {
							//replicate the meta data in the last instance of the HyperOp corresponding to the last HyperOp in the cycle
							list<list<CallInst*> > callChainList;
							for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
								list<CallInst*> callChain;
								std::copy(callSiteCopy.begin(), callSiteCopy.end(), std::back_inserter(callChain));
								list<pair<Function*, CallInst*> > cycle = *cycleItr;
								if (cycle.front().first == createdHyperOpAndOriginalBasicBlockAndArgMap[producerFunction].first.front()->getParent()) {
									callChain.pop_back();
									for (list<pair<Function*, CallInst*> >::iterator cycleItr = cycle.begin(); cycleItr != cycle.end(); cycleItr++) {
										callChain.push_back(cycleItr->second);
									}
									callChainList.push_back(callChain);
								}
							}

							for (list<list<CallInst*> >::iterator callChainListItr = callChainList.begin(); callChainListItr != callChainList.end(); callChainListItr++) {
								list<CallInst*> callChain = *callChainListItr;
								//Find the function corresponding to the callChain
								Value* clonedInstInstance = getClonedArgument(clonedReachingDefItr->second, callChain, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
								if (find(clonedInstructionsToBeLabeled.begin(), clonedInstructionsToBeLabeled.end(), clonedInstInstance) == clonedInstructionsToBeLabeled.end()) {
									clonedInstructionsToBeLabeled.push_back(clonedInstInstance);
								}
							}
						}

						for (list<Value*>::iterator clonedInstItr = clonedInstructionsToBeLabeled.begin(); clonedInstItr != clonedInstructionsToBeLabeled.end(); clonedInstItr++) {
							Value* clonedDefInst = *clonedInstItr;
							Value* originalInst = clonedDefInst;
							if (isa<StoreInst>(clonedDefInst)) {
								clonedDefInst = (Instruction*) ((StoreInst*) clonedDefInst)->getOperand(1);
								if (!isa<AllocaInst>(clonedDefInst)) {
									AllocaInst* ai = new AllocaInst(clonedDefInst->getType());
									ai->setAlignment(4);
									Instruction* castedOriginalInst = (Instruction*) originalInst;
									//Alloc instructions need to be inserted in the entry basic block of the function because other allocs are treated as dynamic stack allocs
									ai->insertBefore(castedOriginalInst->getParent()->getParent()->getEntryBlock().getFirstInsertionPt());
									StoreInst* storeInst = new StoreInst(clonedDefInst, ai);
									storeInst->setAlignment(4);
									storeInst->insertAfter(castedOriginalInst);
									clonedDefInst = ai;
								}
							 } else if(isa<Argument>(clonedDefInst)){
								Function* parentFunction = ((Argument*)clonedDefInst)->getParent();
								AllocaInst* ai = new AllocaInst(clonedDefInst->getType());
								ai->insertBefore(parentFunction->getEntryBlock().getFirstInsertionPt());
								StoreInst* storeInst = new StoreInst(clonedDefInst, ai);
								storeInst->insertAfter(ai);
								clonedDefInst = ai;
							}
							//Is the producer static?
							MDNode * consumedByMetadata;

							//producer and consumer may both be marked dynamic in case of back edges of loop
							bool backedgeOfLoop = false;
							for (auto serialNestedLoopItr = originalSerialLoopBB.begin(); serialNestedLoopItr != originalSerialLoopBB.end(); serialNestedLoopItr++) {
								//All this wouldn't be necessary if loop itself was cached
								list<BasicBlock*> serialLoopBBList = serialNestedLoopItr->first;
								for (auto accumulatedBBItr = accumulatedOriginalBasicBlocks.begin(); accumulatedBBItr != accumulatedOriginalBasicBlocks.end(); accumulatedBBItr++) {
									if (find(serialLoopBBList.begin(), serialLoopBBList.end(), *accumulatedBBItr) != serialLoopBBList.end() && find(originalHeaderBB.begin(), originalHeaderBB.end(), *accumulatedBBItr) != originalHeaderBB.end()) {
										//check if the latch node is in the producer's list of bbs
										list<BasicBlock*> producerBBList = createdHyperOpAndOriginalBasicBlockAndArgMap[producerFunction].first;
										for (auto producerBBItr = producerBBList.begin(); producerBBItr != producerBBList.end(); producerBBItr++) {
											if (find(serialLoopBBList.begin(), serialLoopBBList.end(), *producerBBItr) != serialLoopBBList.end() && find(originalLatchBB.begin(), originalLatchBB.end(), *producerBBItr) != originalLatchBB.end()) {
												backedgeOfLoop = true;
												break;
											}
										}
										if (backedgeOfLoop) {
											break;
										}
									}
								}
								if (backedgeOfLoop) {
									break;
								}
							}

							//serial or parallel loop, it doesn't matter
							bool createdHopPartofLoop = false;

							//TODO this isn't enough for nested recursion cycles
							bool staticMD = (isProducerStatic && isStaticHyperOp) || (!isProducerStatic && !isStaticHyperOp && !backedgeOfLoop);
							if (staticMD) {
								//Add "consumedby" metadata to the function locals that need to be passed to other HyperOps
								Value * values[2];
								values[0] = funcAnnotation;
								//Local reference args don't need a context slot but we are adding them here anyway since they tail the arguments of a function and are required during metadata parsing
								values[1] = ConstantInt::get(ctxt, APInt(32, hyperOpArgumentIndex));
								consumedByMetadata = MDNode::get(ctxt, values);
							} else {
								//Add "consumedby" metadata to the function locals that need to be passed to other HyperOps
								Value * values[3];
								values[0] = funcAnnotation;
								//Local reference args don't need a context slot but we are adding them here anyway since they tail the arguments of a function and are required during metadata parsing
								values[1] = ConstantInt::get(ctxt, APInt(32, hyperOpArgumentIndex));
								if ((isProducerStatic && !isStaticHyperOp) || backedgeOfLoop) {
									list<unsigned> tagId = createdHyperOpAndUniqueId[createdFunction];
									//Create a new dynamic tag
									string tag = "<id,";
									for (list<unsigned>::iterator tagItr = tagId.begin(); tagItr != tagId.end(); tagItr++) {
										tag.append(itostr(*tagItr));
										if (*tagItr != tagId.back()) {
											tag.append(",");
										}
									}
									tag.append(">");
									values[2] = MDString::get(ctxt, tag);
								} else {
									//TODO
									values[2] = MDString::get(ctxt, "<prefixId>");
								}
								consumedByMetadata = MDNode::get(ctxt, values);
							}

							Instruction* metadataHost = 0;
							if (isa<AllocaInst>(clonedDefInst)) {
								metadataHost = (AllocaInst*) clonedDefInst;
							} else if (isa<LoadInst>(clonedDefInst) && isArgInList(producerFunction, ((LoadInst*)clonedDefInst)->getOperand(0))) {
								//function argument is passed on to another HyperOp, find the first load instruction from the memory location and add metadata to it
								for (Function::iterator bbItr = producerFunction->begin(); bbItr != producerFunction->end(); bbItr++) {
									for (BasicBlock::iterator instrItr = bbItr->begin(); instrItr != bbItr->end(); instrItr++) {
										if (isa<LoadInst>(instrItr) && ((LoadInst*) &instrItr)->getOperand(0) == ((LoadInst*)clonedDefInst)->getOperand(0)) {
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
								ai->insertBefore(producerFunction->getEntryBlock().getFirstInsertionPt());
								StoreInst* storeInst = new StoreInst(clonedDefInst, ai);
								storeInst->setAlignment(4);
								Instruction* storeAfter;
								if ((&(producerFunction->getEntryBlock())) == ((Instruction*)clonedDefInst)->getParent()) {
									for (auto instItr = producerFunction->getEntryBlock().begin(); instItr != producerFunction->getEntryBlock().end(); instItr++) {
										if (&*instItr == ai || &*instItr == clonedDefInst) {
											storeAfter = instItr;
										}
									}
								} else {
									storeAfter = (Instruction*)clonedDefInst;
								}

								bool seenStoreafter = false;
								Instruction* storeBefore;
								//Find the last phi instruction in the bb containing storeAfter
								for (auto instItr = storeAfter->getParent()->begin(); instItr != storeAfter->getParent()->end(); instItr++) {
									if ((Instruction*) instItr == storeAfter) {
										seenStoreafter = true;
									}
									if (!isa<PHINode>(instItr) && seenStoreafter) {
										storeBefore = instItr;
										break;
									}
								}

								if (storeBefore == storeAfter) {
									storeInst->insertAfter(storeAfter);
								} else {
									storeInst->insertBefore(storeBefore);
								}

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

							//Parent function buffered to ensure that unnecessary control dependences
							if (find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), metadataHost->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()) {
								addedParentsToCurrentHyperOp.push_back(metadataHost->getParent()->getParent());
							}

							if (find(scalarProducers.begin(), scalarProducers.end(), metadataHost->getParent()->getParent()) == scalarProducers.end()) {
								scalarProducers.push_back(metadataHost->getParent()->getParent());
							}
						}
						hyperOpArgumentIndex++;
					}
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
								list<list<CallInst*> > callChainList;

								for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
									list<CallInst*> callChain;
									std::copy(callSiteCopy.begin(), callSiteCopy.end(), std::back_inserter(callChain));
									list<pair<Function*, CallInst*> > cycle = *cycleItr;
									if (cycle.front().first == producerFunction) {
										callChain.pop_back();
										for (list<pair<Function*, CallInst*> >::iterator cycleItr = cycle.begin(); cycleItr != cycle.end(); cycleItr++) {
											callChain.push_back(cycleItr->second);
										}
										callChainList.push_back(callChain);
									}
								}

								for (list<list<CallInst*> >::iterator callChainListItr = callChainList.begin(); callChainListItr != callChainList.end(); callChainListItr++) {
									list<CallInst*> callChain = *callChainListItr;
									//Find the function corresponding to the callChain
									Instruction* clonedInstInstance = getClonedArgument(reachingDefInstr, callChain, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
									if (find(clonedInstructionsToBeLabeled.begin(), clonedInstructionsToBeLabeled.end(), clonedInstInstance) == clonedInstructionsToBeLabeled.end()) {
										clonedInstructionsToBeLabeled.push_back(clonedInstInstance);
									}
								}
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
								bool backedgeOfLoop = false;
								for (auto serialNestedLoopItr = originalSerialLoopBB.begin(); serialNestedLoopItr != originalSerialLoopBB.end(); serialNestedLoopItr++) {
									//All this wouldn't be necessary if loop itself was cached
									list<BasicBlock*> serialLoopBBList = serialNestedLoopItr->first;
									for (auto accumulatedBBItr = accumulatedOriginalBasicBlocks.begin(); accumulatedBBItr != accumulatedOriginalBasicBlocks.end(); accumulatedBBItr++) {
										if (find(serialLoopBBList.begin(), serialLoopBBList.end(), *accumulatedBBItr) != serialLoopBBList.end() && find(originalHeaderBB.begin(), originalHeaderBB.end(), *accumulatedBBItr) != originalHeaderBB.end()) {
											//check if the latch node is in the producer's list of bbs
											list<BasicBlock*> producerBBList = createdHyperOpAndOriginalBasicBlockAndArgMap[producerFunction].first;
											for (auto producerBBItr = producerBBList.begin(); producerBBItr != producerBBList.end(); producerBBItr++) {
												if (find(serialLoopBBList.begin(), serialLoopBBList.end(), *producerBBItr) != serialLoopBBList.end() && find(originalLatchBB.begin(), originalLatchBB.end(), *producerBBItr) != originalLatchBB.end()) {
													backedgeOfLoop = true;
													break;
												}
											}
											if (backedgeOfLoop) {
												break;
											}
										}
									}
									if (backedgeOfLoop) {
										break;
									}
								}
								bool createdHopPartofLoop = false;
								for (auto accumulatedBBItr = accumulatedOriginalBasicBlocks.begin(); accumulatedBBItr != accumulatedOriginalBasicBlocks.end(); accumulatedBBItr++) {
									list<pair<list<BasicBlock*>, LoopIV*> > tempLoopList;
									std::copy(originalParallelLoopBB.begin(), originalParallelLoopBB.end(), std::back_inserter(tempLoopList));
									std::copy(originalSerialLoopBB.begin(), originalSerialLoopBB.end(), std::back_inserter(tempLoopList));
									for (auto bbItr = tempLoopList.begin(); bbItr != tempLoopList.end(); bbItr++) {
										if (find(bbItr->first.begin(), bbItr->first.end(), *accumulatedBBItr) != bbItr->first.end()) {
											createdHopPartofLoop = true;
											break;
										}
									}
									if (createdHopPartofLoop) {
										break;
									}
								}

								//TODO this isn't enough for nested recursion cycles
								bool staticMD = (isProducerStatic && isStaticHyperOp) || (!isProducerStatic && !isStaticHyperOp && !backedgeOfLoop && (producerFunction != createdFunction)) || (isProducerStatic && !isStaticHyperOp && createdHopPartofLoop);
								Value* predValue;
								if (staticMD) {
									Value * values[2];
									values[0] = funcAnnotation;
									values[1] = MDString::get(ctxt, StringRef("1"));
									newPredicateMetadata = MDNode::get(ctxt, values);
									predValue = values[1];
								} else {
									Value * values[3];
									values[0] = funcAnnotation;
									values[1] = MDString::get(ctxt, StringRef("1"));
									if (isProducerStatic && !isStaticHyperOp) {
										list<unsigned> tagId = createdHyperOpAndUniqueId[createdFunction];
										//Create a new dynamic tag
										string tag = "<id,";
										for (list<unsigned>::iterator tagItr = tagId.begin(); tagItr != tagId.end(); tagItr++) {
											tag.append(itostr(*tagItr));
											if (*tagItr != tagId.back()) {
												tag.append(",");
											}
										}
										tag.append(">");
										values[2] = MDString::get(ctxt, tag);
									} else {
										//TODO
										values[2] = MDString::get(ctxt, "<prefixId>");
									}
									newPredicateMetadata = MDNode::get(ctxt, values);
									predValue = values[1];
								}
								metadataList.push_back(newPredicateMetadata);
								ArrayRef<Value*> metadataRef(metadataList);
								MDNode * predicatesRelation = MDNode::get(ctxt, metadataRef);
								metadataHost->setMetadata(HYPEROP_CONTROLS, predicatesRelation);

								//Parent function buffered to ensure that unnecessary control dependences need not exist
								if (staticMD && find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), metadataHost->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()) {
									addedParentsToCurrentHyperOp.push_back(metadataHost->getParent()->getParent());
								}
								if (staticMD && find(predicateProducers.begin(), predicateProducers.end(), metadataHost->getParent()->getParent()) == predicateProducers.end()) {
									predicateProducers.push_back(metadataHost->getParent()->getParent());
									predicateProducerValueMap[metadataHost->getParent()->getParent()] = predValue;
								}
							}
						}
					}
				}
			}

			DEBUG(dbgs() << "\n----------Dealing with conditional branches from other HyperOps----------\n");
			//Find out if there exist any branch instructions leading to the HyperOp, add metadata to the instruction
			for (map<Instruction*, vector<pair<BasicBlock*, int> > >::iterator conditionalBranchSourceItr = conditionalBranchSources.begin(); conditionalBranchSourceItr != conditionalBranchSources.end(); conditionalBranchSourceItr++) {
				Instruction* conditionalBranchInst = conditionalBranchSourceItr->first;
				vector<pair<BasicBlock*, int> > branchOperands = conditionalBranchSourceItr->second;
				BasicBlock* conditionalTargetBB = branchOperands[0].first;
				Value* predicateOperand = conditionalBranchInst->getOperand(0);
				Instruction* clonedInstr;
				Value* clonedPredicateOperand;

				list<CallInst*> newCallSite;
				Function* replicatedCalledFunction = 0;

				for (auto callItr = callSite.begin(); callItr != callSite.end(); callItr++) {
					if (isa<CallInst>(&conditionalTargetBB->front()) && *callItr == ((CallInst*) (&conditionalTargetBB->front()))) {
						break;
					}
					newCallSite.push_back(*callItr);
				}

				if (isa<CallInst>(predicateOperand)) {
					newCallSite.push_back((CallInst*) predicateOperand);
					while (!newCallSite.empty()) {
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
									if (isa<CallInst>(lastBB->front())) {
										appendCall = (CallInst*) &lastBB->front();
									}
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

				if (isa<CallInst>(predicateOperand) && predicateOperand == conditionalBranchInst->getOperand(0)) {
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
					list<list<CallInst*> > callChainList;

					for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
						list<CallInst*> callChain;
						std::copy(callSite.begin(), callSite.end(), std::back_inserter(callChain));
						list<pair<Function*, CallInst*> > cycle = *cycleItr;
						if (cycle.front().first == producerFunction) {
							callChain.pop_back();
							for (list<pair<Function*, CallInst*> >::iterator cycleItr = cycle.begin(); cycleItr != cycle.end(); cycleItr++) {
								callChain.push_back(cycleItr->second);
							}
							callChainList.push_back(callChain);
						}
					}

					list<Value*> clonedInstInstanceCache;
					for (list<list<CallInst*> >::iterator callChainListItr = callChainList.begin(); callChainListItr != callChainList.end(); callChainListItr++) {
						list<CallInst*> callChain = *callChainListItr;
						Value* clonedPredicateOperand = getClonedArgument(predicateOperand, callChain, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
						Instruction* clonedInstInstance = getClonedArgument(conditionalBranchInst, callChain, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
						if (find(clonedInstInstanceCache.begin(), clonedInstInstanceCache.end(), clonedInstInstance) == clonedInstInstanceCache.end()) {
							clonedInstructionsToBeLabeled.push_back(make_pair(clonedInstInstance, clonedPredicateOperand));
							clonedInstInstanceCache.push_back(clonedInstInstance);
						}
					}
				}

				for (list<pair<Instruction*, Value*> >::iterator clonedInstItr = clonedInstructionsToBeLabeled.begin(); clonedInstItr != clonedInstructionsToBeLabeled.end(); clonedInstItr++) {
					Instruction* clonedInstr = clonedInstItr->first;
					Value* predicateOperand = clonedInstItr->second;
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

					MDString* expectedPredicate;
					if (conditionalBranchSourceItr->second.size() == ((BranchInst*) clonedInstr)->getNumSuccessors()) {
						expectedPredicate = MDString::get(ctxt, StringRef("1"));
					} else {
						//Are there only two operands in a branch instruction always or are there more?
						assert(conditionalBranchSourceItr->second.size() == 1 && "there are more than two targets to branch");
						unsigned positionToBeUpdated = ConstantInt::get(ctxt, APInt(32, conditionalBranchSourceItr->second[0].second))->getZExtValue();
						if (positionToBeUpdated == 0) {
							expectedPredicate = MDString::get(ctxt, StringRef("1"));
						} else {
							expectedPredicate = MDString::get(ctxt, StringRef("0"));
						}
					}
					//TODO this isn't enough for nested recursion cycles
					bool backedgeOfLoop = false;
					for (auto serialNestedLoopItr = originalSerialLoopBB.begin(); serialNestedLoopItr != originalSerialLoopBB.end(); serialNestedLoopItr++) {
						//All this wouldn't be necessary if loop itself was cached
						list<BasicBlock*> serialLoopBBList = serialNestedLoopItr->first;
						for (auto accumulatedBBItr = accumulatedOriginalBasicBlocks.begin(); accumulatedBBItr != accumulatedOriginalBasicBlocks.end(); accumulatedBBItr++) {
							if (find(serialLoopBBList.begin(), serialLoopBBList.end(), *accumulatedBBItr) != serialLoopBBList.end() && find(originalHeaderBB.begin(), originalHeaderBB.end(), *accumulatedBBItr) != originalHeaderBB.end()) {
								//check if the latch node is in the producer's list of bbs
								list<BasicBlock*> producerBBList = createdHyperOpAndOriginalBasicBlockAndArgMap[producerFunction].first;
								for (auto producerBBItr = producerBBList.begin(); producerBBItr != producerBBList.end(); producerBBItr++) {
									if (find(serialLoopBBList.begin(), serialLoopBBList.end(), *producerBBItr) != serialLoopBBList.end() && find(originalLatchBB.begin(), originalLatchBB.end(), *producerBBItr) != originalLatchBB.end()) {
										backedgeOfLoop = true;
										break;
									}
								}
								if (backedgeOfLoop) {
									break;
								}
							}
						}
						if (backedgeOfLoop) {
							break;
						}
					}
					//TODO this isn't enough for nested recursion cycles
					bool staticMD = (isProducerStatic && isStaticHyperOp) || (!isProducerStatic && !isStaticHyperOp && !backedgeOfLoop && (producerFunction != createdFunction));
					if (staticMD) {
						Value * values[2];
						values[0] = funcAnnotation;
						//TODO predicate
						values[1] = expectedPredicate;
						newPredicateMetadata = MDNode::get(ctxt, values);
					} else {
						Value * values[3];
						values[0] = funcAnnotation;
						//TODO predicate
						values[1] = expectedPredicate;
						if (isProducerStatic && !isStaticHyperOp) {
							list<unsigned> tagId = createdHyperOpAndUniqueId[createdFunction];
							//Create a new dynamic tag
							string tag = "<id,";
							for (list<unsigned>::iterator tagItr = tagId.begin(); tagItr != tagId.end(); tagItr++) {
								tag.append(itostr(*tagItr));
								if (*tagItr != tagId.back()) {
									tag.append(",");
								}
							}
							tag.append(">");
							values[2] = MDString::get(ctxt, tag);
						} else {
							//TODO
							values[2] = MDString::get(ctxt, "<prefixId>");
						}
						newPredicateMetadata = MDNode::get(ctxt, values);
					}
					metadataList.push_back(newPredicateMetadata);
					MDNode * predicatesRelation = MDNode::get(ctxt, metadataList);
					metadataHost->setMetadata(HYPEROP_CONTROLS, predicatesRelation);

					for (unsigned branchOperandIndex = 0; branchOperandIndex != conditionalBranchSourceItr->second.size(); branchOperandIndex++) {
						if (conditionalBranchSourceItr->second[branchOperandIndex].second != -1) {
							//Update the cloned conditional branch instruction with the right target
							int conditionalSlot = conditionalBranchSourceItr->second[branchOperandIndex].second;
							BasicBlock* targetBB = ((BranchInst*) conditionalBranchInst)->getSuccessor(conditionalSlot);
							if ((!isa<CallInst>(&targetBB->front()) && find(accumulatedOriginalBasicBlocks.begin(), accumulatedOriginalBasicBlocks.end(), targetBB) == accumulatedOriginalBasicBlocks.end())
									|| (isa<CallInst>(&targetBB->front()) && find(accumulatedOriginalBasicBlocks.begin(), accumulatedOriginalBasicBlocks.end(), (&((CallInst*) &targetBB->front())->getCalledFunction()->getEntryBlock())) == accumulatedOriginalBasicBlocks.end())) {
								continue;
							}
							int positionToBeUpdated = ConstantInt::get(ctxt, APInt(32, conditionalSlot))->getZExtValue();
							Instruction* retInstOfProducer = retInstMap.find(clonedInstr->getParent()->getParent())->second;
							((BranchInst*) clonedInstr)->setSuccessor(positionToBeUpdated, retInstOfProducer->getParent());
						}

						//Fold the branch instruction if all jumps are to the same bb
						list<BasicBlock*> branchTargets;
						for (unsigned successorIndex = 0; successorIndex != ((BranchInst*) clonedInstr)->getNumSuccessors(); successorIndex++) {
							if (find(branchTargets.begin(), branchTargets.end(), ((BranchInst*) clonedInstr)->getSuccessor(successorIndex)) == branchTargets.end()) {
								branchTargets.push_back(((BranchInst*) clonedInstr)->getSuccessor(successorIndex));
							}
						}
					}
					if (staticMD && find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), metadataHost->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()) {
						addedParentsToCurrentHyperOp.push_back(metadataHost->getParent()->getParent());
					}
					if (staticMD && find(predicateProducers.begin(), predicateProducers.end(), metadataHost->getParent()->getParent()) == predicateProducers.end()) {
						predicateProducers.push_back(metadataHost->getParent()->getParent());
						predicateProducerValueMap[metadataHost->getParent()->getParent()] = (Value*) expectedPredicate;
					}
				}
			}
			DEBUG(dbgs() << "\n----------Dealing with unconditional branches from other HyperOps----------\n");
			//Remove unconditional branch instruction, add the annotation to the alloca instruction of the branch
			for (auto unconditionalBranchSourceItr = unconditionalBranchSources.begin(); unconditionalBranchSourceItr != unconditionalBranchSources.end(); unconditionalBranchSourceItr++) {
				Value* unconditionalBranchInstr = unconditionalBranchSourceItr->first;
				Value* originalUnconditionalBranchInstr = unconditionalBranchInstr;
				//There can only be one basic block that happens to be the target
				BasicBlock* targetBB;
				if (unconditionalBranchSourceItr->second.empty()) {
					targetBB = ((BranchInst*) unconditionalBranchInstr)->getSuccessor(0);
				} else {
					targetBB = unconditionalBranchSourceItr->second[0].first;
				}
				Instruction* clonedInstr;
				Function* replicatedCalledFunction = 0;
				list<BasicBlock*> parentBBList;

				list<CallInst*> newCallSite;
				for (auto callItr = callSite.begin(); callItr != callSite.end(); callItr++) {
					if (isa<CallInst>(&targetBB->front()) && *callItr == ((CallInst*) (&targetBB->front()))) {
						break;
					}
					newCallSite.push_back(*callItr);
				}

				if (isa<CallInst>(((Instruction*) unconditionalBranchInstr)->getParent()->front())) {
					newCallSite.push_back((CallInst*) &targetBB->front());
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

				if (unconditionalBranchInstr == unconditionalBranchSourceItr->first && isa<CallInst>(((Instruction*) unconditionalBranchInstr)->getParent()->front())) {
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

				bool isProducerStatic = createdHyperOpAndType[clonedInstr->getParent()->getParent()];
				list<Instruction*> clonedInstructionsToBeLabeled;
				clonedInstructionsToBeLabeled.push_back(clonedInstr);
				if (isa<Instruction>(unconditionalBranchInstr)) {
					Function* producerFunction = ((Instruction*) unconditionalBranchInstr)->getParent()->getParent();
					//Find the replicas of the consumer HyperOp which also need to be annotated, hence populating a list
					if (isProducerStatic && !isStaticHyperOp) {
						//replicate the meta data in the last instance of the HyperOp corresponding to the last HyperOp in the cycle
						list<list<CallInst*> > callChainList;

						for (list<list<pair<Function*, CallInst*> > >::iterator cycleItr = cyclesInCallGraph.begin(); cycleItr != cyclesInCallGraph.end(); cycleItr++) {
							list<CallInst*> callChain;
							std::copy(callSite.begin(), callSite.end(), std::back_inserter(callChain));
							list<pair<Function*, CallInst*> > cycle = *cycleItr;
							if (cycle.front().first == producerFunction) {
								callChain.pop_back();
								for (list<pair<Function*, CallInst*> >::iterator cycleItr = cycle.begin(); cycleItr != cycle.end(); cycleItr++) {
									callChain.push_back(cycleItr->second);
								}
								callChainList.push_back(callChain);
							}
						}

						for (list<list<CallInst*> >::iterator callChainListItr = callChainList.begin(); callChainListItr != callChainList.end(); callChainListItr++) {
							list<CallInst*> callChain = *callChainListItr;
							//Find the function corresponding to the callChain
							Instruction* clonedInstInstance = getClonedArgument(unconditionalBranchInstr, callChain, createdHyperOpAndCallSite, functionOriginalToClonedInstructionMap);
							clonedInstructionsToBeLabeled.push_back(clonedInstInstance);
						}
					}
				}

				for (list<Instruction*>::iterator clonedInstItr = clonedInstructionsToBeLabeled.begin(); clonedInstItr != clonedInstructionsToBeLabeled.end(); clonedInstItr++) {
					Instruction* clonedInstr = *clonedInstItr;
					Instruction* retInstOfProducer = retInstMap.find(clonedInstr->getParent()->getParent())->second;
					BasicBlock* targetBB = (BasicBlock*) clonedInstr->getOperand(0);
					if (isa<BranchInst>(clonedInstr)) {
						//Update the branch instruction to jump to the return basic block
						if (((BranchInst*) clonedInstr)->isUnconditional()) {
							//Update the branch instruction to jump to the return basic block
							clonedInstr->setOperand(0, retInstOfProducer->getParent());
						} else {
							for (unsigned branchOperandIndex = 0; branchOperandIndex != unconditionalBranchSourceItr->second.size(); branchOperandIndex++) {
								if (unconditionalBranchSourceItr->second[branchOperandIndex].second != -1) {
									//Update the cloned conditional branch instruction with the right target
									int positionToBeUpdated = ConstantInt::get(ctxt, APInt(32, unconditionalBranchSourceItr->second[branchOperandIndex].second))->getZExtValue();
									Instruction* retInstOfProducer = retInstMap.find(clonedInstr->getParent()->getParent())->second;
									((BranchInst*) clonedInstr)->setSuccessor(positionToBeUpdated, retInstOfProducer->getParent());
									list<BasicBlock*> uniqueSuccessors;
									for (int i = 0; i < ((BranchInst*) clonedInstr)->getNumSuccessors(); i++) {
										if (find(uniqueSuccessors.begin(), uniqueSuccessors.end(), ((BranchInst*) clonedInstr)->getSuccessor(i)) == uniqueSuccessors.end()) {
											uniqueSuccessors.push_back(((BranchInst*) clonedInstr)->getSuccessor(i));
										}
									}
								}
							}
						}
					}

					//If target is already accumulated, ignore
					if (find(accumulatedOriginalBasicBlocks.begin(), accumulatedOriginalBasicBlocks.end(), clonedInstr->getParent()) != accumulatedOriginalBasicBlocks.end()) {
						continue;
					}
					//Check if the branch is to a loop, ensure that the edges are added
					list<pair<list<BasicBlock*>, LoopIV*> > tempLoopList;
					std::copy(originalParallelLoopBB.begin(), originalParallelLoopBB.end(), std::back_inserter(tempLoopList));
					std::copy(originalSerialLoopBB.begin(), originalSerialLoopBB.end(), std::back_inserter(tempLoopList));
					bool syncRequired = false;
					bool loopHop = false;
					//check if there already exists some other dependence between the source and the target
					for (auto tempLoopItr = tempLoopList.begin(); tempLoopItr != tempLoopList.end(); tempLoopItr++) {
						list<BasicBlock*> loopBBList = tempLoopItr->first;
						if (find(loopBBList.begin(), loopBBList.end(), targetBB) != loopBBList.end()) {
							loopHop = true;
							if (find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), clonedInstr->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()
									&& find(predicateProducers.begin(), predicateProducers.end(), clonedInstr->getParent()->getParent()) == predicateProducers.end()) {
								syncRequired = true;
								break;
							}
						}
					}
					if (!loopHop) {
						syncRequired = (find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), clonedInstr->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()
								&& find(predicateProducers.begin(), predicateProducers.end(), clonedInstr->getParent()->getParent()) == predicateProducers.end());
					}
					if (syncRequired) {
						bool backedgeOfSerialLoop = false;
						//Check if the sync is a backedge
						for (auto serialNestedLoopItr = originalSerialLoopBB.begin(); serialNestedLoopItr != originalSerialLoopBB.end(); serialNestedLoopItr++) {
							//All this wouldn't be necessary if loop point itself is cached
							list<BasicBlock*> serialLoopBBList = serialNestedLoopItr->first;
							for (auto accumulatedBBItr = accumulatedOriginalBasicBlocks.begin(); accumulatedBBItr != accumulatedOriginalBasicBlocks.end(); accumulatedBBItr++) {
								if (find(serialLoopBBList.begin(), serialLoopBBList.end(), *accumulatedBBItr) != serialLoopBBList.end() && find(originalHeaderBB.begin(), originalHeaderBB.end(), *accumulatedBBItr) != originalHeaderBB.end()) {
									//check if the latch node is in the producer's list of bbs
									list<BasicBlock*> producerBBList = createdHyperOpAndOriginalBasicBlockAndArgMap[clonedInstr->getParent()->getParent()].first;
									for (auto producerBBItr = producerBBList.begin(); producerBBItr != producerBBList.end(); producerBBItr++) {
										if (find(serialLoopBBList.begin(), serialLoopBBList.end(), *producerBBItr) != serialLoopBBList.end() && find(originalLatchBB.begin(), originalLatchBB.end(), *producerBBItr) != originalLatchBB.end()) {
											backedgeOfSerialLoop = true;
											break;
										}
									}
									if (backedgeOfSerialLoop) {
										break;
									}
								}
							}
							if (backedgeOfSerialLoop) {
								break;
							}
						}

						if (backedgeOfSerialLoop) {
							continue;
						}
						bool backedgeOfParallelLoop = false;
						BasicBlock* parallelLoopLatchBlock;
						//Check if the sync is a backedge
						for (auto parallelLoopItr = originalParallelLoopBB.begin(); parallelLoopItr != originalParallelLoopBB.end(); parallelLoopItr++) {
							//All this wouldn't be necessary if loop point itself is cached
							list<BasicBlock*> parallelLoopBBList = parallelLoopItr->first;
							for (auto accumulatedBBItr = accumulatedOriginalBasicBlocks.begin(); accumulatedBBItr != accumulatedOriginalBasicBlocks.end(); accumulatedBBItr++) {
								if (find(parallelLoopBBList.begin(), parallelLoopBBList.end(), *accumulatedBBItr) != parallelLoopBBList.end() && find(originalHeaderBB.begin(), originalHeaderBB.end(), *accumulatedBBItr) != originalHeaderBB.end()) {
									//check if the latch node is in the producer's list of bbs
									list<BasicBlock*> producerBBList = createdHyperOpAndOriginalBasicBlockAndArgMap[clonedInstr->getParent()->getParent()].first;
									for (auto producerBBItr = producerBBList.begin(); producerBBItr != producerBBList.end(); producerBBItr++) {
										if (find(parallelLoopBBList.begin(), parallelLoopBBList.end(), *producerBBItr) != parallelLoopBBList.end() && find(originalLatchBB.begin(), originalLatchBB.end(), *producerBBItr) != originalLatchBB.end()) {
											parallelLoopLatchBlock = *producerBBItr;
											backedgeOfParallelLoop = true;
											break;
										}
									}
									if (backedgeOfParallelLoop) {
										break;
									}
								}
							}
							if (backedgeOfParallelLoop) {
								break;
							}
						}

						Function* producerFunction;
						Function* consumerFunction;
						list<Function*> tempPredicateProducers = predicateProducers;
						if (!backedgeOfSerialLoop && !backedgeOfParallelLoop) {
							producerFunction = clonedInstr->getParent()->getParent();
							consumerFunction = createdFunction;
						} else if (backedgeOfParallelLoop) {
							//Find the cloned bb of parallelLoopLatchBB
							LoopInfo & loopInfo = getAnalysis<LoopInfo>(*parallelLoopLatchBlock->getParent());
							BasicBlock* exitBlock = loopInfo.getLoopFor(parallelLoopLatchBlock)->getExitBlock();
							for (auto funcItr = functionOriginalToClonedBBMap.begin(); funcItr != functionOriginalToClonedBBMap.end(); funcItr++) {
								map<BasicBlock*, BasicBlock*> bbCloneMap = funcItr->second;
								if (bbCloneMap.find(parallelLoopLatchBlock) != bbCloneMap.end()) {
									producerFunction = bbCloneMap[parallelLoopLatchBlock]->getParent();
								}
								if (bbCloneMap.find(exitBlock) != bbCloneMap.end()) {
									consumerFunction = bbCloneMap[exitBlock]->getParent();
								}
							}
							tempPredicateProducers = addedPredicateSourcesMap[consumerFunction];
						}

						//Check if a sync can be added or a predicate would be required
						if ((!backedgeOfSerialLoop && !backedgeOfParallelLoop) || backedgeOfParallelLoop) {
							//TODO this isn't enough for nested recursion cycles
							//backedgeofparallelloop isnt considered because such a backedge is translated to a sync with exit hyperOp and is treated static
							bool staticMD = (isProducerStatic && isStaticHyperOp) || (!isProducerStatic && !isStaticHyperOp && !backedgeOfSerialLoop);
							vector<Value*> nodeList;
							MDNode* newPredicateMetadata;
							Value* predicateValue = NULL;
							bool addPredicate = false;
							if (!predicateProducers.empty()) {
								//find the predicate value, there can only be one
								predicateValue = predicateProducerValueMap[tempPredicateProducers.front()];
								addPredicate = true;
							}

							int predicateExpected = 1;
							if (addPredicate) {
								predicateExpected = atoi(((MDString*) predicateValue)->getString().data());
							}
							if (staticMD) {
								if (addPredicate) {
									Value* values[2];
									values[0] = hyperOpAndAnnotationMap[consumerFunction];
									values[1] = predicateValue;
									newPredicateMetadata = MDNode::get(ctxt, values);
								} else {
									Value* values[1];
									values[0] = hyperOpAndAnnotationMap[consumerFunction];
									newPredicateMetadata = MDNode::get(ctxt, values);
								}
							} else {
								if (addPredicate) {
									Value* values[3];
									values[0] = hyperOpAndAnnotationMap[consumerFunction];
									if ((isProducerStatic && !isStaticHyperOp) || backedgeOfSerialLoop) {
										list<unsigned> tagId = createdHyperOpAndUniqueId[createdFunction];
										//Create a new dynamic tag
										string tag = "<id,";
										for (list<unsigned>::iterator tagItr = tagId.begin(); tagItr != tagId.end(); tagItr++) {
											tag.append(itostr(*tagItr));
											if (*tagItr != tagId.back()) {
												tag.append(",");
											}
										}
										tag.append(">");
										values[2] = MDString::get(ctxt, tag);
									} else {
										//TODO
										values[2] = MDString::get(ctxt, "<prefixId>");
									}
									values[1] = predicateValue;
									newPredicateMetadata = MDNode::get(ctxt, values);
								} else {
									Value* values[2];
									values[0] = hyperOpAndAnnotationMap[consumerFunction];
									if ((isProducerStatic && !isStaticHyperOp) || backedgeOfSerialLoop) {
										list<unsigned> tagId = createdHyperOpAndUniqueId[createdFunction];
										//Create a new dynamic tag
										string tag = "<id,";
										for (list<unsigned>::iterator tagItr = tagId.begin(); tagItr != tagId.end(); tagItr++) {
											tag.append(itostr(*tagItr));
											if (*tagItr != tagId.back()) {
												tag.append(",");
											}
										}
										tag.append(">");
										values[1] = MDString::get(ctxt, tag);
									} else {
										//TODO
										values[1] = MDString::get(ctxt, "<prefixId>");
									}
									newPredicateMetadata = MDNode::get(ctxt, values);
								}
							}
							nodeList.push_back(newPredicateMetadata);
							MDNode* node = MDNode::get(ctxt, nodeList);
							AllocaInst* ai = new AllocaInst(Type::getInt1Ty(ctxt));
							ai->setAlignment(4);
							ai->insertBefore(producerFunction->front().getFirstInsertionPt());
							StoreInst* storeInst = new StoreInst(ConstantInt::get(ctxt, APInt(predicateExpected, 1)), ai);
							storeInst->setAlignment(4);
							storeInst->insertBefore(retInstMap[producerFunction]);
							if (addPredicate) {
								ai->setMetadata(HYPEROP_CONTROLS, node);
							} else {
								ai->setMetadata(HYPEROP_SYNC, node);
								errs() << "sync md added from producer " << ai->getParent()->getParent()->getName() << ":";
								newPredicateMetadata->dump();
							}
							if (find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), ai->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()) {
								addedParentsToCurrentHyperOp.push_back(ai->getParent()->getParent());
							}
						}
					}
				}
			}
			addedPredicateSourcesMap[createdFunction] = predicateProducers;
			addedDataSourcesMap[createdFunction] = addedParentsToCurrentHyperOp;

			createdFunction->dump();
		}

		DEBUG(dbgs() << "\n----------Adding sync edges between dangling hyperOps and end HyperOp----------\n");
		for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHyperOpItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHyperOpItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHyperOpItr++) {
			Function* createdFunction = createdHyperOpItr->first;
//			bool isStaticHyperOp = createdHyperOpAndType[createdFunction];
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
					MDNode* newPredicateMetadata;
					Value* values[1];
					values[0] = hyperOpAndAnnotationMap[endHyperOp];
					newPredicateMetadata = MDNode::get(ctxt, values);
					nodeList.push_back(newPredicateMetadata);
					MDNode* mdNode = MDNode::get(ctxt, nodeList);
					//Create a sync edge between the current HyperOp and the last HyperOp
					//We use sync edge here because adding a predicate to the end hyperop
					createdFunction->begin()->front().setMetadata(HYPEROP_SYNC, mdNode);
					syncMDNodeList.push_back(newPredicateMetadata);
				}
			}
		}

		DEBUG(dbgs() << "\n----------Adding a sync from entry hyperop of the function if there are no incoming edges to the hyperop----------\n");
		for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHyperOpItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHyperOpItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHyperOpItr++) {
			Function* createdFunction = createdHyperOpItr->first;
			MDNode* funcAnnotation = hyperOpAndAnnotationMap.find(createdFunction)->second;
			list<Function*> addedParentsToCurrentHyperOp = addedDataSourcesMap[createdFunction];
			list<CallInst*> callSite = createdHyperOpAndCallSite[createdFunction];
			list<BasicBlock*> accumulatedOriginalBasicBlocks = createdHyperOpItr->second.first;
			bool isStaticHyperOp = createdHyperOpAndType[createdFunction];
			//Check if a HyperOp has dynamically added parents
			if (addedParentsToCurrentHyperOp.empty()) {
				for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHyperOpItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHyperOpItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHyperOpItr++) {
					for (Function::iterator bbItr = createdHyperOpItr->first->begin(); bbItr != createdHyperOpItr->first->end(); bbItr++) {
						for (BasicBlock::iterator instItr = bbItr->begin(); instItr != bbItr->end(); instItr++) {
							MDNode* consumedByMDNode = instItr->getMetadata(HYPEROP_CONSUMED_BY);
							if (consumedByMDNode != 0) {
								for (unsigned i = 0; i < consumedByMDNode->getNumOperands(); i++) {
									MDNode* consumer = (MDNode*) ((MDNode*) (consumedByMDNode->getOperand(i)))->getOperand(0);
									if (funcAnnotation == consumer) {
										bool dynamicInstance = false;
										if (((MDNode*) (consumedByMDNode->getOperand(i)))->getNumOperands() == 5) {
											dynamicInstance = true;
											StringRef dynamicInstanceId = ((MDString*) ((MDNode*) (consumedByMDNode->getOperand(i)))->getOperand(4))->getString();
											list<StringRef> parsedId = parseInstanceIdString(dynamicInstanceId);
											//This is clearly not complete
											if (parsedId.back().compare("id") == 0 || isStaticHyperOp) {
												dynamicInstance = false;
											}
										}
										if (!dynamicInstance) {
											addedParentsToCurrentHyperOp.push_back(createdHyperOpItr->first);
											break;
										}
									}
								}
							}
							if (addedParentsToCurrentHyperOp.empty()) {
								MDNode* controlledByMDNode = instItr->getMetadata(HYPEROP_CONTROLS);
								if (controlledByMDNode != 0) {
									for (unsigned i = 0; i < controlledByMDNode->getNumOperands(); i++) {
										MDNode* consumer = (MDNode*) ((MDNode*) (controlledByMDNode->getOperand(i)))->getOperand(0);
										if (funcAnnotation == consumer) {
											bool dynamicInstance = false;
											errs() << "controls by md node from source " << bbItr->getParent()->getName() << " at i:" << i << "\n";
											controlledByMDNode->dump();
											if (((MDNode*) (controlledByMDNode->getOperand(i)))->getNumOperands() == 3) {
												dynamicInstance = true;
												StringRef dynamicInstanceId = ((MDString*) ((MDNode*) (controlledByMDNode->getOperand(i)))->getOperand(2))->getString();
												list<StringRef> parsedId = parseInstanceIdString(dynamicInstanceId);
												//This is clearly not complete
												if (parsedId.back().compare("id") == 0 || isStaticHyperOp) {
													dynamicInstance = false;
												}
											}
											if (!dynamicInstance) {
												addedParentsToCurrentHyperOp.push_back(createdHyperOpItr->first);
												break;
											}
										}
									}
								}
							}
							if (addedParentsToCurrentHyperOp.empty()) {
								MDNode* syncMDNode = instItr->getMetadata(HYPEROP_SYNC);
								if (syncMDNode != 0) {
									for (unsigned i = 0; i < syncMDNode->getNumOperands(); i++) {
										MDNode* consumer = (MDNode*) ((MDNode*) (syncMDNode->getOperand(i)))->getOperand(0);
										if (funcAnnotation == consumer) {
											bool dynamicInstance = false;
											if (((MDNode*) (syncMDNode->getOperand(i)))->getNumOperands() == 3) {
												dynamicInstance = true;
												StringRef dynamicInstanceId = ((MDString*) ((MDNode*) (syncMDNode->getOperand(i)))->getOperand(2))->getString();
												list<StringRef> parsedId = parseInstanceIdString(dynamicInstanceId);
												//This is clearly not complete
												if (parsedId.back().compare("id") == 0 || isStaticHyperOp) {
													dynamicInstance = false;
												}
											}
											if (!dynamicInstance) {
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
			}
			if (addedParentsToCurrentHyperOp.empty() && startHyperOp != createdFunction && isStaticHyperOp) {
				//Find the corresponding function with entry node, this can't be from the start hyperop always because
				Function *entryHyperOp = NULL;
				if (callSite.empty()) {
					entryHyperOp = startHyperOp;
				} else {
					//Check if the current HyperOp contains the entry of callsite, pop once otherwise
					list<BasicBlock*> tempAccumulatedBBList = accumulatedOriginalBasicBlocks;
					while (!callSite.empty()) {
						CallInst* call = callSite.back();
						bool accumulatedEntry = false;
						for (auto accumulatedBB : tempAccumulatedBBList) {
							if ((&accumulatedBB->getParent()->getEntryBlock()) == accumulatedBB) {
								accumulatedEntry = true;
							}
						}

						if (!accumulatedEntry) {
							//check if accumulated entry is predicated i.e., has conditional sources
							//Find the HyperOp created corresponding to the callsite containing the entry bb
							list<Function*> functionList = getFunctionAtCallSite(callSite, createdHyperOpAndCallSite);
							for (auto function : functionList) {
								pair<list<BasicBlock*>, HyperOpArgumentList> bbList = createdHyperOpAndOriginalBasicBlockAndArgMap[function];
								if (find(bbList.first.begin(), bbList.first.end(), &(call->getCalledFunction()->getEntryBlock())) != bbList.first.end() && !addedPredicateSourcesMap[function].empty()) {
									//check if the function is predicated; set it as the entry from where the current function needs to be reached
									entryHyperOp = function;
									break;
								}
							}
						}

						if (entryHyperOp) {
							break;
						}
						//pop the callSite by 1
						callSite.pop_back();
						list<Function*> createdFunctions = getFunctionAtCallSite(callSite, createdHyperOpAndCallSite);
						bool listUpdated = false;
						for (auto createdFunc : createdFunctions) {
							for (auto bbItr : createdHyperOpAndOriginalBasicBlockAndArgMap[createdFunc].first) {
								if (isa<CallInst>(bbItr->front()) && functionOriginalToClonedInstructionMap[createdFunc][call] == (&bbItr->front())) {
									tempAccumulatedBBList = createdHyperOpAndOriginalBasicBlockAndArgMap[createdFunc].first;
									listUpdated = true;
									break;
								}
							}
							if (listUpdated) {
								break;
							}
						}
					}
					if (callSite.empty()) {
						entryHyperOp = startHyperOp;
					}
				}

				//There are no incoming edge to the hyperop, add a predicate edge from the entry HyperOp
				vector<Value*> nodeList;
				Value* values[1];
				values[0] = hyperOpAndAnnotationMap[createdFunction];
				MDNode* newPredicateMetadata = MDNode::get(ctxt, values);
				nodeList.push_back(newPredicateMetadata);
				MDNode* node = MDNode::get(ctxt, nodeList);
				AllocaInst* ai = new AllocaInst(Type::getInt1Ty(ctxt));
				ai->setAlignment(4);
				ai->insertBefore(entryHyperOp->begin()->getFirstInsertionPt());
				StoreInst* storeInst = new StoreInst(ConstantInt::get(ctxt, APInt(1, 1)), ai);
				storeInst->setAlignment(4);
				storeInst->insertBefore(retInstMap[entryHyperOp]);
				ai->setMetadata(HYPEROP_SYNC, node);
				list<Instruction*> incomingEdgesToHop;
				incomingEdgesToHop.push_back(ai);
				addedParentsToCurrentHyperOp.push_back(ai->getParent()->getParent());
			}
		}

		DEBUG(dbgs() << "\n-----------Adding dummy entry nodes to phi nodes with null entry operands-----------\n");
		for (Module::iterator functionItr = M.begin(); functionItr != M.end(); functionItr++) {
			//Remove old functions from module
			if (createdHyperOpAndOriginalBasicBlockAndArgMap.find(functionItr) != createdHyperOpAndOriginalBasicBlockAndArgMap.end() && !functionItr->isIntrinsic()) {
				Function* functionCreated = functionItr;
				BasicBlock* newDummyBB = NULL;
				for (auto bbItr = functionCreated->begin(); bbItr != functionCreated->end(); bbItr++) {
					for (auto instItr = bbItr->begin(); instItr != bbItr->end(); instItr++) {
						Instruction* instr = instItr;
						if (isa<PHINode>(instr)) {
							for (int i = 0; i < ((PHINode*) instr)->getNumIncomingValues(); i++) {
								if (((PHINode*) instr)->getIncomingBlock(i) == NULL) {
									//Add a new dummy entry node
									if (newDummyBB == NULL) {
										string newEntryNode = functionCreated->getName();
										newEntryNode.append(".entry");
										newDummyBB = BasicBlock::Create(ctxt, newEntryNode, functionCreated, instr->getParent());
										Instruction* jumpInstruction = BranchInst::Create(instr->getParent(), newDummyBB);
									}
									((PHINode*) instr)->setIncomingBlock(i, newDummyBB);
								}
							}
						}
					}
				}
			}
		}

		//TODO I don't remember why this exists, gotta check from commit history
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
		DEBUG(dbgs() << "Completed generating HyperOps\n");
		HyperOpInteractionGraph* graph = HyperOpMetadataParser::parseMetadata(&M);
		graph->print(errs());
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
};
char HyperOpCreationPass::ID = 2;
char* HyperOpCreationPass::NEW_NAME = "newName";
static RegisterPass<HyperOpCreationPass> X("HyperOpCreationPass", "Pass to create HyperOps");

#define MAKE_GRAPH_STRUCTURED 1

struct REDEFINEIRPass: public ModulePass {
	static char ID;
	static char* NEW_NAME;
	/* Maximum context frame size in words, not bytes */
	static const unsigned MAX_CONTEXT_FRAME_SIZE = 15;
	static const unsigned MAX_ROW = 1;
	static const unsigned MAX_COL = 1;
	static const unsigned FRAME_SIZE_BYTES = 64;

	REDEFINEIRPass() :
			ModulePass(ID) {
	}

	virtual void getAnalysisUsage(AnalysisUsage &AU) const {
		//Mandatory merge return to be invoked on each function
//		AU.addRequired<HyperOpCreationPass>();
	}

	void addRangeLoopConstructs(HyperOp* child, Function* vertexFunction, const Module &M, BasicBlock** loopBegin, BasicBlock** loopBody, BasicBlock** loopEnd, BasicBlock** insertInBB, Value* lowerBound, Value* upperBound, Value** phiValue) {
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

		BranchInst* loopBodyJump = BranchInst::Create(*loopBegin, (*insertInBB)->getTerminator());
		(*insertInBB)->getTerminator()->removeFromParent();

		PHINode* phiNode = PHINode::Create(Type::getInt32Ty(M.getContext()), 0, "", *loopBegin);
		phiNode->addIncoming(lowerBound, *insertInBB);
		*phiValue = phiNode;
		CmpInst* cmpInst = CmpInst::Create(Instruction::ICmp, llvm::CmpInst::ICMP_UGT, phiNode, upperBound, "cmpinst", *loopBegin);
		BranchInst* bgeItrInst = BranchInst::Create(*loopEnd, *loopBody, cmpInst, *loopBegin);
		BranchInst* loopEndJump = BranchInst::Create(*loopBegin, *loopBody);
		ReturnInst* ret = ReturnInst::Create(M.getContext(), *loopEnd);
	}

	static inline void getSyncCount(BasicBlock** bb, list<SyncValue> syncCount, Module *M, Value** syncCountValue) {
		Value* localSymCount = NULL;
		bool first = true;
		for (list<SyncValue>::iterator syncCountIterator = syncCount.begin(); syncCountIterator != syncCount.end(); syncCountIterator++) {
			Value* currentSyncCount;
			if (syncCountIterator->getType() == SyncValueType::INT_SYNC_TYPE) {
				currentSyncCount = ConstantInt::get(Type::getInt32Ty(M->getContext()), syncCountIterator->getInt());
			} else if (syncCountIterator->getType() == SyncValueType::HYPEROP_SYNC_TYPE) {
				Value* min = syncCountIterator->getHyperOp()->getRangeLowerBound();
				Value* max = syncCountIterator->getHyperOp()->getRangeUpperBound();
				Value * stride = syncCountIterator->getHyperOp()->getStride();
				Value* difference = BinaryOperator::Create(Instruction::BinaryOps::Sub, max, min, "diff", &(*bb)->back());
				currentSyncCount = BinaryOperator::Create(Instruction::BinaryOps::UDiv, difference, stride, "", &(*bb)->back());
			}
			if (localSymCount == NULL) {
				localSymCount = currentSyncCount;
			} else {
				localSymCount = BinaryOperator::Create(Instruction::BinaryOps::Add, currentSyncCount, localSymCount, "syncCount", &(*bb)->back());
			}
		}
		*syncCountValue = localSymCount;
	}

	static void getSourceData(HyperOpEdge* childEdge, HyperOp* vertex, HyperOp* child, Value** originalSourceData) {
		HyperOpInteractionGraph * graph = vertex->getParentGraph();
		Value *edgeValue = childEdge->getValue();
		*originalSourceData = childEdge->getValue();
		if (childEdge->getType() == HyperOpEdge::SCALAR || childEdge->getType() == HyperOpEdge::LOCAL_REFERENCE || childEdge->getType() == HyperOpEdge::PREDICATE) {
			/* Find the parent that passed this argument*/
			*originalSourceData = graph->getAllocInstrForLocalReferenceData(edgeValue, vertex);
		}
	}

	static void loadAndStoreData(Value* sourceData, Value* originalData, HyperOpEdge* parentEdge, HyperOp* edgeSource, Value* targetMemFrameBaseAddress, LLVMContext & ctxt, BasicBlock** insertInBB, BasicBlock** nextInsertionPoint) {
		unsigned targetOffsetInBytes = parentEdge->getMemoryOffsetInTargetFrame();
		Value* targetMemBaseInst = BinaryOperator::CreateNUWAdd(targetMemFrameBaseAddress, ConstantInt::get(ctxt, APInt(32, targetOffsetInBytes)));

		Type* dataType = originalData->getType();
		//Map of primitive data types and their memory locations
		list<pair<Type*, unsigned> > primitiveTypesMap;
		list<pair<Type*, int> > containedTypesForTraversal;
		if (originalData->getType()->isArrayTy()) {
			containedTypesForTraversal.push_front(make_pair(dataType, dataType->getArrayNumElements()));
		} else {
			containedTypesForTraversal.push_front(make_pair(dataType, 1));
		}
		unsigned memoryOfType = 0;
		//Find the primitive types of allocated data type
		while (!containedTypesForTraversal.empty()) {
			Type* traversingType = containedTypesForTraversal.front().first;
			int typeCount = containedTypesForTraversal.front().second;
			containedTypesForTraversal.pop_front();
			if (!traversingType->isAggregateType()) {
				for (int i = 0; i < typeCount; i++) {
					primitiveTypesMap.push_back(make_pair(traversingType, memoryOfType));
					memoryOfType += (32 / 8);
				}
			} else {
				if (traversingType->isArrayTy()) {
					containedTypesForTraversal.push_back(make_pair(traversingType->getArrayElementType(), traversingType->getArrayNumElements()));
				} else {
					for (unsigned i = 0; i < traversingType->getNumContainedTypes(); i++) {
						containedTypesForTraversal.push_back(make_pair(traversingType->getContainedType(i), 1));
					}
				}
			}
		}

		assert(isa<AllocaInst>(originalData) && "Original data is not alloc type\n");
		unsigned arraySize = ((ConstantInt*) ((AllocaInst*) originalData)->getArraySize())->getZExtValue();
		if (edgeSource->hasRangeBaseInput() && parentEdge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF) {
			/* Find the range base of the parent hyperop */
			Value* selfAddress = edgeSource->getFunction()->arg_begin();
			Value* frameBlockBaseAddress = edgeSource->getFunction()->arg_begin()->getNextNode();
			/* Skip forwarding addresses from all range hops, just the first one forwarding data is enough */
			Value* predicate = CmpInst::Create(Instruction::ICmp, llvm::CmpInst::ICMP_EQ, selfAddress, frameBlockBaseAddress, "firstpred", &((*insertInBB)->back()));
			/* Update the terminator for end */
			assert((*insertInBB)->getTerminator()->getNumSuccessors() <= 1 && "Insert BB can either be a part of the loop or the last bb of the function\n");
			BasicBlock* next = NULL;
			if ((*insertInBB)->getTerminator()->getNumSuccessors() == 1) {
				next = (*insertInBB)->getTerminator()->getSuccessor(0);
			}
			BasicBlock* communicationInsertionBB = BasicBlock::Create(ctxt, "", (*insertInBB)->getParent(), next);
			BasicBlock* fallbackBB = BasicBlock::Create(ctxt, "", (*insertInBB)->getParent(), next);

			/* Remove the last instruction in insertInBB and jump to the next landing pad */
			BranchInst* branchInst = BranchInst::Create(communicationInsertionBB, fallbackBB, predicate, &(*insertInBB)->back());
			(*insertInBB)->back().eraseFromParent();
			BranchInst* jumpInst = BranchInst::Create(next, &communicationInsertionBB->back());

			if (next == NULL) {
				ReturnInst::Create(ctxt, fallbackBB);
			}
			*nextInsertionPoint = fallbackBB;
			*insertInBB = communicationInsertionBB;
		}
		//TODO replace with a loop
		for (unsigned allocatedDataIndex = 0; allocatedDataIndex != arraySize; allocatedDataIndex++) {
			//Add a load instruction from memory and store to the memory frame of the consumer HyperOp
			for (list<pair<Type*, unsigned> >::iterator containedPrimitiveItr = primitiveTypesMap.begin(); containedPrimitiveItr != primitiveTypesMap.end(); containedPrimitiveItr++) {
				Type* containedType = containedPrimitiveItr->first;
				unsigned sourceOffset = allocatedDataIndex * memoryOfType + containedPrimitiveItr->second;
				Value* loadOffset = BinaryOperator::CreateNUWAdd(sourceData, ConstantInt::get(ctxt, APInt(32, sourceOffset)));
				LoadInst* lw = new LoadInst(loadOffset, "", &((*insertInBB)->back()));

				Value* storeOffset = BinaryOperator::CreateNUWAdd(targetMemFrameBaseAddress, ConstantInt::get(ctxt, APInt(32, sourceOffset)));
				new StoreInst(targetMemBaseInst, lw, &((*insertInBB)->back()));
			}
		}
	}

	static void addRedefineCommInstructions(HyperOpEdge * parentEdge, Value * sourceData, Value* childCFBaseAddr, HyperOp* edgeSource, BasicBlock** insertInBB, BasicBlock** nextInsertionPoint) {
		Module* M = (*insertInBB)->getParent()->getParent();
		LLVMContext & ctxt = (*insertInBB)->getContext();
		if (edgeSource->hasRangeBaseInput() && parentEdge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR) {
			/* Find the range base of the parent hyperop */
			Value* selfAddress = edgeSource->getFunction()->arg_begin();
			Value* frameBlockBaseAddress = edgeSource->getFunction()->arg_begin()->getNextNode();
			/* Skip forwarding addresses from all range hops, just the first one forwarding data is enough */
			Value* predicate = CmpInst::Create(Instruction::ICmp, llvm::CmpInst::ICMP_EQ, selfAddress, frameBlockBaseAddress, "firstpred", &((*insertInBB)->back()));
			/* Update the terminator for end */
			assert((*insertInBB)->getTerminator()->getNumSuccessors() <= 1 && "Insert BB can either be a part of the loop or the last bb of the function\n");
			BasicBlock* next = NULL;
			if ((*insertInBB)->getTerminator()->getNumSuccessors() == 1) {
				next = (*insertInBB)->getTerminator()->getSuccessor(0);
			}
			BasicBlock* communicationInsertionBB = BasicBlock::Create(ctxt, "", (*insertInBB)->getParent(), next);
			BasicBlock* fallbackBB = BasicBlock::Create(ctxt, "", (*insertInBB)->getParent(), next);

			/* Remove the last instruction in insertInBB and jump to the next landing pad */
			BranchInst* branchInst = BranchInst::Create(communicationInsertionBB, fallbackBB, predicate, &(*insertInBB)->back());
			(*insertInBB)->back().eraseFromParent();
			BranchInst* jumpInst = BranchInst::Create(next, communicationInsertionBB);

			if (next == NULL) {
				ReturnInst::Create(ctxt, fallbackBB);
			}
			*nextInsertionPoint = fallbackBB;
			*insertInBB = communicationInsertionBB;
		}
		if (sourceData->getType()->isPointerTy()) {
			sourceData = new LoadInst(sourceData, "", &(*insertInBB)->back());
		}
		if (parentEdge->getType() == HyperOpEdge::SCALAR || parentEdge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || parentEdge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_BASE) {
			/* -1 because there is one arg corresponding to the hyperop id itself, but we don't update the edges context slot because it is easier to match value pointers of argument with context slot this way */
			Value* writeCMArgs[] = { childCFBaseAddr, ConstantInt::get(ctxt, APInt(32, (parentEdge->getPositionOfContextSlot() - 1) * 4)), sourceData };
			CallInst::Create((Value*) Intrinsic::getDeclaration(M, (llvm::Intrinsic::ID) Intrinsic::writecm, 0), writeCMArgs, "", &((*insertInBB)->back()));
		} else if (parentEdge->getType() == HyperOpEdge::PREDICATE) {
			Value* predicate = sourceData;
			if (predicate->getType() != Type::getInt32Ty(ctxt)) {
				predicate = new ZExtInst(predicate, Type::getInt32Ty(ctxt), "", &((*insertInBB)->back()));
			}
			if (!parentEdge->getPredicateValue()) {
				//Invert the predicate value
				predicate = CmpInst::Create(Instruction::ICmp, llvm::CmpInst::ICMP_UGE, predicate, ConstantInt::get(ctxt, APInt(32, 1)), "invertedPred", &((*insertInBB)->back()));
				predicate = new ZExtInst(predicate, Type::getInt32Ty(ctxt), "", &((*insertInBB)->back()));
			}
			if (parentEdge->getDecrementOperandCount() > 0) {
				predicate = BinaryOperator::CreateNUWAdd(predicate, ConstantInt::get(ctxt, APInt(32, parentEdge->getDecrementOperandCount())), "", &((*insertInBB)->back()));
			}
			Value* predicateArgs[] = { childCFBaseAddr, predicate };
			CallInst::Create((Value*) Intrinsic::getDeclaration(M, (llvm::Intrinsic::ID) Intrinsic::writecmp, 0), predicateArgs, "", &((*insertInBB)->back()));
		}
	}

	static void inline addExpectedSyncCount(HyperOp* child, Value* baseAddress, BasicBlock** insertInBB, Module &M){
		/* This has to be added here because a hyperop may be created and its producer may deliver sync count before the expected sync count argument of the consumer hyperop is even set */
		DEBUG(dbgs() << "Adding expected sync count to the hyperop instance created " << child->asString() << "\n");
		Value* predicatedSyncCount = NULL;
		Value* unpredicateSyncCount = NULL;
		Value* syncCount = NULL;
		if (!child->getSyncCount(0).empty() && !child->getSyncCount(1).empty() && child->getSyncCount(0).size() != child->getSyncCount(1).size()) {
			list<SyncValue> zeroSyncEdgesOnHop = child->getSyncCount(0);
			Value* zeroSync;
			getSyncCount(insertInBB, zeroSyncEdgesOnHop, child->getFunction()->getParent(), &zeroSync);
			Value* firstPredicate = child->getIncomingSyncPredicate(0);
			Value * invertZeroSync = CmpInst::Create(Instruction::OtherOps::ICmp, llvm::CmpInst::ICMP_EQ, zeroSync, ConstantInt::get(M.getContext(), APInt(32, 1)), "zeropredsync", &(*insertInBB)->back());
			Value* mulForZeroSync = BinaryOperator::Create(Instruction::BinaryOps::Mul, firstPredicate, invertZeroSync, "mulzerosync", &(*insertInBB)->back());

			list<SyncValue> oneSyncEdgesOnHop = child->getSyncCount(1);
			Value* secondPredicate = child->getIncomingSyncPredicate(1);
			if (secondPredicate == NULL) {
				secondPredicate = firstPredicate;
			}
			Value* oneSync;
			getSyncCount(insertInBB, oneSyncEdgesOnHop, child->getFunction()->getParent(), &oneSync);
			predicatedSyncCount = BinaryOperator::Create(Instruction::BinaryOps::Add, mulForZeroSync, oneSync, "predsynccount", &(*insertInBB)->back());
		} else if (!child->getSyncCount(0).empty()) {
			list<SyncValue> syncEdgesOnHop = child->getSyncCount(0);
			getSyncCount(insertInBB, syncEdgesOnHop, child->getFunction()->getParent(), &predicatedSyncCount);
			predicatedSyncCount = CmpInst::Create(Instruction::OtherOps::ICmp, llvm::CmpInst::ICMP_EQ, predicatedSyncCount, ConstantInt::get(M.getContext(), APInt(32, 1)), "onepredsync", &(*insertInBB)->back());
		} else if (!child->getSyncCount(1).empty()) {
			list<SyncValue> syncEdgesOnHop = child->getSyncCount(1);
			getSyncCount(insertInBB, syncEdgesOnHop, child->getFunction()->getParent(), &predicatedSyncCount);
		}
		if (!child->getSyncCount(2).empty()) {
			list<SyncValue> syncEdgesOnHop = child->getSyncCount(2);
			getSyncCount(insertInBB, syncEdgesOnHop, child->getFunction()->getParent(), &unpredicateSyncCount);
		}

		assert((predicatedSyncCount!=NULL || unpredicateSyncCount !=NULL) && "None of the sync counts are valid for a barrier hyperop\n");
		if (predicatedSyncCount == NULL && unpredicateSyncCount != NULL) {
			syncCount = unpredicateSyncCount;
		} else if (unpredicateSyncCount == NULL && predicatedSyncCount != NULL) {
			if (predicatedSyncCount->getType() != Type::getInt32Ty(M.getContext())) {
				predicatedSyncCount = new ZExtInst(predicatedSyncCount, Type::getInt32Ty(M.getContext()), "", &(*insertInBB)->back());
			}
			syncCount = predicatedSyncCount;
		} else {
			if (predicatedSyncCount->getType() != Type::getInt32Ty(M.getContext())) {
				predicatedSyncCount = new ZExtInst(predicatedSyncCount, Type::getInt32Ty(M.getContext()), "", &(*insertInBB)->back());
			}
			syncCount = BinaryOperator::CreateNUWAdd(predicatedSyncCount, unpredicateSyncCount, "synccount", &(*insertInBB)->back());
		}
		Value *syncArgs[] = { baseAddress, ConstantInt::get(M.getContext(), APInt(32, 60)), syncCount };
		CallInst::Create((Value*) Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID) Intrinsic::writecm, 0), syncArgs, "", &(*insertInBB)->back());
	}

	static void inline addCommunicationInstructions(HyperOp* child, HyperOp* vertex, Value* baseAddress, BasicBlock** insertInBB, Module &M){
		/* Find out if the child hyperop has any incoming localref edges and add mem frame base address load instruction if so */
		Value * memFrameAddress = NULL;
		for (auto incomingEdgeItr = child->ParentMap.begin(); incomingEdgeItr != child->ParentMap.end(); incomingEdgeItr++) {
			if (incomingEdgeItr->second == vertex
					&& (incomingEdgeItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_BASE_LOCALREF || incomingEdgeItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF || incomingEdgeItr->first->getType() == HyperOpEdge::LOCAL_REFERENCE)) {
				Value* memFrameAddrArgs[] = { baseAddress };
				memFrameAddress = CallInst::Create((Value*) Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID) Intrinsic::getmemframe, 0), memFrameAddrArgs, "memframe_reg", &(*insertInBB)->back());
				break;
			}
		}

		DEBUG(dbgs() << "Adding lw and sw instructions to the hyperop instance passed\n");
		for (auto childEdgeItr = vertex->ChildMap.begin(); childEdgeItr != vertex->ChildMap.end(); childEdgeItr++) {
			if (childEdgeItr->second == child
					&& (childEdgeItr->first->getType() == HyperOpEdge::LOCAL_REFERENCE || childEdgeItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF || childEdgeItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_BASE_LOCALREF)) {
				Value* originalSourceData;
				getSourceData(childEdgeItr->first, vertex, child, &originalSourceData);
				assert(memFrameAddress!=NULL && "mem frame address instructions must be added already\n");
				/* load every data token or use it as immediate value in store operation */
				BasicBlock* newInsertionPoint = NULL;
				loadAndStoreData(childEdgeItr->first->getValue(), originalSourceData, childEdgeItr->first, vertex, memFrameAddress, M.getContext(), insertInBB, &newInsertionPoint);
				if (newInsertionPoint != NULL) {
					*insertInBB = newInsertionPoint;
				}
			}
		}

		DEBUG(dbgs() << "Adding writecm instructions to the hyperop instance passed\n");
		for (auto childEdgeItr = vertex->ChildMap.begin(); childEdgeItr != vertex->ChildMap.end(); childEdgeItr++) {
			if (childEdgeItr->second == child && (childEdgeItr->first->getType() == HyperOpEdge::SCALAR || childEdgeItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || childEdgeItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_BASE)) {
				/* Ensure that the target location is marked inreg */
				assert(child->getFunction()->getAttributes().hasAttribute(childEdgeItr->first->getPositionOfContextSlot() + 1, Attribute::InReg) && "Incorrect argument attribute, should be inreg\n");
				BasicBlock* newInsertionPoint = NULL;
				addRedefineCommInstructions(childEdgeItr->first, childEdgeItr->first->getValue(), baseAddress, child, insertInBB, &newInsertionPoint);
				if (newInsertionPoint != NULL) {
					*insertInBB = newInsertionPoint;
				}
			}
		}

		/* This is separated from the previous method because the frame is deactivated if false predicate is delivered: TODO check with Madhav if this constraint is necessary */
		DEBUG(dbgs() << "Adding writecmp instructions to the hyperop instance passed\n");
		for (auto childEdgeItr = vertex->ChildMap.begin(); childEdgeItr != vertex->ChildMap.end(); childEdgeItr++) {
			if (childEdgeItr->second == child && childEdgeItr->first->getType() == HyperOpEdge::PREDICATE) {
				BasicBlock* newInsertionPoint = NULL;
				addRedefineCommInstructions(childEdgeItr->first, childEdgeItr->first->getValue(), baseAddress, child, insertInBB, &newInsertionPoint);
				if (newInsertionPoint != NULL) {
					*insertInBB = newInsertionPoint;
				}
			}
		}

		if (child->isBarrierHyperOp()) {
			DEBUG(dbgs() << "Adding sync instructions to the hyperop instance passed\n");
			for (auto childEdgeItr = vertex->ChildMap.begin(); childEdgeItr != vertex->ChildMap.end(); childEdgeItr++) {
				if (childEdgeItr->second == child && childEdgeItr->first->getType() == HyperOpEdge::SYNC) {
					Value* syncArgs[] = { baseAddress, ConstantInt::get(M.getContext(), APInt(32, 60)), ConstantInt::get(M.getContext(), APInt(32, -1)) };
					CallInst::Create((Value*) Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID) Intrinsic::sync, 0), syncArgs, "", &(*insertInBB)->back());
				}
			}
		}
	}

	static inline MDString* getMDStringForProperty(string property, string propValue, LLVMContext &ctxt) {
		string startHop = property;
		startHop.append("=").append(propValue);
		return MDString::get(ctxt, startHop);
	}

	static inline Value* getConstantValue(int value, Module &M){
		return ConstantInt::get(M.getContext(), APInt(32, value));
	}

	virtual bool runOnModule(Module &M) {
		HyperOpInteractionGraph* graph = HyperOpMetadataParser::parseMetadata(&M);

		if (MAKE_GRAPH_STRUCTURED) {
			graph->removeUnreachableHops();
			graph->makeGraphStructured();
			graph->computeDominatorInfo();
		}
		graph->addContextFrameAddressForwardingEdges();
		graph->addSelfFrameAddressRegisters();
		graph->setDimensions(MAX_ROW, MAX_COL);
		graph->clusterNodes();
		graph->mapClustersToComputeResources();
		graph->convertRemoteScalarsToStores();
		graph->shuffleHyperOpArguments();
		graph->setMaxContextFrameSize(MAX_CONTEXT_FRAME_SIZE);
		graph->convertSpillScalarsToStores();
		graph->addNecessarySyncEdges();
		graph->addArgDecrementCountOnControlPaths();
		graph->addSyncCountDecrementOnControlPaths();
		graph->associateStaticContextFrames();
		graph->updateLocalRefEdgeMemSizeAndOffset();
		graph->verify(1);
		map<Function*, unsigned> functionAndIndexMap;

		unsigned index = 0;
		for (auto functionItr = M.begin(); functionItr != M.end(); functionItr++) {
			Function* func = functionItr;
			functionAndIndexMap.insert(make_pair(func, index));
			index++;
		}

		for (auto vertexItr : graph->Vertices) {
			HyperOp* vertex = vertexItr;
			errs() << "\n---------\nadding instructions to vertex " << vertex->asString() << "\n";
			//Do nothing in unrolled instance functions
			if (vertex->isUnrolledInstance()) {
				continue;
			}
			Function* vertexFunction = vertex->getFunction();
			BasicBlock* insertInBB = &vertexFunction->back();
			map<HyperOp*, pair<Value*, Value*> > createdHopBaseAddressMap;

			/* we add fallocs for predicated and non range dynamic hops ealy on, to ensure that if there are communication instructions that carry address of such hyperops*/
			DEBUG(dbgs() << "Adding falloc, fbind instructions to module for dynamic non range or predicated range hops\n");
			for (auto childItr : graph->Vertices) {
				HyperOp* child = childItr;
				if (child == vertex || child->getImmediateDominator() != vertex || child->isStaticHyperOp() || (child->getInRange() && !child->isPredicatedHyperOp())) {
					continue;
				}
				BasicBlock * loopBegin, *loopBody, *loopEnd;
				/* Range HyperOp's base address, obtained either with falloc or by loading with forwarded address */
				Value* baseAddress = NULL;
				Value* numFrames = NULL;
				/* Range and predicated hops have to be created in a block */
				if(child->getInRange()){
					numFrames = BinaryOperator::CreateUDiv(BinaryOperator::CreateSub(child->getRangeUpperBound(), child->getRangeLowerBound(), "", &insertInBB->back()), child->getStride(), "", &insertInBB->back());
					numFrames = BinaryOperator::CreateSub(numFrames, getConstantValue(1, M), "", &insertInBB->back());
				}else{
					numFrames = ConstantInt::get(M.getContext(), APInt(32, 0));
				}
				vector<Value*> fallocArgs;
				fallocArgs.push_back(numFrames);
				Value* func = NULL;
				if (vertex->getTargetResource() != child->getTargetResource()) {
					fallocArgs.push_back(ConstantInt::get(M.getContext(), APInt(32, child->getTargetResource())));
					func = (Value*) Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID) Intrinsic::rfalloc, 0);
				} else {
					func = (Value*) Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID) Intrinsic::falloc, 0);
				}

				baseAddress = CallInst::Create(func, fallocArgs, "falloc_reg", &insertInBB->back());
				assert(baseAddress!=NULL && "Could not load the base address of the child hyperop\n");
				Value* baseAddressMax = NULL;
				Value* baseAddressWithoutPhi = baseAddress;

				if(child->getInRange()){
					baseAddressMax = BinaryOperator::CreateNUWAdd(BinaryOperator::CreateNUWMul(numFrames, getConstantValue(64, M), "", &insertInBB->back()), baseAddress, "", &insertInBB->back());
					addRangeLoopConstructs(child, vertexFunction, M, &loopBegin, &loopBody, &loopEnd, &insertInBB, baseAddress, baseAddressMax, &baseAddress);
					insertInBB = loopBody;
					assert(isa<PHINode>(baseAddress) && "updated base address must be a phi node\n");
				}

				/* Use this for communication instruction insertion later, and do it now cos range hyperop may be predicated */
				createdHopBaseAddressMap.insert(make_pair(child, make_pair(baseAddressWithoutPhi, baseAddressMax)));

				for (auto outgoingEdgeItr = vertex->ChildMap.begin(); outgoingEdgeItr != vertex->ChildMap.end(); outgoingEdgeItr++) {
					if ((outgoingEdgeItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || outgoingEdgeItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF || outgoingEdgeItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_BASE
							|| outgoingEdgeItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_BASE_LOCALREF) && outgoingEdgeItr->first->getContextFrameAddress() == child) {
						outgoingEdgeItr->first->setValue(baseAddressWithoutPhi);
					}
				}

				Value *fbindArgs[] = { ConstantInt::get(M.getContext(), APInt(32, functionAndIndexMap[child->getFunction()])), baseAddress };
				CallInst::Create((Value*) Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID) Intrinsic::fbind, 0), fbindArgs, "", &insertInBB->back());

				if (child->isBarrierHyperOp()) {
					addExpectedSyncCount(child, baseAddress, &insertInBB, M);
				}

				if (child->getInRange()) {
					Value* updatedValue = BinaryOperator::CreateNUWAdd(baseAddress, getConstantValue(64, M), "", &loopBody->back());
					assert(isa<PHINode>(baseAddress) && "iterator of base address must be a phi node\n");
					((PHINode*) baseAddress)->addIncoming(updatedValue, loopBody);
					insertInBB = loopEnd;
				}
			}


		DEBUG(dbgs() << "Adding falloc, fbind instructions to module for non predicated range hops\n");
			for (auto childItr : graph->Vertices) {
				int numCR = MAX_ROW*MAX_COL;
				HyperOp* child = childItr;
				if (child == vertex || child->getImmediateDominator() != vertex || child->isStaticHyperOp() || !child->getInRange() || child->isPredicatedHyperOp()) {
					continue;
				}
				BasicBlock * loopBegin, *loopBody, *loopEnd;
				/* Range HyperOp's base address, obtained either with falloc or by loading with forwarded address */
				Value* iterator = NULL;
				Value* baseAddress = NULL;
				Value* memFrameAddress = NULL;

				addRangeLoopConstructs(child, vertexFunction, M, &loopBegin, &loopBody, &loopEnd, &insertInBB, child->getRangeLowerBound(), child->getRangeUpperBound(), &iterator);
				insertInBB = loopBody;

				vector<Value*> fallocArgs;
				fallocArgs.push_back(getConstantValue(0, M));
				Value* targetResource = BinaryOperator::Create(Instruction::BinaryOps::URem, iterator, getConstantValue(numCR, M), "", &insertInBB->back());
				fallocArgs.push_back(targetResource);
				Value* func = (Value*) Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID) Intrinsic::rfalloc, 0);
				baseAddress = CallInst::Create(func, fallocArgs, "falloc_reg", &insertInBB->back());

				Value *fbindArgs[] = { ConstantInt::get(M.getContext(), APInt(32, functionAndIndexMap[child->getFunction()])), baseAddress };
				CallInst::Create((Value*) Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID) Intrinsic::fbind, 0), fbindArgs, "", &insertInBB->back());
				if (child->isBarrierHyperOp()) {
					addExpectedSyncCount(child, baseAddress, &insertInBB, M);
				}
				/* Add to only unpredicated range hyperops that have no incoming parents other than idom */
				if (child->getInRange() && !child->isPredicatedHyperOp()) {
					addCommunicationInstructions(child, vertex, baseAddress, &insertInBB, M);
				}

				if (child->getInRange()) {
					Value* updatedValue = BinaryOperator::CreateNUWAdd(iterator, child->getStride(), "", &loopBody->back());
					assert(isa<PHINode>(iterator) && "iterator of base address must be a phi node\n");
					((PHINode*) iterator)->addIncoming(updatedValue, loopBody);
					insertInBB = loopEnd;
				}
			}


			DEBUG(dbgs() << "Adding all communication instructions to frames that are range and predicated or not range\n");
			for (auto childItr : vertex->getChildList()) {
				HyperOp* child = childItr;
				/* These were added in the previous iteration, they don't need to be added now */
				if(child->getImmediateDominator() == vertex && child->getInRange() && !child->isPredicatedHyperOp()){
					continue;
				}
				BasicBlock * loopBegin, *loopBody, *loopEnd;
				Value* loadInst;
				/* Range HyperOp's base address, obtained either with falloc or by loading with forwarded address */
				Value* baseAddress = NULL, *memFrameAddress = NULL;
				Value* baseAddressUpperBound = NULL;
				if (child->isStaticHyperOp()) {
					baseAddress = getConstantValue(child->getContextFrame() * 64, M);
				}
				else if (child->getImmediateDominator() == vertex) {
					baseAddress = createdHopBaseAddressMap[child].first;
					baseAddressUpperBound = createdHopBaseAddressMap[child].second;
					assert(baseAddress!=NULL && "Could not load the base address of the child hyperop\n");
				} else {
					/* Address was forwarded to the hyperop */
					for (auto parentItr = vertex->ParentMap.begin(); parentItr != vertex->ParentMap.end(); parentItr++) {
						if ((parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF) && parentItr->first->getContextFrameAddress() == child) {
							Argument* childHopCFAddress = NULL;
							int argIndex = 0;
							for (auto argItr = vertex->getFunction()->arg_begin(); argItr != vertex->getFunction()->arg_end(); argItr++, argIndex++) {
								Argument* arg = argItr;
								if (parentItr->first->getPositionOfContextSlot() == argIndex) {
									childHopCFAddress = arg;
									break;
								}
							}
							assert(childHopCFAddress!=NULL && "Context frame address of the child HyperOp was never passed to the current HyperOp");
							bool hasInRegAttr = vertex->getFunction()->getAttributes().hasAttribute(argIndex + 1, Attribute::InReg);
							assert(((parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR && hasInRegAttr) || (parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF && !hasInRegAttr)) && "Incorrect attrbute to the argument");

							if (parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF) {
								baseAddress = new LoadInst(childHopCFAddress, "", insertInBB);
							} else {
								baseAddress = (Value*) childHopCFAddress;
							}
							break;
						}
					}
					assert(baseAddress!=NULL && "Could not load the base address of the child hyperop\n");
					if (child->getInRange()) {
						Value* numFrames = BinaryOperator::CreateUDiv(BinaryOperator::CreateSub(child->getRangeUpperBound(), child->getRangeLowerBound(), "", &insertInBB->back()), child->getStride(), "", &insertInBB->back());
						baseAddressUpperBound = BinaryOperator::CreateNUWAdd(BinaryOperator::CreateNUWMul(numFrames, getConstantValue(64, M), "", &insertInBB->back()), baseAddress, "", &insertInBB->back());
					}
				}

				if (child->getInRange()) {
					addRangeLoopConstructs(child, vertexFunction, M, &loopBegin, &loopBody, &loopEnd, &insertInBB, baseAddress, baseAddressUpperBound, &baseAddress);
					insertInBB = loopBody;
				}
				addCommunicationInstructions(child, vertex, baseAddress, &insertInBB, M);
				if (child->getInRange()) {
					Value* updatedValue = BinaryOperator::CreateNUWAdd(baseAddress, getConstantValue(64, M), "", &loopBody->back());
					assert(isa<PHINode>(baseAddress) && "Phi node has to be formed for use subsequently\n");
					((PHINode*) baseAddress)->addIncoming(updatedValue, loopBody);
					insertInBB = loopEnd;
				}
			}

			for (auto childItr : vertex->getChildList()) {
				HyperOp* child = childItr;
				DEBUG(dbgs() << "Adding fdelete instructions to module\n");
				if (child != vertex && child->isPredicatedHyperOp() && child->getImmediateDominator() != NULL && child->getImmediateDominator()->getImmediatePostDominator() == vertex) {
					Value* argContainingAddress = NULL;
					if(child->isStaticHyperOp()){
						argContainingAddress = getConstantValue(child->getContextFrame()*64, M);
					} else {
						HyperOpEdge* contextFrameAddressEdge = NULL;
						for (auto incomingEdgeItr : child->ParentMap) {
							HyperOpEdge* edge = incomingEdgeItr.first;
							if (edge->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR && edge->getContextFrameAddress() == child) {
								contextFrameAddressEdge = edge;
								break;
							}
						}
						assert((contextFrameAddressEdge!=NULL)&&"The address must be forwarded from another HyperOp");
						unsigned argSlot = contextFrameAddressEdge->getPositionOfContextSlot();
						unsigned index = 0;
						for (auto argItr = vertexFunction->getArgumentList().begin(); argItr != vertexFunction->getArgumentList().end(); argItr++, index++) {
							if (argSlot == index) {
								argContainingAddress = argItr;
								break;
							}
						}
					}
					assert(argContainingAddress!=NULL && "Address cant be NULL for fdelete\n");
					BasicBlock * loopBegin, *loopBody, *loopEnd;
					if (child->getInRange()) {
						Value* diff = BinaryOperator::CreateSub(child->getRangeUpperBound(), child->getRangeLowerBound(), "", &insertInBB->back());
						Value* numFrames = BinaryOperator::CreateNUWSub(BinaryOperator::CreateExactUDiv(diff, child->getStride(), "", &insertInBB->back()), getConstantValue(1, M), "", &insertInBB->back());
						Value* upperBound = BinaryOperator::CreateNUWAdd(BinaryOperator::CreateNUWMul(getConstantValue(FRAME_SIZE_BYTES, M), numFrames, "", &insertInBB->back()), argContainingAddress, "", &insertInBB->back());
						addRangeLoopConstructs(child, vertexFunction, M, &loopBegin, &loopBody, &loopEnd, &insertInBB, argContainingAddress, upperBound, &argContainingAddress);
						insertInBB = loopBody;
					}

					CallInst* fdeleteInst;
					Value *fdeleteArgs[] = { argContainingAddress };
					fdeleteInst = CallInst::Create((Value*) Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID) Intrinsic::fdelete, 0), fdeleteArgs, "", &insertInBB->back());
					if (child->getInRange()) {
						Value* updatedValue = BinaryOperator::CreateNUWAdd(argContainingAddress, getConstantValue(64, M), "", &loopBody->back());
						assert(isa<PHINode>(argContainingAddress) && "Phi node has to be formed for use subsequently\n");
						((PHINode*) argContainingAddress)->addIncoming(updatedValue, loopBody);
						insertInBB = loopEnd;
					}
				}

			}

			DEBUG(dbgs() << "Adding fdelete self instruction\n");
			if (!vertex->isPredicatedHyperOp()) {
				CallInst* fdeleteInst;
				assert(vertexFunction->getArgumentList().size() >= 1);
				Value* frameAddress = vertexFunction->arg_begin();
				Value *fdeleteArgs[] = { frameAddress };
				fdeleteInst = CallInst::Create((Value*) Intrinsic::getDeclaration(&M, (llvm::Intrinsic::ID) Intrinsic::fdelete, 0), fdeleteArgs, "", &insertInBB->back());
			}
		}
		/* Delete all metadata */
		NamedMDNode * redefineAnnotationsNode = M.getOrInsertNamedMetadata(REDEFINE_ANNOTATIONS);
		redefineAnnotationsNode->eraseFromParent();
		for (auto funcItr = M.begin(); funcItr != M.end(); funcItr++) {
			for (auto bbItr = funcItr->begin(); bbItr != funcItr->end(); bbItr++) {
				for (auto instItr = bbItr->begin(); instItr != bbItr->end(); instItr++) {
					Instruction * inst = instItr;
					inst->setMetadata(HYPEROP_CONSUMED_BY, NULL);
					inst->setMetadata(HYPEROP_CONTROLS, NULL);
					inst->setMetadata(HYPEROP_SYNC, NULL);
				}
			}
		}
		NamedMDNode * hyperOpAnnotationsNode = M.getOrInsertNamedMetadata(HYPEROP_ANNOTATIONS);
		for (auto vertexItr : graph->Vertices) {
			HyperOp* vertex = vertexItr;
			if(vertex->isUnrolledInstance()){
				continue;
			}
			vector<Value*> hopAnnotations;
			hopAnnotations.push_back(MDString::get(M.getContext(), vertex->getFunction()->getName()));

			stringstream id;
			id << functionAndIndexMap[vertex->getFunction()];
			hopAnnotations.push_back(getMDStringForProperty(HYPEROP_ID, id.str(), M.getContext()));

			stringstream targetResource;
			targetResource << vertex->getTargetResource();
			hopAnnotations.push_back(getMDStringForProperty(HYPEROP_AFFINITY, targetResource.str(), M.getContext()));

			hopAnnotations.push_back(getMDStringForProperty(STATIC_HYPEROP, vertex->isStaticHyperOp() ? "yes" : "no", M.getContext()));

			stringstream contextFrame;
			contextFrame << vertex->getContextFrame();
			hopAnnotations.push_back(getMDStringForProperty(HYPEROP_FRAME, contextFrame.str(), M.getContext()));

			hopAnnotations.push_back(getMDStringForProperty(HYPEROP_ENTRY, vertex->isStartHyperOp() ? "yes" : "no", M.getContext()));

			hopAnnotations.push_back(getMDStringForProperty(HYPEROP_EXIT, vertex->isEndHyperOp() ? "yes" : "no", M.getContext()));

			hopAnnotations.push_back(getMDStringForProperty(HYPEROP_PREDICATED, vertex->isPredicatedHyperOp() ? "yes" : "no", M.getContext()));

			hopAnnotations.push_back(getMDStringForProperty(HYPEROP_BARRIER, vertex->isBarrierHyperOp() ? "yes" : "no", M.getContext()));

			vector<Value*> offsetList;
			int argIndex = 0;
			for (auto argItr = vertex->getFunction()->arg_begin(); argItr != vertex->getFunction()->arg_end(); argItr++, argIndex++) {
				if (!vertex->getFunction()->getAttributes().hasAttribute(argIndex + 1, Attribute::InReg)) {
					int size = 0;
					/* Iterate through memory arguments to identify their sizes */
					for (auto incomingEdgeItr = vertex->ParentMap.begin(); incomingEdgeItr != vertex->ParentMap.end(); incomingEdgeItr++) {
						if (incomingEdgeItr->first->getPositionOfContextSlot() == argIndex) {
							size = incomingEdgeItr->first->getMemorySize();
							break;
						}
					}
					assert((size > 0) && "Size of an arg in memory arg must be greater than 0\n");
					offsetList.push_back(getMDStringForProperty(itostr(argIndex), itostr(size), M.getContext()));
				}
			}

			MDNode* offsetAnnotation = MDNode::get(M.getContext(), offsetList);
			hopAnnotations.push_back(offsetAnnotation);
			hyperOpAnnotationsNode->addOperand(MDNode::get(M.getContext(), hopAnnotations));
		}
		/* Set attributes of each hyperop */
		return true;
	}
};

char REDEFINEIRPass::ID = 3;
char* REDEFINEIRPass::NEW_NAME = "newName";
static RegisterPass<REDEFINEIRPass> X3("REDEFINEIRPass", "Pass to create REDEFINE specific IR from Kavitha's metadata based IR");
