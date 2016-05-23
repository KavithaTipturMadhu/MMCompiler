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
				hyperOp->setHyperOpId(hyperOpId++);
				graph->addHyperOp(hyperOp);
				hyperOpMetadataMap.insert(std::make_pair(hyperOpMDNode, hyperOp));
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
	for (Module::iterator moduleItr = M->begin(); moduleItr != M->end(); moduleItr++) {
		//Traverse through instructions of the module
		HyperOp* sourceHyperOp = graph->getHyperOp(moduleItr);
		for (Function::iterator funcItr = (*moduleItr).begin(); funcItr != (*moduleItr).end(); funcItr++) {
			for (BasicBlock::iterator bbItr = (*funcItr).begin(); bbItr != (*funcItr).end(); bbItr++) {
				Instruction* instr = bbItr;
				if (instr->hasMetadata()) {
					MDNode* consumedByMDNode = instr->getMetadata(HYPEROP_CONSUMED_BY);
					if (consumedByMDNode != 0) {
						for (unsigned consumerMDNodeIndex = 0; consumerMDNodeIndex != consumedByMDNode->getNumOperands(); consumerMDNodeIndex++) {
							//Create an edge between two HyperOps labeled by the instruction
							MDNode* consumerMDNode = (MDNode*) consumedByMDNode->getOperand(consumerMDNodeIndex);
							HyperOp* consumerHyperOp = hyperOpMetadataMap[(MDNode*) consumerMDNode->getOperand(0)];
							StringRef dataType = ((MDString*) consumerMDNode->getOperand(1))->getName();
							HyperOpEdge* edge=new HyperOpEdge();
							if (dataType.compare(SCALAR) == 0) {
								edge->Type = HyperOpEdge::SCALAR;
							} else if (dataType.compare(LOCAL_REFERENCE) == 0) {
								edge->Type = HyperOpEdge::LOCAL_REFERENCE;
								list<unsigned> volumeOfCommunication;
								volumeOfCommunication.push_back(sizeof((Value*) instr));
								edge->setVolume(volumeOfCommunication);
							}
							unsigned positionOfContextSlot = ((ConstantInt*) consumerMDNode->getOperand(2))->getZExtValue();
							edge->setValue((Value*) instr);
							edge->setPositionOfContextSlot(positionOfContextSlot);
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
							HyperOpEdge* edge=new HyperOpEdge();
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

//	for (list<HyperOp*>::iterator itr = graph->Vertices.begin(); itr != graph->Vertices.end(); itr++) {
//			if ((*itr)->ChildMap.empty()) {
//				(*itr)->setEndHyperOp();
//			}
//			if ((*itr)->ParentMap.empty()) {
//				(*itr)->setStartHyperOp();
//			}
//		}

	graph->print(errs());
	return graph;
}

