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
	const string HYPEROP_PREDICATES = "Predicates";
	const string HYPEROP_AFFINITY = "Affinity";
	const string HYPEROP_START = "Start";
	const string HYPEROP_END = "End";
	const string SCALAR_ARGUMENT = "Scalar";
	const string REFERENCE_ARGUMENT = "Reference";

	const unsigned int FRAME_SIZE = 4;

	enum HyperOpArgumentType {
		SCALAR, REFERENCE
	};

	typedef map<unsigned, pair<list<Value*>, HyperOpArgumentType> > HyperOpArgumentMap;

	HyperOpCreationPass() :
			ModulePass(ID) {
	}

	//TODO, floating point data cannot be passed in register
//	Size of each entry of arguments list in terms of the number of integer registers occupied
//	static unsigned int sizeOfArgumentList(list<Value*> arguments) {
//		unsigned int argSize = 0;
//		for (list<Value*>::iterator argItr = arguments.begin(); argItr != arguments.end(); argItr++) {
//			switch ((*argItr)->getType()->getTypeID()) {
//			case Type::VoidTyID:
//				argSize += 0;
//				break;
//			case Type::HalfTyID:
//				//Treated as single precision floating point
//			case Type::FloatTyID:
//			case Type::IntegerTyID:
//				argSize += 1;
//				break;
//				//All these types are treated as references in REDEFINE
//			case Type::StructTyID:
//			case Type::ArrayTyID:
//			case Type::PointerTyID:
//			case Type::VectorTyID:     ///< 15: SIMD 'packed' format, or other vector type
//			case Type::DoubleTyID:
//				argSize += 2;
//				break;
//			default:
//				argSize += 0;
//				break;
//			}
//		}
//		return argSize;
//	}

	bool pathExistsInCFG(BasicBlock* source, BasicBlock* target) {
		for (int i = 0; i < source->getTerminator()->getNumSuccessors(); i++) {
			if (source->getTerminator()->getSuccessor(i) == target) {
				return true;
			} else if (pathExistsInCFG(source->getTerminator()->getSuccessor(i), target)) {
				return true;
			}
		}
		return false;
	}

	virtual bool runOnModule(Module &M) {
		LLVMContext & ctxt = M.getContext();
		list<Function*> addedFunctions;
		//Top level annotation corresponding to all annotations REDEFINE
		NamedMDNode * annotationsNode = M.getOrInsertNamedMetadata(REDEFINE_ANNOTATIONS);

		//Contains all created HyperOp functions and the basic blocks they contain in the original module and HyperOp arguments
		map<Function*, pair<list<BasicBlock*>, HyperOpArgumentMap> > createdHyperOpAndOriginalBasicBlockAndArgMap;

		//Original Instructions and their clones
		map<Value*, Value*> originalToClonedInstrMap;

		//Slightly complex data structure, required when supporting instances of HyperOps though, so leaving it as is
		map<Function*, list<Function*> > originalFunctionToCreatedHyperOpsMap;

		//Map of a created HyperOp to its annotation
		map<Function*, MDNode*> hyperOpAndAnnotationMap;
		int bbIndex = 0, hyperOpIndex = 0;

		for (Module::iterator funcItr = M.begin(); funcItr != M.end() && find(addedFunctions.begin(), addedFunctions.end(), funcItr) == addedFunctions.end(); funcItr++) {
			StringRef name = funcItr->begin()->getName();
			list<BasicBlock*> accumulatedBasicBlocks;
			HyperOpArgumentMap hyperOpArguments;
			int hyperOpArgCount = 0;
			bool endOfHyperOp = false;
			list<BasicBlock*> traversedBasicBlocks;
			list<BasicBlock*> bbTraverser;
			bbTraverser.push_back(funcItr->begin());
			while (!bbTraverser.empty()) {
				BasicBlock* bbItr = bbTraverser.front();
				bool canAcquireBBItr = true;
				//If basic block does not have a unique predecessor and basic block is not the entry block
				if (bbItr->getUniquePredecessor() == 0 && bbItr != &(funcItr->getEntryBlock())) {
					//Check if all the parent nodes of the basic block are in the same HyperOp
					//While this should in principle be done to all producers of data as well, since we are choosing one basic-block after another from a CFG to form a HyperOp, immediate predecessors should suffice
					list<BasicBlock*> predecessorsFromSameFunction;
					for (pred_iterator predecessorItr = pred_begin(bbItr); predecessorItr != pred_end(bbItr); predecessorItr++) {
						BasicBlock* predecessor = *predecessorItr;
						if (predecessor->getParent() == funcItr) {
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
								if (pathExistsInCFG(dependenceSource, dependenceTarget)
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
				}

				if (!canAcquireBBItr) {
					//Create a HyperOp at this boundary
					endOfHyperOp = true;
				} else {
					bbTraverser.pop_front();
					accumulatedBasicBlocks.push_back(bbItr);
					for (BasicBlock::iterator instItr = bbItr->begin(); instItr != bbItr->end(); instItr++) {
						if (!(isa<AllocaInst>(instItr) || isa<ReturnInst>(instItr))) {
							//Function calls that couldn't be inlined when generating the IR
							if (isa<CallInst>(instItr) || isa<InvokeInst>(instItr)) {
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
										if (isa<AllocaInst>(argument)) {
											hyperOpArguments.insert(make_pair(i, make_pair(argumentList, REFERENCE)));
										} else {
											hyperOpArguments.insert(make_pair(i, make_pair(argumentList, SCALAR)));
										}
									}
								}
								endOfHyperOp = true;
								break;
							}

							list<Value*> newHyperOpArguments;
							for (unsigned int i = 0; i < instItr->getNumOperands(); i++) {
								Value * argument = instItr->getOperand(i);
								if (!isa<Constant>(argument)) {
									if ((isa<Instruction>(argument)) && find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), ((Instruction*) argument)->getParent()) != accumulatedBasicBlocks.end()) {
										//Instruction belongs to the same bb, need not be added as an argument
										continue;
									}

									bool argAddedPreviously = false;
									//Check if same argument is used multiple times in the same instruction
									//We need to track the arguments in the instruction separately
									if (find(newHyperOpArguments.begin(), newHyperOpArguments.end(), argument) != newHyperOpArguments.end()) {
										argAddedPreviously = true;
									} else {
										for (HyperOpArgumentMap::iterator previousArgsItr = hyperOpArguments.begin(); previousArgsItr != hyperOpArguments.end(); previousArgsItr++) {
											if (find(previousArgsItr->second.first.begin(), previousArgsItr->second.first.end(), argument) != previousArgsItr->second.first.end()) {
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
										if (isa<AllocaInst>(*newArgItr)) {
											hyperOpArguments.insert(make_pair(hyperOpArgCount++, make_pair(newArg, REFERENCE)));
										} else {
											hyperOpArguments.insert(make_pair(hyperOpArgCount++, make_pair(newArg, SCALAR)));
										}
									}
								} else {
									//Phi instruction's arguments correspond to only one argument to a HyperOp
									hyperOpArguments.insert(make_pair(hyperOpArgCount++, make_pair(newHyperOpArguments, SCALAR)));
								}
							}
						}

						if (endOfHyperOp) {
							break;
						}
					}
				}

				//Create a new HyperOp
				if (endOfHyperOp || bbItr->getNextNode() == funcItr->end()) {
					errs() << "Creating a new HyperOp\n";
					//Couldn't use splice here since it clears away accumulatedBasicBlocks list
					for (list<BasicBlock*>::iterator accumulatedItr = accumulatedBasicBlocks.begin(); accumulatedItr != accumulatedBasicBlocks.end(); accumulatedItr++) {
						traversedBasicBlocks.push_back(*accumulatedItr);
					}

					vector<Type*> argsList;
					for (HyperOpArgumentMap::iterator hyperOpArgumentItr = hyperOpArguments.begin(); hyperOpArgumentItr != hyperOpArguments.end(); hyperOpArgumentItr++) {
						//Set type of each argument of the HyperOp
						Value* argument = hyperOpArgumentItr->second.first.front();
						argsList.push_back(argument->getType());
					}
					ArrayRef<Type*> dataTypes(argsList);

					FunctionType *FT = FunctionType::get(Type::getVoidTy(getGlobalContext()), dataTypes, false);
					Function *newFunction = Function::Create(FT, Function::ExternalLinkage, name, &M);
					addedFunctions.push_back(newFunction);
					createdHyperOpAndOriginalBasicBlockAndArgMap.insert(make_pair(newFunction, make_pair(accumulatedBasicBlocks, hyperOpArguments)));

					//Add metadata to label the function as a HyperOp
					Value * values[2];
					values[0] = MDString::get(ctxt, HYPEROP);
					values[1] = newFunction;
					MDNode *funcAnnotation = MDNode::get(ctxt, values);
					hyperOpAndAnnotationMap.insert(make_pair(newFunction, funcAnnotation));
					annotationsNode->addOperand(funcAnnotation);

					list<Instruction*> instructionsToBeMoved;
					//Add produces meta data from source HyperOp to the HyperOp being created
					for (HyperOpArgumentMap::iterator hyperOpArgumentItr = hyperOpArguments.begin(); hyperOpArgumentItr != hyperOpArguments.end(); hyperOpArgumentItr++) {
						list<Value*> individualArguments = hyperOpArgumentItr->second.first;
						HyperOpArgumentType argumentType = hyperOpArgumentItr->second.second;
						unsigned positionOfArgument = hyperOpArgumentItr->first;

						for (list<Value*>::iterator individualArgItr = individualArguments.begin(); individualArgItr != individualArguments.end(); individualArgItr++) {
							Value* argument = *individualArgItr;
							if (isa<Instruction>(argument)) {
								BasicBlock* parentBBOfDefinition = ((Instruction*) argument)->getParent();
								//Get the producer HyperOp
								for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentMap> >::iterator createdHopItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHopItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHopItr++) {
									Function* createdHyperOp = createdHopItr->first;
									list<BasicBlock*> createdHyperOpBBList = createdHopItr->second.first;
									//Find the HyperOp containing the argument's definition
									if (isa<AllocaInst>(argument) && find(createdHyperOpBBList.begin(), createdHyperOpBBList.end(), parentBBOfDefinition) != createdHyperOpBBList.end()) {
										//Check if the source of dependence has any uses in the function it belongs to or in basic blocks not corresponding to the HyperOp being created
										bool atleastOneUseInOtherHyperOp = false;
										if (isa<AllocaInst>(argument)) {
											for (Value::use_iterator argItr = argument->use_begin(); argItr != argument->use_end(); argItr++) {
												Instruction* useInstruction = (Instruction*) *argItr;
												if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), useInstruction->getParent()) == accumulatedBasicBlocks.end()) {
													atleastOneUseInOtherHyperOp = true;
													break;
												}
											}
										}
										if (atleastOneUseInOtherHyperOp) {
											//Add "consumedby" metadata on the function locals that need to be passed to other HyperOps
											Function * sourceFunction = createdHyperOp;
											Value * values[3];
											values[0] = funcAnnotation;
											if (argumentType == SCALAR) {
												values[1] = MDString::get(ctxt, SCALAR_ARGUMENT);
											} else {
												values[1] = MDString::get(ctxt, REFERENCE_ARGUMENT);
											}
											values[2] = ConstantInt::get(ctxt, APInt(32, positionOfArgument));
											MDNode * consumedByMetadata = MDNode::get(ctxt, values);
											MDNode* currentMetadataOfInstruction = ((Instruction*) originalToClonedInstrMap.find(argument)->second)->getMetadata(HYPEROP_CONSUMED_BY);
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
											((Instruction*) originalToClonedInstrMap.find(argument)->second)->setMetadata(HYPEROP_CONSUMED_BY, newMDNode);
										} else {
											//No uses outside the function being created, move the data to the current HyperOp
											//Remove from the hyperOp argument list
											int indexToBeRemoved = -1;
											HyperOpArgumentMap updateList;
											list<unsigned> entriesToBeRemovedTemporarily;
											for (HyperOpArgumentMap::iterator hyperOpArgItr = hyperOpArguments.begin(); hyperOpArgItr != hyperOpArguments.end(); hyperOpArgItr++) {
												if (hyperOpArgItr->second.first.size() == 1 && hyperOpArgItr->second.first.front() == argument) {
													indexToBeRemoved = hyperOpArgItr->first;
													for (HyperOpArgumentMap::iterator validArgsItr = hyperOpArguments.begin(); validArgsItr != hyperOpArguments.end(); validArgsItr++) {
														if (validArgsItr->first > indexToBeRemoved) {
															entriesToBeRemovedTemporarily.push_back(validArgsItr->first);
															updateList.insert(make_pair((validArgsItr->first - 1), make_pair(validArgsItr->second.first, validArgsItr->second.second)));
														}
													}
													break;
												}
											}

											if (indexToBeRemoved >= 0) {
												hyperOpArguments.erase(hyperOpArguments.find((unsigned) indexToBeRemoved));
												for (list<unsigned>::iterator removalListItr = entriesToBeRemovedTemporarily.begin(); removalListItr != entriesToBeRemovedTemporarily.end(); removalListItr++) {
													hyperOpArguments.erase(hyperOpArguments.find((unsigned) *removalListItr));
												}
												for (HyperOpArgumentMap::iterator tempArgItr = hyperOpArguments.begin(); tempArgItr != hyperOpArguments.end(); tempArgItr++) {
													hyperOpArguments.insert(*tempArgItr);
												}

												//Replace uses of the argument by the new instruction cloned into the accumulated basic blocks
												//Add the data movement instructions to the created basic blocks
												//Find where it was in the first place and remove its clone
												Instruction* clonedInstruction = (Instruction*) originalToClonedInstrMap.find((Instruction*)argument)->second;
												argument->replaceAllUsesWith(clonedInstruction);
												//Replace uses of the cloned instruction
												instructionsToBeMoved.push_back(clonedInstruction);
											}
										}
									}
								}
							}
						}
					}

					//Mark HyperOp function arguments which are not addresses as inReg
					int functionArgumentIndex = 1;
					for (HyperOpArgumentMap::iterator hyperOpArgItr = hyperOpArguments.begin(); hyperOpArgItr != hyperOpArguments.end(); hyperOpArgItr++, functionArgumentIndex++) {
						HyperOpArgumentType type = hyperOpArgItr->second.second;
						if (type == SCALAR) {
							newFunction->addAttribute(functionArgumentIndex, Attribute::InReg);
						}
					}

					list<Function*> hyperOpFunctions;
					if (originalFunctionToCreatedHyperOpsMap.find(funcItr) != originalFunctionToCreatedHyperOpsMap.end()) {
						hyperOpFunctions = originalFunctionToCreatedHyperOpsMap.find(funcItr)->second;
						originalFunctionToCreatedHyperOpsMap.erase(funcItr);
					}
					hyperOpFunctions.push_back(newFunction);
					originalFunctionToCreatedHyperOpsMap.insert(make_pair(funcItr, hyperOpFunctions));

					//Map of predicate's source HyperOp
					map<Function*, int> predicateSources;
					for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
						//Find out if any basic block is predicated
						BasicBlock* originalBB = *accumulatedBBItr;
						//Predicates are only for non-entry basic blocks
						if (originalBB != &(funcItr->getEntryBlock())) {
							for (pred_iterator predecessorItr = pred_begin(originalBB); predecessorItr != pred_end(originalBB); predecessorItr++) {
								BasicBlock* predecessor = *predecessorItr;
								//Control flow edge exists from a predecessor basic block not in the same HyperOp
								if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), predecessor) == accumulatedBasicBlocks.end()) {
									for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentMap> >::iterator createdHopItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHopItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHopItr++) {
										Function* createdHyperOp = createdHopItr->first;
										list<BasicBlock*> containedBasicBlocks = createdHopItr->second.first;
										if (find(containedBasicBlocks.begin(), containedBasicBlocks.end(), predecessor) != containedBasicBlocks.end()) {
											TerminatorInst* terminator = predecessor->getTerminator();
											for (int i = 0; i < terminator->getNumSuccessors(); i++) {
												if (terminator->getSuccessor(i) == originalBB) {
													predicateSources.insert(make_pair(createdHyperOp, i));
												}
											}
										}
									}
								}
							}
						}
						BasicBlock *newBB = BasicBlock::Create(getGlobalContext(), (*accumulatedBBItr)->getName(), newFunction);
						for (BasicBlock::reverse_iterator instItr = (*accumulatedBBItr)->rbegin(); instItr != (*accumulatedBBItr)->rend(); instItr++) {
							if (*accumulatedBBItr == accumulatedBasicBlocks.back() && instItr == (*accumulatedBBItr)->rbegin()) {
								Instruction* retInst = ReturnInst::Create(getGlobalContext());
								newBB->getInstList().insert(newBB->getFirstInsertionPt(), retInst);
							} else {
								Instruction* clonedInst = instItr->clone();
								Instruction * originalInstruction = &*instItr;
								originalToClonedInstrMap.insert(std::make_pair(originalInstruction, clonedInst));
								if (!isa<AllocaInst>(clonedInst)) {
									for (int operandIndex = 0; operandIndex < clonedInst->getNumOperands(); operandIndex++) {
										Value* operandToBeReplaced = clonedInst->getOperand(operandIndex);
										bool argUpdated = false;
										for (HyperOpArgumentMap::iterator argumentItr = hyperOpArguments.begin(); argumentItr != hyperOpArguments.end(); argumentItr++) {
											unsigned hyperOpArgIndex = argumentItr->first;
											list<Value*> individualArguments = argumentItr->second.first;
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
									}
								}

								//Propogate the instruction as Value* to all those places that use the instruction
								for (Value::use_iterator useItr = instItr->use_begin(); useItr != instItr->use_end(); useItr++) {
									//Find out if the use is in the same HyperOp
									for (list<BasicBlock*>::iterator useBBItr = accumulatedBasicBlocks.begin(); useBBItr != accumulatedBasicBlocks.end(); useBBItr++) {
										for (BasicBlock::iterator useInstrItr = (*useBBItr)->begin(); useInstrItr != (*useBBItr)->end(); useInstrItr++) {
											//Found use in one of the instructions of the HyperOp

											if (*useItr == useInstrItr && originalToClonedInstrMap.find(*useItr) != originalToClonedInstrMap.end()) {
												Instruction * originalUseInstruction = (Instruction*) originalToClonedInstrMap.find(*useItr)->first;
												Instruction * clonedUseInstruction = (Instruction*) originalToClonedInstrMap.find(*useItr)->second;
												for (int i = 0; i < originalUseInstruction->getNumOperands(); i++) {
													if (originalUseInstruction->getOperand(i) == originalInstruction) {
														clonedUseInstruction->setOperand(i, (Value*) clonedInst);
													}
												}
											}
										}
									}
								}
								newBB->getInstList().insert(newBB->getFirstInsertionPt(), clonedInst);
							}
						}

						for (map<Function*, int>::iterator predicateSourceItr = predicateSources.begin(); predicateSourceItr != predicateSources.end(); predicateSourceItr++) {
							Value* data = ConstantInt::getTrue(getGlobalContext());
							Value * values[4];
							values[0] = MDString::get(ctxt, HYPEROP_PREDICATES);
							values[1] = hyperOpAndAnnotationMap.find(predicateSourceItr->first)->second;
							values[2] = funcAnnotation;
							values[3] = ConstantInt::get(ctxt, APInt(32, predicateSourceItr->second));
							MDNode * producesRelationship = MDNode::get(ctxt, values);
							annotationsNode->addOperand(producesRelationship);
						}
					}

					for(list<Instruction*>::iterator instrForMove=instructionsToBeMoved.begin();instrForMove!=instructionsToBeMoved.end();instrForMove++){
						(*instrForMove)->removeFromParent();
						newFunction->getEntryBlock().getInstList().insert(newFunction->getEntryBlock().getFirstInsertionPt(),*instrForMove);
					}
					DEBUG(dbgs() << "Created HyperOp:");
					newFunction->print(dbgs());
					accumulatedBasicBlocks.clear();
					hyperOpArguments.clear();
					hyperOpArgCount = 0;
					endOfHyperOp = false;
				}
				//Ensure breadth biased traversal such that all predecessors of a basic block are traversed before the basic block
				for (int i = 0; i < bbItr->getTerminator()->getNumSuccessors(); i++) {
					if (find(traversedBasicBlocks.begin(), traversedBasicBlocks.end(), bbItr->getTerminator()->getSuccessor(i)) == traversedBasicBlocks.end()) {
						bbTraverser.push_back(bbItr->getTerminator()->getSuccessor(i));
					}
				}
			}
		}

		for (map<Function*, list<Function*> >::iterator originalFunctionItr = originalFunctionToCreatedHyperOpsMap.begin(); originalFunctionItr != originalFunctionToCreatedHyperOpsMap.end(); originalFunctionItr++) {
			//Remove old functions from module
			originalFunctionItr->first->eraseFromParent();
		}

		return true;
	}
}
;
char HyperOpCreationPass::ID = 2;
char* HyperOpCreationPass::NEW_NAME = "newName";
static RegisterPass<HyperOpCreationPass> X("HyperOpCreationPass", "Pass to create HyperOps");
