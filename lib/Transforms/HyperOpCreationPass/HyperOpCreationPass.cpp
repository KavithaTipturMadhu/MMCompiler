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
	const string REFERENCE_ARGUMENT = "Reference";

	const unsigned int FRAME_SIZE = 4;

	enum HyperOpArgumentType {
		SCALAR, LOCAL_REFERENCE, GLOBAL_REFERENCE
	};

	typedef map<unsigned, pair<list<Value*>, HyperOpArgumentType> > HyperOpArgumentMap;

	HyperOpCreationPass() :
			ModulePass(ID) {
	}

	virtual void getAnalysisUsage(AnalysisUsage &AU) const {
		//Mandatory merge return to be invoked on each function
		AU.addRequired<UnifyFunctionExitNodes>();
	}

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

	HyperOpArgumentType supportedArgType(Value* argument, Module::GlobalListType globalVariableList) {
		if (isa<AllocaInst>(argument) || argument->getType()->getTypeID()==Type::FloatTyID) {
			return LOCAL_REFERENCE;
		}
		//TODO
		else if (isa<GetElementPtrInst>(argument)) {
			for(Module::GlobalListType::iterator globalVarItr = globalVariableList.begin();globalVarItr!=globalVariableList.end();globalVarItr++){
				if(((GetElementPtrInst*)argument)->getPointerOperand()==*globalVarItr){
					return GLOBAL_REFERENCE;
				}
			}
			return LOCAL_REFERENCE;
		}

		return SCALAR;
	}

	unsigned distanceToExitBlock(BasicBlock* basicBlock, list<BasicBlock*> visitedBasicBlocks){
		//Merge return assumed here
		unsigned depthOfSuccessor = 0;
		bool first = true;
		for(unsigned i=0;i<basicBlock->getTerminator()->getNumSuccessors();i++){
			BasicBlock* succBB  = basicBlock->getTerminator()->getSuccessor(i);
			visitedBasicBlocks.push_back(succBB);
			if(first||depthOfSuccessor>distanceToExitBlock(succBB, visitedBasicBlocks)){
				depthOfSuccessor = distanceToExitBlock(succBB, visitedBasicBlocks);
				first = false;
			}
			visitedBasicBlocks.pop_back();
		}
		return 1+depthOfSuccessor;
	}

	virtual bool runOnModule(Module &M) {
		LLVMContext & ctxt = M.getContext();
		list<Function*> addedFunctions;
		//Top level annotation corresponding to all annotations REDEFINE
		NamedMDNode * annotationsNode = M.getOrInsertNamedMetadata(REDEFINE_ANNOTATIONS);

		//Contains all created HyperOp functions and the basic blocks they contain in the original module and HyperOp arguments
		map<Function*, pair<list<BasicBlock*>, HyperOpArgumentMap> > createdHyperOpAndOriginalBasicBlockAndArgMap;

		//Original Instructions and their clones
		map<Value*, Value*> originalToClonedInstMap;

		//Map of the return instruction introduced in each HyperOp function
		map<Function*, Instruction*> retInstMap;

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
								Function * calledFunction;
								if(isa<CallInst>(instItr)){
									calledFunction = ((CallInst*)instItr)->getCalledFunction();
								}else if(isa<InvokeInst>(instItr)){
									calledFunction = ((InvokeInst*)instItr)->getCalledFunction();
								}
								//An intrinsic function translates to an instruction in the backend, don't treat it like a function call
								//Otherwise, create a function boundary as follows
								if(!calledFunction->isIntrinsic()){
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
											hyperOpArguments.insert(make_pair(i, make_pair(argumentList, supportedArgType(argument, M.getGlobalList()))));
										}
									}
									endOfHyperOp = true;
									break;
								}
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
										hyperOpArguments.insert(make_pair(hyperOpArgCount++, make_pair(newArg, supportedArgType(*newArgItr, M.getGlobalList()))));
									}
								} else {
									//Phi instruction's arguments correspond to only one argument to a HyperOp
									hyperOpArguments.insert(make_pair(hyperOpArgCount++, make_pair(newHyperOpArguments, supportedArgType(newHyperOpArguments.front(), M.getGlobalList()))));
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


					bool isEntry = false;
					bool isExit = false;
					if(accumulatedBasicBlocks.front()->getParent()->getName()=="redefinemain"){
						if(find(accumulatedBasicBlocks.begin(),accumulatedBasicBlocks.end(), funcItr->getEntryBlock())!=accumulatedBasicBlocks.end()){
							isEntry = true;
						}else if(find(accumulatedBasicBlocks.begin(),accumulatedBasicBlocks.end(), funcItr->back())!=accumulatedBasicBlocks.end()){
							isExit = true;
						}
					}
					//Add metadata to label the function as a HyperOp
					Value * values[3];
					values[0] = MDString::get(ctxt, HYPEROP);
					values[1] = newFunction;
					values[2] = isEntry?"Entry":(isExit?"Exit":"Intermediate");
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
											MDNode* currentMetadataOfInstruction = ((Instruction*) originalToClonedInstMap.find(argument)->second)->getMetadata(HYPEROP_CONSUMED_BY);
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
											((Instruction*) originalToClonedInstMap.find(argument)->second)->setMetadata(HYPEROP_CONSUMED_BY, newMDNode);
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
												Instruction* clonedInstruction = (Instruction*) originalToClonedInstMap.find((Instruction*) argument)->second;
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

					//Map of predicate's source instruction in a different HyperOp, the branch instruction needs to be deleted after the predicate is marked for delivery since we need writecmp instructions to predicate HyperOps unlike the regular branch instructions(j, beq etc)
					map<Instruction*, unsigned> conditionalBranchSources;
					list<Instruction*> unconditionalBranchSources;
					for (list<BasicBlock*>::iterator accumulatedBBItr = accumulatedBasicBlocks.begin(); accumulatedBBItr != accumulatedBasicBlocks.end(); accumulatedBBItr++) {
						//Find out if any basic block is predicated
						BasicBlock* originalBB = *accumulatedBBItr;
						//Predicates are only for non-entry basic blocks
						if (originalBB != &(funcItr->getEntryBlock())) {
							for (pred_iterator predecessorItr = pred_begin(originalBB); predecessorItr != pred_end(originalBB); predecessorItr++) {
								BasicBlock* predecessor = *predecessorItr;
								//Control flow edge exists from a predecessor basic block that does not belong to the same HyperOp
								if (find(accumulatedBasicBlocks.begin(), accumulatedBasicBlocks.end(), predecessor) == accumulatedBasicBlocks.end()) {
									for (map<Function*, pair<list<BasicBlock*>, HyperOpArgumentMap> >::iterator createdHopItr = createdHyperOpAndOriginalBasicBlockAndArgMap.begin(); createdHopItr != createdHyperOpAndOriginalBasicBlockAndArgMap.end(); createdHopItr++) {
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
												for (int i = 0; i < terminator->getNumSuccessors(); i++) {
													if (terminator->getSuccessor(i) == originalBB) {
														//Add only those successors that correspond to a basic block in the current HyperOp
														conditionalBranchSources.insert(make_pair(terminator, i));
													}
												}
											}
										}
									}
								}
							}
						}
						BasicBlock *newBB = BasicBlock::Create(getGlobalContext(), (*accumulatedBBItr)->getName(), newFunction);
						for (BasicBlock::reverse_iterator instItr = (*accumulatedBBItr)->rbegin(); instItr != (*accumulatedBBItr)->rend(); instItr++) {
							Instruction* clonedInst = instItr->clone();
							Instruction * originalInstruction = &*instItr;
							originalToClonedInstMap.insert(std::make_pair(originalInstruction, clonedInst));
							newBB->getInstList().insert(newBB->getFirstInsertionPt(), clonedInst);
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

								//Propogate the instruction as Value* to all those places that use the instruction
								for (Value::use_iterator useItr = instItr->use_begin(); useItr != instItr->use_end(); useItr++) {
									//Find out if the use is in the same HyperOp
									for (list<BasicBlock*>::iterator useBBItr = accumulatedBasicBlocks.begin(); useBBItr != accumulatedBasicBlocks.end(); useBBItr++) {
										for (BasicBlock::iterator useInstrItr = (*useBBItr)->begin(); useInstrItr != (*useBBItr)->end(); useInstrItr++) {
											//Found use in one of the instructions of the HyperOp

											if (*useItr == useInstrItr && originalToClonedInstMap.find(*useItr) != originalToClonedInstMap.end()) {
												Instruction * originalUseInstruction = (Instruction*) originalToClonedInstMap.find(*useItr)->first;
												Instruction * clonedUseInstruction = (Instruction*) originalToClonedInstMap.find(*useItr)->second;
												for (int i = 0; i < originalUseInstruction->getNumOperands(); i++) {
													if (originalUseInstruction->getOperand(i) == originalInstruction) {
														clonedUseInstruction->setOperand(i, (Value*) clonedInst);
													}
												}
											}
										}
									}
								}
							}
						}
					}

					//Move alloca instructions which don't need to be retained in the first HyperOp since it has no uses anywhere other than the current HyperOp being created
					for (list<Instruction*>::iterator instrToBeMoved = instructionsToBeMoved.begin(); instrToBeMoved != instructionsToBeMoved.end(); instrToBeMoved++) {
						(*instrToBeMoved)->removeFromParent();
						newFunction->getEntryBlock().getInstList().insert(newFunction->getEntryBlock().getFirstInsertionPt(), *instrToBeMoved);
					}

					//Add a basic block with a dummy return instruction such that branch instructions to other HyperOps can jump to it
					BasicBlock* retBB = BasicBlock::Create(ctxt, newFunction->getName().str().append("return"), newFunction);
					Instruction* retInst = ReturnInst::Create(ctxt);
					retInstMap.insert(make_pair(newFunction, retInst));
					retBB->getInstList().insert(retBB->getFirstInsertionPt(), retInst);

					//Find out if there exist any branch instructions leading to the HyperOp, add metadata to the instruction
					for (map<Instruction*, unsigned>::iterator conditionalBranchSourceItr = conditionalBranchSources.begin(); conditionalBranchSourceItr != conditionalBranchSources.end(); conditionalBranchSourceItr++) {
						//Find the branch instruction operand's clone
						Instruction* clonedInstruction = (Instruction*) originalToClonedInstMap.find(conditionalBranchSourceItr->first)->second;
						//Branch instruction's first operand
						Instruction* predicateOperand = (Instruction*) clonedInstruction->getOperand(0);

						MDNode* predicateMetadata = clonedInstruction->getMetadata(HYPEROP_CONTROLLED_BY);
						vector<Value*> metadataList;
						if (predicateMetadata != 0) {
							predicateMetadata = clonedInstruction->getMetadata(HYPEROP_CONTROLLED_BY);
							for (unsigned i = 0; i < predicateMetadata->getNumOperands(); i++) {
								metadataList.push_back(predicateMetadata->getOperand(i));
							}
						}
						//Label the instruction with predicates metadata
						Value * values[2];
						values[0] = funcAnnotation;
						values[1] = ConstantInt::get(ctxt, APInt(32, conditionalBranchSourceItr->second));
						MDNode* newPredicateMetadata = MDNode::get(ctxt, values);
						metadataList.push_back(newPredicateMetadata);
						ArrayRef<Value*> metadataRef(metadataList);
						MDNode * predicatesRelation = MDNode::get(ctxt, metadataRef);
						predicateOperand->setMetadata(HYPEROP_CONTROLLED_BY, predicatesRelation);

						//Update the branch instruction to point to its return instruction
						clonedInstruction->setOperand(conditionalBranchSourceItr->second, retInstMap.find(clonedInstruction->getParent()->getParent())->second);
					}

					errs() << "Dealing with unconditional branches from other HyperOps\n";
					//Remove unconditional branch instruction, add the annotation to the return instruction of the branch
					for (list<Instruction*>::iterator unconditionalBranchSourceItr = unconditionalBranchSources.begin(); unconditionalBranchSourceItr != unconditionalBranchSources.end(); unconditionalBranchSourceItr++) {
						Instruction* unconditionalBranchInstr = *unconditionalBranchSourceItr;
						Instruction* clonedInstr = (Instruction*) originalToClonedInstMap.find(unconditionalBranchInstr)->second;
						retInstMap.find(clonedInstr->getParent()->getParent())->second->dump();
						//Update the branch instruction to jump to the return basic block
						Instruction* retInstOfProducer = retInstMap.find(clonedInstr->getParent()->getParent())->second;
						originalToClonedInstMap.erase(unconditionalBranchInstr);
						clonedInstr->setOperand(0, retInstOfProducer->getParent());
						originalToClonedInstMap.insert(make_pair(unconditionalBranchInstr, clonedInstr));
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
						retInstOfProducer->setMetadata(HYPEROP_CONTROLLED_BY, predicatesRelation);
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
					map<unsigned, list<BasicBlock*> > untraversedBasicBlocks;
					vector<unsigned> depthsInSortedOrder;
					if (find(traversedBasicBlocks.begin(), traversedBasicBlocks.end(), bbItr->getTerminator()->getSuccessor(i)) == traversedBasicBlocks.end()) {
						BasicBlock* succBB = bbItr->getTerminator()->getSuccessor(i);
						list<BasicBlock*> visitedBasicBlockList;
						unsigned distanceFromExit = distanceToExitBlock(succBB, visitedBasicBlockList);
						list<BasicBlock*> basicBlockList;
						if(untraversedBasicBlocks.find(distanceFromExit)!=untraversedBasicBlocks.end()){
							basicBlockList = untraversedBasicBlocks.find(distanceFromExit)->second;
							untraversedBasicBlocks.erase(distanceFromExit);
							depthsInSortedOrder.erase(distanceFromExit);
						}
						basicBlockList.push_back(succBB);
						untraversedBasicBlocks.insert(make_pair(distanceFromExit, basicBlockList));
						depthsInSortedOrder.push_back(distanceFromExit);
					}

					//Sort the basic blocks in descending order of their distance to exit block
					list<BasicBlock* > sortedSuccBasicBlockList;
					for(unsigned i=0;i<depthsInSortedOrder.size();i++){
						unsigned min = depthsInSortedOrder[i];
						for(unsigned j=i+1;j<depthsInSortedOrder.size();j++){
							if(min>depthsInSortedOrder[j]){
								unsigned temp= depthsInSortedOrder[j];
								depthsInSortedOrder[i]=temp;
								depthsInSortedOrder[j]=min;
								min = temp;
							}
						}
					}

					for(unsigned i=0;i<depthsInSortedOrder.size();i++){
						list<BasicBlock*> succBBList = untraversedBasicBlocks.find(i)->second;
						for(list<BasicBlock*>::iterator succBBItr =succBBList.begin();succBBItr!=succBBList.end(); succBBItr++){
							bbTraverser.push_front(*succBBItr);
						}
					}
				}
			}
		}

		for (map<Function*, list<Function*> >::iterator originalFunctionItr = originalFunctionToCreatedHyperOpsMap.begin(); originalFunctionItr != originalFunctionToCreatedHyperOpsMap.end(); originalFunctionItr++) {
			//Remove old functions from module
			originalFunctionItr->first->eraseFromParent();
		}
		DEBUG(dbgs(), "Final module contents:");
		M.print(dbgs());
		return true;
	}
}
;
char HyperOpCreationPass::ID = 2;
char* HyperOpCreationPass::NEW_NAME = "newName";
static RegisterPass<HyperOpCreationPass> X("HyperOpCreationPass", "Pass to create HyperOps");
