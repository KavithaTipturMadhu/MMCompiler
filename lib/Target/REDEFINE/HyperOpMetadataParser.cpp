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

AllocaInst* getAllocInstrForLocalReferenceData(Module &M, Value* sourceValue, map<MDNode*, HyperOp*> hyperOpMetadataMap, NamedMDNode* RedefineAnnotations) {
	if (isa<AllocaInst>(sourceValue)) {
		return (AllocaInst*) sourceValue;
	}
	else if (isa<LoadInst>(sourceValue)) {
		unsigned argIndex = 0;
		Value* operand = ((Instruction*) sourceValue)->getOperand(0);
		Function* parentFunction = ((Instruction*) sourceValue)->getParent()->getParent();
		for (Function::arg_iterator argItr = parentFunction->arg_begin(); argItr != parentFunction->arg_end(); argItr++, argIndex++) {
			if (argItr == operand && !parentFunction->getAttributes().hasAttribute(argIndex, Attribute::InReg)) {
				//Find the function thats passing the argument recursively
				for (unsigned i = 0; i < RedefineAnnotations->getNumOperands(); i++) {
					MDNode* hyperOpMDNode = RedefineAnnotations->getOperand(i);
					StringRef annotationName = ((MDString*) hyperOpMDNode->getOperand(0))->getString();
					if (annotationName.compare(HYPEROP_PRODUCES)) {
						MDNode* consumerHyperOpMDNode = (MDNode*) hyperOpMDNode->getOperand(2);
						Value* dataAtProducer = (Value*) (MDNode*) hyperOpMDNode->getOperand(3);
						unsigned positionOfContextSlot = ((ConstantInt*) hyperOpMDNode->getOperand(5))->getZExtValue();
						if (parentFunction == hyperOpMetadataMap[consumerHyperOpMDNode]->getFunction() && argIndex == positionOfContextSlot) {
							return getAllocInstrForLocalReferenceData(M, dataAtProducer, hyperOpMetadataMap, RedefineAnnotations);
						}
					}
				}
			}
		}
	}
	//TODO isn't the else part required for pointers?
//	else {
//		return getAllocInstrForLocalReferenceData(M, sourceValue, functionMetadataMap, RedefineAnnotations);
//	}
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
				StringRef hyperOpType = ((MDString*) hyperOpMDNode->getOperand(2))->getName();
				hyperOp->setStaticHyperOp(true);
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
			} else if (type.compare(HYPEROP_PRODUCES) == 0) {
				MDNode* producerHyperOpMDNode = (MDNode*) hyperOpMDNode->getOperand(1);
				MDNode* consumerHyperOpMDNode = (MDNode*) hyperOpMDNode->getOperand(2);
				HyperOp* sourceHyperOp = hyperOpMetadataMap[producerHyperOpMDNode];
				HyperOp* targetHyperOp = hyperOpMetadataMap[consumerHyperOpMDNode];
				Value* data = (Value*) hyperOpMDNode->getOperand(3);
				StringRef dataType = ((MDString*) hyperOpMDNode->getOperand(4))->getName();
				HyperOpEdge* edge = new HyperOpEdge();
				if (dataType.compare(SCALAR) == 0) {
					edge->Type = HyperOpEdge::SCALAR;
				} else if (dataType.compare(LOCAL_REFERENCE) == 0) {
					edge->Type = HyperOpEdge::LOCAL_REFERENCE;
					list<unsigned> volumeOfCommunication;
					AllocaInst* allocInst = getAllocInstrForLocalReferenceData(*M, data, hyperOpMetadataMap, RedefineAnnotations);
					unsigned volume = REDEFINEUtils::getSizeOfType(allocInst->getType()) / 4;
					volumeOfCommunication.push_back(volume);
					edge->setVolume(volumeOfCommunication);
				}
				edge->setValue(data);
				unsigned positionOfContextSlot = ((ConstantInt*) hyperOpMDNode->getOperand(5))->getZExtValue();
				edge->setPositionOfContextSlot(positionOfContextSlot);
				sourceHyperOp->addChildEdge(edge, targetHyperOp);
				targetHyperOp->addParentEdge(edge, sourceHyperOp);
			} else if (type.compare(HYPEROP_CONTROLS) == 0 || type.compare(HYPEROP_SYNC) == 0) {
				MDNode* producerHyperOpMDNode = (MDNode*) hyperOpMDNode->getOperand(1);
				MDNode* consumerHyperOpMDNode = (MDNode*) hyperOpMDNode->getOperand(2);
				HyperOp* sourceHyperOp = hyperOpMetadataMap[producerHyperOpMDNode];
				HyperOp* targetHyperOp = hyperOpMetadataMap[consumerHyperOpMDNode];
				Value* data = (Value*) hyperOpMDNode->getOperand(3);
				targetHyperOp->setPredicatedHyperOp();
				HyperOpEdge* edge = new HyperOpEdge();
				if (type.compare(HYPEROP_SYNC) == 0) {
					edge->Type = HyperOpEdge::SYNC;
				} else {
					edge->Type = HyperOpEdge::PREDICATE;
				}
				edge->setValue(data);
				sourceHyperOp->addChildEdge(edge, targetHyperOp);
				targetHyperOp->addParentEdge(edge, sourceHyperOp);
			}
		}
	}
	//Traverse instructions for metadata
	unsigned maxFrameSizeOfHyperOp = 0;
	for (Module::iterator moduleItr = M->begin(); moduleItr != M->end(); moduleItr++) {
		//Traverse through instructions of the module
		HyperOp* sourceHyperOp = graph->getHyperOp(moduleItr);
		unsigned frameSizeOfHyperOp = 0;
		for (Function::iterator funcItr = (*moduleItr).begin(); funcItr != (*moduleItr).end(); funcItr++) {
			for (BasicBlock::iterator bbItr = (*funcItr).begin(); bbItr != (*funcItr).end(); bbItr++) {
				Instruction* instr = bbItr;
				if (isa<AllocaInst>(instr)) {
					frameSizeOfHyperOp += REDEFINEUtils::getSizeOfType(((AllocaInst*) instr)->getType());
				} else if (isa<LoadInst>(instr)) {
					frameSizeOfHyperOp += getAllocInstrForLocalReferenceData(*M, instr, hyperOpMetadataMap, RedefineAnnotations);
				}
			}
		}
		if (maxFrameSizeOfHyperOp < frameSizeOfHyperOp) {
			maxFrameSizeOfHyperOp = frameSizeOfHyperOp;
		}
	}

	graph->setMaxMemFrameSize(maxFrameSizeOfHyperOp);
	return graph;
}

