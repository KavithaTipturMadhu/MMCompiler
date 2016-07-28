/*
 * HyperOpInteractionGraph.cpp
 *
 *  Created on: 02-Jul-2015
 *      Author: kavitha
 */

#include "HyperOpInteractionGraph.h"

#include "lpsolve/lp_lib.h"

#include <cstdlib>
#include <utility>
#include <sstream>
#include <map>
#include <cassert>
#include <iostream>
#include <iterator>
using namespace std;

#include "llvm/IR/Function.h"
#include "llvm/Support/Debug.h"
using namespace llvm;

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/sequential_vertex_coloring.hpp>
using namespace boost;

HyperOp::HyperOp(Function* function) {
	this->function = function;
	this->ImmediateDominator = 0;
	this->ImmediatePostDominator = 0;
	this->IsBarrier = false;
	this->IsStart = false;
	this->IsEnd = false;
	this->IsPredicated = false;
	this->TargetResource = 0;
	this->contextFrame = 0;
	this->executionTimeEstimate.push_back(1);
	this->fbindRequired = false;
	this->gcRequired = false;
	this->staticHyperOp = true;
}

HyperOp::~HyperOp() {
}

HyperOpEdge::EdgeType HyperOpEdge::getType() {
	return Type;
}
void HyperOpEdge::setType(HyperOpEdge::EdgeType type) {
	this->Type = type;
}

HyperOp* HyperOpEdge::getContextFrameAddress() {
	return contextFrameAddress;
}

void HyperOpEdge::setContextFrameAddress(HyperOp* contextFrameAddress) {
	this->contextFrameAddress = contextFrameAddress;
}

int HyperOpEdge::getPositionOfContextSlot() const {
	return positionOfContextSlot;
}

void HyperOpEdge::setPositionOfContextSlot(int positionOfInput) {
	this->positionOfContextSlot = positionOfInput;
}
void HyperOpEdge::setValue(Value* value) {
	this->variable = value;
}
Value* HyperOpEdge::getValue() {
	return variable;
}

const list<HyperOp*>& HyperOp::getCreateFrameList() const {
	return CreateFrameList;
}
void HyperOp::setCreateFrameList(const list<HyperOp*>& createFrameList) {
	CreateFrameList = createFrameList;
}

unsigned int HyperOp::getContextFrame() const {
	return contextFrame;
}

bool HyperOp::isFbindRequired() const {
	return fbindRequired;
}

void HyperOp::setFbindRequired(bool fbindRequired) {
	this->fbindRequired = fbindRequired;
}

bool HyperOp::isStaticHyperOp() const {
	return staticHyperOp;
}

void HyperOp::setStaticHyperOp(bool staticHyperOp) {
	this->staticHyperOp = staticHyperOp;
}

unsigned HyperOp::getHyperOpId() const {
	return hyperOpId;
}

void HyperOp::setHyperOpId(unsigned hyperOpId) {
	this->hyperOpId = hyperOpId;
}

void HyperOp::setNumCEInputs(unsigned ceId, unsigned numInputs) {
	numInputsPerCE[ceId] = numInputs;
}

void HyperOp::setNumCEs(unsigned ceCount) {
	numInputsPerCE.reserve(ceCount);
	for (unsigned i = 0; i < ceCount; i++) {
		numInputsPerCE[i] = 0;
	}
}

unsigned HyperOp::getNumInputsPerCE(unsigned ceId) {
	return numInputsPerCE[ceId];
}

list<HyperOp*> HyperOp::getParentList() {
	list<HyperOp*> parentList;
	for (map<HyperOpEdge*, HyperOp*>::iterator parentIterator = ParentMap.begin(); parentIterator != ParentMap.end(); parentIterator++) {
		if (std::find(parentList.begin(), parentList.end(), parentIterator->second) == parentList.end()) {
			parentList.push_back(parentIterator->second);
		}
	}
	return parentList;
}

list<HyperOp*> HyperOp::getChildList() {
	list<HyperOp*> childList;
	for (map<HyperOpEdge*, HyperOp*>::iterator childIterator = ChildMap.begin(); childIterator != ChildMap.end(); childIterator++) {
		if (std::find(childList.begin(), childList.end(), childIterator->second) == childList.end()) {
			childList.push_back(childIterator->second);
		}
	}
	return childList;
}

void HyperOp::addChildEdge(HyperOpEdge * Edge, HyperOp* ChildVertex) {
	this->ChildMap.insert(std::make_pair(Edge, ChildVertex));
}

void HyperOp::addParentEdge(HyperOpEdge * Edge, HyperOp* ParentVertex) {
	this->ParentMap.insert(std::make_pair(Edge, ParentVertex));
}

void HyperOp::setImmediateDominator(HyperOp* ImmediateDominator) {
	this->ImmediateDominator = ImmediateDominator;
}

void HyperOp::setImmediatePostDominator(HyperOp* ImmediatePostDominator) {
	this->ImmediatePostDominator = ImmediatePostDominator;
}

void HyperOp::setDominanceFrontier(list<HyperOp*> DominanceFrontier) {
	this->DominanceFrontier = DominanceFrontier;
}
list<HyperOp*> HyperOp::getDominanceFrontier() {
	return DominanceFrontier;
}

HyperOp* HyperOp::getImmediateDominator() {
	return ImmediateDominator;
}
HyperOp* HyperOp::getImmediatePostDominator() {
	return ImmediatePostDominator;
}

void HyperOp::setStartHyperOp() {
	this->IsStart = true;
}
void HyperOp::setEndHyperOp() {
	this->IsEnd = true;
}
void HyperOp::setBarrierHyperOp() {
	this->IsBarrier = true;
}

bool HyperOp::isBarrierHyperOp() {
	return IsBarrier;
}

bool HyperOp::isPredicatedHyperOp() {
	return IsPredicated;
}

void HyperOp::setPredicatedHyperOp() {
	this->IsPredicated = true;
}

bool HyperOp::isStartHyperOp() {
	return this->IsStart;
}

bool HyperOp::isEndHyperOp() {
	return this->IsEnd;
}

bool HyperOp::isIntermediateHyperOp() {
	return IsIntermediate;
}

void HyperOp::setIntermediateHyperOp() {
	IsIntermediate = true;
}

unsigned int HyperOp::getTargetResource() const {
	return TargetResource;
}

void HyperOp::setTargetResource(unsigned int targetResource) {
	TargetResource = targetResource;
}

Function* HyperOp::getFunction() {
	return function;
}

bool HyperOp::isPredecessor(HyperOp* successorHyperOp) {
	if (successorHyperOp == 0) {
		return false;
	}
	list<HyperOp*> childList = this->getChildList();
	for (list<HyperOp*>::iterator childItr = childList.begin(); childItr != childList.end(); childItr++) {
		if (*childItr == successorHyperOp || ((*childItr) != 0 && (*childItr)->isPredecessor(successorHyperOp))) {
			return true;
		}
	}
	return false;
}

void HyperOp::setContextFrame(unsigned contextFrame) {
	this->contextFrame = contextFrame;
}

map<HyperOp*, list<unsigned int> > HyperOp::getChildNodeEdgeWeights() {
	map<HyperOp*, list<unsigned int> > childNodeWeightMap;
	if (!isEndHyperOp()) {
		for (map<HyperOpEdge*, HyperOp*>::iterator childItr = this->ChildMap.begin(); childItr != this->ChildMap.end(); childItr++) {
			HyperOpEdge* edge = childItr->first;
			HyperOp* childNode = childItr->second;
			list<unsigned int> communicationVolume;
			if (childNodeWeightMap.find(childNode) != childNodeWeightMap.end()) {
				communicationVolume = childNodeWeightMap.find(childNode)->second;
				childNodeWeightMap.erase(childNode);
			}
			if (edge->isEdgeZeroedOut() || edge->isEdgeIgnored()) {
				communicationVolume.clear();
				communicationVolume.push_back(0);
			} else if (edge->Type == HyperOpEdge::PREDICATE || edge->Type == HyperOpEdge::SCALAR) {
				//Linearize communication volume since this is only in linear order of magnitude
				unsigned int volume = 0;
				if (communicationVolume.size() > 0) {
					volume = communicationVolume.front();
					volume++;
					communicationVolume.pop_front();
				}
				communicationVolume.push_front(volume);
			} else if (edge->Type == HyperOpEdge::LOCAL_REFERENCE) {
				list<unsigned int> tempVolume = edge->getVolume();
				list<unsigned int> volume;
				int originalSize = communicationVolume.size();
				int tempVolumeSize = tempVolume.size();
				for (int i = 0; i < min(originalSize, tempVolumeSize); i++) {
					volume.push_back(communicationVolume.front() + tempVolume.front());
					communicationVolume.pop_front();
					tempVolume.pop_front();
				}
				for (int i = min(originalSize, tempVolumeSize); i < max(originalSize, tempVolumeSize); i++) {
					unsigned int volumeByIndex = 0, tempVolumeByIndex = 0;
					if (!communicationVolume.empty()) {
						volumeByIndex = communicationVolume.front();
						communicationVolume.pop_front();
					}
					if (!tempVolume.empty()) {
						tempVolumeByIndex = tempVolume.front();
						tempVolume.pop_front();
					}
					volume.push_back(volumeByIndex + tempVolumeByIndex);
				}
				std::copy(volume.begin(), volume.end(), back_inserter(communicationVolume));
			}

			childNodeWeightMap.insert(std::make_pair(childNode, communicationVolume));
		}
	}
	return childNodeWeightMap;
}

map<HyperOp*, list<unsigned int> > HyperOp::getParentNodeEdgeWeights() {
	map<HyperOp*, list<unsigned int> > parentNodeWeightMap;
	if (!isStartHyperOp()) {
		for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = ParentMap.begin(); parentItr != ParentMap.end(); parentItr++) {
			HyperOpEdge* edge = (*parentItr).first;
			HyperOp* parentNode = (*parentItr).second;
			list<unsigned int> communicationVolume;
			if (parentNodeWeightMap.find(parentNode) != parentNodeWeightMap.end()) {
				communicationVolume = parentNodeWeightMap.find(parentNode)->second;
				parentNodeWeightMap.erase(parentNode);
			}
			if (edge->isEdgeZeroedOut() || edge->isEdgeIgnored()) {
				communicationVolume.clear();
				communicationVolume.push_back(0);
			} else if (edge->Type == HyperOpEdge::PREDICATE || edge->Type == HyperOpEdge::SCALAR) {
				unsigned int volume = 0;
				if (communicationVolume.size() > 0) {
					volume = communicationVolume.front();
					volume++;
					communicationVolume.pop_front();
				}
				communicationVolume.push_front(volume);

			} else if (edge->Type == HyperOpEdge::LOCAL_REFERENCE) {
				list<unsigned int> tempVolume = edge->getVolume();
				list<unsigned int> volume;
				int originalSize = communicationVolume.size();
				int tempVolumeSize = tempVolume.size();
				for (int i = 0; i < min(originalSize, tempVolumeSize); i++) {
					unsigned int volumeByIndex = communicationVolume.front();
					communicationVolume.pop_front();
					unsigned int tempVolumeByIndex = tempVolume.front();
					tempVolume.pop_front();
					volume.push_back(volumeByIndex + tempVolumeByIndex);
				}
				if (originalSize > tempVolumeSize) {
					for (int i = tempVolumeSize; i < originalSize; i++) {
						unsigned int volumeByIndex = communicationVolume.front();
						communicationVolume.pop_front();
						volume.push_back(volumeByIndex);
					}
				} else {
					for (int i = originalSize; i < tempVolumeSize; i++) {
						unsigned int volumeByIndex = tempVolume.front();
						tempVolume.pop_front();
						volume.push_back(volumeByIndex);
					}
				}
				std::copy(volume.begin(), volume.end(), back_inserter(communicationVolume));
			}

			parentNodeWeightMap.insert(std::make_pair(parentNode, communicationVolume));
		}
	}
	return parentNodeWeightMap;
}

void HyperOp::setExecutionTimeEstimate(list<unsigned int> estimate) {
	this->executionTimeEstimate = estimate;
}

list<unsigned int> HyperOp::getExecutionTimeEstimate() {
	return this->executionTimeEstimate;
}

void HyperOp::zeroOutChildEdge(HyperOp* childNode, bool zeroOut) {
	for (map<HyperOpEdge*, HyperOp*>::iterator childItr = ChildMap.begin(); childItr != ChildMap.end(); childItr++) {
		if ((*childItr).second == childNode) {
			(*childItr).first->zeroOutEdge(zeroOut);
		}
	}
}

