/*
 * HyperOpMetadataParser.cpp
 *
 *  Created on: 06-Jul-2015
 *      Author: kavitha
 */

#include "llvm/IR/HyperOpMetadataParser.h"
#include "llvm/Support/Debug.h"

HyperOpMetadataParser::HyperOpMetadataParser() {
	// TODO Auto-generated constructor stub
}

HyperOpMetadataParser::~HyperOpMetadataParser() {
	// TODO Auto-generated destructor stub
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
				HyperOp *hyperOp = new HyperOp(function, graph);
				errs() << "new hop:"<<(hyperOp->getFunction()!=NULL)<<":function "<<function->getName() ;
				errs()<< hyperOp->getFunction()->getName() << "\n";
				hyperOp->setHyperOpId(hyperOpId++);
				StringRef hyperOpType = ((MDString*) hyperOpMDNode->getOperand(2))->getName();
				if (hyperOpType.equals("Static")) {
					hyperOp->setStaticHyperOp(true);
					hyperOp->setInstanceof(function);
				} else if (hyperOpType.equals("Dynamic")) {
					hyperOp->setStaticHyperOp(false);
					hyperOp->setInstanceof((Function *) hyperOpMDNode->getOperand(3));
					StringRef instanceTag = ((MDString*) hyperOpMDNode->getOperand(4))->getName();
					list<unsigned> parsedId = parseInstanceId(instanceTag);
					hyperOp->setInstanceId(parsedId);
					if (hyperOpMDNode->getNumOperands() > 5) {
						hyperOp->setInRange();
						if (isa<MDString>(hyperOpMDNode->getOperand(5))) {
							StringRef lowerBound = ((MDString*) hyperOpMDNode->getOperand(5))->getName();
							hyperOp->setRangeLowerBound(ConstantInt::get(ctxt, APInt(32, atoi(lowerBound.str().c_str()))));
						} else {
							hyperOp->setRangeLowerBound(hyperOpMDNode->getOperand(5));
						}
						if (isa<MDString>(hyperOpMDNode->getOperand(6))) {
							StringRef upperBound = ((MDString*) hyperOpMDNode->getOperand(6))->getName();
							hyperOp->setRangeUpperBound(ConstantInt::get(ctxt, APInt(32, atoi(upperBound.str().c_str()))));
						} else {
							hyperOp->setRangeUpperBound(hyperOpMDNode->getOperand(6));
						}
						assert(isa<MDString>(hyperOpMDNode->getOperand(7)) && "Unsupported induction var update function");
						StringRef strideFunction = ((MDString*) hyperOpMDNode->getOperand(7))->getName();
						if (graph->StridedFunctionKeyValue.find(strideFunction) != graph->StridedFunctionKeyValue.end()) {
							hyperOp->setInductionVarUpdateFunc(graph->StridedFunctionKeyValue[strideFunction]);
						}

						assert(isa<ConstantInt>(hyperOpMDNode->getOperand(8)) && "Unsupported increment value, only constants allowed at this point");
						hyperOp->setStride(hyperOpMDNode->getOperand(8));
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
		hyperOpTraversalList.pop_front();
		traversedList.push_back(sourceHyperOp);
		Function* sourceFunction;
		sourceFunction = sourceHyperOp->getFunction();

		for (Function::iterator funcItr = sourceFunction->begin(); funcItr != sourceFunction->end(); funcItr++) {
			for (BasicBlock::iterator bbItr = (*funcItr).begin(); bbItr != (*funcItr).end(); bbItr++) {
				Instruction* instr = bbItr;
				if (instr->hasMetadata()) {
					MDNode* consumedByMDNode = instr->getMetadata(HYPEROP_CONSUMED_BY);
					if (consumedByMDNode != 0) {
						for (unsigned consumerMDNodeIndex = 0; consumerMDNodeIndex != consumedByMDNode->getNumOperands(); consumerMDNodeIndex++) {
							HyperOp* consumerHyperOp = 0;
							//Create an edge between two HyperOps labeled by the instruction
							MDNode* consumerMDNode = (MDNode*) consumedByMDNode->getOperand(consumerMDNodeIndex);
							unsigned positionOfContextSlot = ((ConstantInt*) consumerMDNode->getOperand(1))->getZExtValue();
							if (consumerMDNode->getNumOperands() > 2) {
								//An instance is consuming the data
								StringRef parseString = ((MDString*) consumerMDNode->getOperand(2))->getName();
								list<StringRef> consumerInstanceId = parseInstanceIdString(parseString);
								MDNode* hyperOp = (MDNode*) consumerMDNode->getOperand(0);
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
										while (prefixCount--) {
											consumerHyperOpId.pop_back();
										}
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
								edge->Type = HyperOpEdge::SCALAR;
								list<unsigned> volumeOfCommunication;
								unsigned volume = REDEFINEUtils::getSizeOfType(instr->getType()) / 4;
								volumeOfCommunication.push_back(volume);
								edge->setVolume(volumeOfCommunication);
								edge->setPositionOfContextSlot(positionOfContextSlot);
								edge->setValue((Value*) instr);
								sourceHyperOp->addChildEdge(edge, consumerHyperOp);
								consumerHyperOp->addParentEdge(edge, sourceHyperOp);
								if (!hyperOpInList(consumerHyperOp, traversedList) && !hyperOpInList(consumerHyperOp, hyperOpTraversalList)) {
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
								StringRef predicateValue = ((MDString*) predicatedMDNode->getOperand(1))->getName();
								if (predicateValue.compare("0") == 0) {
									edge->setPredicateValue(0);
								} else {
									edge->setPredicateValue(1);
								}
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
										while (prefixCount--) {
											consumerHyperOpId.pop_back();
										}
										StringRef staticFlag = ((MDString*) hyperOp->getOperand(2))->getName();
										if (staticFlag.compare("Static") == 0) {
											//Hack
											if (!consumerHyperOpId.empty()) {
												consumerHyperOpId.clear();
											}
											consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(1), consumerHyperOpId);
										} else {
											consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(3), consumerHyperOpId);
										}
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
								sourceHyperOp->addChildEdge(edge, consumerHyperOp);
								consumerHyperOp->addParentEdge(edge, sourceHyperOp);
								consumerHyperOp->setBarrierHyperOp();
								if (sourceHyperOp->getInRange()) {
									consumerHyperOp->addIncomingSyncValue(0, (SyncValue)sourceHyperOp);
								} else {
									consumerHyperOp->addIncomingSyncValue(0, (SyncValue)1);
								}
								if (!hyperOpInList(consumerHyperOp, traversedList) && !hyperOpInList(consumerHyperOp, hyperOpTraversalList)) {
									hyperOpTraversalList.push_back(consumerHyperOp);
								}
							}
						}
					}
				}
			}
		}
	}
	graph->print(errs());
	return graph;
}

