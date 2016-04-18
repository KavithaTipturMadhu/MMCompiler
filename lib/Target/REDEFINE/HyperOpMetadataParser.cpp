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

//TODO
//
//void processRange(AggregateData* aggregateData, MDNode* edgeMDNode) {
//	aggregateData = new Range();
//	ConstantInt* start = (ConstantInt*) edgeMDNode->getOperand(1);
//	((Range*) aggregateData)->Start = start->getValue().getSExtValue();
//	ConstantInt* end = (ConstantInt*) edgeMDNode->getOperand(2);
//	((Range*) aggregateData)->End = end->getValue().getSExtValue();
//}
//
//void processRegion(AggregateData* aggregateData, MDNode* edgeMDNode) {
//	aggregateData = new Region();
//	ConstantInt* programVariableConst = (ConstantInt*) edgeMDNode->getOperand(1);
//	int programVariableCount = ((int) programVariableConst->getSExtValue());
//	for (int i = 2; i < programVariableCount + 2; i++) {
//		MDString* programVariable = (MDString*) edgeMDNode->getOperand(i);
//		((Region *) aggregateData)->ProgramVariables.push_back(programVariable->getString().str());
//	}
//	ConstantInt* iterationVariableConst = (ConstantInt*) edgeMDNode->getOperand(programVariableCount + 2);
//	int iterationVariableCount = ((int) iterationVariableConst->getSExtValue());
//	for (int i = programVariableCount + 3; i < iterationVariableCount + programVariableCount + 3; i++) {
//		MDString* iterationVariable = (MDString*) edgeMDNode->getOperand(i);
//		((Region *) aggregateData)->IterationVariables.push_back(iterationVariable->getString().str());
//	}
//	ConstantInt* inequalityConst = (ConstantInt*) edgeMDNode->getOperand(programVariableCount + iterationVariableCount + 3);
//	int inequalityCount = ((int) inequalityConst->getSExtValue());
//	for (int i = programVariableCount + iterationVariableCount + 4; i < inequalityCount + programVariableCount + iterationVariableCount + 4; i++) {
//		MDString* inequalityString = (MDString*) edgeMDNode->getOperand(i);
//		((Region *) aggregateData)->Inequalities.push_back(inequalityString->getString().str());
//	}
//}
//
//void processUnion(AggregateData* aggregateData, MDNode* edgeMDNode) {
//	aggregateData = new Union();
//	ConstantInt* argConst = (ConstantInt*) edgeMDNode->getOperand(1);
//	int argCount = ((int) argConst->getSExtValue());
//	for (int i = 1; i <= argCount; i++) {
//		MDNode* operand = (MDNode*) edgeMDNode->getOperand(i);
//		StringRef aggregateType = ((MDString*) operand->getOperand(0))->getString();
//		AggregateData * component;
//		if (aggregateType.compare(SCALAR) == 0) {
//			processScalar(component);
//		}
//		if (aggregateType.compare(RANGE) == 0) {
//			processRange(component, operand);
//		}
//		if (aggregateType.compare(REGION) == 0) {
//			processRegion(component, operand);
//		}
//		if (aggregateType.compare(UNION) == 0) {
//			processUnion(component, operand);
//		}
//		((Union*) aggregateData)->ContainedAggregates.push_back(component);
//	}
//}