list<HyperOpEdge*> HyperOp::ignoreChildEdge(HyperOp* childNode, bool ignore) {
	list<HyperOpEdge*> stateChangedEdges;
	for (map<HyperOpEdge*, HyperOp*>::iterator childItr = ChildMap.begin(); childItr != ChildMap.end(); childItr++) {
		if ((*childItr).second == childNode && !(*childItr).first->isEdgeIgnored()) {
			(*childItr).first->setIsEdgeIgnored(ignore);
			stateChangedEdges.push_back((*childItr).first);
		}
	}
	return stateChangedEdges;
}

void HyperOp::zeroOutParentEdge(HyperOp* parentNode, bool zeroOut) {
	for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = ParentMap.begin(); parentItr != ParentMap.end(); parentItr++) {
		if ((*parentItr).second == parentNode) {
			(*parentItr).first->zeroOutEdge(zeroOut);
		}
	}
}
list<HyperOpEdge*> HyperOp::ignoreParentEdge(HyperOp* parentNode, bool ignore) {
	list<HyperOpEdge*> stateChangedEdges;
	for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = ParentMap.begin(); parentItr != ParentMap.end(); parentItr++) {
		if ((*parentItr).second == parentNode && !(*parentItr).first->isEdgeIgnored()) {
			(*parentItr).first->setIsEdgeIgnored(ignore);
			stateChangedEdges.push_back((*parentItr).first);
		}
	}
	return stateChangedEdges;
}

void HyperOp::removeChildEdge(HyperOpEdge* edge) {
	ChildMap.erase(edge);
}
void HyperOp::removeParentEdge(HyperOpEdge* edge) {
	ParentMap.erase(edge);
}

void HyperOp::setTopLevel(list<unsigned int> topLevel) {
	this->topLevel = topLevel;
}

//TODO This method should use execution time of each HyperOp and weight of communication edges, I am ignoring HyperOp execution time and edge weigths are assumed to be 1 unit each.
unsigned HyperOp::computeDepthInGraph() {
	unsigned executionTime = 0;
	bool firstIteration = true;
	list<HyperOp*> parentList = this->getParentList();
	if (!parentList.empty()) {
		for (list<HyperOp*>::iterator parentIterator = parentList.begin(); parentIterator != parentList.end(); parentIterator++) {
			HyperOp* parentVertex = (*parentIterator);
			if (firstIteration || parentVertex->computeDepthInGraph() + 1 > executionTime) {
				executionTime = parentVertex->computeDepthInGraph() + 1;
				firstIteration = false;
			}
		}
	}
	return executionTime;
}

bool HyperOp::frameNeedsGC() const {
	return gcRequired;
}

void HyperOp::setFrameNeedsGC(bool gcRequired) {
	this->gcRequired = gcRequired;
}

list<unsigned int> HyperOp::getTopLevel() {
	return this->topLevel;
}
HyperOpEdge::HyperOpEdge() {
	this->Type = SCALAR;
	this->isZeroedOut = false;
	this->isIgnoredEdge = false;
	this->positionOfContextSlot = -1;
	this->variable = 0;
}
HyperOpEdge::~HyperOpEdge() {

}

list<unsigned int> HyperOpEdge::getVolume() {
	return volume;
}

void HyperOpEdge::setVolume(list<unsigned> volume) {
	this->volume = volume;
}

void HyperOpEdge::zeroOutEdge(bool zeroOut) {
	this->isZeroedOut = zeroOut;
}

bool HyperOpEdge::isEdgeZeroedOut() {
	return this->isZeroedOut;
}
bool HyperOpEdge::isEdgeIgnored() const {
	return isIgnoredEdge;
}

void HyperOpEdge::setIsEdgeIgnored(bool isIgnoredEdge) {
	this->isIgnoredEdge = isIgnoredEdge;
}

HyperOpInteractionGraph::HyperOpInteractionGraph() {
}

HyperOpInteractionGraph::~HyperOpInteractionGraph() {
}

void HyperOpInteractionGraph::setDimensions(unsigned int columnCount, unsigned int rowCount) {
	this->rowCount = rowCount;
	this->columnCount = columnCount;
}

void HyperOpInteractionGraph::setNumContextFrames(unsigned int numContextFrames) {
	this->numContextFrames = numContextFrames;
}

unsigned int HyperOpInteractionGraph::getMaxMemFrameSize() const {
	return maxMemFrameSize;
}

void HyperOpInteractionGraph::setMaxMemFrameSize(unsigned int maxFrameSize) {
	this->maxMemFrameSize = maxFrameSize;
}

unsigned int HyperOpInteractionGraph::getMaxContextFrameSize() const {
	return maxContextFrameSize;
}

void HyperOpInteractionGraph::setMaxContextFrameSize(unsigned int maxFrameSize) {
	this->maxContextFrameSize = maxFrameSize;
}

void HyperOpInteractionGraph::addHyperOp(HyperOp *Vertex) {
	this->Vertices.push_back(Vertex);
}

void HyperOpInteractionGraph::addEdge(HyperOp* SourceVertex, HyperOp * TargetVertex, HyperOpEdge * Edge) {
	TargetVertex->addParentEdge(Edge, SourceVertex);
	SourceVertex->addChildEdge(Edge, TargetVertex);
}

list<HyperOp*> setIntersection(list<HyperOp*> firstSet, list<HyperOp*> secondSet) {
	list<HyperOp*> intersection;
	for (list<HyperOp*>::iterator itr = firstSet.begin(); itr != firstSet.end(); itr++) {
		if (std::find(secondSet.begin(), secondSet.end(), *itr) != secondSet.end()) {
			intersection.push_back(*itr);
		}
	}

	for (list<HyperOp*>::iterator itr = secondSet.begin(); itr != secondSet.end(); itr++) {
		if (std::find(firstSet.begin(), firstSet.end(), *itr) != firstSet.end() && std::find(intersection.begin(), intersection.end(), *itr) == intersection.end()) {
			intersection.push_back(*itr);
		}
	}

	return intersection;
}

list<HyperOp*> setDifference(list<HyperOp*> firstSet, list<HyperOp*> secondSet) {
	list<HyperOp*> difference;
	for (list<HyperOp*>::iterator itr = firstSet.begin(); itr != firstSet.end(); itr++) {
		if (std::find(secondSet.begin(), secondSet.end(), *itr) == secondSet.end()) {
			difference.push_back(*itr);
		}
	}

	return difference;
}

void HyperOpInteractionGraph::computePostImmediateDominatorInfo() {
	map<HyperOp*, list<HyperOp*> > postDominatorMap;
	HyperOp* EndHyperOp;
	for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
		HyperOp* vertex = *vertexIterator;
		list<HyperOp*> dominatedVertexList;
		if (vertex->isEndHyperOp()) {
			EndHyperOp = vertex;
			dominatedVertexList.push_back(vertex);
		} else {
			std::copy(Vertices.begin(), Vertices.end(), std::back_inserter(dominatedVertexList));
		}
		postDominatorMap.insert(std::make_pair(vertex, dominatedVertexList));
	}

	bool change;
	do {
		change = false;
		for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
			HyperOp* vertex = *vertexIterator;
			if (!vertex->isEndHyperOp()) {
				list<HyperOp*> computedPostDominator;
				std::copy(Vertices.begin(), Vertices.end(), std::back_inserter(computedPostDominator));

				list<HyperOp*> childList = vertex->getChildList();
				for (list<HyperOp*>::iterator childIterator = childList.begin(); childIterator != childList.end(); childIterator++) {
					HyperOp* child = (*childIterator);
					//computedPostDominator is updated to intersection of computedPostDominator and postdominator set of its children;
					list<HyperOp*> postDomSetOfChild = postDominatorMap.find(child)->second;
					list<HyperOp*> insertItr = setIntersection(computedPostDominator, postDomSetOfChild);
					computedPostDominator.clear();
					std::copy(insertItr.begin(), insertItr.end(), back_inserter(computedPostDominator));
				}

				if (std::find(computedPostDominator.begin(), computedPostDominator.end(), vertex) == computedPostDominator.end()) {
					computedPostDominator.push_back(vertex);
				}

				list<HyperOp*> oldPostDomSet = postDominatorMap.find(vertex)->second;

				list<HyperOp*> forwardDiffSet = setDifference(computedPostDominator, oldPostDomSet);
				list<HyperOp*> backwardDiffSet = setDifference(oldPostDomSet, computedPostDominator);
				if (!forwardDiffSet.empty() || !backwardDiffSet.empty()) {
					change = true;
				}
				postDominatorMap.find(vertex)->second = computedPostDominator;
			}
		}
	} while (change);

	//Compute immediate post-dominator for each node
	map<HyperOp*, list<HyperOp*> > temporaryPostIdomMap;
	for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
		list<HyperOp*> postDominatorForVertex = postDominatorMap.find(*vertexIterator)->second;
		list<HyperOp*> tempList;
		std::copy(postDominatorForVertex.begin(), postDominatorForVertex.end(), std::back_inserter(tempList));
		tempList.remove(*vertexIterator);
		temporaryPostIdomMap.insert(std::make_pair(*vertexIterator, tempList));
	}

	for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
		if (!(*vertexIterator)->isEndHyperOp()) {
			list<HyperOp*> tempList = temporaryPostIdomMap.find(*vertexIterator)->second;
			list<HyperOp*> updatedList;
			std::copy(tempList.begin(), tempList.end(), std::back_inserter(updatedList));
			for (list<HyperOp*>::iterator tempListItr = tempList.begin(); tempListItr != tempList.end(); tempListItr++) {
				list<HyperOp*> tempListItrIdom = temporaryPostIdomMap.find(*tempListItr)->second;
				for (list<HyperOp*>::iterator intermediateItr = tempList.begin(); intermediateItr != tempList.end(); intermediateItr++) {
					if (*intermediateItr != *tempListItr && std::find(tempListItrIdom.begin(), tempListItrIdom.end(), *intermediateItr) != tempListItrIdom.end()) {
						updatedList.remove(*intermediateItr);
					}
				}
			}
			temporaryPostIdomMap.find(*vertexIterator)->second = updatedList;
		}
	}

	EndHyperOp->setImmediatePostDominator(0);
	for (map<HyperOp*, list<HyperOp*> >::iterator temporaryIdomIterator = temporaryPostIdomMap.begin(); temporaryIdomIterator != temporaryPostIdomMap.end(); temporaryIdomIterator++) {
		if (!(*temporaryIdomIterator).first->isEndHyperOp()) {
			assert((*temporaryIdomIterator).second.size() == 1);
			(*temporaryIdomIterator).first->setImmediatePostDominator(*((*temporaryIdomIterator).second.begin()));
		}
	}
	//End of computing immediate post-dominator
}

