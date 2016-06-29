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
	return 0;
}

HyperOpInteractionGraph * HyperOpMetadataParser::parseMetadata(Module *M) {
	HyperOpInteractionGraph* graph = new HyperOpInteractionGraph();
	NamedMDNode * RedefineAnnotations = M->getOrInsertNamedMetadata(REDEFINE_ANNOTATIONS);
	map<MDNode*, HyperOp*> hyperOpMetadataMap;

	//Since bijective map isnt available
	map<Function*, MDNode*> functionMetadataMap;

	unsigned hyperOpId = 0;
	if (RedefineAnnotations != 0) {
		for (int i = 0; i < RedefineAnnotations->getNumOperands(); i++) {
			MDNode* hyperOpMDNode = RedefineAnnotations->getOperand(i);
			StringRef type = ((MDString*) hyperOpMDNode->getOperand(0))->getName();
			if (type.compare(HYPEROP) == 0) {
				Function* function = (Function *) hyperOpMDNode->getOperand(1);
				HyperOp *hyperOp = new HyperOp(function);
				hyperOp->setHyperOpId(hyperOpId++);
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
				HyperOp* entryHyperOp = hyperOpMetadataMap[(MDNode*) hyperOpMDNode->getOperand(1)];
				entryHyperOp->setStartHyperOp();
			} else if (type.compare(HYPEROP_EXIT) == 0) {
				HyperOp* exitHyperOp = hyperOpMetadataMap[(MDNode*) hyperOpMDNode->getOperand(1)];
				exitHyperOp->setEndHyperOp();
			} else if (type.compare(HYPEROP_INTERMEDIATE) == 0) {
				HyperOp* intermediateHyperOp = hyperOpMetadataMap[(MDNode*) hyperOpMDNode->getOperand(1)];
				intermediateHyperOp->setIntermediateHyperOp();
			}
		}
	}
	//Traverse instructions for metadata
	unsigned maxFrameSizeOfHyperOp = 0;
	for (Module::iterator moduleItr = M->begin(); moduleItr != M->end(); moduleItr++) {
		//Traverse through instructions of the module
		HyperOp* sourceHyperOp = graph->getHyperOp(moduleItr);
		for (Function::iterator funcItr = (*moduleItr).begin(); funcItr != (*moduleItr).end(); funcItr++) {
			for (BasicBlock::iterator bbItr = (*funcItr).begin(); bbItr != (*funcItr).end(); bbItr++) {
				Instruction* instr = bbItr;
				if (isa<AllocaInst>(instr)) {
					unsigned frameSizeOfHyperOp = REDEFINEUtils::getSizeOfType(((AllocaInst*) instr)->getType());
					if (maxFrameSizeOfHyperOp < frameSizeOfHyperOp) {
						maxFrameSizeOfHyperOp = frameSizeOfHyperOp;
					}
				}
				if (instr->hasMetadata()) {
					MDNode* consumedByMDNode = instr->getMetadata(HYPEROP_CONSUMED_BY);
					if (consumedByMDNode != 0) {
						for (unsigned consumerMDNodeIndex = 0; consumerMDNodeIndex != consumedByMDNode->getNumOperands(); consumerMDNodeIndex++) {
							//Create an edge between two HyperOps labeled by the instruction
							MDNode* consumerMDNode = (MDNode*) consumedByMDNode->getOperand(consumerMDNodeIndex);
							HyperOp* consumerHyperOp = hyperOpMetadataMap[(MDNode*) consumerMDNode->getOperand(0)];
							StringRef dataType = ((MDString*) consumerMDNode->getOperand(1))->getName();
							HyperOpEdge* edge = new HyperOpEdge();
							if (dataType.compare(SCALAR) == 0) {
								edge->Type = HyperOpEdge::SCALAR;
							} else if (dataType.compare(LOCAL_REFERENCE) == 0) {
								edge->Type = HyperOpEdge::LOCAL_REFERENCE;
								list<unsigned> volumeOfCommunication;
								Function* consumerFunction = consumerHyperOp->getFunction();
								AllocaInst* allocInst = getAllocInstrForLocalReferenceData(*M, instr, consumedByMDNode, functionMetadataMap);
								if (!isa<AllocaInst>(instr)) {
									consumerHyperOp->loadInstrAndAllocaMap[instr] = allocInst;
								}
								unsigned volume = REDEFINEUtils::getSizeOfType(allocInst->getType()) / 4;
								(*M, instr, consumedByMDNode, functionMetadataMap);
								volumeOfCommunication.push_back(volume);
								//Position of context slot doesn't make any sense in case of local references but we need it here because we need to find out where the reference is coming from to get the size of the local reference object being passed
								unsigned positionOfContextSlot = ((ConstantInt*) consumerMDNode->getOperand(2))->getZExtValue();
								edge->setPositionOfContextSlot(positionOfContextSlot);
								edge->setVolume(volumeOfCommunication);
							}
							edge->setValue((Value*) instr);
							sourceHyperOp->addChildEdge(edge, consumerHyperOp);
							consumerHyperOp->addParentEdge(edge, sourceHyperOp);
						}
					}
					MDNode* controlledByMDNode = instr->getMetadata(HYPEROP_CONTROLLED_BY);
					if (controlledByMDNode != 0) {
						for (unsigned predicatedMDNodeIndex = 0; predicatedMDNodeIndex != controlledByMDNode->getNumOperands(); predicatedMDNodeIndex++) {
							MDNode* predicatedMDNode = (MDNode*) controlledByMDNode->getOperand(predicatedMDNodeIndex);
							//Create an edge between two HyperOps labeled by the instruction
							HyperOp* predicatedHyperOp = hyperOpMetadataMap[(MDNode*) predicatedMDNode->getOperand(0)];
							HyperOpEdge* edge = new HyperOpEdge();
							edge->Type = HyperOpEdge::PREDICATE;
							edge->setValue((Value*) instr);
							sourceHyperOp->addChildEdge(edge, predicatedHyperOp);
							predicatedHyperOp->addParentEdge(edge, sourceHyperOp);
						}
					}
				}
			}
		}
	}

	graph->setMaxMemFrameSize(maxFrameSizeOfHyperOp);
	return graph;
}