static pair<string, string> getParsedProperty(string keyvaluestr){
	string key = keyvaluestr.substr(0, keyvaluestr.find("="));
	string value = keyvaluestr.substr(keyvaluestr.find("="));
	value.replace(0,1,"");
	return make_pair(key, value);
}

/* Parse HyperOps list from the output of IR pass*/
map<HyperOp*, map<int, int> > HyperOpMetadataParser::parseHyperOpMetadata(Module * M){
	map<HyperOp*, map<int, int> > hyperOps;

	LLVMContext & ctxt = M->getContext();
	NamedMDNode * HyperOpAnnotations = M->getOrInsertNamedMetadata(HYPEROP_ANNOTATIONS);

	if (HyperOpAnnotations != 0) {
		for (int i = 0; i < HyperOpAnnotations->getNumOperands(); i++) {
			MDNode* hyperOpMDNode = HyperOpAnnotations->getOperand(i);

			Function* function = (Function *) hyperOpMDNode->getOperand(0);
			HyperOp *hyperOp = new HyperOp(function, NULL);
			for (int j = 1; j < hyperOpMDNode->getNumOperands() - 1; j++){
				/* Find all key value pairs */
				Value* property = hyperOpMDNode->getOperand(j);
				assert(isa<MDString>(property) && "No property type other than string supported");
				MDString* propString = (MDString*)property;
				pair<string, string> keyValuePair = getParsedProperty(propString->getString().str());
				if(!keyValuePair.first.compare(HYPEROP_ID)){
					hyperOp->setHyperOpId(atoi(keyValuePair.second.data()));
				}if(!keyValuePair.first.compare(HYPEROP_AFFINITY)){
					hyperOp->setTargetResource(atoi(keyValuePair.second.data()));
				}if(!keyValuePair.first.compare(HYPEROP_FRAME)){
					hyperOp->setContextFrame(atoi(keyValuePair.second.data()));
				}if(!keyValuePair.first.compare(STATIC_HYPEROP)){
					if(!keyValuePair.second.compare("yes"))
						hyperOp->setStaticHyperOp(true);
					else
						hyperOp->setStaticHyperOp(false);
				}if(!keyValuePair.first.compare(HYPEROP_ENTRY)){
					if(!keyValuePair.second.compare("yes"))
						hyperOp->setStartHyperOp();
				}if(!keyValuePair.first.compare(HYPEROP_EXIT)){
					if(!keyValuePair.second.compare("yes"))
						hyperOp->setEndHyperOp();
				}if(!keyValuePair.first.compare(HYPEROP_PREDICATED)){
					if(!keyValuePair.second.compare("yes"))
						hyperOp->setPredicatedHyperOp();
				}if(!keyValuePair.first.compare(HYPEROP_BARRIER)){
					if(!keyValuePair.second.compare("yes"))
						hyperOp->setBarrierHyperOp();
				}
			}
			map<int, int > argIndexOffsetMap;
			/* Last operand is the offset map */
			Value* offsetMap = hyperOpMDNode->getOperand(hyperOpMDNode->getNumOperands() - 1);
			assert(isa<MDNode>(offsetMap) && "Offset map must be a list of MD nodes\n");
			for (int j = 0; j < ((MDNode*)offsetMap)->getNumOperands(); j++){
				Value* offsetKeyValue = ((MDNode*)offsetMap)->getOperand(j);
				assert(isa<MDString>(offsetKeyValue) && "No property type other than string supported");
				MDString* offsetString = (MDString*)offsetKeyValue;
				pair<string, string> keyValuePair = getParsedProperty(offsetString->getString().str());
				int argIndex = atoi(keyValuePair.first.c_str());
				int argOffset = atoi(keyValuePair.second.c_str());
				argIndexOffsetMap.insert(make_pair(argIndex, argOffset));
			}
			hyperOps.insert(make_pair(hyperOp, argIndexOffsetMap));
		}
	}

	return hyperOps;
}