void HyperOpInteractionGraph::computeImmediateDominatorInfo() {
	map<HyperOp*, list<HyperOp*> > dominatorMap;
	HyperOp* StartHyperOp;
	for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
		HyperOp* vertex = *vertexIterator;
		list<HyperOp*> dominatedVertexList;
		if (vertex->isStartHyperOp()) {
			StartHyperOp = vertex;
			dominatedVertexList.push_back(StartHyperOp);
		} else {
			std::copy(Vertices.begin(), Vertices.end(), std::back_inserter(dominatedVertexList));
		}
		dominatorMap.insert(std::make_pair(vertex, dominatedVertexList));
	}

	bool change = false;
	do {
		change = false;
		for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
			HyperOp* vertex = *vertexIterator;
			if (!vertex->isStartHyperOp()) {
				list<HyperOp*> computedDominator;
				std::copy(Vertices.begin(), Vertices.end(), std::back_inserter(computedDominator));

				list<HyperOp*> parentList = vertex->getParentList();
				for (list<HyperOp*>::iterator parentIterator = parentList.begin(); parentIterator != parentList.end(); parentIterator++) {
					HyperOp* parent = (*parentIterator);
					//ComputedDominator is updated to intersection of computedDominator and dominator set of its parents;
					list<HyperOp*> domSetOfParent = dominatorMap.find(parent)->second;
					list<HyperOp*> insertItr = setIntersection(computedDominator, domSetOfParent);
					computedDominator.clear();
					std::copy(insertItr.begin(), insertItr.end(), std::back_inserter(computedDominator));
				}

				if (std::find(computedDominator.begin(), computedDominator.end(), vertex) == computedDominator.end()) {
					computedDominator.push_back(vertex);
				}
				list<HyperOp*> oldDomSet = dominatorMap.find(vertex)->second;
				list<HyperOp*> forwardDiffSet = setDifference(computedDominator, oldDomSet);
				list<HyperOp*> backwardDiffSet = setDifference(oldDomSet, computedDominator);
				if (!forwardDiffSet.empty() || !backwardDiffSet.empty()) {
					change = true;
				}
				dominatorMap.find(vertex)->second = computedDominator;
			}
		}
	} while (change);

	//Compute immediate dominator for each node
	map<HyperOp*, list<HyperOp*> > temporaryIdomMap;
	for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
		list<HyperOp*> dominatorForVertex = dominatorMap.find(*vertexIterator)->second;
		list<HyperOp*> tempList;
		std::copy(dominatorForVertex.begin(), dominatorForVertex.end(), std::back_inserter(tempList));
		tempList.remove(*vertexIterator);
		temporaryIdomMap.insert(std::make_pair(*vertexIterator, tempList));
	}

	for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
		if (!(*vertexIterator)->isStartHyperOp()) {
			list<HyperOp*> tempList = temporaryIdomMap.find(*vertexIterator)->second;
			list<HyperOp*> updatedList;
			std::copy(tempList.begin(), tempList.end(), std::back_inserter(updatedList));
			for (list<HyperOp*>::iterator tempListItr = tempList.begin(); tempListItr != tempList.end(); tempListItr++) {
				list<HyperOp*> tempListItrIdom = temporaryIdomMap.find(*tempListItr)->second;
				for (list<HyperOp*>::iterator intermediateItr = tempList.begin(); intermediateItr != tempList.end(); intermediateItr++) {
					if (*intermediateItr != *tempListItr && std::find(tempListItrIdom.begin(), tempListItrIdom.end(), *intermediateItr) != tempListItrIdom.end()) {
						updatedList.remove(*intermediateItr);
					}
				}
			}
			temporaryIdomMap.find(*vertexIterator)->second = updatedList;
		}
	}

	StartHyperOp->setImmediateDominator(0);
	for (map<HyperOp*, list<HyperOp*> >::iterator temporaryIdomIterator = temporaryIdomMap.begin(); temporaryIdomIterator != temporaryIdomMap.end(); temporaryIdomIterator++) {
		if (!temporaryIdomIterator->first->isStartHyperOp()) {
			assert((*temporaryIdomIterator).second.size() == 1);
			(*temporaryIdomIterator).first->setImmediateDominator(*((*temporaryIdomIterator).second.begin()));
		}
	}
	//End of computing immediate dominator

	//Compute and set dominance frontier
	for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
		HyperOp* vertex = *vertexIterator;
		list<HyperOp*> dominanceFrontier;
		for (list<HyperOp*>::iterator dominatedNodeIterator = Vertices.begin(); dominatedNodeIterator != Vertices.end(); dominatedNodeIterator++) {
			list<HyperOp*> dominators = dominatorMap.find(*dominatedNodeIterator)->second;
			if (std::find(dominators.begin(), dominators.end(), vertex) != dominators.end()) {
				list<HyperOp*> successorList = (*dominatedNodeIterator)->getChildList();
				for (list<HyperOp*>::iterator childItr = successorList.begin(); childItr != successorList.end(); childItr++) {
					list<HyperOp*> dominatorOfChild = dominatorMap.find(*childItr)->second;
					if (*childItr == vertex || std::find(dominatorOfChild.begin(), dominatorOfChild.end(), vertex) == dominatorOfChild.end()) {
						dominanceFrontier.push_back(*childItr);
					}
				}
			}
		}
		vertex->setDominanceFrontier(dominanceFrontier);
	}
}

//Associate immediate dominator and dominance frontier information with vertices
void HyperOpInteractionGraph::computeDominatorInfo() {
	computeImmediateDominatorInfo();
	computePostImmediateDominatorInfo();

	for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
		HyperOp* vertex = *vertexIterator;
		list<HyperOp*> immediatelyDominatedHyperOps;
		for (list<HyperOp*>::iterator dominatedItr = Vertices.begin(); dominatedItr != Vertices.end(); dominatedItr++) {
			if ((*dominatedItr)->getImmediateDominator() == vertex) {
				immediatelyDominatedHyperOps.push_back(*dominatedItr);
			}
		}
		vertex->setCreateFrameList(immediatelyDominatedHyperOps);
	}
}

/**
 * Indicates additional edges corresponding to WriteCM instructions for forwarding context frame addresses
 */
void HyperOpInteractionGraph::addContextFrameAddressForwardingEdges() {
	//Forward addresses to producers that have a HyperOp in their dominance frontier
	for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
		HyperOp* vertex = *vertexIterator;
		list<HyperOp*> vertexDomFrontier = vertex->getDominanceFrontier();
		if (!vertex->getDominanceFrontier().empty()) {
			for (list<HyperOp*>::iterator dominanceFrontierIterator = vertexDomFrontier.begin(); dominanceFrontierIterator != vertexDomFrontier.end(); dominanceFrontierIterator++) {
				HyperOp* dominanceFrontierHyperOp = *dominanceFrontierIterator;
				if (dominanceFrontierHyperOp->getImmediateDominator() == vertex->getImmediateDominator()) {
					HyperOpEdge* contextFrameEdge = new HyperOpEdge();
					contextFrameEdge->setContextFrameAddress(dominanceFrontierHyperOp);
					this->addEdge(vertex->getImmediateDominator(), vertex, (HyperOpEdge*) contextFrameEdge);
				} else {
					list<HyperOp*> immediateDominatorDominanceFrontier = vertex->getImmediateDominator()->getDominanceFrontier();
					if (std::find(immediateDominatorDominanceFrontier.begin(), immediateDominatorDominanceFrontier.end(), dominanceFrontierHyperOp) != immediateDominatorDominanceFrontier.end()) {
						HyperOpEdge* contextFrameEdge = new HyperOpEdge();
						contextFrameEdge->setContextFrameAddress(dominanceFrontierHyperOp);
						int freeContextSlot = -1;
						if (vertex->ParentMap.empty()) {
							freeContextSlot++;
						} else {
							for (map<HyperOpEdge*, HyperOp*>::iterator parentEdgeItr = vertex->ParentMap.begin(); parentEdgeItr != vertex->ParentMap.end(); parentEdgeItr++) {
								HyperOpEdge* const previouslyAddedEdge = parentEdgeItr->first;
								if (previouslyAddedEdge->getPositionOfContextSlot() > freeContextSlot) {
									freeContextSlot = previouslyAddedEdge->getPositionOfContextSlot() + 1;
								}
							}
						}
						if (freeContextSlot < maxContextFrameSize) {
							contextFrameEdge->setPositionOfContextSlot(freeContextSlot);
						}
						this->addEdge(vertex->getImmediateDominator(), vertex, (HyperOpEdge*) contextFrameEdge);
					}
				}
			}
		}
	}
}

void estimateExecutionTime(HyperOp *hyperOp) {
	list<unsigned int> estimate;
	estimate.push_back(1);
	hyperOp->setExecutionTimeEstimate(estimate);
}

//Returns 0 if the two hierarchical times are equal, 1 if first > second, -1 otherwise
int compareHierarchicalVolume(list<unsigned int> first, list<unsigned int> second) {
	if (first.size() > second.size()) {
		return 1;
	} else if (first.size() < second.size()) {
		return -1;
	} else {
		while (!first.empty()) {
			unsigned int firstVal = first.back();
			unsigned int secondVal = second.back();
			first.pop_back();
			second.pop_back();
			if (firstVal > secondVal) {
				return 1;
			} else if (firstVal < secondVal) {
				return -1;
			}
		}
	}
	return 0;
}

list<unsigned int> addHierarchicalVolume(list<unsigned int> first, list<unsigned int> second) {
	list<unsigned int> sum;
	unsigned int i;
	int minVal = min(first.size(), second.size());
	int maxVal = max(first.size(), second.size());
	for (i = 0; i < minVal; i++) {
		int firstInt = first.front();
		int secondInt = second.front();
		sum.push_back(firstInt + secondInt);
		first.pop_front();
		second.pop_front();
	}

	for (i = minVal; i < maxVal; i++) {
		int firstInt = 0, secondInt = 0;
		if (!first.empty()) {
			firstInt = first.front();
			first.pop_front();
		}
		if (!second.empty()) {
			secondInt = second.front();
			second.pop_front();
		}
		sum.push_back(firstInt + secondInt);
	}
	return sum;
}

/**
 * Dominant sequence and its execution time
 */
pair<list<HyperOp*>, list<unsigned int> > computeDominantSequence(list<HyperOp*> partialDominantSequence) {
//		, list<list<HyperOp*> > excludeList) {
	list<unsigned int> executionTime;
	list<HyperOp*> dominantSequence;
	HyperOp* source = partialDominantSequence.back();
	map<HyperOp*, list<unsigned int> > childNodeWeightMap = source->getChildNodeEdgeWeights();
	bool firstChild = true;
	bool atleastOneChildTraversed = false;
	if (childNodeWeightMap.size() > 0) {
		for (map<HyperOp*, list<unsigned int> >::iterator childIterator = childNodeWeightMap.begin(); childIterator != childNodeWeightMap.end(); childIterator++) {
			HyperOp* childVertex = (*childIterator).first;
			list<unsigned int> edgeWeight = (*childIterator).second;
			list<HyperOp*> tempPartialDS;
			std::copy(partialDominantSequence.begin(), partialDominantSequence.end(), std::back_inserter(tempPartialDS));
			tempPartialDS.push_back(childVertex);

//			//Check if the partial DS is in exclude list
//			if (isPathInExcludeList(tempPartialDS, excludeList)) {
//				continue;
//			}
			pair<list<HyperOp*>, list<unsigned int> > dominantSequenceOfChild = computeDominantSequence(tempPartialDS);
//					, excludeList);
			if (!dominantSequenceOfChild.first.empty()) {
				atleastOneChildTraversed = true;
				list<unsigned int> newExecutionTime = addHierarchicalVolume(edgeWeight, dominantSequenceOfChild.second);
				if (firstChild || compareHierarchicalVolume(executionTime, newExecutionTime) <= 0) {
					dominantSequence = dominantSequenceOfChild.first;
					executionTime = newExecutionTime;
					firstChild = false;
				}
			}
		}
	} else {
		atleastOneChildTraversed = true;
	}
	if (atleastOneChildTraversed) {
		dominantSequence.push_front(source);
		executionTime = addHierarchicalVolume(executionTime, source->getExecutionTimeEstimate());
	}
	return std::make_pair(dominantSequence, executionTime);
}

list<unsigned int> computeTopLevelofNode(HyperOp * node) {
	list<unsigned int> executionTime;
	bool firstIteration = true;
	map<HyperOp*, list<unsigned int> > parentNodeWeightMap = node->getParentNodeEdgeWeights();
	if (!parentNodeWeightMap.empty()) {
		for (map<HyperOp*, list<unsigned int> >::iterator parentIterator = parentNodeWeightMap.begin(); parentIterator != parentNodeWeightMap.end(); parentIterator++) {
			HyperOp* parentVertex = (*parentIterator).first;
			list<unsigned int> edgeWeight = (*parentIterator).second;
			list<unsigned int> depthOfParent = computeTopLevelofNode(parentVertex);
			list<unsigned int> newExecutionTime = addHierarchicalVolume(parentVertex->getExecutionTimeEstimate(), addHierarchicalVolume(edgeWeight, depthOfParent));
			if (firstIteration || compareHierarchicalVolume(executionTime, newExecutionTime) < 0) {
				executionTime = newExecutionTime;
				firstIteration = false;
			}
		}
	}

	if (executionTime.empty()) {
		executionTime.push_back(0);
	}

	return executionTime;
}

bool alreadyExamined(HyperOp* source, HyperOp* target, list<pair<HyperOp*, HyperOp*> > examinedEdges) {
	for (list<pair<HyperOp*, HyperOp*> >::iterator examinedEdgeItr = examinedEdges.begin(); examinedEdgeItr != examinedEdges.end(); examinedEdgeItr++) {
		if ((*examinedEdgeItr).first == source && (*examinedEdgeItr).second == target) {
			return true;
		}
	}
	return false;
}

list<HyperOp*> getIntersection(list<HyperOp*> first, list<HyperOp*> second) {
	list<HyperOp*> intersection;
	for (list<HyperOp*>::iterator firstItr = first.begin(); firstItr != first.end(); firstItr++) {
		if (std::find(second.begin(), second.end(), *firstItr) != second.end()) {
			intersection.push_back(*firstItr);
		}
	}
	return intersection;
}

/**
 * Returns execution time of the merged cluster
 */
