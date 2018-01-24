/*
 * HyperOpMetadataParser.cpp
 *
 *  Created on: 06-Jul-2015
 *      Author: kavitha
 */

#include "HyperOpMetadataParser.h"
#include "llvm/Support/Debug.h"

HyperOpMetadataParser::HyperOpMetadataParser() {
	// TODO Auto-generated constructor stub
}

HyperOpMetadataParser::~HyperOpMetadataParser() {
	// TODO Auto-generated destructor stub
}

AllocaInst* getAllocInstrForLocalReferenceData(Module &M, Instruction* sourceInstr, map<Function*, MDNode*> functionMetadataMap) {
	if (isa<AllocaInst>(sourceInstr)) {
		return (AllocaInst*) sourceInstr;
	}
	if (isa<LoadInst>(sourceInstr)) {
		MDNode* sourceMDNode = functionMetadataMap[sourceInstr->getParent()->getParent()];
		unsigned argIndex = 0;
		Function* parentFunction = sourceInstr->getParent()->getParent();
		for (Function::arg_iterator argItr = parentFunction->arg_begin(); argItr != parentFunction->arg_end(); argItr++, argIndex++) {
			if (argItr == sourceInstr->getOperand(0)) {
				//Find the function that contains the consumed by annotation
				for (Module::iterator funcItr = M.begin(); funcItr != M.end(); funcItr++) {
					if (&*funcItr != parentFunction) {
						for (Function::iterator bbItr = funcItr->begin(); bbItr != funcItr->end(); bbItr++) {
							for (BasicBlock::iterator instrItr = bbItr->begin(); instrItr != bbItr->end(); instrItr++) {
								if (instrItr->hasMetadata()) {
									MDNode* consumedByMDNode = instrItr->getMetadata(HYPEROP_CONSUMED_BY);
									if (consumedByMDNode != 0) {
										for (unsigned i = 0; i < consumedByMDNode->getNumOperands(); i++) {
											MDNode* consumerMDNode = (MDNode*) consumedByMDNode->getOperand(i);
											((MDNode*) consumerMDNode->getOperand(0))->dump();
											if (((MDNode*) consumerMDNode->getOperand(0)) == sourceMDNode && ((ConstantInt*) consumerMDNode->getOperand(2))->getZExtValue() == argIndex) {
												return getAllocInstrForLocalReferenceData(M, instrItr, functionMetadataMap);
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
	return NULL;
}

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

bool inline hyperOpInList(HyperOp* hyperOp, list<HyperOp*> traversedList) {
	return (std::find(traversedList.begin(), traversedList.end(), hyperOp) != traversedList.end());
}

list<unsigned> parseInstanceId(StringRef instanceTag) {
	list<StringRef> parsedList = parseInstanceIdString(instanceTag);
	list<unsigned> parsedIntegerList;
//	bool isRange = false;
	for (list<StringRef>::iterator stringItr = parsedList.begin(); stringItr != parsedList.end(); stringItr++) {
		APInt id;
		pair<StringRef, StringRef> splitString = stringItr->split(',');
		splitString.first.getAsInteger(0, id);
//		if(stringItr->find(':')!=StringRef::npos){
//			isRange = true;
//		}
		parsedIntegerList.push_back(id.getZExtValue());
	}

	return parsedIntegerList;
}

//Take care of unrolling here so that unrolling is performed in a target aware manner instead of at the front end which doesn't know what the target params are
HyperOpInteractionGraph * HyperOpMetadataParser::parseMetadata(Module * M) {
	LLVMContext & ctxt = M->getContext();
	HyperOpInteractionGraph* graph = new HyperOpInteractionGraph();
	NamedMDNode * RedefineAnnotations = M->getOrInsertNamedMetadata(REDEFINE_ANNOTATIONS);
	map<MDNode*, HyperOp*> hyperOpMetadataMap;
	//Since bijective map isn't available
	map<Function*, MDNode*> functionMetadataMap;
	HyperOp* entryHyperOp;
	HyperOp* exitHyperOp;

	unsigned hyperOpId = 0;
	if (RedefineAnnotations != 0) {
		for (int i = 0; i < RedefineAnnotations->getNumOperands(); i++) {
			MDNode* hyperOpMDNode = RedefineAnnotations->getOperand(i);
			StringRef type = ((MDString*) hyperOpMDNode->getOperand(0))->getName();
			if (type.compare(HYPEROP) == 0) {
				Function* function = (Function *) hyperOpMDNode->getOperand(1);
				HyperOp *hyperOp = new HyperOp(function);
				errs() << "new hop:" << hyperOp->getFunction()->getName() << "\n";
				hyperOp->setHyperOpId(hyperOpId++);
				StringRef hyperOpType = ((MDString*) hyperOpMDNode->getOperand(2))->getName();
				if (hyperOpType.equals("Static")) {
					hyperOp->setStaticHyperOp(true);
					hyperOp->setInstanceof(function);
				} else if (hyperOpType.equals("Dynamic")) {
					hyperOp->setStaticHyperOp(false);
					hyperOp->setInstanceof((Function *) hyperOpMDNode->getOperand(3));
					hyperOp->setFbindRequired(true);
					StringRef instanceTag = ((MDString*) hyperOpMDNode->getOperand(4))->getName();
					list<unsigned> parsedId = parseInstanceId(instanceTag);
					hyperOp->setInstanceId(parsedId);
					if (hyperOpMDNode->getNumOperands() > 5) {
						hyperOp->setInRange();
						StringRef rangeTag = ((MDString*) hyperOpMDNode->getOperand(5))->getName();
						errs() << "range tag:" << rangeTag << "\n";
						list<StringRef> parsedList = parseInstanceIdString(rangeTag, ':');
						assert(parsedList.size() == 3 && "Invalid range format");
						for (auto parsedlistItr : parsedList) {
							errs() << parsedlistItr << ",";
						}
						errs() << "\n";
						StringRef lowerBound = parsedList.front();
						parsedList.pop_front();
						StringRef upperBound = parsedList.front();
						parsedList.pop_front();
						StringRef strideFunction = parsedList.front();
						pair<StringRef, StringRef> functionAndStride = strideFunction.split('(');
						StringRef function = functionAndStride.first;
						pair<StringRef, StringRef> strideSplit = functionAndStride.second.split(")");
						StringRef stride = strideSplit.first;
						hyperOp->setRangeLowerBound(ConstantInt::get(ctxt, APInt(32, atoi(lowerBound.str().c_str()))));
						hyperOp->setRangeUpperBound(ConstantInt::get(ctxt, APInt(32, atoi(upperBound.str().c_str()))));
						if (graph->StridedFunctionKeyValue.find(strideFunction) != graph->StridedFunctionKeyValue.end()) {
							hyperOp->setInductionVarUpdateFunc(graph->StridedFunctionKeyValue[strideFunction]);
						}
					}
				}
				graph->addHyperOp(hyperOp);
				DEBUG(dbgs() << "new hyop#" << hyperOp->getHyperOpId() << ":" << hyperOp->asString() << "\n");
				hyperOpMetadataMap.insert(std::make_pair(hyperOpMDNode, hyperOp));
				functionMetadataMap.insert(std::make_pair(function, hyperOpMDNode));
			}
		}
		//Traversing again to ensure that all nodes are added before labeling them as entry/exit/intermediate
		for (int i = 0; i < RedefineAnnotations->getNumOperands(); i++) {
			MDNode* hyperOpMDNode = RedefineAnnotations->getOperand(i);
			StringRef type = ((MDString*) hyperOpMDNode->getOperand(0))->getName();
			if (type.compare(HYPEROP_ENTRY) == 0) {
				entryHyperOp = hyperOpMetadataMap[(MDNode*) hyperOpMDNode->getOperand(1)];
				entryHyperOp->setStartHyperOp();
			} else if (type.compare(HYPEROP_EXIT) == 0) {
				exitHyperOp = hyperOpMetadataMap[(MDNode*) hyperOpMDNode->getOperand(1)];
				exitHyperOp->setEndHyperOp();
			} else if (type.compare(HYPEROP_INTERMEDIATE) == 0) {
				HyperOp* intermediateHyperOp = hyperOpMetadataMap[(MDNode*) hyperOpMDNode->getOperand(1)];
				intermediateHyperOp->setIntermediateHyperOp();
			}
		}
	}
	//Traverse instructions for metadata
	unsigned maxFrameSizeOfHyperOp = 0;
	list<HyperOp*> hyperOpTraversalList;
	list<HyperOp*> traversedList;

	for (Module::iterator moduleItr = M->begin(); moduleItr != M->end(); moduleItr++) {
		if (moduleItr->getName().compare("abort") != 0) {
			//Traverse through instructions of the module
			HyperOp* sourceHyperOp = graph->getHyperOp(moduleItr);
			if (sourceHyperOp != 0) {
				hyperOpTraversalList.push_back(sourceHyperOp);
			}
		} else {
			errs() << "skipping abort\n";
		}
	}
	while (!hyperOpTraversalList.empty()) {
		//Traverse through instructions of the module
		HyperOp* sourceHyperOp = hyperOpTraversalList.front();
		errs() << "source hop:" << sourceHyperOp->asString() << "\n";
		hyperOpTraversalList.pop_front();
		traversedList.push_back(sourceHyperOp);
		Function* sourceFunction;
		sourceFunction = sourceHyperOp->getFunction();
		unsigned frameSizeOfHyperOp = 0;
		unsigned argIndex = 1;
		//Traverse through instructions of the module
		for (Function::arg_iterator funcArgItr = sourceFunction->arg_begin(); funcArgItr != sourceFunction->arg_end(); funcArgItr++, argIndex++) {
			Argument* argument = &*funcArgItr;
			if (!sourceHyperOp->getFunction()->getAttributes().hasAttribute(argIndex, Attribute::InReg)) {
				frameSizeOfHyperOp += REDEFINEUtils::getSizeOfType(funcArgItr->getType());
			}
		}
		for (Function::iterator funcItr = sourceFunction->begin(); funcItr != sourceFunction->end(); funcItr++) {
			for (BasicBlock::iterator bbItr = (*funcItr).begin(); bbItr != (*funcItr).end(); bbItr++) {
				Instruction* instr = bbItr;
				if (isa<AllocaInst>(instr)) {
					frameSizeOfHyperOp += REDEFINEUtils::getSizeOfType(((AllocaInst*) instr)->getType());
				}
				if (instr->hasMetadata()) {
					MDNode* consumedByMDNode = instr->getMetadata(HYPEROP_CONSUMED_BY);
					if (consumedByMDNode != 0) {
						for (unsigned consumerMDNodeIndex = 0; consumerMDNodeIndex != consumedByMDNode->getNumOperands(); consumerMDNodeIndex++) {
							HyperOp* consumerHyperOp = 0;
							//Create an edge between two HyperOps labeled by the instruction
							MDNode* consumerMDNode = (MDNode*) consumedByMDNode->getOperand(consumerMDNodeIndex);
							errs()<<"dealing with consumed by metadata for hop "<<sourceHyperOp->asString()<<":";
							consumerMDNode->dump();
							StringRef dataType = ((MDString*) consumerMDNode->getOperand(1))->getName();
							unsigned positionOfContextSlot = ((ConstantInt*) consumerMDNode->getOperand(2))->getZExtValue();
							if (consumerMDNode->getNumOperands() > 3) {
								errs()<<"problem kya hai?\n";
								//An instance is consuming the data
								StringRef parseString = ((MDString*) consumerMDNode->getOperand(3))->getName();
								list<StringRef> consumerInstanceId = parseInstanceIdString(parseString);
								MDNode* hyperOp = (MDNode*) consumerMDNode->getOperand(0);
								//TODO
								list<unsigned> consumerHyperOpId = sourceHyperOp->getInstanceId();
								errs()<<"is source unrolled?"<<sourceHyperOp->isUnrolledInstance()<<" or prefix?"<<consumerInstanceId.front().compare("prefixId")<<"\n";
								if (!sourceHyperOp->isUnrolledInstance() || consumerInstanceId.front().compare("prefixId") == 0) {
									if (consumerInstanceId.front().compare("id") == 0) {
										consumerInstanceId.pop_front();
										APInt id;
										consumerInstanceId.front().getAsInteger(0, id);
										consumerHyperOpId.push_back(id.getZExtValue());
										StringRef staticFlag = ((MDString*) hyperOp->getOperand(2))->getName();
										if (staticFlag.compare("Static") == 0) {
											if (!consumerHyperOpId.empty()) {
												consumerHyperOpId.clear();
											}
											consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(1), consumerHyperOpId);
										} else {
											consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(3), consumerHyperOpId);
										}
									} else if (consumerInstanceId.front().compare("prefixId") == 0) {
										unsigned prefixCount = 1;
										if (consumerInstanceId.size() == 2) {
											consumerInstanceId.pop_front();
											if (consumerInstanceId.front().compare("id") == 0) {
												prefixCount = consumerHyperOpId.size() - 1;
											} else {
												prefixCount = atoi(consumerInstanceId.front().data());
											}
										}
										errs()<<"prefix count should be 1 yes?"<<prefixCount<<"\n";
										while (prefixCount--) {
											consumerHyperOpId.pop_back();
										}
										StringRef staticFlag = ((MDString*) hyperOp->getOperand(2))->getName();
										errs()<<"whats static flag?"<<staticFlag<<" for hyperOp ";
										hyperOp->dump();
//										if (staticFlag.compare("Static") == 0) {
//											if (!consumerHyperOpId.empty()) {
//												consumerHyperOpId.clear();
//											}
											consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(1), consumerHyperOpId);
//											errs() << "consumer:" << consumerHyperOp->asString() << "\n";
//										} else {
//											errs()<<"popped once and looking for dynamic instance:";
//											for(auto itr:consumerHyperOpId){
//												errs()<<itr<<",";
//											}
//											errs()<<"\n";
//											consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(3), consumerHyperOpId);
//											errs() << "consumer:" << consumerHyperOp->asString() << "\n";
////										}
									}
								}
							} else {
								consumerHyperOp = hyperOpMetadataMap[(MDNode*) consumerMDNode->getOperand(0)];
								if (sourceHyperOp->isUnrolledInstance()) {
									consumerHyperOp = graph->getOrCreateHyperOpInstance(consumerHyperOp->getFunction(), consumerHyperOp->getInstanceof(), sourceHyperOp->getInstanceId());
								}
							}
							if (consumerHyperOp != 0) {
								HyperOpEdge* edge = new HyperOpEdge();
								if (dataType.compare(SCALAR) == 0) {
									edge->Type = HyperOpEdge::SCALAR;
									//Find out if the data is being passed to an instance
								} else if (dataType.compare(LOCAL_REFERENCE) == 0) {
									edge->Type = HyperOpEdge::LOCAL_REFERENCE;
									list<unsigned> volumeOfCommunication;
									Function* consumerFunction = consumerHyperOp->getFunction();
									AllocaInst* allocInst = getAllocInstrForLocalReferenceData(*M, instr, functionMetadataMap);
									edge->setValue(allocInst);
									unsigned volume = REDEFINEUtils::getSizeOfType(allocInst->getType()) / 4;
									volumeOfCommunication.push_back(volume);
									edge->setVolume(volumeOfCommunication);
								}
								edge->setPositionOfContextSlot(positionOfContextSlot);
								edge->setValue((Value*) instr);
								sourceHyperOp->addChildEdge(edge, consumerHyperOp);
								consumerHyperOp->addParentEdge(edge, sourceHyperOp);
								if (!hyperOpInList(consumerHyperOp, traversedList) && !hyperOpInList(consumerHyperOp, hyperOpTraversalList)) {
									//						&& !sourceHyperOp->isUnrolledInstance()) {
									hyperOpTraversalList.push_back(consumerHyperOp);
								}
							}
						}
					}

					//If consumer has not been traversed already
					MDNode* controlledByMDNode = instr->getMetadata(HYPEROP_CONTROLS);
					if (controlledByMDNode != 0) {
						for (unsigned predicatedMDNodeIndex = 0; predicatedMDNodeIndex != controlledByMDNode->getNumOperands(); predicatedMDNodeIndex++) {
							HyperOp* consumerHyperOp = 0;
							MDNode* predicatedMDNode = (MDNode*) controlledByMDNode->getOperand(predicatedMDNodeIndex);
							errs() << "predicated md node:";
							predicatedMDNode->dump();
							//Create an edge between two HyperOps labeled by the instruction
							if (predicatedMDNode->getNumOperands() > 2) {
								//An instance is consuming the data
								list<StringRef> consumerInstanceId = parseInstanceIdString(((MDString*) predicatedMDNode->getOperand(2))->getName());
								MDNode* hyperOp = (MDNode*) predicatedMDNode->getOperand(0);
								//TODO
								list<unsigned> consumerHyperOpId = sourceHyperOp->getInstanceId();
								if (!sourceHyperOp->isUnrolledInstance() || consumerInstanceId.front().compare("prefixId") == 0) {
									if (consumerInstanceId.front().compare("id") == 0) {
										consumerInstanceId.pop_front();
										APInt id;
										consumerInstanceId.front().getAsInteger(0, id);
										consumerHyperOpId.push_back(id.getZExtValue());
										StringRef staticFlag = ((MDString*) hyperOp->getOperand(2))->getName();
										if (staticFlag.compare("Static") == 0) {
											if (!consumerHyperOpId.empty()) {
												consumerHyperOpId.clear();
											}
											consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(1), consumerHyperOpId);
											errs() << "consumer:" << consumerHyperOp->asString() << "\n";
										} else {
											consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(3), consumerHyperOpId);
											errs() << "consumer:" << consumerHyperOp->asString() << "\n";
										}
									} else if (consumerInstanceId.front().compare("prefixId") == 0) {
										unsigned prefixCount = 1;
										if (consumerInstanceId.size() == 2) {
											consumerInstanceId.pop_front();
											if (consumerInstanceId.front().compare("id") == 0) {
												prefixCount = consumerHyperOpId.size() - 1;
											} else {
												prefixCount = atoi(consumerInstanceId.front().data());
											}
										}
										while (prefixCount--) {
											consumerHyperOpId.pop_back();
										}
//										StringRef staticFlag = ((MDString*) hyperOp->getOperand(2))->getName();
//										errs()<<"how could hyperop be marked static?"<<staticFlag.compare("Static")<<"\n";
//										if (staticFlag.compare("Static") == 0) {
//											if (!consumerHyperOpId.empty()) {
//												consumerHyperOpId.clear();
//											}
											consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(1), consumerHyperOpId);
//											errs() << "consumer:" << consumerHyperOp->asString() << "\n";
//										} else {
//											consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(3), consumerHyperOpId);
//											errs() << "consumer:" << consumerHyperOp->asString() << "\n";
//										}
									}
								}
							} else {
								consumerHyperOp = hyperOpMetadataMap[(MDNode*) predicatedMDNode->getOperand(0)];
								if (sourceHyperOp->isUnrolledInstance()) {
									consumerHyperOp = graph->getOrCreateHyperOpInstance(consumerHyperOp->getFunction(), consumerHyperOp->getInstanceof(), sourceHyperOp->getInstanceId());
								}
							}
							if (consumerHyperOp != 0) {
								consumerHyperOp->setPredicatedHyperOp();
								HyperOpEdge* edge = new HyperOpEdge();
								edge->Type = HyperOpEdge::PREDICATE;
								edge->setValue((Value*) instr);
								errs() << "predicate value set to ";
								instr->dump();
								errs() << "number of operands in predicate md node:" << predicatedMDNode->getNumOperands() << "\n";
								StringRef predicateValue = ((MDString*) predicatedMDNode->getOperand(1))->getName();
								errs() << "predicate value:" << predicateValue << "\n";
								if (predicateValue.compare("0") == 0) {
									edge->setPredicateValue(0);
								} else {
									edge->setPredicateValue(1);
								}
								errs() << "md node on instruction:";
								instr->dump();
								errs() << "Added control edge between " << sourceHyperOp->asString() << " and " << consumerHyperOp->asString() << "\n";
								errs() << "predicate :" << predicateValue << "\n";
								sourceHyperOp->addChildEdge(edge, consumerHyperOp);
								consumerHyperOp->addParentEdge(edge, sourceHyperOp);
								if (!hyperOpInList(consumerHyperOp, traversedList) && !hyperOpInList(consumerHyperOp, hyperOpTraversalList)) {
									//						&& !sourceHyperOp->isUnrolledInstance()) {
									errs() << "added instance:" << consumerHyperOp->asString() << " and is it an instance:" << consumerHyperOp->isUnrolledInstance() << "\n";
									hyperOpTraversalList.push_back(consumerHyperOp);
								}
							}
						}
					}

					MDNode* syncMDNode = instr->getMetadata(HYPEROP_SYNC);
					if (syncMDNode != 0) {
						for (unsigned syncMDNodeIndex = 0; syncMDNodeIndex != syncMDNode->getNumOperands(); syncMDNodeIndex++) {
							HyperOp* consumerHyperOp = 0;
							MDNode* syncedMDNode = (MDNode*) syncMDNode->getOperand(syncMDNodeIndex);
							errs() << "synced md node:";
							syncedMDNode->dump();
							//Create an edge between two HyperOps labeled by the instruction
							if (syncedMDNode->getNumOperands() > 1) {
								//An instance is consuming the data
								list<StringRef> consumerInstanceId = parseInstanceIdString(((MDString*) syncedMDNode->getOperand(1))->getName());
								MDNode* hyperOp = (MDNode*) syncedMDNode->getOperand(0);
								list<unsigned> consumerHyperOpId = sourceHyperOp->getInstanceId();
								if (!sourceHyperOp->isUnrolledInstance() || consumerInstanceId.front().compare("prefixId") == 0) {
									if (consumerInstanceId.front().compare("id") == 0) {
										consumerInstanceId.pop_front();
										APInt id;
										consumerInstanceId.front().getAsInteger(0, id);
										consumerHyperOpId.push_back(id.getZExtValue());
										StringRef staticFlag = ((MDString*) hyperOp->getOperand(2))->getName();
										if (staticFlag.compare("Static") == 0) {
											if (!consumerHyperOpId.empty()) {
												consumerHyperOpId.clear();
											}
											consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(1), consumerHyperOpId);
											errs() << "consumer:" << consumerHyperOp->asString() << "\n";
										} else {
											consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(3), consumerHyperOpId);
											errs() << "consumer:" << consumerHyperOp->asString() << "\n";
										}
									} else if (consumerInstanceId.front().compare("prefixId") == 0) {
										unsigned prefixCount = 1;
										if (consumerInstanceId.size() == 2) {
											consumerInstanceId.pop_front();
											if (consumerInstanceId.front().compare("id") == 0) {
												prefixCount = consumerHyperOpId.size() - 1;
											} else {
												prefixCount = atoi(consumerInstanceId.front().data());
											}
										}
										while (prefixCount--) {
											consumerHyperOpId.pop_back();
										}
//										StringRef staticFlag = ((MDString*) hyperOp->getOperand(2))->getName();
//										if (staticFlag.compare("Static") == 0) {
//											//Hack
//											if (!consumerHyperOpId.empty()) {
//												consumerHyperOpId.clear();
//											}
											consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(1), consumerHyperOpId);
//											errs() << "static consumer:" << consumerHyperOp->asString() << "\n";
//										} else {
//											consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(3), consumerHyperOpId);
//											errs() << "dynamic consumer:" << consumerHyperOp->asString() << "\n";
//										}
									}
								}
							} else {
								consumerHyperOp = hyperOpMetadataMap[(MDNode*) syncedMDNode->getOperand(0)];
								if (sourceHyperOp->isUnrolledInstance()) {
									consumerHyperOp = graph->getOrCreateHyperOpInstance(consumerHyperOp->getFunction(), consumerHyperOp->getInstanceof(), sourceHyperOp->getInstanceId());
								}
							}
							if (consumerHyperOp != 0) {
								//Create an edge between two HyperOps labeled by the instruction
								HyperOpEdge* edge = new HyperOpEdge();
								edge->Type = HyperOpEdge::SYNC;
								errs() << "Added sync edge between " << sourceHyperOp->asString() << " and " << consumerHyperOp->asString() << "\n";
								sourceHyperOp->addChildEdge(edge, consumerHyperOp);
								consumerHyperOp->addParentEdge(edge, sourceHyperOp);
								consumerHyperOp->setBarrierHyperOp();
								consumerHyperOp->incrementIncomingSyncCount(0);
								if (!hyperOpInList(consumerHyperOp, traversedList) && !hyperOpInList(consumerHyperOp, hyperOpTraversalList)) {
									//						&& !sourceHyperOp->isUnrolledInstance()) {
									errs() << "added instance:" << consumerHyperOp->asString() << " and is it an instance:" << consumerHyperOp->isUnrolledInstance() << "\n";
									hyperOpTraversalList.push_back(consumerHyperOp);
								}
							}
						}
					}
					MDNode* rangeMDNode = instr->getMetadata(HYPEROP_RANGE);
					if (rangeMDNode != 0) {
						errs() << "range node\n";
						for (unsigned rangeMDNodeIndex = 0; rangeMDNodeIndex != rangeMDNode->getNumOperands(); rangeMDNodeIndex++) {
							MDNode* indirectionNode = (MDNode*) rangeMDNode->getOperand(rangeMDNodeIndex);
							if (indirectionNode != 0) {
								MDNode* hyperOpMDNode = (MDNode*) indirectionNode->getOperand(0);
								HyperOp* consumerDynamicHyperOp = hyperOpMetadataMap[hyperOpMDNode];
								//Add an edge from the HyperOp housing the range node
								HyperOpEdge* edge = new HyperOpEdge();
								edge->setType(HyperOpEdge::RANGE);
								sourceHyperOp->addChildEdge(edge, consumerDynamicHyperOp);
								consumerDynamicHyperOp->addChildEdge(edge, sourceHyperOp);
							}
						}

					}
				}
			}
		}
		if (maxFrameSizeOfHyperOp < frameSizeOfHyperOp) {
			maxFrameSizeOfHyperOp = frameSizeOfHyperOp;
		}
	}
	errs() << "before deleting nodes:";
	graph->print(errs());

	//This had to be written as follows because removal of one node may cause other nodes to go hanging
	while (true) {
		bool updatedGraph = false;
		list<HyperOp*> vertices = graph->Vertices;
		for (list<HyperOp*>::iterator vertexItr = vertices.begin(); vertexItr != vertices.end(); vertexItr++) {
			if (!(*vertexItr)->isEndHyperOp() && (*vertexItr)->ChildMap.empty()) {
				if (!(*vertexItr)->isUnrolledInstance()) {
					(*vertexItr)->getFunction()->eraseFromParent();
				}
				graph->removeHyperOp(*vertexItr);
				updatedGraph = true;
				break;
			}

			else if (!(*vertexItr)->isStartHyperOp() && (*vertexItr)->ParentMap.empty()) {
				if (!(*vertexItr)->isUnrolledInstance()) {
					(*vertexItr)->getFunction()->eraseFromParent();
				}
				graph->removeHyperOp(*vertexItr);
				updatedGraph = true;
				break;
			}
		}
		if (!updatedGraph) {
			break;
		}
	}

	graph->setMaxMemFrameSize(maxFrameSizeOfHyperOp);
	graph->print(errs());
	return graph;
}

