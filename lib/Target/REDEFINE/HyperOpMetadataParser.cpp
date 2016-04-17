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
	HyperOp* one, *two, *three, *four;
	map<MDNode*, HyperOp*> metadataMap;
	unsigned hyperOpId = 0;
	if (RedefineAnnotations != 0) {
		for (int i = 0; i < RedefineAnnotations->getNumOperands(); i++) {
			MDNode* hyperOpMDNode = RedefineAnnotations->getOperand(i);
			StringRef type = ((MDString*) hyperOpMDNode->getOperand(0))->getName();
			if (type.compare(HYPEROP) == 0) {
				Function* function = (Function *) hyperOpMDNode->getOperand(1);
				HyperOp *hyperOp = new HyperOp(function);
				StringRef hyperOpType = ((MDString*) hyperOpMDNode->getOperand(1))->getName();
				if (hyperOpType.compare(HYPEROP_START) == 0) {
					hyperOp->setStart();
				} else if (hyperOpType.compare(HYPEROP_END) == 0) {
					hyperOp->setEnd();
				}
				hyperOp->setHyperOpId(hyperOpId++);
				graph->addHyperOp(hyperOp);
				metadataMap.insert(std::make_pair(hyperOpMDNode, hyperOp));
			} else if (type.compare(HYPEROP_CONSUMED_BY) == 0) {
				MDNode* edgeSource = (MDNode*) hyperOpMDNode->getOperand(1);
				MDNode* edgeDestination = (MDNode*) hyperOpMDNode->getOperand(2);
				StringRef name = ((MDString*) hyperOpMDNode->getOperand(3))->getName();
				StringRef dataAggregateType = ((MDString*) hyperOpMDNode->getOperand(4))->getName();
				StringRef positionStr = ((MDString*) hyperOpMDNode->getOperand(5))->getName();
				int position = atoi(positionStr.data());
				AggregateData* aggregateData;
				if (dataAggregateType.compare(SCALAR) == 0) {
//					processScalar(aggregateData, name);
					aggregateData = new Scalar();
					HyperOpEdge* dependenceEdge = new DataDependenceEdge(aggregateData, name.str());
					graph->addEdge(metadataMap.find(edgeSource)->second, metadataMap.find(edgeDestination)->second, dependenceEdge);
					dependenceEdge->setPositionOfInput(position);
				//}else if(dataAggregateType.compare(REFERENCE)==0){
					//TODO

				}
//				if (dataAggregateType.compare(RANGE) == 0) {
//					processRange(aggregateData, dataAggregateType);
//				}
//				if (dataAggregateType.compare(REGION) == 0) {
//					processRegion(aggregateData, dataAggregateType);
//				}
//				if (dataAggregateType.compare(UNION) == 0) {
//					processUnion(aggregateData, dataAggregateType);
//				}
			} else if (type.compare(HYPEROP_CONTROLLED_BY) == 0) {
				//add control edges
				MDNode* edgeSource = (MDNode*) hyperOpMDNode->getOperand(1);
				MDNode* edgeDestination = (MDNode*) hyperOpMDNode->getOperand(2);
				StringRef name = ((MDString*) hyperOpMDNode->getOperand(3))->getName();
				HyperOpEdge* dependenceEdge = new ControlDependenceEdge();
				dependenceEdge->setName(name.str());
				HyperOp* source = metadataMap.find(edgeSource)->second;
				HyperOp* destination = metadataMap.find(edgeDestination)->second;
				destination->setIsPredicatedHyperOp();
				graph->addEdge(source, destination , dependenceEdge);
			} else if (type.compare(HYPEROP_AFFINITY) == 0) {
				//Add mapping details
				HyperOp* hyperOp = metadataMap.find((MDNode*) hyperOpMDNode->getOperand(1))->second;
				int target = atoi(((MDString*) hyperOpMDNode->getOperand(2))->getName().data());
				hyperOp->setTargetResource(target);
			}
		}
	}

	for (list<HyperOp*>::iterator itr = graph->Vertices.begin(); itr != graph->Vertices.end(); itr++) {
		if ((*itr)->ChildMap.empty()) {
			(*itr)->setEnd();
		}
		if ((*itr)->ParentMap.empty()) {
			(*itr)->setStart();
		}
	}

	return graph;
}