pair<list<unsigned int>, list<pair<list<HyperOp*>, unsigned int> > > mergeNodesAndReturnExecutionTime(HyperOp* startHyperOp, pair<list<HyperOp*>, unsigned int> sourceClusterPair, pair<list<HyperOp*>, unsigned int> targetClusterPair, list<pair<list<HyperOp*>, unsigned int> > clusterList,
		bool revert) {
	list<HyperOp*> sourceCluster = sourceClusterPair.first;
	list<HyperOp*> targetCluster = targetClusterPair.first;
	list<HyperOpEdge*> stateChangedEdges;

	list<pair<HyperOpEdge*, pair<HyperOp*, HyperOp*> > > additionalEdgesMap;
	list<pair<list<HyperOp*>, unsigned int> > tempClusterList;
	std::copy(clusterList.begin(), clusterList.end(), std::back_inserter(tempClusterList));
	//Merge target cluster with source cluster and create zeroed out edges between them
	for (list<HyperOp*>::iterator targetClusterIterator = targetCluster.begin(); targetClusterIterator != targetCluster.end(); targetClusterIterator++) {
		HyperOp* targetNodeForMerge = *targetClusterIterator;
		//Find the child nodes of the node
		list<HyperOp*> targetNodeChildNodes = targetNodeForMerge->getChildList();

		//Find the bottom level of node
		list<unsigned int> targetNodeTopLevel = computeTopLevelofNode(targetNodeForMerge);

		//The source node which is at a higher level in the graph as compared to the node being merged into the cluster
		HyperOp* sourceClusterNodeFirst = 0;
		HyperOp* sourceClusterNodeSecond = 0;
		list<HyperOp*>::iterator itr, secondItr;
		list<HyperOp*> nodesBelowTarget;
		list<HyperOp*> nodesAboveTarget;
		bool first = true;
		for (list<HyperOp*>::iterator sourceClusterIterator = sourceCluster.begin(); sourceClusterIterator != sourceCluster.end(); sourceClusterIterator++) {
			//Compute bottom level of sourceClusterNode
			list<unsigned int> topLevel = computeTopLevelofNode(*sourceClusterIterator);
			//First hyperOp which is below the target node that needs merging
			if (compareHierarchicalVolume(targetNodeTopLevel, topLevel) < 0) {
				if (first) {
					itr = sourceClusterIterator;
					secondItr = sourceClusterIterator;
					sourceClusterNodeSecond = *sourceClusterIterator;
					first = false;
				}
				nodesBelowTarget.push_back(*sourceClusterIterator);
			} else {
				nodesAboveTarget.push_back(*sourceClusterIterator);
			}
		}

		if (sourceClusterNodeSecond == 0) {
			sourceClusterNodeFirst = sourceCluster.back();
			itr = sourceCluster.end();
			secondItr = sourceCluster.end();
		} else if (sourceClusterNodeSecond != sourceCluster.front()) {
			std::advance(secondItr, -1);
			sourceClusterNodeFirst = *secondItr;
		}

		list<HyperOp*> temporaryInserter;
		temporaryInserter.push_back(targetNodeForMerge);
		//Add target node that gets merged into source cluster
		sourceCluster.splice(itr, temporaryInserter);

		//Find out if there are edges to the same node from a node in nodesBelowTarget and mark them as ignored
		for (list<HyperOp*>::iterator bottomLevelNodeItr = nodesBelowTarget.begin(); bottomLevelNodeItr != nodesBelowTarget.end(); bottomLevelNodeItr++) {
			HyperOp* bottomLevelNode = *bottomLevelNodeItr;
			list<HyperOp*> bottomLevelNodeChildren = bottomLevelNode->getChildList();
			list<HyperOp*> intersectionSet = getIntersection(bottomLevelNodeChildren, targetNodeChildNodes);
			if (!intersectionSet.empty()) {
				//Mark the edges between the target node and the child nodes as ignored edges
				for (list<HyperOp*>::iterator childNodeItr = intersectionSet.begin(); childNodeItr != intersectionSet.end(); childNodeItr++) {
					list<HyperOpEdge*> stateChangedEdgeList = targetNodeForMerge->ignoreChildEdge(*childNodeItr, true);
					stateChangedEdges.merge(stateChangedEdgeList);
				}
			}
		}

		//Find out if there are edges to the same node from a node in nodesAboveTarget and mark them as ignored
		for (list<HyperOp*>::iterator topLevelNodeItr = nodesAboveTarget.begin(); topLevelNodeItr != nodesAboveTarget.end(); topLevelNodeItr++) {
			HyperOp* topLevelNode = *topLevelNodeItr;
			list<HyperOp*> topLevelNodeChildren = topLevelNode->getChildList();
			list<HyperOp*> intersectionSet = getIntersection(topLevelNodeChildren, targetNodeChildNodes);
			if (!intersectionSet.empty()) {
				//Mark the edges between the target node and the child nodes as ignored edges
				for (list<HyperOp*>::iterator childNodeItr = intersectionSet.begin(); childNodeItr != intersectionSet.end(); childNodeItr++) {
					list<HyperOpEdge*> stateChangedEdgeList = topLevelNode->ignoreChildEdge(*childNodeItr, true);
					stateChangedEdges.merge(stateChangedEdgeList);
				}
			}
		}

		//Ignore all the edges from the same cluster to the target node
		for (list<HyperOp*>::iterator nodeAboveTargetItr = nodesAboveTarget.begin(); nodeAboveTargetItr != nodesAboveTarget.end(); nodeAboveTargetItr++) {
			HyperOp* nodeAboveTarget = *nodeAboveTargetItr;
			list<HyperOp*> childNodesOfNodeAboveTarget = nodeAboveTarget->getChildList();
			if (std::find(childNodesOfNodeAboveTarget.begin(), childNodesOfNodeAboveTarget.end(), targetNodeForMerge) != childNodesOfNodeAboveTarget.end()) {
				list<HyperOpEdge*> stateChangedEdgeList = nodeAboveTarget->ignoreChildEdge(targetNodeForMerge, true);
				stateChangedEdges.merge(stateChangedEdgeList);
			}
		}
		//Ignore all the edges from the target to the same cluster
		for (list<HyperOp*>::iterator nodeBelowTargetItr = nodesBelowTarget.begin(); nodeBelowTargetItr != nodesBelowTarget.end(); nodeBelowTargetItr++) {
			HyperOp* nodeBelowTarget = *nodeBelowTargetItr;
			list<HyperOp*> parentNodesOfNodeAboveTarget = nodeBelowTarget->getParentList();
			if (std::find(parentNodesOfNodeAboveTarget.begin(), parentNodesOfNodeAboveTarget.end(), targetNodeForMerge) != parentNodesOfNodeAboveTarget.end()) {
				list<HyperOpEdge*> stateChangedEdgeList = targetNodeForMerge->ignoreParentEdge(nodeBelowTarget, true);
				stateChangedEdges.merge(stateChangedEdgeList);
			}
		}

		//Merge represents a serial schedule being generated between two clusters, edges need to be added such that there is a serial schedule between the
		if (sourceClusterNodeFirst != 0) {
			//The edge ensures that the parent node has been executed and always receives a true predicate from the parent
			HyperOpEdge *edge = new HyperOpEdge();
			edge->setType(HyperOpEdge::PREDICATE);
			edge->setIsEdgeIgnored(true);
			additionalEdgesMap.push_back(std::make_pair((HyperOpEdge*) edge, make_pair(sourceClusterNodeFirst, targetNodeForMerge)));
			sourceClusterNodeFirst->addChildEdge((HyperOpEdge*) edge, targetNodeForMerge);
			targetNodeForMerge->addParentEdge((HyperOpEdge*) edge, sourceClusterNodeFirst);
		}
		if (sourceClusterNodeSecond != 0) {
			HyperOpEdge *edge = new HyperOpEdge();
			edge->setType(HyperOpEdge::PREDICATE);
			edge->setIsEdgeIgnored(true);
			additionalEdgesMap.push_back(std::make_pair((HyperOpEdge*) edge, make_pair(targetNodeForMerge, sourceClusterNodeSecond)));
			targetNodeForMerge->addChildEdge((HyperOpEdge*) edge, sourceClusterNodeSecond);
			sourceClusterNodeSecond->addParentEdge((HyperOpEdge*) edge, targetNodeForMerge);
		}
	}

	//remove the merged cluster from the original cluster list
	clusterList.remove(targetClusterPair);

	//Set the merged cluster
	for (list<pair<list<HyperOp*>, unsigned int> >::iterator clusterItr = clusterList.begin(); clusterItr != clusterList.end(); clusterItr++) {
		if (*clusterItr == sourceClusterPair) {
			(*clusterItr).first = sourceCluster;
			break;
		}
	}

	//Check if zeroing helped reduce execution time
	list<HyperOp*> tempDominantSequence;
	tempDominantSequence.push_back(startHyperOp);
	list<list<HyperOp*> > excludeList;
	pair<list<HyperOp*>, list<unsigned int> > computedDominantSequencePair = computeDominantSequence(tempDominantSequence);

	if (revert) {
		clusterList.clear();
		std::copy(tempClusterList.begin(), tempClusterList.end(), back_inserter(clusterList));
		for (list<pair<HyperOpEdge*, pair<HyperOp*, HyperOp*> > >::iterator addedEdgeIterator = additionalEdgesMap.begin(); addedEdgeIterator != additionalEdgesMap.end(); addedEdgeIterator++) {
			(*addedEdgeIterator).second.first->removeChildEdge((*addedEdgeIterator).first);
			(*addedEdgeIterator).second.second->removeParentEdge((*addedEdgeIterator).first);
		}
		for (list<HyperOpEdge*>::iterator ignoreEdgeIterator = stateChangedEdges.begin(); ignoreEdgeIterator != stateChangedEdges.end(); ignoreEdgeIterator++) {
			(*ignoreEdgeIterator)->setIsEdgeIgnored(false);
		}
	}
	return std::make_pair(computedDominantSequencePair.second, clusterList);
}

void printDS(list<HyperOp*> dominantSequence) {
	errs() << "Dominant sequence computed:\n";
	for (list<HyperOp*>::iterator dominantSequenceItr = dominantSequence.begin(); dominantSequenceItr != dominantSequence.end(); dominantSequenceItr++) {
		errs() << (*dominantSequenceItr)->getFunction()->getName() << "->";
	}
}

void HyperOpInteractionGraph::clusterNodes() {
	list<pair<list<HyperOp*>, unsigned int> > computeClusterList;
	HyperOp* startHyperOp;
	for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
		list<HyperOp*> newCluster;
		newCluster.push_back(*vertexIterator);
		estimateExecutionTime(*vertexIterator);
		unsigned int hyperOpType = 2;
		if ((*vertexIterator)->isStartHyperOp()) {
			startHyperOp = *vertexIterator;
			hyperOpType = 0;
		}
		if ((*vertexIterator)->isEndHyperOp()) {
			hyperOpType = 1;
		}
		computeClusterList.push_back(std::make_pair(newCluster, hyperOpType));
	}

	list<pair<HyperOp*, HyperOp*> > examinedEdges;
	list<list<HyperOp*> > excludeList;
