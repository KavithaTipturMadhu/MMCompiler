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
		return false;
	}

	static bool isArgInList(Function* function, Value* operand) {
		for (Function::arg_iterator argItr = function->arg_begin(); argItr != function->arg_end(); argItr++) {
			if (argItr == operand) {
				return true;
			}
		}
		return false;
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
				if (isa < StoreInst > (instr) &&((StoreInst*) instr)->getOperand(0) == useInstr && pathExistsInCFG(originalBB, useInstr->getParent(), visitedBasicBlocks)) {
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

		if(isa<AllocaInst>(argument)){
			if(((AllocaInst*)argument)->getType()->getPointerElementType()->getTypeID()==Type::IntegerTyID){
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

	static void addInitializationInstructions(GlobalVariable* global, Constant* initializer, vector<Value*> idList, Instruction* insertBefore, Type* type) {
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

	virtual bool runOnModule(Module &M) {
		LLVMContext & ctxt = M.getContext();
		list<Function*> addedFunctions;
		//Top level annotation corresponding to all annotations REDEFINE
		NamedMDNode * redefineAnnotationsNode = M.getOrInsertNamedMetadata(REDEFINE_ANNOTATIONS);

		//Contains all created HyperOp functions and the basic blocks they contain in the original module and HyperOp arguments
		map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> > createdHyperOpAndOriginalBasicBlockAndArgMap;

		//Original Instructions and their clones
		map<Value*, Value*> originalToClonedInstMap;

		map<BasicBlock*, BasicBlock*> originalToClonedBBMap;

		//Map of the return instruction introduced in each HyperOp function
		map<Function*, Instruction*> retInstMap;

		//Slightly complex data structure, required when supporting instances of HyperOps though, so leaving it as is
		map<Function*, list<Function*> > originalFunctionToCreatedHyperOpsMap;

		//Map of a created HyperOp to its annotation
		map<Function*, MDNode*> hyperOpAndAnnotationMap;

		map<Value*, Instruction*> allocaInstCreatedForIntermediateValues;

		int bbIndex = 0, hyperOpIndex = 0;

		map<Function*, ReturnInst*> originalReturnInstrs;

		list<Function*> orderOfFunctionProcessing;
		Function* mainFunction = 0;
		for (Module::iterator funcItr = M.begin(); funcItr != M.end(); funcItr++) {
			Function* function = funcItr;
			if (function->getName().compare(REDEFINE_START_FUNCTION) == 0) {
				mainFunction = function;
				break;
			}
		}

		if (mainFunction == 0) {
			DEBUG(errs() << "REDEFINE kernel's entry point i.e., redefine_start function missing, aborting\n");
			return false;
		}
		list<Function*> stack;
		stack.push_front(mainFunction);
		while (!stack.empty()) {
			Function* currentFunction = stack.front();
			stack.pop_front();
			orderOfFunctionProcessing.push_front(currentFunction);
			for (Function::iterator bbItr = currentFunction->begin(); bbItr != currentFunction->end(); bbItr++) {
				for (BasicBlock::reverse_iterator instrItr = bbItr->rbegin(); instrItr != bbItr->rend(); instrItr++) {
					if (isa<CallInst>(&*instrItr)) {
						Function* calledFunction = ((CallInst*) &*instrItr)->getCalledFunction();
						if (!calledFunction->isIntrinsic() && find(orderOfFunctionProcessing.begin(), orderOfFunctionProcessing.end(), calledFunction) == orderOfFunctionProcessing.end()) {
							stack.push_front(calledFunction);
						}
					}

				}
			}
		}

		while (!orderOfFunctionProcessing.empty()) {
			Function* function = orderOfFunctionProcessing.front();
			orderOfFunctionProcessing.pop_front();
			StringRef name = function->getName();
			if (name.compare(REDEFINE_START_FUNCTION) == 0) {
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
			list<BasicBlock*> accumulatedBasicBlocks;
			HyperOpArgumentList hyperOpArguments;
			HyperOpArgumentList globalReferences;
			bool endOfHyperOp = false;
			list<BasicBlock*> traversedBasicBlocks;
			list<BasicBlock*> bbTraverser;
			bbTraverser.push_back(function->begin());
			unsigned hyperOpArgCount = 0;
			while (!bbTraverser.empty()) {
				BasicBlock* bbItr = bbTraverser.front();
				bbTraverser.pop_front();
				bool canAcquireBBItr = true;
				//If basic block does not have a unique predecessor and basic block is not the entry block
				if (bbItr->getUniquePredecessor() == 0 && bbItr != &(function->getEntryBlock())) {
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
					if (canAcquireBBItr && accumulatedBasicBlocks.size() > 1) {
						//Find the basic blocks that jump to the basic block being acquired
						for (Function::iterator bbItr = function->begin(); bbItr != function->end(); bbItr++) {
							for (unsigned i = 0; i < bbItr->getTerminator()->getNumSuccessors(); i++) {
								BasicBlock* successor = bbItr->getTerminator()->getSuccessor(i);
								if (successor == bbItr && find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), bbItr) == accumulatedBasicBlocks.end()) {
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
				} else {
					accumulatedBasicBlocks.push_back(bbItr);
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
									if (bbItr->getInstList().size() > 1) {
										//Call is the not only instruction here, a separate HyperOp must be created for the call statement
										stringstream firstBBName(NEW_NAME);
										firstBBName << bbIndex;
										bbIndex++;
										BasicBlock* newFunctionCallBlock = bbItr->splitBasicBlock(instItr, firstBBName.str());
										Instruction* nonTerminalInstruction = instItr;
										if (nonTerminalInstruction != (Instruction*) bbItr->getTerminator()) {
											Instruction* instructionAfterCall = newFunctionCallBlock->begin()->getNextNode();
											stringstream secondBBName(NEW_NAME);
											secondBBName << bbIndex;
											bbIndex++;
											newFunctionCallBlock->splitBasicBlock(instructionAfterCall, secondBBName.str());
										}
									} else {
										for (unsigned int i = 0; i < instItr->getNumOperands(); i++) {
											Value * argument = instItr->getOperand(i);
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

				//Create a new HyperOp
				if (endOfHyperOp || bbItr->getNextNode() == function->end()) {
					//Shuffle the arguments to the HyperOp such that scalar arguments are positioned first and the rest of the arguments are positioned after
					HyperOpArgumentList tempHyperOpArguments;
					for (HyperOpArgumentList::iterator hyperOpArgumentItr = hyperOpArguments.begin(); hyperOpArgumentItr != hyperOpArguments.end(); hyperOpArgumentItr++) {
						if (hyperOpArgumentItr->second == SCALAR) {
							tempHyperOpArguments.push_back(*hyperOpArgumentItr);
						}
					}

					for (HyperOpArgumentList::iterator hyperOpArgumentItr = hyperOpArguments.begin(); hyperOpArgumentItr != hyperOpArguments.end(); hyperOpArgumentItr++) {
						if (hyperOpArgumentItr->second !=SCALAR) {
							tempHyperOpArguments.push_back(*hyperOpArgumentItr);
						}
					}
					hyperOpArguments.clear();
					std::copy(tempHyperOpArguments.begin(), tempHyperOpArguments.end(), std::back_inserter(hyperOpArguments));
					//end of shuffling HyperOp arguments

					list<const Function*> addedParentsToCurrentHyperOp;

					//Couldn't use splice here since it clears away accumulatedBasicBlocks list
					for (list<BasicBlock*>::iterator accumulatedItr = accumulatedBasicBlocks.begin(); accumulatedItr != accumulatedBasicBlocks.end(); accumulatedItr++) {
						traversedBasicBlocks.push_back(*accumulatedItr);
					}

					vector<Type*> argsList;
					for (HyperOpArgumentList::iterator hyperOpArgumentItr = hyperOpArguments.begin(); hyperOpArgumentItr != hyperOpArguments.end(); hyperOpArgumentItr++) {
						//Set type of each argument of the HyperOp
						Value* argument = hyperOpArgumentItr->first.front();
						if (hyperOpArgumentItr->second == GLOBAL_REFERENCE) {
							continue;
						}
						argsList.push_back(argument->getType());
					}
					ArrayRef<Type*> dataTypes(argsList);

					FunctionType *FT = FunctionType::get(Type::getVoidTy(getGlobalContext()), dataTypes, false);
					Function *newFunction = Function::Create(FT, Function::ExternalLinkage, name, &M);
					addedFunctions.push_back(newFunction);

					bool isEntry = false;
					bool isExit = false;
					if (accumulatedBasicBlocks.front()->getParent()->getName() == REDEFINE_START_FUNCTION) {
						if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), &function->getEntryBlock()) != accumulatedBasicBlocks.end()) {
							isEntry = true;
						}
						if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), &function->back()) != accumulatedBasicBlocks.end()) {
							isExit = true;
						}
					}
					//Add metadata to label the function as a HyperOp
					Value * values[2];
					values[0] = MDString::get(ctxt, HYPEROP);
					values[1] = newFunction;
					MDNode *funcAnnotation = MDNode::get(ctxt, values);
					hyperOpAndAnnotationMap.insert(make_pair(newFunction, funcAnnotation));
					redefineAnnotationsNode->addOperand(funcAnnotation);

					//Is the function an entry node/exit node/intermediate ?
					if (isEntry) {
						Value* values[2];
						values[0] = MDString::get(ctxt, HYPEROP_ENTRY);
						values[1] = funcAnnotation;
						MDNode* entryNode = MDNode::get(ctxt, values);
						redefineAnnotationsNode->addOperand(entryNode);
					}
					if (isExit) {
						Value* values[2];
						values[0] = MDString::get(ctxt, HYPEROP_EXIT);
						values[1] = funcAnnotation;
						MDNode* exitNode = MDNode::get(ctxt, values);
						redefineAnnotationsNode->addOperand(exitNode);
					}
					if (!isEntry && !isExit) {
						Value* values[2];
						values[0] = MDString::get(ctxt, HYPEROP_INTERMEDIATE);
						values[1] = funcAnnotation;
						MDNode* intermediateNode = MDNode::get(ctxt, values);
						redefineAnnotationsNode->addOperand(intermediateNode);
					}

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

					//Map of predicate's source instruction in a different HyperOp, the branch instruction needs to be deleted after the predicate is marked for delivery since we need writecmp instructions to predicate HyperOps instead of regular branch instructions (j, beq etc)
					map<Instruction*, unsigned> conditionalBranchSources;
					list<Instruction*> unconditionalBranchSources;
					for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
						string newBBName = newFunction->getName();
						newBBName.append(".").append((*accumulatedBBItr)->getName());
						BasicBlock *newBB = BasicBlock::Create(getGlobalContext(), newBBName, newFunction);
						originalToClonedBBMap.insert(std::make_pair(*accumulatedBBItr, newBB));
						//Cloning instructions in the reverse order so that the user instructions are cloned before the definition instructions
						for (BasicBlock::iterator instItr = (*accumulatedBBItr)->begin(); instItr != (*accumulatedBBItr)->end(); instItr++) {
							if (isa<ReturnInst>(&*instItr)) {
								originalReturnInstrs.insert(make_pair(function, (ReturnInst*) &*instItr));
							}
							Instruction* clonedInst = instItr->clone();
							Instruction * originalInstruction = &*instItr;
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

					//Add a basic block with a dummy return instruction as the single point of exit for a HyperOp
					BasicBlock* retBB = BasicBlock::Create(ctxt, newFunction->getName().str().append(".return"), newFunction);
					Instruction* retInst = ReturnInst::Create(ctxt);
					retInstMap.insert(make_pair(newFunction, retInst));
					retBB->getInstList().insert(retBB->getFirstInsertionPt(), retInst);

					//Created a temporary arg list since the list gets cleared later
					HyperOpArgumentList tempArgList;
					std::copy(hyperOpArguments.begin(), hyperOpArguments.end(), std::back_inserter(tempArgList));
					createdHyperOpAndOriginalBasicBlockAndArgMap.insert(make_pair(newFunction, make_pair(accumulatedBasicBlocks, tempArgList)));

					//Add the newly created function to original functions and their corresponding created functions map
					list<Function*> hyperOpFunctions;
					if (originalFunctionToCreatedHyperOpsMap.find(function) != originalFunctionToCreatedHyperOpsMap.end()) {
						hyperOpFunctions = originalFunctionToCreatedHyperOpsMap.find(function)->second;
						originalFunctionToCreatedHyperOpsMap.erase(function);
					}
					hyperOpFunctions.push_back(newFunction);
					originalFunctionToCreatedHyperOpsMap.insert(make_pair(function, hyperOpFunctions));

					DEBUG(dbgs() << "Created a new HyperOp:");
					newFunction->print(dbgs());
					accumulatedBasicBlocks.clear();
					hyperOpArguments.clear();
					hyperOpArgCount = 0;
					endOfHyperOp = false;
				}

				vector<unsigned> distanceToExit;
				map<unsigned, list<BasicBlock*> > untraversedBasicBlocks;
				//Ensure breadth biased traversal such that all predecessors of a basic block are traversed before the basic block
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

				//Sort the basic blocks in descending order of their distance to the exit basic block to ensure that the basic block withmore children gets traversed sooner, is this required though?
				list<BasicBlock*> sortedSuccBasicBlockList;
				if (distanceToExit.size() > 1) {
					for (unsigned i = 0; i < distanceToExit.size() - 1; i++) {
						unsigned max = distanceToExit[i];
						for (unsigned j = i + 1; j < distanceToExit.size(); j++) {
							if (max < distanceToExit[j]) {
								unsigned temp = distanceToExit[j];
								distanceToExit[i] = temp;
								distanceToExit[j] = max;
								max = temp;
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
		}

		//Add metadata: This code is moved here to ensure that all the functions (corresponding to HyperOps) that need to be created have already been created
		for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHyperOpItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHyperOpItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHyperOpItr++) {
			Function* newFunction = createdHyperOpItr->first;
			DEBUG(dbgs() << "Dealing with function " << newFunction->getName() << "\n");
			list<BasicBlock*> accumulatedBasicBlocks = createdHyperOpItr->second.first;
			HyperOpArgumentList hyperOpArguments = createdHyperOpItr->second.second;
			list<const Function*> addedParentsToCurrentHyperOp;
			MDNode* funcAnnotation = hyperOpAndAnnotationMap.find(newFunction)->second;
			list<Instruction*> instructionsToBeMoved;

			DEBUG(dbgs() << "Adding consumed by metadata\n");
			//Add consumedby meta data from source HyperOp to the HyperOp being created
			unsigned hyperOpArgumentIndex = 0;
			for (HyperOpArgumentList::iterator hyperOpArgumentItr = hyperOpArguments.begin(); hyperOpArgumentItr != hyperOpArguments.end(); hyperOpArgumentItr++) {
				list<Value*> individualArguments = hyperOpArgumentItr->first;
				HyperOpArgumentType argumentType = hyperOpArgumentItr->second;
				if (argumentType == GLOBAL_REFERENCE) {
					continue;
				}
				for (list<Value*>::iterator individualArgItr = individualArguments.begin(); individualArgItr != individualArguments.end(); individualArgItr++) {
					Value* argument = *individualArgItr;
					if (isa<Instruction>(argument)) {
						Function* originalFunction = createdHyperOpAndOriginalBasicBlockAndArgMap[newFunction].first.front()->getParent();
						//Get Reaching definitions of the argument to the accumulated basic block list
						map<BasicBlock*, Instruction*> reachingDefBasicBlocks;
						if(isa<AllocaInst>(argument)){
							//Reaching definitions are only for memory instructions
							reachingDefBasicBlocks = reachingStoreOperations((Instruction*)argument, originalFunction, accumulatedBasicBlocks);
						}else{
							reachingDefBasicBlocks.insert(make_pair(((Instruction*)argument)->getParent(),(Instruction*)argument));
						}
						//Get the producer HyperOp
						bool atleastOneUseInOtherHyperOp = false;
						for (map<BasicBlock*, Instruction*>::iterator reachingDefItr = reachingDefBasicBlocks.begin(); reachingDefItr != reachingDefBasicBlocks.end(); reachingDefItr++) {
							BasicBlock* parentBBOfDefinition = reachingDefItr->first;
							for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHopItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHopItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHopItr++) {
								list<BasicBlock*> createdHyperOpBBList = createdHopItr->second.first;
								//Find the HyperOp containing the argument's definition
								if (find(createdHyperOpBBList.begin(), createdHyperOpBBList.end(), parentBBOfDefinition) != createdHyperOpBBList.end()) {
									//Check if the source of dependence has any uses in the function it belongs to or in basic blocks not corresponding to the HyperOp being created
									for (Value::use_iterator argItr = argument->use_begin(); argItr != argument->use_end(); argItr++) {
										Instruction* useInstruction = (Instruction*) *argItr;
										if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), useInstruction->getParent()) == accumulatedBasicBlocks.end()) {
											atleastOneUseInOtherHyperOp = true;
											break;
										}
									}
								} else {
									atleastOneUseInOtherHyperOp = true;
								}
							}
							if (atleastOneUseInOtherHyperOp) {
								break;
							}
						}

						if (atleastOneUseInOtherHyperOp) {
							//Find the HyperOp with the reaching definition
							for (map<BasicBlock*, Instruction*>::iterator reachingDefItr = reachingDefBasicBlocks.begin(); reachingDefItr != reachingDefBasicBlocks.end(); reachingDefItr++) {
								BasicBlock* reachingDefBB = reachingDefItr->first;
								Instruction* reachingDefInstr = reachingDefItr->second;
								Instruction* clonedReachingDefInst = (Instruction*) originalToClonedInstMap[reachingDefInstr];
								if (isa<AllocaInst>(clonedReachingDefInst) || isa<StoreInst>(clonedReachingDefInst) || isArgInList(clonedReachingDefInst->getParent()->getParent(), clonedReachingDefInst->getOperand(0))) {
									//Add "consumedby" metadata to the function locals that need to be passed to other HyperOps
									Value * values[3];
									values[0] = funcAnnotation;
									if (argumentType == SCALAR) {
										values[1] = MDString::get(ctxt, SCALAR_ARGUMENT);
									} else if (argumentType == LOCAL_REFERENCE) {
										values[1] = MDString::get(ctxt, LOCAL_REFERENCE_ARGUMENT);
									}
									values[2] = ConstantInt::get(ctxt, APInt(32, hyperOpArgumentIndex));
									MDNode * consumedByMetadata = MDNode::get(ctxt, values);
									Instruction* clonedInstruction = ((Instruction*) originalToClonedInstMap.find(argument)->second);
									Instruction* metadataHost = 0;
									//If the argument has a memory location associated with it beforehand or does not have an alloca instruction inserted explicitly for it (is a temporary instruction)
									if (isa<AllocaInst>(argument) || allocaInstCreatedForIntermediateValues.find(clonedInstruction) == allocaInstCreatedForIntermediateValues.end()) {
										metadataHost = clonedInstruction;
									} else if (isArgInList(clonedReachingDefInst->getParent()->getParent(), clonedReachingDefInst->getOperand(1))) {
										//local argument is passed on to another HyperOp, find the first load instruction from the memory location and add metadata to it
										for (Function::iterator bbItr = clonedReachingDefInst->getParent()->getParent()->begin(); bbItr != clonedReachingDefInst->getParent()->getParent()->end(); bbItr++) {
											for (BasicBlock::iterator instrItr = bbItr->begin(); instrItr != bbItr->end(); instrItr++) {
												if (isa <LoadInst> (instrItr) &&((LoadInst*) &instrItr)->getOperand(0) == clonedReachingDefInst->getOperand(0)) {
													metadataHost = clonedReachingDefInst;
													break;
												}
											}
											if (metadataHost != 0) {
												break;
											}
										}
									} else {
										//Get the equivalent alloca inserted before
										metadataHost = allocaInstCreatedForIntermediateValues.find(((Instruction*) originalToClonedInstMap.find(argument)->second))->second;
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

									if (!isa < AllocaInst > (metadataHost) &&!isArgInList(clonedReachingDefInst->getParent()->getParent(), clonedReachingDefInst->getOperand(0))) {
										//Temporary data, add an alloca and a store instruction after the argument and label the alloca instruction with metadata
										AllocaInst* ai = new AllocaInst(argument->getType());
										ai->setAlignment(4);
										//Alloc instructions need to be inserted in the entry basic block of the function because other allocs are treated as dynamic stack allocs
										ai->insertBefore(clonedInstruction->getParent()->getParent()->getEntryBlock().getFirstInsertionPt());
										StoreInst* storeInst = new StoreInst(clonedInstruction, ai);
										storeInst->setAlignment(4);
										storeInst->insertBefore(clonedInstruction);
										allocaInstCreatedForIntermediateValues.insert(make_pair(clonedInstruction, ai));
										metadataHost = ai;
									}
									metadataHost->setMetadata(HYPEROP_CONSUMED_BY, newMDNode);
									//Parent function buffered to ensure that unnecessary control dependences need not exist
									if (find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), metadataHost->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()) {
										addedParentsToCurrentHyperOp.push_back(metadataHost->getParent()->getParent());
									}
								}
							}
						} else {
//							No uses outside the function being created, move the data to the current HyperOp and remove it from the hyperOp argument list
							HyperOpArgumentList::iterator argToBeRemoved;
							bool argNeedsRemoval = false;
							HyperOpArgumentList updateList;
							for (HyperOpArgumentList::iterator hyperOpArgItr = hyperOpArguments.begin(); hyperOpArgItr != hyperOpArguments.end(); hyperOpArgItr++) {
								if (hyperOpArgItr->first.size() == 1 && hyperOpArgItr->first.front() == argument) {
									argToBeRemoved = hyperOpArgItr;
									argNeedsRemoval = true;
									break;
								}
							}

							if (argNeedsRemoval) {
								hyperOpArguments.erase(argToBeRemoved);
								//Add the data movement instructions to the created basic blocks
								//Find where it was in the first place and remove its clone
								Instruction* clonedInstruction = (Instruction*) originalToClonedInstMap.find((Instruction*) argument)->second;
								argument->replaceAllUsesWith(clonedInstruction);
								//Replace uses of the cloned instruction
								instructionsToBeMoved.push_back(clonedInstruction);
							}
						}
					}
				}
				hyperOpArgumentIndex++;
			}
			//Move alloca instructions that don't need to be retained in the first HyperOp since it has no uses anywhere other than the current HyperOp being created
			for (list<Instruction*>::iterator instrToBeMoved = instructionsToBeMoved.begin(); instrToBeMoved != instructionsToBeMoved.end(); instrToBeMoved++) {
				(*instrToBeMoved)->removeFromParent();
				newFunction->getEntryBlock().getInstList().insert(newFunction->getEntryBlock().getFirstInsertionPt(), *instrToBeMoved);
			}

			//Compute conditional branch sources
			map<Instruction*, unsigned> conditionalBranchSources;
			list<Instruction*> unconditionalBranchSources;
			for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
				//Find out if any basic block is predicated
				BasicBlock* originalBB = *accumulatedBBItr;
				//Predicates are only for non-entry basic blocks
				if (originalBB != &(*accumulatedBBItr)->getParent()->getEntryBlock()) {
					for (pred_iterator predecessorItr = pred_begin(originalBB); predecessorItr != pred_end(originalBB); predecessorItr++) {
						BasicBlock* predecessor = *predecessorItr;
						//Control flow edge exists from a predecessor basic block that does not belong to the same HyperOp
						if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), predecessor) == accumulatedBasicBlocks.end()) {
							for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentList> >::iterator createdHopItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHopItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHopItr++) {
								Function* createdHyperOp = createdHopItr->first;
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
												successorBBList.push_back(i);
											}
										}
										if (successorBBList.size() == terminator->getNumSuccessors()) {
											//All successors of the terminator instruction are in the same target HyperOp, mark the jump as unconditional
											unconditionalBranchSources.push_back(terminator);
										} else {
											for (unsigned i = 0; i < successorBBList.size(); i++) {
												conditionalBranchSources.insert(make_pair(terminator, successorBBList[i]));
											}
										}

									}
								}
							}
						}
					}
				}
				for (list<BasicBlock*>::iterator bbItr = accumulatedBasicBlocks.begin(); bbItr != accumulatedBasicBlocks.end(); bbItr++) {
					TerminatorInst* terminator = (*bbItr)->getTerminator();
					for (unsigned i = 0; i < terminator->getNumSuccessors(); i++) {
						if (terminator->getSuccessor(i) == (*accumulatedBBItr)) {
							Instruction* clonedTerminator = (Instruction*) originalToClonedInstMap[terminator];
							assert(isa<TerminatorInst>(clonedTerminator) || "a terminator instruction is mapped to a non-terminator!");
							((TerminatorInst*) clonedTerminator)->setSuccessor(i, originalToClonedBBMap[*accumulatedBBItr]);
						}
					}
				}
			}

			DEBUG(dbgs() << "Dealing with conditional branches from other HyperOps\n");
			//Find out if there exist any branch instructions leading to the HyperOp, add metadata to the instruction
			for (map<Instruction*, unsigned>::iterator conditionalBranchSourceItr = conditionalBranchSources.begin(); conditionalBranchSourceItr != conditionalBranchSources.end(); conditionalBranchSourceItr++) {
				//Find the branch instruction operand's clone
				Instruction* clonedInstruction = (Instruction*) originalToClonedInstMap.find(conditionalBranchSourceItr->first)->second;
				errs()<<"cloned instruction:";
				clonedInstruction->dump();
				//Branch instruction's first operand
				Instruction* predicateOperand = (Instruction*) clonedInstruction->getOperand(0);
				MDNode* predicateMetadata = predicateOperand->getMetadata(HYPEROP_CONTROLLED_BY);
				vector<Value*> metadataList;
				if (predicateMetadata != 0) {
					predicateMetadata = predicateOperand->getMetadata(HYPEROP_CONTROLLED_BY);
					for (unsigned i = 0; i < predicateMetadata->getNumOperands(); i++) {
						metadataList.push_back(predicateMetadata->getOperand(i));
					}
				}
				//Label the instruction with predicates metadata
				Value * values[1];
				values[0] = funcAnnotation;
				MDNode* newPredicateMetadata = MDNode::get(ctxt, values);
				metadataList.push_back(newPredicateMetadata);
				ArrayRef<Value*> metadataRef(metadataList);
				MDNode * predicatesRelation = MDNode::get(ctxt, metadataRef);
				Instruction* metadataHost = 0;
				if (isa<AllocaInst>(predicateOperand) || allocaInstCreatedForIntermediateValues.find(predicateOperand) == allocaInstCreatedForIntermediateValues.end()) {
					metadataHost = predicateOperand;
				} else {
					//Get the equivalent alloca inserted before
					metadataHost = allocaInstCreatedForIntermediateValues.find(predicateOperand)->second;
				}

				if (!isa<AllocaInst>(metadataHost)) {
					//Since the temporary does not have a memory location associated with it, add an alloca and a store instruction after the argument and label the alloca instruction with metadata
					AllocaInst* ai = new AllocaInst(predicateOperand->getType());
					ai->setAlignment(4);
					ai->insertBefore(clonedInstruction->getParent()->getParent()->getEntryBlock().getFirstInsertionPt());
					StoreInst* storeInst = new StoreInst(predicateOperand, ai);
					storeInst->setAlignment(4);
					storeInst->insertBefore(clonedInstruction);
					allocaInstCreatedForIntermediateValues.insert(make_pair(clonedInstruction, ai));
					metadataHost = ai;
				}
				metadataHost->setMetadata(HYPEROP_CONTROLLED_BY, predicatesRelation);

				//Update the cloned conditional branch instruction with the right target
				unsigned positionToBeUpdated = ConstantInt::get(ctxt, APInt(32, conditionalBranchSourceItr->second))->getZExtValue();
				Instruction* retInstOfProducer = retInstMap.find(clonedInstruction->getParent()->getParent())->second;
				clonedInstruction->setOperand(positionToBeUpdated, retInstOfProducer->getParent());
			}
			DEBUG(dbgs() << "Dealing with unconditional branches from other HyperOps\n");
			//Remove unconditional branch instruction, add the annotation to the return instruction of the branch
			for (list<Instruction*>::iterator unconditionalBranchSourceItr = unconditionalBranchSources.begin(); unconditionalBranchSourceItr != unconditionalBranchSources.end(); unconditionalBranchSourceItr++) {
				Instruction* unconditionalBranchInstr = *unconditionalBranchSourceItr;
				Instruction* clonedInstr = (Instruction*) originalToClonedInstMap.find(unconditionalBranchInstr)->second;
				const Function* sourceParentFunction = clonedInstr->getParent()->getParent();
				Instruction* retInstOfProducer = retInstMap.find(clonedInstr->getParent()->getParent())->second;
				//Update the branch instruction to jump to the return basic block
				clonedInstr->setOperand(0, retInstOfProducer->getParent());

				//No need to add unconditional jump if there is data being produced already
				if (find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), sourceParentFunction) != addedParentsToCurrentHyperOp.end()) {
					continue;
				}
				MDNode* predicateMetadata = clonedInstr->getMetadata(HYPEROP_CONTROLLED_BY);
				vector<Value*> metadataList;
				if (predicateMetadata != 0) {
					predicateMetadata = clonedInstr->getMetadata(HYPEROP_CONTROLLED_BY);
					for (unsigned i = 0; i < predicateMetadata->getNumOperands(); i++) {
						metadataList.push_back(predicateMetadata->getOperand(i));
					}
				}

				//Label the instruction with predicates metadata
				Value * values[1];
				values[0] = funcAnnotation;
				MDNode* newPredicateMetadata = MDNode::get(ctxt, values);
				metadataList.push_back(newPredicateMetadata);
				ArrayRef<Value*> metadataRef(metadataList);
				MDNode * predicatesRelation = MDNode::get(ctxt, metadataRef);
				//Allocated a location and store 1 there
				AllocaInst* ai = new AllocaInst(Type::getInt32Ty(ctxt));
				ai->setAlignment(4);
				ai->insertBefore(retInstOfProducer->getParent()->getParent()->getEntryBlock().getFirstInsertionPt());
				StoreInst* storeInst = new StoreInst(ConstantInt::get(ctxt, APInt(32, 1)), ai);
				storeInst->setAlignment(4);
				storeInst->insertBefore(retInstOfProducer);
				ai->setMetadata(HYPEROP_CONTROLLED_BY, predicatesRelation);
				addedParentsToCurrentHyperOp.push_back(sourceParentFunction);
			}
			//TODO compute unconditional jump chains

			Function* originalFunction = const_cast<Function*>(createdHyperOpAndOriginalBasicBlockAndArgMap[newFunction].first.front()->getParent());
			//Check the reaching definition of the global reference object to this HyperOp
			for (HyperOpArgumentList::iterator hyperOpArgItr = hyperOpArguments.begin(); hyperOpArgItr != hyperOpArguments.end(); hyperOpArgItr++) {
				HyperOpArgumentType type = hyperOpArgItr->second;
				if (type == GLOBAL_REFERENCE) {
					//check the reaching definitions of the value
					Value* argValue = hyperOpArgItr->first.front();
					//Check the uses in BasicBlocks that are predecessors and use argValue
					list<BasicBlock*> basicBlocksWithDefinitions;
					//Find uses of argValue in the basic blocks of created function
					for (Function::iterator originalBBItr = originalFunction->begin(); originalBBItr != originalFunction->end(); originalBBItr++) {
						BasicBlock* originalBB = &*originalBBItr;
						for (BasicBlock::iterator instrItr = originalBB->begin(); instrItr != originalBB->end(); instrItr++) {
							Instruction* instr = instrItr;
							if (isa < StoreInst > (instr) &&((StoreInst*) instr)->getOperand(0) == argValue && find(basicBlocksWithDefinitions.begin(), basicBlocksWithDefinitions.end(), originalBB) == basicBlocksWithDefinitions.end()) {
								//Check if the store instruction is reachable to any of the uses of the argument in the accumulated bb list
								bool originalBBHasDefinition = false;
								for (list<Value*>::iterator phiItr = hyperOpArgItr->first.begin(); phiItr != hyperOpArgItr->first.end(); phiItr++) {
									for (Value::use_iterator useItr = (*phiItr)->use_begin(); useItr != (*phiItr)->use_end(); useItr++) {
										User* user = *useItr;
										list<BasicBlock*> visitedBasicBlocks;
										if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), ((Instruction*) user)->getParent()) != accumulatedBasicBlocks.end() && pathExistsInCFG(originalBB, ((Instruction*) user)->getParent(), visitedBasicBlocks)) {
											originalBBHasDefinition = true;
											break;
										}
									}
									if (originalBBHasDefinition) {
										break;
									}
								}
								if (originalBBHasDefinition) {
									basicBlocksWithDefinitions.push_back(originalBB);
								}
								break;
							}
						}
					}

					list<BasicBlock*> discardList;
					for (list<BasicBlock*>::iterator defBBItr = basicBlocksWithDefinitions.begin(); defBBItr != basicBlocksWithDefinitions.end(); defBBItr++) {
						BasicBlock* defBB = *defBBItr;
						for (list<BasicBlock*>::iterator secDefBBItr = basicBlocksWithDefinitions.begin(); secDefBBItr != basicBlocksWithDefinitions.end(); secDefBBItr++) {
							BasicBlock* secDefBB = *secDefBBItr;
							list<BasicBlock*> visitedBasicBlocks;
							if (secDefBB != defBB && (pathExistsInCFG(defBB, secDefBB, visitedBasicBlocks) || find(discardList.begin(), discardList.end(), secDefBB) != discardList.end())) {
								discardList.push_back(defBB);
							}
						}
					}

					for (list<BasicBlock*>::iterator defBBItr = basicBlocksWithDefinitions.begin(); defBBItr != basicBlocksWithDefinitions.end(); defBBItr++) {
						if (find(discardList.begin(), discardList.end(), *defBBItr) == discardList.end()) {
							for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
								Instruction* cloneInstruction = (Instruction*) originalToClonedInstMap.find(&(*defBBItr)->front())->second;
								list<BasicBlock*> visitedBasicBlocks;
								if (pathExistsInCFG(*defBBItr, *accumulatedBBItr, visitedBasicBlocks) && find(addedParentsToCurrentHyperOp.begin(), addedParentsToCurrentHyperOp.end(), cloneInstruction->getParent()->getParent()) == addedParentsToCurrentHyperOp.end()) {
									//Add a predicate between the source and the target function
									//Label the instruction with predicates metadata
									Value * values[1];
									values[0] = funcAnnotation;
									MDNode* newPredicateMetadata = MDNode::get(ctxt, values);
									vector<Value*> metadataList;
									metadataList.push_back(newPredicateMetadata);
									ArrayRef<Value*> metadataRef(metadataList);
									MDNode * predicatesRelation = MDNode::get(ctxt, metadataRef);
									//Allocated a location and store 1 there
									AllocaInst* ai = new AllocaInst(Type::getInt32Ty(ctxt));
									ai->setAlignment(4);
									Instruction* retInstOfProducer = retInstMap.find(const_cast<Function*>(cloneInstruction->getParent()->getParent()))->second;
									ai->insertBefore(retInstOfProducer->getParent()->getParent()->getEntryBlock().getFirstInsertionPt());
									StoreInst* storeInst = new StoreInst(ConstantInt::get(ctxt, APInt(32, 1)), ai);
									storeInst->setAlignment(4);
									storeInst->insertBefore(retInstOfProducer);
									ai->setMetadata(HYPEROP_CONTROLLED_BY, predicatesRelation);
									addedParentsToCurrentHyperOp.push_back(cloneInstruction->getParent()->getParent());
								}
							}
						}
					}
				}
			}
		}

		for (map<Function*, list<Function*> >::iterator originalFunctionItr = originalFunctionToCreatedHyperOpsMap.begin(); originalFunctionItr != originalFunctionToCreatedHyperOpsMap.end(); originalFunctionItr++) {
			//Remove old functions from module
			originalFunctionItr->first->eraseFromParent();
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
	const char* REDEFINE_INPUT_PREFIX = "redefine_in_";
	const char* REDEFINE_OUTPUT_PREFIX = "redefine_out_";
	const char* REDEFINE_INOUT_PREFIX = "redefine_inout_";
}
;
char HyperOpCreationPass::ID = 2;
char* HyperOpCreationPass::NEW_NAME = "newName";
static RegisterPass<HyperOpCreationPass> X("HyperOpCreationPass", "Pass to create HyperOps");