HyperOpInteractionGraph * HyperOpMetadataParser::parseMetadata(Module *M) {
	HyperOpInteractionGraph* graph = new HyperOpInteractionGraph();
	NamedMDNode * RedefineAnnotations = M->getOrInsertNamedMetadata(REDEFINE_ANNOTATIONS);
	map<MDNode*, HyperOp*> hyperOpMetadataMap;
	unsigned hyperOpId = 0;
	if (RedefineAnnotations != 0) {
		for (int i = 0; i < RedefineAnnotations->getNumOperands(); i++) {
			MDNode* hyperOpMDNode = RedefineAnnotations->getOperand(i);
			StringRef type = ((MDString*) hyperOpMDNode->getOperand(0))->getName();
			if (type.compare(HYPEROP) == 0) {
				Function* function = (Function *) hyperOpMDNode->getOperand(1);
				HyperOp *hyperOp = new HyperOp(function);
				StringRef hyperOpType = ((MDString*) hyperOpMDNode->getOperand(2))->getName();
				if (hyperOpType.compare(HYPEROP_ENTRY) == 0) {
					hyperOp->setStartHyperOp();
				} else if (hyperOpType.compare(HYPEROP_EXIT) == 0) {
					hyperOp->setEndHyperOp();
				} else {
					hyperOp->setIntermediateHyperOp();
				}
				hyperOp->setHyperOpId(hyperOpId++);
				graph->addHyperOp(hyperOp);
				hyperOpMetadataMap.insert(std::make_pair(hyperOpMDNode, hyperOp));
			}
		}
	}
	//Traverse instructions for metadata
	for (Module::iterator moduleItr = M->begin(); moduleItr != M->end(); moduleItr++) {
		//Traverse through instructions of the module
		HyperOp* sourceHyperOp = graph->getHyperOp(moduleItr);
		for (Function::iterator funcItr = (*moduleItr).begin(); funcItr != (*moduleItr).end(); funcItr++) {
			for (BasicBlock::iterator bbItr = (*funcItr).begin(); bbItr != (*funcItr).end(); bbItr++) {
				Instruction* instr = bbItr;
				if (instr->hasMetadata()) {
					MDNode* consumedByMDNode = instr->getMetadata(HYPEROP_CONSUMED_BY);
					if (consumedByMDNode != 0) {
						MDNode* consumerList = (MDNode*)consumedByMDNode->getOperand(0);
						for (unsigned consumerMDNodeIndex = 0; consumerMDNodeIndex != consumerList->getNumOperands(); consumerMDNodeIndex++) {
							//Create an edge between two HyperOps labeled by the instruction
							MDNode* consumerMDNode = (MDNode*)consumerList->getOperand(consumerMDNodeIndex);
							HyperOp* consumerHyperOp = hyperOpMetadataMap[(MDNode*)consumerMDNode->getOperand(0)];
							StringRef dataType = ((MDString*) consumerMDNode->getOperand(1))->getName();
							HyperOpEdge edge;
							if (dataType.compare(SCALAR) == 0) {
								edge.Type = HyperOpEdge::SCALAR;
							} else if (dataType.compare(LOCAL_REFERENCE) == 0) {
								edge.Type = HyperOpEdge::LOCAL_REFERENCE;
								list<unsigned> volumeOfCommunication;
								volumeOfCommunication.push_back(sizeof((Value*) instr));
								edge.setVolume(volumeOfCommunication);
							}
							unsigned positionOfInput = ((ConstantInt*) consumerMDNode->getOperand(2))->getUniqueInteger().getZExtValue();
							edge.setValue((Value*) instr);
							edge.setPositionOfInput(positionOfInput);
							sourceHyperOp->addChildEdge(&edge, consumerHyperOp);
							consumerHyperOp->addParentEdge(&edge, sourceHyperOp);
						}
					}
					MDNode* controlledByMDNode = instr->getMetadata(HYPEROP_CONTROLLED_BY);
					if (controlledByMDNode != 0) {
						MDNode* predicatedList = (MDNode*)controlledByMDNode->getOperand(0);
						for (unsigned predicatedMDNodeIndex = 0; predicatedMDNodeIndex != predicatedList->getNumOperands(); predicatedMDNodeIndex++) {
							MDNode* predicatedMDNode = (MDNode*)predicatedList->getOperand(predicatedMDNodeIndex);
							//Create an edge between two HyperOps labeled by the instruction
							HyperOp* predicatedHyperOp = hyperOpMetadataMap[(MDNode*)predicatedMDNode->getOperand(0)];
							HyperOpEdge edge;
							edge.Type = HyperOpEdge::PREDICATE;
							//Unconditional predicate
							if (predicatedMDNode->getNumOperands() > 1) {
								edge.setValue((Value*) instr);
								unsigned predicateValue =  ((ConstantInt*) predicatedMDNode->getOperand(1))->getUniqueInteger().getZExtValue();
								edge.setPredicateValue(predicateValue);
							}
							sourceHyperOp->addChildEdge(&edge, predicatedHyperOp);
							predicatedHyperOp->addParentEdge(&edge, sourceHyperOp);
						}

					}
				}
			}
		}
	}

	return graph;
}