//Find the initial Dominant Sequence
	list<HyperOp*> partialDominantSequence;
	partialDominantSequence.push_back(startHyperOp);
	pair<list<HyperOp*>, list<unsigned int> > dominantSequencePair = computeDominantSequence(partialDominantSequence);
	printDS(dominantSequencePair.first);
	list<HyperOp*> dominantSequence = dominantSequencePair.first;
	list<unsigned int> executionTime = dominantSequencePair.second;

	while (true) {
		HyperOp* source;
		HyperOp* target;
		bool noEdgeToExamine = true;
		for (list<HyperOp*>::iterator dominantSequenceEdgeItr = dominantSequence.begin(); dominantSequenceEdgeItr != --dominantSequence.end();) {
			source = *dominantSequenceEdgeItr;
			target = *(++dominantSequenceEdgeItr);

			if (!alreadyExamined(source, target, examinedEdges)) {
				noEdgeToExamine = false;
				break;
			}
		}

		if (noEdgeToExamine) {
			//No more edges to examine
			break;
		}

		errs() << "dominant sequence computed:";
		printDS(dominantSequence);
		errs() << "zeroing edge between " << source->getFunction()->getName() << " and " << target->getFunction()->getName() << "\n";
		//Find the cluster containing source and target
		list<HyperOp*> sourceCluster;
		list<HyperOp*> targetCluster;
		pair<list<HyperOp*>, unsigned int> sourceClusterPair;
		pair<list<HyperOp*>, unsigned int> targetClusterPair;

		for (list<pair<list<HyperOp*>, unsigned int> >::iterator clusterListIterator = computeClusterList.begin(); clusterListIterator != computeClusterList.end(); clusterListIterator++) {
			list<HyperOp*> cluster = (*clusterListIterator).first;
			if (std::find(cluster.begin(), cluster.end(), source) != cluster.end()) {
				sourceCluster = cluster;
				sourceClusterPair = (*clusterListIterator);
			} else if (std::find(cluster.begin(), cluster.end(), target) != cluster.end()) {
				targetCluster = cluster;
				targetClusterPair = (*clusterListIterator);
			}
		}

		//Variables used to revert the changes made previously
		list<pair<HyperOpEdge*, pair<HyperOp*, HyperOp*> > > additionalEdgesMap;

		list<pair<list<HyperOp*>, unsigned int> > tempClusterList;
		std::copy(computeClusterList.begin(), computeClusterList.end(), std::back_inserter(tempClusterList));

		//Zero out the edges between source and target nodes
		source->zeroOutChildEdge(target, true);

		list<HyperOpEdge*> stateChangedEdgeList;
		//Merge target cluster with source cluster and create zeroed out edges between them
		for (list<HyperOp*>::iterator targetClusterIterator = targetCluster.begin(); targetClusterIterator != targetCluster.end(); targetClusterIterator++) {
			HyperOp* targetNodeForMerge = *targetClusterIterator;
			//Find the child nodes of the node
			list<HyperOp*> targetNodeChildNodes = targetNodeForMerge->getChildList();

			//Find the bottom level of node
			list<unsigned int> targetNodeTopLevel = computeTopLevelofNode(targetNodeForMerge);

			//The source node which is at a higher level in the graph as compared to the node being merged into the cluster
			HyperOp* sourceClusterNodeFirst = 0;
			HyperOp* sourceClusterNodeSecond = 0;
			list<HyperOp*>::iterator itr, secondItr;
			list<HyperOp*> nodesBelowTarget;
			list<HyperOp*> nodesAboveTarget;
			bool first = true;
			for (list<HyperOp*>::iterator sourceClusterIterator = sourceCluster.begin(); sourceClusterIterator != sourceCluster.end(); sourceClusterIterator++) {
				//Compute bottom level of sourceClusterNode
				list<unsigned int> topLevel = computeTopLevelofNode(*sourceClusterIterator);
				//First hyperOp which is below the target node that needs merging
				if (compareHierarchicalVolume(targetNodeTopLevel, topLevel) < 0) {
					if (first) {
						itr = sourceClusterIterator;
						secondItr = sourceClusterIterator;
						sourceClusterNodeSecond = *sourceClusterIterator;
						first = false;
					}
					nodesBelowTarget.push_back(*sourceClusterIterator);
				} else {
					nodesAboveTarget.push_back(*sourceClusterIterator);
				}
			}

			if (sourceClusterNodeSecond == 0) {
				sourceClusterNodeFirst = sourceCluster.back();
				itr = sourceCluster.end();
				secondItr = sourceCluster.end();
			} else if (sourceClusterNodeSecond != sourceCluster.front()) {
				std::advance(secondItr, -1);
				sourceClusterNodeFirst = *secondItr;
			}

			list<HyperOp*> temporaryInserter;
			temporaryInserter.push_back(targetNodeForMerge);
			//Add target node that gets merged into source cluster
			sourceCluster.splice(itr, temporaryInserter);

			//Find out if there are edges to the same node from a node in nodesBelowTarget and mark them as ignored
			for (list<HyperOp*>::iterator bottomLevelNodeItr = nodesBelowTarget.begin(); bottomLevelNodeItr != nodesBelowTarget.end(); bottomLevelNodeItr++) {
				HyperOp* bottomLevelNode = *bottomLevelNodeItr;
				list<HyperOp*> bottomLevelNodeChildren = bottomLevelNode->getChildList();
				list<HyperOp*> intersectionSet = getIntersection(bottomLevelNodeChildren, targetNodeChildNodes);
				if (!intersectionSet.empty()) {
					//Mark the edges between the target node and the child nodes as ignored edges
					for (list<HyperOp*>::iterator childNodeItr = intersectionSet.begin(); childNodeItr != intersectionSet.end(); childNodeItr++) {
						list<HyperOpEdge*> stateChangedEdges = targetNodeForMerge->ignoreChildEdge(*childNodeItr, true);
						stateChangedEdgeList.merge(stateChangedEdgeList);
					}
				}
			}

			//Find out if there are edges to the same node from a node in nodesAboveTarget and mark them as ignored
			for (list<HyperOp*>::iterator topLevelNodeItr = nodesAboveTarget.begin(); topLevelNodeItr != nodesAboveTarget.end(); topLevelNodeItr++) {
				HyperOp* topLevelNode = *topLevelNodeItr;
				list<HyperOp*> topLevelNodeChildren = topLevelNode->getChildList();
				list<HyperOp*> intersectionSet = getIntersection(topLevelNodeChildren, targetNodeChildNodes);
				if (!intersectionSet.empty()) {
					//Mark the edges between the target node and the child nodes as ignored edges
					for (list<HyperOp*>::iterator childNodeItr = intersectionSet.begin(); childNodeItr != intersectionSet.end(); childNodeItr++) {
						list<HyperOpEdge*> stateChangedEdges = topLevelNode->ignoreChildEdge(*childNodeItr, true);
						stateChangedEdgeList.merge(stateChangedEdges);
					}
				}
			}

			//Ignore all the edges from the same cluster to the target node
			for (list<HyperOp*>::iterator nodeAboveTargetItr = nodesAboveTarget.begin(); nodeAboveTargetItr != nodesAboveTarget.end(); nodeAboveTargetItr++) {
				HyperOp* nodeAboveTarget = *nodeAboveTargetItr;
				if (!(nodeAboveTarget == source && targetNodeForMerge == target)) {
					list<HyperOp*> childNodesOfNodeAboveTarget = nodeAboveTarget->getChildList();
					if (std::find(childNodesOfNodeAboveTarget.begin(), childNodesOfNodeAboveTarget.end(), targetNodeForMerge) != childNodesOfNodeAboveTarget.end()) {
						list<HyperOpEdge*> stateChangedEdges = nodeAboveTarget->ignoreChildEdge(targetNodeForMerge, true);
						stateChangedEdgeList.merge(stateChangedEdges);
					}
				}
			}
			//Ignore all the edges from the target to the same cluster
			for (list<HyperOp*>::iterator nodeBelowTargetItr = nodesBelowTarget.begin(); nodeBelowTargetItr != nodesBelowTarget.end(); nodeBelowTargetItr++) {
				HyperOp* nodeBelowTarget = *nodeBelowTargetItr;
				if (!(nodeBelowTarget == source && targetNodeForMerge == target)) {
					list<HyperOp*> parentNodesOfNodeAboveTarget = nodeBelowTarget->getParentList();
					if (std::find(parentNodesOfNodeAboveTarget.begin(), parentNodesOfNodeAboveTarget.end(), targetNodeForMerge) != parentNodesOfNodeAboveTarget.end()) {
						list<HyperOpEdge*> stateChangedEdges = targetNodeForMerge->ignoreParentEdge(nodeBelowTarget, true);
						stateChangedEdgeList.merge(stateChangedEdges);
					}
				}
			}

			//Merge represents a serial schedule being generated between two clusters, edges need to be added such that there is a serial schedule between the merged clusters
			if (sourceClusterNodeFirst != 0) {
				HyperOpEdge *edge = new HyperOpEdge();
				edge->setType(HyperOpEdge::ORDERING);
				edge->setIsEdgeIgnored(true);
				additionalEdgesMap.push_back(std::make_pair((HyperOpEdge*) edge, make_pair(sourceClusterNodeFirst, targetNodeForMerge)));
				sourceClusterNodeFirst->addChildEdge((HyperOpEdge*) edge, targetNodeForMerge);
				targetNodeForMerge->addParentEdge((HyperOpEdge*) edge, sourceClusterNodeFirst);
			}
			if (sourceClusterNodeSecond != 0) {
				HyperOpEdge *edge = new HyperOpEdge();
				edge->setType(HyperOpEdge::ORDERING);
				edge->setIsEdgeIgnored(true);
				additionalEdgesMap.push_back(std::make_pair(edge, make_pair(targetNodeForMerge, sourceClusterNodeSecond)));
				targetNodeForMerge->addChildEdge((HyperOpEdge*) edge, sourceClusterNodeSecond);
				sourceClusterNodeSecond->addParentEdge((HyperOpEdge*) edge, targetNodeForMerge);
			}
		}

		//remove the merged cluster from the original cluster list
		computeClusterList.remove(targetClusterPair);
		//Set the merged cluster
		for (list<pair<list<HyperOp*>, unsigned int> >::iterator clusterItr = computeClusterList.begin(); clusterItr != computeClusterList.end(); clusterItr++) {
			if (*clusterItr == sourceClusterPair) {
				(*clusterItr).first = sourceCluster;
				break;
			}
		}

		//Check if zeroing helped reduce execution time
		list<HyperOp*> tempDominantSequence;
		tempDominantSequence.push_back(startHyperOp);
		pair<list<HyperOp*>, list<unsigned int> > computedDominantSequencePair = computeDominantSequence(tempDominantSequence);
//				, excludeList);
		//If execution time decreases from the previous step, clustering is acceptable
		if (compareHierarchicalVolume(executionTime, computedDominantSequencePair.second) >= 0) {
//			excludeList.clear();
			dominantSequence = computedDominantSequencePair.first;
			executionTime = computedDominantSequencePair.second;
			for (list<pair<HyperOpEdge*, pair<HyperOp*, HyperOp*> > >::iterator addedEdgeIterator = additionalEdgesMap.begin(); addedEdgeIterator != additionalEdgesMap.end(); addedEdgeIterator++) {
				examinedEdges.push_back(make_pair((*addedEdgeIterator).second.first, (*addedEdgeIterator).second.second));
			}

		} else {
			std::copy(tempClusterList.begin(), tempClusterList.end(), back_inserter(computeClusterList));
			for (list<pair<HyperOpEdge*, pair<HyperOp*, HyperOp*> > >::iterator addedEdgeIterator = additionalEdgesMap.begin(); addedEdgeIterator != additionalEdgesMap.end(); addedEdgeIterator++) {
				(*addedEdgeIterator).second.first->removeChildEdge((*addedEdgeIterator).first);
				(*addedEdgeIterator).second.second->removeParentEdge((*addedEdgeIterator).first);
			}
			for (list<HyperOpEdge*>::iterator ignoreEdgeIterator = stateChangedEdgeList.begin(); ignoreEdgeIterator != stateChangedEdgeList.end(); ignoreEdgeIterator++) {
				(*ignoreEdgeIterator)->setIsEdgeIgnored(false);
			}
			source->zeroOutChildEdge(target, false);
//			excludeList.push_back(dominantSequencePair.first);
		}
		//Add the edge to examined list;
		examinedEdges.push_back(std::make_pair(source, target));

//		printDS(dominantSequencePair.first);
	}

	//Merge clusters till the number of compute resources matches the number of clusters created
	while (computeClusterList.size() > (this->rowCount * this->columnCount)) {
		//Find all pairs of clusters and merge the one that leads to the least execution time
		//This is expensive but still cheaper when operating on clusters since the number of clusters is way smaller than the number of nodes in the original graph
		pair<list<HyperOp*>, unsigned int> sourceClusterPair;
		pair<list<HyperOp*>, unsigned int> targetClusterPair;
		list<HyperOp*> sourceCluster;
		list<HyperOp*> targetCluster;
		list<pair<list<HyperOp*>, unsigned int> > tempClusterList;

		list<unsigned int> minimumExecutionTime;
		bool first = true;
		for (list<pair<list<HyperOp*>, unsigned int> >::iterator sourceClusterItr = computeClusterList.begin(); sourceClusterItr != computeClusterList.end(); sourceClusterItr++) {
			list<HyperOp*> sourceCluster = sourceClusterItr->first;
			for (list<pair<list<HyperOp*>, unsigned int> >::iterator targetClusterItr = computeClusterList.begin(); targetClusterItr != computeClusterList.end(); targetClusterItr++) {
				if (targetClusterItr != sourceClusterItr) {
					pair<list<unsigned int>, list<pair<list<HyperOp*>, unsigned int> > > returnValue = mergeNodesAndReturnExecutionTime(startHyperOp, *sourceClusterItr, *targetClusterItr, computeClusterList, true);
					list<unsigned int> newExecutionTime = returnValue.first;
					if (first || compareHierarchicalVolume(minimumExecutionTime, newExecutionTime) >= 0) {
						minimumExecutionTime = newExecutionTime;
						sourceClusterPair = *sourceClusterItr;
						sourceCluster = sourceClusterPair.first;
						targetClusterPair = *targetClusterItr;
						targetCluster = targetClusterPair.first;
						first = false;
					}
				}
			}
		}

		pair<list<unsigned int>, list<pair<list<HyperOp*>, unsigned int> > > returnValue = mergeNodesAndReturnExecutionTime(startHyperOp, sourceClusterPair, targetClusterPair, computeClusterList, false);
		computeClusterList = returnValue.second;
	}

	for (list<pair<list<HyperOp*>, unsigned int> >::iterator clusterItr = computeClusterList.begin(); clusterItr != computeClusterList.end(); clusterItr++) {
		clusterList.push_back(clusterItr->first);
	}

}

