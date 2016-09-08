/*
 * HyperOpMetadataParser.cpp
 *
 *  Created on: 06-Jul-2015
 *      Author: kavitha
 */

#include "HyperOpMetadataParser.h"

HyperOpMetadataParser::HyperOpMetadataParser() {
	// TODO Auto-generated constructor stub
}

HyperOpMetadataParser::~HyperOpMetadataParser() {
	// TODO Auto-generated destructor stub
}

AllocaInst* getAllocInstrForLocalReferenceData(Module &M, Instruction* sourceInstr, MDNode* sourceMDNode, map<Function*, MDNode*> functionMetadataMap) {
	if (isa<AllocaInst>(sourceInstr)) {
		return (AllocaInst*) sourceInstr;
	}
	if (isa<LoadInst>(sourceInstr)) {
		unsigned argIndex = 0;
		Function* parentFunction = sourceInstr->getParent()->getParent();
		for (Function::arg_iterator argItr = parentFunction->arg_begin(); argItr != parentFunction->arg_end(); argItr++, argIndex++) {
			if (argItr == sourceInstr->getOperand(0)) {
				//Find the function that contains the consumed by annotation
				for (Module::iterator funcItr = M.begin(); funcItr != M.end(); funcItr) {
					if (&*funcItr != parentFunction) {
						for (Function::iterator bbItr = funcItr->begin(); bbItr != funcItr->end(); bbItr++) {
							for (BasicBlock::iterator instrItr = bbItr->begin(); instrItr != bbItr->end(); instrItr++) {
								if (instrItr->hasMetadata()) {
									MDNode* consumedByMDNode = instrItr->getMetadata(HYPEROP_CONSUMED_BY);
									if (consumedByMDNode != 0) {
										for (unsigned i = 0; i < consumedByMDNode->getNumOperands(); i++) {
											MDNode* consumerMDNode = (MDNode*) consumedByMDNode->getOperand(i);
											if (((MDNode*) consumerMDNode->getOperand(0)) == sourceMDNode && ((ConstantInt*) consumerMDNode->getOperand(2))->getZExtValue() == argIndex) {
												return getAllocInstrForLocalReferenceData(M, instrItr, functionMetadataMap[funcItr], functionMetadataMap);
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

list<StringRef> parseInstanceIdString(StringRef instanceTag) {
	list<StringRef> instanceId;
//Parse string to get a list of identifiers
	instanceTag = instanceTag.drop_back();
	instanceTag = instanceTag.drop_front();
	StringRef tempString = instanceTag;
	while (!tempString.empty()) {
		pair<StringRef, StringRef> tokens = tempString.split(',');
		StringRef idPart = tokens.first;
		tempString = tokens.second;
		instanceId.push_back(idPart);
	}
	return instanceId;
}

list<unsigned> parseInstanceId(StringRef instanceTag) {
	list<StringRef> parsedList = parseInstanceIdString(instanceTag);
	list<unsigned> parsedIntegerList;
	for (list<StringRef>::iterator stringItr = parsedList.begin(); stringItr != parsedList.end(); stringItr++) {
		APInt id;
		stringItr->getAsInteger(0, id);
		parsedIntegerList.push_back(id.getZExtValue());
	}
	return parsedIntegerList;
}

HyperOpInteractionGraph * HyperOpMetadataParser::parseMetadata(Module * M) {
	HyperOpInteractionGraph* graph = new HyperOpInteractionGraph();
	NamedMDNode * RedefineAnnotations = M->getOrInsertNamedMetadata(REDEFINE_ANNOTATIONS);
	map<MDNode*, HyperOp*> hyperOpMetadataMap;
	//Since bijective map isnt available
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
				hyperOp->setHyperOpId(hyperOpId++);
				StringRef hyperOpType = ((MDString*) hyperOpMDNode->getOperand(2))->getName();
				if (hyperOpType.equals("Static")) {
					hyperOp->setStaticHyperOp(true);
				} else if (hyperOpType.equals("Dynamic")) {
					hyperOp->setStaticHyperOp(false);
					hyperOp->setInstanceof((Function *) hyperOpMDNode->getOperand(3));
					StringRef instanceTag = ((MDString*) hyperOpMDNode->getOperand(4))->getName();
					hyperOp->setInstanceId(parseInstanceId(instanceTag));
				}
				graph->addHyperOp(hyperOp);
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

	for (Module::iterator moduleItr = M->begin(); moduleItr != M->end(); moduleItr++) {
		//Traverse through instructions of the module
		HyperOp* sourceHyperOp = graph->getHyperOp(moduleItr);
		if (sourceHyperOp != 0) {
			hyperOpTraversalList.push_back(sourceHyperOp);
		}
	}
	while (!hyperOpTraversalList.empty()) {
		//Traverse through instructions of the module
		HyperOp* sourceHyperOp = hyperOpTraversalList.front();
		hyperOpTraversalList.pop_front();
		//Traverse through instructions of the module

		unsigned frameSizeOfHyperOp = 0;
		unsigned argIndex = 1;
		for (Function::arg_iterator funcArgItr = sourceHyperOp->getFunction()->arg_begin(); funcArgItr != sourceHyperOp->getFunction()->arg_end(); funcArgItr++, argIndex++) {
			Argument* argument = &*funcArgItr;
			if (!sourceHyperOp->getFunction()->getAttributes().hasAttribute(argIndex, Attribute::InReg)) {
				frameSizeOfHyperOp += REDEFINEUtils::getSizeOfType(funcArgItr->getType());
			}
		}
		for (Function::iterator funcItr = sourceHyperOp->getFunction()->begin(); funcItr != sourceHyperOp->getFunction()->end(); funcItr++) {
			for (BasicBlock::iterator bbItr = (*funcItr).begin(); bbItr != (*funcItr).end(); bbItr++) {
				Instruction* instr = bbItr;
				if (isa<AllocaInst>(instr)) {
					frameSizeOfHyperOp += REDEFINEUtils::getSizeOfType(((AllocaInst*) instr)->getType());
				}
				if (instr->hasMetadata()) {
					MDNode* consumedByMDNode = instr->getMetadata(HYPEROP_CONSUMED_BY);
					if (consumedByMDNode != 0) {
						for (unsigned consumerMDNodeIndex = 0; consumerMDNodeIndex != consumedByMDNode->getNumOperands(); consumerMDNodeIndex++) {
							//Create an edge between two HyperOps labeled by the instruction
							MDNode* consumerMDNode = (MDNode*) consumedByMDNode->getOperand(consumerMDNodeIndex);
							HyperOp* consumerHyperOp;
							if (consumerMDNode->getNumOperands() > 3) {
								//An instance is consuming the data
								StringRef parseString = ((MDString*) consumerMDNode->getOperand(3))->getName();
								list<StringRef> consumerInstanceId = parseInstanceIdString(parseString);
								MDNode* hyperOp = (MDNode*) consumerMDNode->getOperand(0);
								//TODO
								list<unsigned> consumerHyperOpId = sourceHyperOp->getInstanceId();
								if (consumerInstanceId.front().compare("id") == 0) {
									consumerHyperOpId.push_back(0);
									consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(3), consumerHyperOpId);
								} else if (consumerInstanceId.front().compare("prefixId") == 0) {
									consumerHyperOpId.pop_back();
									consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(3), consumerHyperOpId);
								}
							} else {
								consumerHyperOp = hyperOpMetadataMap[(MDNode*) consumerMDNode->getOperand(0)];
							}
							StringRef dataType = ((MDString*) consumerMDNode->getOperand(1))->getName();
							unsigned positionOfContextSlot = ((ConstantInt*) consumerMDNode->getOperand(2))->getZExtValue();

							HyperOpEdge* edge = new HyperOpEdge();
							if (dataType.compare(SCALAR) == 0) {
								edge->Type = HyperOpEdge::SCALAR;
							} else if (dataType.compare(LOCAL_REFERENCE) == 0) {
								edge->Type = HyperOpEdge::LOCAL_REFERENCE;
								list<unsigned> volumeOfCommunication;
								Function* consumerFunction = consumerHyperOp->getFunction();
								AllocaInst* allocInst = getAllocInstrForLocalReferenceData(*M, instr, consumedByMDNode, functionMetadataMap);
								if (!isa<AllocaInst>(instr)) {
									//TODO TERRIBLE CODE, CHECK IF THIS CAN BE CLEANED
									consumerHyperOp->loadInstrAndAllocaMap[instr] = allocInst;
								}
								unsigned volume = REDEFINEUtils::getSizeOfType(allocInst->getType()) / 4;
								volumeOfCommunication.push_back(volume);
								edge->setVolume(volumeOfCommunication);
							}
							//Find out if the data is being passed to an instance
							edge->setPositionOfContextSlot(positionOfContextSlot);
							edge->setValue((Value*) instr);
							sourceHyperOp->addChildEdge(edge, consumerHyperOp);
							consumerHyperOp->addParentEdge(edge, sourceHyperOp);
						}
					}
					MDNode* controlledByMDNode = instr->getMetadata(HYPEROP_CONTROLS);
					if (controlledByMDNode != 0) {
						for (unsigned predicatedMDNodeIndex = 0; predicatedMDNodeIndex != controlledByMDNode->getNumOperands(); predicatedMDNodeIndex++) {
							MDNode* predicatedMDNode = (MDNode*) controlledByMDNode->getOperand(predicatedMDNodeIndex);
							HyperOp* consumerHyperOp;
							//Create an edge between two HyperOps labeled by the instruction
							if (predicatedMDNode->getNumOperands() > 1) {
								//An instance is consuming the data
								list<StringRef> consumerInstanceId = parseInstanceIdString(((MDString*) predicatedMDNode->getOperand(1))->getName());
								MDNode* hyperOp = (MDNode*) predicatedMDNode->getOperand(0);
								//TODO
								list<unsigned> consumerHyperOpId = sourceHyperOp->getInstanceId();
								if (consumerInstanceId.front().compare("id") == 0) {
									consumerHyperOpId.push_back(0);
									consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(3), consumerHyperOpId);
								} else if (consumerInstanceId.front().compare("prefixId") == 0) {
									consumerHyperOpId.pop_back();
									consumerHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(3), consumerHyperOpId);
								}
							} else {
								consumerHyperOp = hyperOpMetadataMap[(MDNode*) predicatedMDNode->getOperand(0)];
							}
							consumerHyperOp->setPredicatedHyperOp();
							HyperOpEdge* edge = new HyperOpEdge();
							edge->Type = HyperOpEdge::PREDICATE;
							edge->setValue((Value*) instr);
							sourceHyperOp->addChildEdge(edge, consumerHyperOp);
							consumerHyperOp->addParentEdge(edge, sourceHyperOp);
						}
					}

					MDNode* syncMDNode = instr->getMetadata(HYPEROP_SYNC);
					if (syncMDNode != 0) {
						for (unsigned syncMDNodeIndex = 0; syncMDNodeIndex != syncMDNode->getNumOperands(); syncMDNodeIndex++) {
							MDNode* syncedMDNode = (MDNode*) syncMDNode->getOperand(syncMDNodeIndex);
							//Create an edge between two HyperOps labeled by the instruction
							HyperOp* syncBarrierHyperOp;
							if (syncedMDNode->getNumOperands() > 1) {
								//An instance is consuming the data
								list<StringRef> consumerInstanceId = parseInstanceIdString(((MDString*) syncedMDNode->getOperand(1))->getName());
								MDNode* hyperOp = (MDNode*) syncMDNode->getOperand(0);
								list<unsigned> consumerHyperOpId = sourceHyperOp->getInstanceId();
								if (consumerInstanceId.front().compare("id") == 0) {
									consumerHyperOpId.push_back(0);
									syncBarrierHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(3), consumerHyperOpId);
								} else if (consumerInstanceId.front().compare("prefixId") == 0) {
									consumerHyperOpId.pop_back();
									syncBarrierHyperOp = graph->getOrCreateHyperOpInstance((Function*) hyperOp->getOperand(1), (Function*) hyperOp->getOperand(3), consumerHyperOpId);
								}
							} else {
								syncBarrierHyperOp = hyperOpMetadataMap[(MDNode*) syncedMDNode->getOperand(0)];
							}
							//Create an edge between two HyperOps labeled by the instruction
							HyperOpEdge* edge = new HyperOpEdge();
							edge->Type = HyperOpEdge::SYNC;
							sourceHyperOp->addChildEdge(edge, syncBarrierHyperOp);
							syncBarrierHyperOp->addParentEdge(edge, sourceHyperOp);
							syncBarrierHyperOp->setBarrierHyperOp();
							syncBarrierHyperOp->incrementIncomingSyncCount();
						}
					}
				}
			}
		}
		if (maxFrameSizeOfHyperOp < frameSizeOfHyperOp) {
			maxFrameSizeOfHyperOp = frameSizeOfHyperOp;
		}
	}

	//Add dummy edges to the hanging hyperops
	for (list<HyperOp*>::iterator vertexItr = graph->Vertices.begin(); vertexItr != graph->Vertices.end(); vertexItr++) {
		if (!(*vertexItr)->isEndHyperOp() && (*vertexItr)->ChildMap.empty() && (*vertexItr)->isUnrolledInstance()) {
			//Create an edge between unrolled instance and end HyperOp
			HyperOpEdge* edge = new HyperOpEdge();
			edge->Type = HyperOpEdge::SYNC;
			(*vertexItr)->addChildEdge(edge, exitHyperOp);
			exitHyperOp->addParentEdge(edge, (*vertexItr));
			exitHyperOp->setBarrierHyperOp();
			exitHyperOp->incrementIncomingSyncCount();
		}
	}

	graph->setMaxMemFrameSize(maxFrameSizeOfHyperOp);
	graph->print(errs());
	return graph;
}