int linearizeTime(list<unsigned int> time) {
	int linearTime = 0, temp = 1;
	for (list<unsigned int>::iterator timeItr = time.begin(); timeItr != time.end(); timeItr++) {
		linearTime = linearTime + (int) ((*timeItr) * temp);
		temp = temp * 60;
	}
	return linearTime;
}

int factorial(int n) {
	int retVal = 1;
	for (int i = 1; i <= n; i++) {
		retVal = retVal * i;
	}
	return retVal;
}
int combination(int n, int r) {
	if (n < r) {
		return 0;
	}
	return factorial(n) / (factorial(n - r) * factorial(r));
}

void HyperOpInteractionGraph::mapClustersToComputeResources() {
//Start of the edge is defined the top level of the source HyperOp of the edge and end is top level + edge weight

//First entry corresponds to the HyperOp and second, index of the cluster it belongs to in clusterList
	map<HyperOp*, unsigned int> clusterMap;
	for (list<HyperOp*>::iterator vertexItr = Vertices.begin(); vertexItr != Vertices.end(); vertexItr++) {
		HyperOp* vertex = *vertexItr;
		list<unsigned int> topLevel = computeTopLevelofNode(vertex);
		vertex->setTopLevel(topLevel);
		int i = 0;
		for (list<list<HyperOp*> >::iterator clusterItr = clusterList.begin(); clusterItr != clusterList.end(); clusterItr++) {
			if (std::find(clusterItr->begin(), clusterItr->end(), vertex) != clusterItr->end()) {
				break;
			}
			//Incrementing by 2 because we compute two indices for x and y coordinates
			i = i + 2;
		}
		clusterMap.insert(std::make_pair(vertex, i));
	}

//First pair is <source cluster index, target cluster index> tuple and second entry indicates <weight of communication, start time> tuple
//This map is used to ensure that the source and target clusters are placed closest to each other
//I wish tuple could be used here, but unfortunately, tuple is c++11 and boost tuple is the best I can do. If anyone ever touches this snippet ever and tuple is supported then, please use it.
	list<pair<pair<unsigned int, unsigned int>, pair<list<unsigned int>, list<unsigned int> > > > communicationMap;
	for (list<HyperOp*>::iterator vertexItr = Vertices.begin(); vertexItr != Vertices.end(); vertexItr++) {
		HyperOp* vertex = *vertexItr;
		map<HyperOp*, list<unsigned int> > childNodes = vertex->getChildNodeEdgeWeights();
		for (map<HyperOp*, list<unsigned int> >::iterator childNodeItr = childNodes.begin(); childNodeItr != childNodes.end(); childNodeItr++) {
			HyperOp* childNode = childNodeItr->first;
			list<unsigned int> tempTime;
			tempTime.push_back(0);
			//If edge has non-zero weightage and does not correspond to edges from the same cluster
			if (compareHierarchicalVolume(childNodeItr->second, tempTime) > 0 && clusterMap.find(vertex)->second != clusterMap.find(childNode)->second) {
				pair<list<unsigned int>, list<unsigned int> > timeTuple = std::make_pair(childNodeItr->second, addHierarchicalVolume(vertex->getTopLevel(), vertex->getExecutionTimeEstimate()));
				pair<unsigned int, unsigned int> clusterTuple = std::make_pair(clusterMap.find(vertex)->second, clusterMap.find(childNode)->second);
				communicationMap.push_back(std::make_pair(clusterTuple, timeTuple));
			}
		}
	}

//A conflict between edges is defined as edges that execute concurrently across clusters and don't share the same source or target cluster
//	list<pair<pair<unsigned int, unsigned int>, pair<unsigned int, unsigned int> > > conflictingEdges;
//	for (list<pair<pair<unsigned int, unsigned int>, pair<list<unsigned int>, list<unsigned int> > > >::iterator edgeItr = communicationMap.begin(); edgeItr != communicationMap.end(); edgeItr++) {
//		list<unsigned int> edgeStartTime = (edgeItr->second).second;
//		list<unsigned int> edgeEndTime = addHierarchicalVolume((edgeItr->second).first, (edgeItr->second).second);
//		for (list<pair<pair<unsigned int, unsigned int>, pair<list<unsigned int>, list<unsigned int> > > >::iterator conflictingEdgeItr = communicationMap.begin(); conflictingEdgeItr != communicationMap.end(); conflictingEdgeItr++) {
//			//If not the same edges
//			if (edgeItr != conflictingEdgeItr && (!(edgeItr->first.first == conflictingEdgeItr->first.first && edgeItr->first.second == conflictingEdgeItr->first.second))) {
//				list<unsigned int> conflictEdgeStartTime = (conflictingEdgeItr->second).second;
//				list<unsigned int> conflictEdgeEndTime = addHierarchicalVolume((conflictingEdgeItr->second).first, (conflictingEdgeItr->second).second);
//				if (!(compareHierarchicalVolume(conflictEdgeStartTime, edgeEndTime) >= 0 || compareHierarchicalVolume(conflictEdgeEndTime, edgeStartTime) >= 0)) {
//					//Edges that execute concurrently
//					conflictingEdges.push_back(std::make_pair(edgeItr->first, conflictingEdgeItr->first));
//				}
//			}
//		}
//	}

//Create the linear programming problem
	int Ncol = clusterList.size() * 2;
	int maxDimM = columnCount;
	int maxDimN = rowCount;
	int ret = 0;
	int combinationVariables = 4 * combination(clusterList.size(), 2);

	lprec *lp;
	int *colno = NULL;
	REAL *row = NULL;
//First set of columns are the actual x and y coordinates being computed
//Second set of columns are to ensure that the <x,y> pair for each cluster is unique
//Third set of columns corresponds to boolean variables being used to compute |x1-x2| and them assigned to a scalar value d1=|xj-xk| whose lower bound is 0; b1,d1 correspond to boolean variable and scalar being used for |x| followed by similar set for |y|
//Fourth set of columns are to minimize the overlap between concurrent communication edges
	int numColumns = Ncol + combinationVariables + communicationMap.size() * 2;
//	+ conflictingEdges.size() * 2;
	lp = make_lp(0, numColumns);

	int i;
// Set bounds on x,y coordinates being computed as a map of clusters
	for (i = 1; i <= Ncol; i += 2) {
		//X bound
		set_bounds(lp, i, 0, maxDimM - 1);
		set_int(lp, i, 1);
		//Y bound
		set_bounds(lp, i + 1, 0, maxDimN - 1);
		set_int(lp, i + 1, 1);
	}

	int addedVariableIndex = Ncol + 1;
	int j;

	colno = (int *) malloc(5 * sizeof(*colno));
	row = (REAL *) malloc(5 * sizeof(*row));
//This is to ensure that two clusters don't get mapped to the same compute resource
	for (i = 1; i <= Ncol - 2; i += 2) {
		for (j = i + 2; j <= Ncol; j += 2) {
			//Add boolean constraints for |x1-x2|>=d1
			colno[0] = i;
			row[0] = 1;
			colno[1] = j;
			row[1] = -1;
			colno[2] = addedVariableIndex;
			row[2] = maxDimM + 1;
			colno[3] = addedVariableIndex + 1;
			row[3] = -1;
			add_constraintex(lp, 4, row, colno, GE, 0);

			colno[0] = i;
			row[0] = 1;
			colno[1] = j;
			row[1] = -1;
			colno[2] = addedVariableIndex;
			row[2] = maxDimM + 1;
			colno[3] = addedVariableIndex + 1;
			row[3] = 1;
			add_constraintex(lp, 4, row, colno, LE, maxDimM + 1);
			set_binary(lp, addedVariableIndex, 1);
			set_lowbo(lp, addedVariableIndex + 1, 0);

			addedVariableIndex = addedVariableIndex + 2;
			//Add boolean constraints for |y1-y2|>=d2
			colno[0] = i + 1;
			row[0] = 1;
			colno[1] = j + 1;
			row[1] = -1;
			colno[2] = addedVariableIndex;
			row[2] = maxDimN + 1;
			colno[3] = addedVariableIndex + 1;
			row[3] = -1;
			add_constraintex(lp, 4, row, colno, GE, 0);

			colno[0] = i + 1;
			row[0] = 1;
			colno[1] = j + 1;
			row[1] = -1;
			colno[2] = addedVariableIndex;
			row[2] = maxDimN + 1;
			colno[3] = addedVariableIndex + 1;
			row[3] = 1;
			add_constraintex(lp, 4, row, colno, LE, maxDimN + 1);
			set_binary(lp, addedVariableIndex, 1);
			set_lowbo(lp, addedVariableIndex + 1, 0);

			//Add a constraint to ensure that the added variables sum up to >=1
			colno[0] = addedVariableIndex - 1;
			row[0] = 1;
			colno[1] = addedVariableIndex + 1;
			row[1] = 1;
			add_constraintex(lp, 2, row, colno, GE, 1);
			addedVariableIndex = addedVariableIndex + 2;
		}
	}

	int diffVariableCount = communicationMap.size() * 2;
//			+ conflictingEdges.size() * 2;
	int *minimizationColumn = (int *) malloc(diffVariableCount * sizeof(*minimizationColumn));
	REAL* minimizationRow = (REAL *) malloc(diffVariableCount * sizeof(*minimizationRow));
//Constraints for communicating edges
	j = 0;
	for (list<pair<pair<unsigned int, unsigned int>, pair<list<unsigned int>, list<unsigned int> > > >::iterator communicationItr = communicationMap.begin(); communicationItr != communicationMap.end(); communicationItr++) {
		int sourcex = communicationItr->first.first + 1;
		int sourcey = sourcex + 1;

		int targetx = communicationItr->first.second + 1;
		int targety = targetx + 1;

		//Add constraints for |x1-x2|<d1
		colno[0] = sourcex;
		row[0] = 1;
		colno[1] = targetx;
		row[1] = -1;
		colno[2] = addedVariableIndex;
		row[2] = -1;
		add_constraintex(lp, 3, row, colno, LE, 0);

		colno[0] = sourcex;
		row[0] = -1;
		colno[1] = targetx;
		row[1] = 1;
		colno[2] = addedVariableIndex;
		row[2] = -1;
		add_constraintex(lp, 3, row, colno, LE, 0);

		set_int(lp, addedVariableIndex, 1);
		set_lowbo(lp, addedVariableIndex, 0);

		//Associate weightage
		minimizationColumn[j] = addedVariableIndex;
		minimizationRow[j] = linearizeTime(communicationItr->second.first);
		j++;

		//Add constraints for |y1-y2|<d2
		colno[0] = sourcey;
		row[0] = 1;
		colno[1] = targety;
		row[1] = -1;
		colno[2] = addedVariableIndex + 1;
		row[2] = -1;
		add_constraintex(lp, 3, row, colno, LE, 0);

		colno[0] = sourcey;
		row[0] = -1;
		colno[1] = targety;
		row[1] = 1;
		colno[2] = addedVariableIndex + 1;
		row[2] = -1;
		add_constraintex(lp, 3, row, colno, LE, 0);

		//Associate weightage
		minimizationColumn[j] = addedVariableIndex + 1;
		minimizationRow[j] = linearizeTime(communicationItr->second.first);
		j++;
		set_int(lp, addedVariableIndex + 1, 1);
		set_lowbo(lp, addedVariableIndex + 1, 0);
		addedVariableIndex += 2;
	}

//	for (list<pair<pair<unsigned int, unsigned int>, pair<unsigned int, unsigned int> > >::iterator conflictItr = conflictingEdges.begin(); conflictItr != conflictingEdges.end(); conflictItr++) {
//		unsigned int firstSourceX = conflictItr->first.first + 1;
//		unsigned int firstSourceY = firstSourceX + 1;
//
//		unsigned int firstTargetX = conflictItr->first.second + 1;
//		unsigned int firstTargetY = firstTargetX + 1;
//
//		unsigned int secondSourceX = conflictItr->second.first + 1;
//		unsigned int secondSourceY = secondSourceX + 1;
//
//		unsigned int secondTargetX = conflictItr->second.second + 1;
//		unsigned int secondTargetY = secondTargetX + 1;
//
//		//Add constraints for |x1-x2|+|x3-x4|<=d1
//		int columnQuotientsArray[4][4] = { { 1, -1, 1, -1 }, { -1, 1, 1, -1 }, { 1, -1, -1, 1 }, { -1, 1, -1, 1 } };
//
//		for (int i = 0; i < 4; i++) {
//			int columnOne = columnQuotientsArray[i][0];
//			int columnTwo = columnQuotientsArray[i][1];
//			int columnThree = columnQuotientsArray[i][2];
//			int columnFour = columnQuotientsArray[i][3];
//
//			if (firstSourceX == secondSourceX) {
//				columnOne = columnThree = columnQuotientsArray[i][0] + columnQuotientsArray[i][2];
//			}
//			if (firstSourceX == secondTargetX) {
//				columnOne = columnFour = columnQuotientsArray[i][0] + columnQuotientsArray[i][3];
//			}
//			if (firstTargetX == secondSourceX) {
//				columnTwo = columnThree = columnQuotientsArray[i][1] + columnQuotientsArray[i][2];
//			}
//			if (firstTargetX == secondTargetX) {
//				columnTwo = columnFour = columnQuotientsArray[i][1] + columnQuotientsArray[i][3];
//			}
//
//			int k = 0;
//			if (columnOne) {
//				colno[k] = firstSourceX;
//				row[k++] = columnOne;
//			}
//			if (columnTwo) {
//				colno[k] = firstTargetX;
//				row[k++] = columnTwo;
//			}
//			if (columnThree) {
//				colno[k] = secondSourceX;
//				row[k++] = columnThree;
//			}
//			if (columnFour) {
//				colno[k] = secondTargetX;
//				row[k++] = columnFour;
//			}
//
//			colno[k] = addedVariableIndex;
//			row[k++] = -1;
//			add_constraintex(lp, k, row, colno, LE, 0);
//
//			columnOne = columnQuotientsArray[i][0];
//			columnTwo = columnQuotientsArray[i][1];
//			columnThree = columnQuotientsArray[i][2];
//			columnFour = columnQuotientsArray[i][3];
//
//			if (firstSourceY == secondSourceY) {
//				columnOne = columnThree = columnQuotientsArray[i][0] + columnQuotientsArray[i][2];
//			}
//			if (firstSourceY == secondTargetY) {
//				columnOne = columnFour = columnQuotientsArray[i][0] + columnQuotientsArray[i][3];
//			}
//			if (firstTargetY == secondSourceY) {
//				columnTwo = columnThree = columnQuotientsArray[i][1] + columnQuotientsArray[i][2];
//			}
//			if (firstTargetY == secondTargetY) {
//				columnTwo = columnFour = columnQuotientsArray[i][1] + columnQuotientsArray[i][3];
//			}
//
//			k = 0;
//			if (columnOne) {
//				colno[k] = firstSourceY;
//				row[k++] = columnOne;
//			}
//			if (columnTwo) {
//				colno[k] = firstTargetY;
//				row[k++] = columnTwo;
//			}
//			if (columnThree) {
//				colno[k] = secondSourceY;
//				row[k++] = columnThree;
//			}
//			if (columnFour) {
//				colno[k] = secondTargetY;
//				row[k++] = columnFour;
//			}
//			colno[k] = addedVariableIndex + 1;
//			row[k++] = -1;
//			add_constraintex(lp, k, row, colno, LE, 0);
//		}
//
//		minimizationColumn[j] = addedVariableIndex;
//		minimizationRow[j] = -1;
//		minimizationColumn[j + 1] = addedVariableIndex + 1;
//		minimizationRow[j + 1] = -1;
//		set_lowbo(lp, addedVariableIndex, 0);
//		set_int(lp, addedVariableIndex, 1);
//		set_lowbo(lp, addedVariableIndex + 1, 0);
//		set_int(lp, addedVariableIndex + 1, 1);
//		addedVariableIndex += 2;
//		j += 2;
//	}

	set_add_rowmode(lp, 0);
	/* set the objective in lpsolve */
	set_obj_fnex(lp, diffVariableCount, minimizationRow, minimizationColumn);
	set_minim(lp);
	set_verbose(lp, IMPORTANT);
	ret = solve(lp);
	if (ret == OPTIMAL)
		ret = 0;

	list<pair<int, int> > solution;
	REAL clusterCRMap[clusterMap.size() * 2];
	if (ret == 0) {
		get_variables(lp, clusterCRMap);
		//TODO (previously:a solution is calculated, now lets compute the overlap in edges; this part is now commented out because we need to employ branch and bound to eliminate each solution from the next problem being solved)
		for (i = 0; i < Ncol; i += 2) {
			solution.push_back(std::make_pair((int) clusterCRMap[i], (int) clusterCRMap[i + 1]));
		}
	}
	if (lp != NULL) {
		delete_lp(lp);
	}

	i = 0;
	list<list<HyperOp*> >::iterator clusterItr = clusterList.begin();
	for (list<pair<int, int> >::iterator solutionItr = solution.begin(); solutionItr != solution.end(); solutionItr++) {
		list<HyperOp*> cluster = *clusterItr;
		int x = solutionItr->first;
		int y = solutionItr->second;
		DEBUG(dbgs() << "Cluster " << i << "goes to " << x << ":" << y << "\n");
		i++;
		for (list<HyperOp*>::iterator nodeItr = cluster.begin(); nodeItr != cluster.end(); nodeItr++) {
			(*nodeItr)->setTargetResource(x * maxDimM + y);
		}
		std::advance(clusterItr, 1);
	}
}

//void associateContextFramesToCluster(list<HyperOp*> cluster, int numContextFrames) {
//	unsigned int numExtraVariables = 0, numExtraVariablesForOptimization = 0;
//	if (cluster.size() == 1) {
//		(*cluster.begin())->setContextFrame(0);
//	} else {
//		list<HyperOp*> temporaryCluster;
//		std::copy(cluster.begin(), cluster.end(), back_inserter(temporaryCluster));
//		int column[4];
//		REAL row[4];
//		map<int, pair<list<int>, list<int> > > conflictGraph;
//		int vertexIteratorIndex = 1;
//		for (list<HyperOp*>::iterator vertexIterator = cluster.begin(); vertexIterator != cluster.end(); vertexIterator++) {
//			HyperOp* vertex = *vertexIterator;
//			HyperOp* liveStartOfVertex = vertex->getImmediateDominator();
//			HyperOp* liveEndOfVertex;
//			if (liveStartOfVertex != 0) {
//				liveEndOfVertex = liveStartOfVertex->getImmediatePostDominator();
//			} else {
//				liveEndOfVertex = vertex;
//			}
//			int secondIteratorIndex = vertexIteratorIndex + 1;
//			list<int> conflictEntries, nonConflictEntries;
//			list<HyperOp*>::iterator secondVertexIterator = vertexIterator;
//			advance(secondVertexIterator, 1);
//			if (secondVertexIterator != cluster.end()) {
//				for (; secondVertexIterator != cluster.end(); secondVertexIterator++) {
//					HyperOp* secondVertex = *secondVertexIterator;
//					HyperOp* liveStartOfSecondVertex = secondVertex->getImmediateDominator();
//					HyperOp* liveEndOfSecondVertex;
//					if (liveStartOfSecondVertex != 0) {
//						liveEndOfSecondVertex = liveStartOfSecondVertex->getImmediatePostDominator();
//					} else {
//						liveEndOfSecondVertex = secondVertex;
//					}
//					if (liveEndOfVertex->isPredecessor(liveStartOfSecondVertex) || liveEndOfSecondVertex->isPredecessor(liveStartOfVertex)) {
//						nonConflictEntries.push_back(secondIteratorIndex);
//						//One binary variable and one diff value is added here
//						numExtraVariables += 2;
//						numExtraVariablesForOptimization += 1;
//					} else {
//						//Conflict in the graph, add a new row of condition
//						conflictEntries.push_back(secondIteratorIndex);
//						//Only one binary variable is added
//						numExtraVariables++;
//					}
//					secondIteratorIndex++;
//				}
//				conflictGraph.insert(std::make_pair(vertexIteratorIndex, std::make_pair(conflictEntries, nonConflictEntries)));
//				vertexIteratorIndex++;
//			}
//		}
//
////Compute liveness of HyperOp's frame and construct conflict graph
//		lprec *lp = make_lp(0, cluster.size() + numExtraVariables);
//		set_add_rowmode(lp, TRUE); /* makes building the model faster if it is done rows by row */
//
//		//Add the vertex bounds and binary variables for conflicting nodes
//		int columnCount = cluster.size() + 1;
//
//		int * maxColumn = (int*) malloc(numExtraVariablesForOptimization * sizeof(maxColumn));
//		REAL* maxRow = (REAL*) malloc(numExtraVariablesForOptimization * sizeof(maxRow));
//		int optIterator = 0;
//		for (map<int, pair<list<int>, list<int> > >::iterator conflictGraphIterator = conflictGraph.begin(); conflictGraphIterator != conflictGraph.end(); conflictGraphIterator++) {
//			int sourceVertex = conflictGraphIterator->first;
//			set_int(lp, sourceVertex, 1);
//			pair<list<int>, list<int> > conflictingVertices = conflictGraphIterator->second;
//			for (list<int>::iterator conflictVertexIterator = conflictingVertices.first.begin(); conflictVertexIterator != conflictingVertices.first.end(); conflictVertexIterator++) {
//				int targetVertex = *conflictVertexIterator;
//				set_binary(lp, columnCount, 1);
//				column[0] = sourceVertex;
//				row[0] = 1;
//				column[1] = targetVertex;
//				row[1] = -1;
//				column[2] = columnCount;
//				row[2] = numContextFrames;
//				add_constraintex(lp, 3, row, column, GE, 1);
//				add_constraintex(lp, 3, row, column, LE, numContextFrames - 1);
//				columnCount++;
//			}
//
//			//Add optimization criterion for non-conflicting nodes
//			for (list<int>::iterator nonConflictVertexIterator = conflictingVertices.second.begin(); nonConflictVertexIterator != conflictingVertices.second.end(); nonConflictVertexIterator++) {
//				int targetVertex = *nonConflictVertexIterator;
//				//Introduce a slack variable (with lower bound zero) corresponding to the difference in color between non-conflicting nodes and minimize the sum of these variables
//				set_int(lp, columnCount, 1);
//				set_lowbo(lp, columnCount, 0);
//
//				columnCount++;
//				set_binary(lp, columnCount, 1);
//				column[0] = sourceVertex;
//				row[0] = 1;
//				column[1] = targetVertex;
//				row[1] = -1;
//				column[2] = columnCount - 1;
//				row[2] = -1;
//				column[3] = columnCount;
//				row[3] = numContextFrames;
//				add_constraintex(lp, 4, row, column, GE, 0);
//
//				column[0] = sourceVertex;
//				row[0] = 1;
//				column[1] = targetVertex;
//				row[1] = -1;
//				column[2] = columnCount - 1;
//				row[2] = 1;
//				column[3] = columnCount;
//				row[3] = numContextFrames;
//				add_constraintex(lp, 4, row, column, LE, numContextFrames);
//
//				maxColumn[optIterator] = columnCount - 1;
//				maxRow[optIterator++] = 1;
//				columnCount++;
//			}
//		}
//
//		int ret = 0;
//		set_add_rowmode(lp, FALSE); /* rowmode should be turned off again when done building the model */
//
//		set_obj_fnex(lp, numExtraVariablesForOptimization, maxRow, maxColumn);
//		set_maxim(lp);
//		set_verbose(lp, IMPORTANT);
//		ret = solve(lp);
//		free(maxColumn);
//		free(maxRow);
////			if (ret == 2) {
////				TODO:Infeasible solution, break the graph into smaller chunks and repeat coloring
////			}
//		if (ret == 0) {
//			REAL resultRow[cluster.size()];
//			/* variable values */
//			get_variables(lp, resultRow);
//			int i = 0;
//			for (list<HyperOp*>::iterator vertexIterator = temporaryCluster.begin(); vertexIterator != temporaryCluster.end(); vertexIterator++) {
//				(*vertexIterator)->setContextFrame((unsigned) resultRow[i++]);
//			}
//		}
//		if (lp != NULL) {
//			delete_lp(lp);
//		}
//	}
//}

void associateContextFramesToCluster(list<HyperOp*> cluster, int numContextFrames) {
	typedef adjacency_list<listS, vecS, undirectedS, no_property, no_property, no_property, listS> MyGraph;
	typedef graph_traits<MyGraph>::vertices_size_type vertices_size_type;
	typedef property_map<MyGraph, vertex_index_t>::const_type vertex_index_map;
	typedef graph_traits<MyGraph>::vertex_descriptor vertex_descriptor;
	MyGraph g;
	map<HyperOp*, vertex_descriptor> hyperOpAndVertexDescriptorMap;
	map<vertex_descriptor, HyperOp*> vertexDescriptorAndHyperOpMap;

	for (list<HyperOp*>::iterator vertexIterator = cluster.begin(); vertexIterator != cluster.end(); vertexIterator++) {
		vertex_descriptor newVertex = add_vertex(g);
		hyperOpAndVertexDescriptorMap[*vertexIterator] = newVertex;
		vertexDescriptorAndHyperOpMap[newVertex] = *vertexIterator;
	}
	for (list<HyperOp*>::iterator vertexIterator = cluster.begin(); vertexIterator != cluster.end(); vertexIterator++) {
		HyperOp* vertex = *vertexIterator;
		HyperOp* liveStartOfVertex = vertex->getImmediateDominator();
		HyperOp* liveEndOfVertex;
		if (liveStartOfVertex != 0) {
			liveEndOfVertex = liveStartOfVertex->getImmediatePostDominator();
		} else {
			liveEndOfVertex = vertex;
		}
		list<HyperOp*>::iterator secondVertexIterator = vertexIterator;
		advance(secondVertexIterator, 1);
		if (secondVertexIterator != cluster.end()) {
			for (; secondVertexIterator != cluster.end(); secondVertexIterator++) {
				HyperOp* secondVertex = *secondVertexIterator;
				HyperOp* liveStartOfSecondVertex = secondVertex->getImmediateDominator();
				HyperOp* liveEndOfSecondVertex;
				if (liveStartOfSecondVertex != 0) {
					liveEndOfSecondVertex = liveStartOfSecondVertex->getImmediatePostDominator();
				} else {
					liveEndOfSecondVertex = secondVertex;
				}
				if (liveEndOfVertex->isPredecessor(liveStartOfSecondVertex) || liveEndOfSecondVertex->isPredecessor(liveStartOfVertex)) {
					continue;
				} else {
					//Conflict in the graph, add an edge in the graph
					add_edge(hyperOpAndVertexDescriptorMap[vertex], hyperOpAndVertexDescriptorMap[secondVertex], g);
				}
			}
		}
	}
	//Color the conflict graph
	MyGraph::vertex_iterator boostVertexIt, boostVertexEnd;
	vector<vertices_size_type> color_vec(numContextFrames);
	iterator_property_map<vertices_size_type*, vertex_index_map> color(&color_vec.front(), get(vertex_index, g));
	vertices_size_type num_colors = sequential_vertex_coloring(g, color);
	int index = 0;
	boost::tie(boostVertexIt, boostVertexEnd) = vertices(g);
	for (; boostVertexIt != boostVertexEnd; ++boostVertexIt) {
		HyperOp* hyperOp = vertexDescriptorAndHyperOpMap[*boostVertexIt];
		if (hyperOp->isStaticHyperOp()) {
			hyperOp->setContextFrame(color_vec[index++]);
		}
	}
}

void HyperOpInteractionGraph::associateStaticContextFrames() {
//Clusters might be mapped to the same compute resource, undesirably of course; Further, HyperOps maybe mapped to the same context frame within a cluster
//The problem is to identify conflicting HyperOps and set the flag fbindinstrrequired for such HyperOps
	map<unsigned, list<list<HyperOp*> > > crAndClusterMap;
	for (list<list<HyperOp*> >::iterator clusterListItr = clusterList.begin(); clusterListItr != clusterList.end(); clusterListItr++) {
		associateContextFramesToCluster(*clusterListItr, numContextFrames);
		unsigned targetCR = (*clusterListItr->begin())->getTargetResource();
		list<list<HyperOp*> > conflictList;
		if (crAndClusterMap.find(targetCR) != crAndClusterMap.end()) {
			conflictList = crAndClusterMap.find(targetCR)->second;
			crAndClusterMap.erase(crAndClusterMap.find(targetCR));
		}
		conflictList.push_back(*clusterListItr);
		crAndClusterMap.insert(make_pair(targetCR, conflictList));
	}

	for (map<unsigned, list<list<HyperOp*> > >::iterator conflictingClusterItr = crAndClusterMap.begin(); conflictingClusterItr != crAndClusterMap.end(); conflictingClusterItr++) {
		list<list<HyperOp*> > conflictingHyperOps = conflictingClusterItr->second;
		map<unsigned, list<HyperOp*> > contextFramesAndAssociatedHyperOps;
		for (list<list<HyperOp*> >::iterator conflictingClusters = conflictingHyperOps.begin(); conflictingClusters != conflictingHyperOps.end(); conflictingClusters++) {
			for (list<HyperOp*>::iterator conflictingClusterContents = conflictingClusters->begin(); conflictingClusterContents != conflictingClusters->end(); conflictingClusterContents++) {
				HyperOp* hyperOp = (*conflictingClusterContents);
				unsigned contextFrame = (*conflictingClusterContents)->getContextFrame();
				list<HyperOp*> associatedHyperOps;
				if (contextFramesAndAssociatedHyperOps.find(contextFrame) != contextFramesAndAssociatedHyperOps.end()) {
					associatedHyperOps = contextFramesAndAssociatedHyperOps.find(contextFrame)->second;
					contextFramesAndAssociatedHyperOps.erase(contextFrame);
				}
				associatedHyperOps.push_back(hyperOp);
				contextFramesAndAssociatedHyperOps.insert(make_pair(contextFrame, associatedHyperOps));
			}
		}

		for (map<unsigned, list<HyperOp*> >::iterator contextFrameItr = contextFramesAndAssociatedHyperOps.begin(); contextFrameItr != contextFramesAndAssociatedHyperOps.end(); contextFrameItr++) {
			list<HyperOp*> conflictingContextFrameHyperOps = contextFrameItr->second;
			HyperOp* topmostHyperOp = 0;
			HyperOp* bottommostHyperOp = 0;
			for (list<HyperOp*>::iterator conflictItr = conflictingContextFrameHyperOps.begin(); conflictItr != conflictingContextFrameHyperOps.end(); conflictItr++) {
				HyperOp* conflictingHyperOp = *conflictItr;
				if (topmostHyperOp == 0 || conflictingHyperOp->isPredecessor(topmostHyperOp)) {
					topmostHyperOp = conflictingHyperOp;
				}
				if (bottommostHyperOp == 0 || !(conflictingHyperOp->isPredecessor(bottommostHyperOp))) {
					bottommostHyperOp = conflictingHyperOp;
				}
			}
			for (list<HyperOp*>::iterator conflictItr = conflictingContextFrameHyperOps.begin(); conflictItr != conflictingContextFrameHyperOps.end(); conflictItr++) {
				HyperOp* conflictingHyperOp = *conflictItr;
				if (conflictingHyperOp != topmostHyperOp && !conflictingHyperOp->isStaticHyperOp()) {
					conflictingHyperOp->setFbindRequired(true);
				}
				if (conflictingHyperOp == bottommostHyperOp && !conflictingHyperOp->isStartHyperOp()) {
					conflictingHyperOp->setFrameNeedsGC(true);
				}
			}
		}
	}
}
void HyperOpInteractionGraph::minimizeControlEdges() {
	//Remove multiple control edges between HyperOps first
	for (list<HyperOp*>::iterator vertexItr = Vertices.begin(); vertexItr != Vertices.end(); vertexItr++) {
		HyperOp* vertex = *vertexItr;
		list<HyperOp*> children = vertex->getChildList();
		for (list<HyperOp*>::iterator childItr = children.begin(); childItr != children.end(); childItr++) {
			HyperOp* childVertex = *childItr;
			list<HyperOpEdge*> orderingEdges;
			bool hasIncomingDataOrControlEdge = false;
			//If there are multiple edges between the vertex and childVertex
			for (map<HyperOpEdge*, HyperOp*>::iterator childEdgeItr = vertex->ChildMap.begin(); childEdgeItr != vertex->ChildMap.end(); childEdgeItr++) {
				if (childEdgeItr->second == childVertex) {
					HyperOpEdge* edge = childEdgeItr->first;
					if (edge->getType() == HyperOpEdge::ORDERING) {
						orderingEdges.push_back(edge);
					} else {
						hasIncomingDataOrControlEdge = true;
					}
				}
			}

			if (hasIncomingDataOrControlEdge) {
				//Remove the ordering edges since they were only introduced for clustering and have no reason to be here
				//This isn't necessary since ordering edges will not be used to generate writecmp instructions anyway, but retaining this for readability of the graph
				for (list<HyperOpEdge*>::iterator orderingEdgeItr = orderingEdges.begin(); orderingEdgeItr != orderingEdges.end(); orderingEdgeItr++) {
					vertex->removeChildEdge(*orderingEdgeItr);
					childVertex->removeParentEdge(*orderingEdgeItr);
				}
			} else if (orderingEdges.size() > 1) {
				//Retain only one ordering edge between source and destination vertices
				orderingEdges.pop_front();
				for (list<HyperOpEdge*>::iterator orderingEdgeItr = orderingEdges.begin(); orderingEdgeItr != orderingEdges.end(); orderingEdgeItr++) {
					vertex->removeChildEdge(*orderingEdgeItr);
					childVertex->removeParentEdge(*orderingEdgeItr);
				}
			}
		}

	}
}

HyperOp * HyperOpInteractionGraph::getHyperOp(Function* F) {
	for (list<HyperOp*>::iterator vertexItr = Vertices.begin(); vertexItr != Vertices.end(); vertexItr++) {
		//TODO comparing names because metadata parser gives function references that don't match with cached references, could remove the function reference in the object completely later
		if ((*vertexItr)->getFunction()->getName().compare(F->getName()) == 0) {
			return (*vertexItr);
		}
	}
	return 0;
}
void HyperOpInteractionGraph::print(raw_ostream &os) {
	os << "digraph{\n";
	if (!this->Vertices.empty()) {
		for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
			HyperOp* vertex = *vertexIterator;
			os << vertex->getFunction()->getName() << "[label=\"Name:" << vertex->getFunction()->getName() << ",";
			string dom, postdom;
			if ((*vertexIterator)->getImmediateDominator() != 0) {
				dom = (*vertexIterator)->getImmediateDominator()->getFunction()->getName();
			} else {
				dom = "NULL";
			}
			if ((*vertexIterator)->getImmediatePostDominator() != 0) {
				postdom = (*vertexIterator)->getImmediatePostDominator()->getFunction()->getName();
			} else {
				postdom = "NULL";
			}

//			os << "Dom:" << dom << ", PostDom:" << postdom << ",";
			os << "Map:" << ((*vertexIterator)->getTargetResource() / columnCount) << ":" << ((*vertexIterator)->getTargetResource() % columnCount) << ", Context frame:" << (*vertexIterator)->getContextFrame() << ",";
//			os << "Domf:";
//			if (!vertex->getDominanceFrontier().empty()) {
//				list<HyperOp*> domf = vertex->getDominanceFrontier();
//				for (list<HyperOp*>::iterator domfItr = domf.begin(); domfItr != domf.end(); domfItr++) {
//					os << (*domfItr)->getFunction()->getName() << ";";
//				}
//			}
			os << "\"];\n";
			map<HyperOpEdge*, HyperOp*> children = vertex->ChildMap;
			for (map<HyperOpEdge*, HyperOp*>::iterator childItr = children.begin(); childItr != children.end(); childItr++) {
				os << vertex->getFunction()->getName() << "->" << childItr->second->getFunction()->getName() << "[label=";
				HyperOpEdge* edge = (*childItr).first;
				if (edge->Type == HyperOpEdge::SCALAR) {
					os << "scalar:";
//					edge->getValue()->print(os);
				} else if (edge->Type == HyperOpEdge::LOCAL_REFERENCE) {
					os << "localref:";
//					edge->getValue()->print(os);
				} else if (edge->Type == HyperOpEdge::CONTEXT_FRAME_ADDRESS) {
					os << "context frame address";
				} else if (edge->Type == HyperOpEdge::PREDICATE) {
					os << "control:";
//					if (edge->getValue() != 0) {
//						edge->getValue()->print(os);
//					} else {
//						os << "order";
//					}
				} else if (edge->Type == HyperOpEdge::ORDERING) {
					os << "order";
				}
				os << "];\n";
			}
		}
	}
	os << "}\n";
}
