/*
 * HyperOpInteractionGraph.cpp
 *
 *  Created on: 02-Jul-2015
 *      Author: kavitha
 */

#include <cassert>
#include <cstdlib>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/HyperOpInteractionGraph.h"
#include "llvm/ADT/StringExtras.h"
using namespace llvm;

using namespace std;
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/Debug.h"
using namespace llvm;

#include "scotch.h"
//Returns size of the type in bytes
//Duplicate cos utils can't be added as header
unsigned duplicateGetSizeOfType(Type * type) {
	Type* objectType = type;
	if (isa<PointerType>(type)) {
		objectType = ((PointerType*) type)->getPointerElementType();
	}
	if (objectType->isAggregateType()) {
		unsigned size = 0;
		for (unsigned i = 0; i < objectType->getNumContainedTypes(); i++) {
			size += duplicateGetSizeOfType(objectType->getContainedType(i));
			objectType->getContainedType(i)->dump();
		}
		unsigned numElements = 1;
		if (objectType->isArrayTy()) {
			numElements = objectType->getArrayNumElements();
		}
		return size * numElements;
	}
	return (32 / 8);
}


//Returns predicate from start hyperop
list<pair<HyperOpEdge*, HyperOp*> > lastPredicateInput(HyperOp * currentHyperOp) {
//Now we find the shortest predicate chain
	list<pair<HyperOpEdge*, HyperOp*> > reachingPredicateChain;
	while (!currentHyperOp->isStartHyperOp()) {
		HyperOp* immediateDominator = currentHyperOp->getImmediateDominator();
//Check if the current HyperOp is an immediate child of the immediateDom HyperOp
		list<HyperOpEdge*> parentEdgeList;
		for (auto childItr = immediateDominator->ChildMap.begin(); childItr != immediateDominator->ChildMap.end(); childItr++) {
			if (childItr->second == currentHyperOp) {
				parentEdgeList.push_back(childItr->first);
			}
		}

		for (auto parentEdgeItr = parentEdgeList.begin(); parentEdgeItr != parentEdgeList.end(); parentEdgeItr++) {
			if ((*parentEdgeItr)->getType() == HyperOpEdge::PREDICATE) {
				reachingPredicateChain.push_back(make_pair((*parentEdgeItr), immediateDominator));
				break;
			}
		}
		currentHyperOp = immediateDominator;
	}

	return reachingPredicateChain;
}

//Returns predicate from the argument parent hyperop
list<pair<HyperOpEdge*, HyperOp*> > lastPredicateInputUptoParent(HyperOp * currentHyperOp, HyperOp * parentHyperOp) {
//Now we find the shortest predicate chain
	list<pair<HyperOpEdge*, HyperOp*> > reachingPredicateChain;
	while (currentHyperOp != parentHyperOp) {
		HyperOp* immediateDominator = currentHyperOp->getImmediateDominator();
//Check if the current HyperOp is an immediate child of the immediateDom HyperOp
		list<HyperOpEdge*> parentEdgeList;
		for (auto childItr = immediateDominator->ChildMap.begin(); childItr != immediateDominator->ChildMap.end(); childItr++) {
			if (childItr->second == currentHyperOp) {
				parentEdgeList.push_back(childItr->first);
			}
		}

		for (auto parentEdgeItr = parentEdgeList.begin(); parentEdgeItr != parentEdgeList.end(); parentEdgeItr++) {
			if ((*parentEdgeItr)->getType() == HyperOpEdge::PREDICATE) {
				reachingPredicateChain.push_back(make_pair((*parentEdgeItr), immediateDominator));
				break;
			}
		}
		currentHyperOp = immediateDominator;
	}

	return reachingPredicateChain;
}


list<list<pair<HyperOpEdge*, HyperOp*> > > mergePredicateChains(list<list<pair<HyperOpEdge*, HyperOp*> > > predicateChains) {
//Start merging edges
	bool change = true;
	while (change) {
		change = false;
		list<list<pair<HyperOpEdge*, HyperOp*> > > removalList;
		list<list<pair<HyperOpEdge*, HyperOp*> > > additionList;
//remove duplicate predicate chains
		int i = 0;
		for (list<list<pair<HyperOpEdge*, HyperOp*> > >::iterator predicateChainItr = predicateChains.begin(); predicateChainItr != predicateChains.end(); predicateChainItr++, i++) {
			if (*predicateChainItr != predicateChains.back()) {
				int j = i + 1;
				list<list<pair<HyperOpEdge*, HyperOp*> > >::iterator secondPredicateChainItr = predicateChainItr;
				secondPredicateChainItr++;
				//Check if the predicate chains are the same and mark duplicates for removal
				for (; secondPredicateChainItr != predicateChains.end(); secondPredicateChainItr++, j++) {
					if (predicateChainItr->size() != secondPredicateChainItr->size()) {
						continue;
					}
					//Check if the rest of the predicate chain matches
					list<pair<HyperOpEdge*, HyperOp*> >::reverse_iterator secondChainItr = secondPredicateChainItr->rbegin();
					bool chainMatches = true;
					for (list<pair<HyperOpEdge*, HyperOp*> >::reverse_iterator firstChainItr = predicateChainItr->rbegin(); firstChainItr != predicateChainItr->rend() && secondChainItr != secondPredicateChainItr->rend(); firstChainItr++, secondChainItr++) {
						if (firstChainItr->first->getValue() != secondChainItr->first->getValue() || firstChainItr->first->getPredicateValue() != secondChainItr->first->getPredicateValue()) {
							chainMatches = false;
							break;
						}
					}

					if (chainMatches && find(removalList.begin(), removalList.end(), *secondPredicateChainItr) == removalList.end()) {
						removalList.push_back(*secondPredicateChainItr);
					}
				}
			}
		}
		unsigned previousPredChainSize = predicateChains.size();
		for (list<list<pair<HyperOpEdge*, HyperOp*> > >::iterator removalItr = removalList.begin(); removalItr != removalList.end(); removalItr++) {
			predicateChains.remove(*removalItr);
		}

		if (!removalList.empty()) {
			change = true;
		}
		removalList.clear();

		i = 0;
		for (list<list<pair<HyperOpEdge*, HyperOp*> > >::iterator predicateChainItr = predicateChains.begin(); predicateChainItr != predicateChains.end(); predicateChainItr++, i++) {
			if (*predicateChainItr != predicateChains.back()) {
				int j = i + 1;
				list<list<pair<HyperOpEdge*, HyperOp*> > >::iterator secondPredicateChainItr = predicateChainItr;
				secondPredicateChainItr++;
				for (; secondPredicateChainItr != predicateChains.end(); secondPredicateChainItr++, j++) {
					//Check if the predicate chains match
					list<pair<HyperOpEdge*, HyperOp*> > shorterChain;
					list<pair<HyperOpEdge*, HyperOp*> >::reverse_iterator secondChainItr = secondPredicateChainItr->rbegin();
					for (list<pair<HyperOpEdge*, HyperOp*> >::reverse_iterator firstChainItr = predicateChainItr->rbegin(); firstChainItr != predicateChainItr->rend() && secondChainItr != secondPredicateChainItr->rend();) {
						if (*firstChainItr == predicateChainItr->front() && *secondChainItr != secondPredicateChainItr->front()) {
							shorterChain = *predicateChainItr;
							break;
						}
						if (*secondChainItr == secondPredicateChainItr->front() && *firstChainItr != predicateChainItr->front()) {
							shorterChain = *secondPredicateChainItr;
							break;
						}
						if (firstChainItr->first->getValue() != secondChainItr->first->getValue() || firstChainItr->first->getPredicateValue() != secondChainItr->first->getPredicateValue()) {
							break;
						}
						firstChainItr++;
						secondChainItr++;
					}
					if (!shorterChain.empty() && find(removalList.begin(), removalList.end(), shorterChain) == removalList.end()) {
						removalList.push_back(shorterChain);
					}
				}

			}
		}

		previousPredChainSize = predicateChains.size();
		for (list<list<pair<HyperOpEdge*, HyperOp*> > >::iterator removalItr = removalList.begin(); removalItr != removalList.end(); removalItr++) {
			predicateChains.remove(*removalItr);
		}

		if (!removalList.empty()) {
			change = true;
		}
		removalList.clear();

//		unsigned chainCount = 0;
//		for (auto chainItr = predicateChains.begin(); chainItr != predicateChains.end(); chainItr++, chainCount++) {
//			errs() << "\nchain " << chainCount << ":";
//			for (auto chainContentsItr = chainItr->rbegin(); chainContentsItr != chainItr->rend(); chainContentsItr++) {
//				errs() << chainContentsItr->second->asString() << "(";
//				chainContentsItr->first->getValue()->print(errs());
//				errs() << ":" << chainContentsItr->first->getPredicateValue() << ")->";
//			}
//			errs() << "\n---------------------------------------------------------------------------------------------------------\n";
//		}

		assert(predicateChains.size() <= previousPredChainSize && "Removal of duplicate chains resulted in inconsistent number of chains remaining\n");

//		errs() << "number of chains before finding prefixes:" << predicateChains.size() << "\n";
//Check if the predicate chains are mutually exclusive
		unsigned one = 0;
		for (list<list<pair<HyperOpEdge*, HyperOp*> > >::iterator firstPredicateChainItr = predicateChains.begin(); firstPredicateChainItr != predicateChains.end(); firstPredicateChainItr++, one++) {
			if (*firstPredicateChainItr != predicateChains.back()) {
				list<list<pair<HyperOpEdge*, HyperOp*> > >::iterator secondPredicateChainItr = firstPredicateChainItr;
				secondPredicateChainItr++;
				unsigned two = one + 1;
				for (; secondPredicateChainItr != predicateChains.end(); secondPredicateChainItr++, two++) {
					auto secondPredItr = secondPredicateChainItr->rbegin();
					auto firstPredItr = firstPredicateChainItr->rbegin();
					bool divergentPaths = false;
					bool atleastOneControlEdgeInChains = false;
					for (; firstPredItr != firstPredicateChainItr->rend() && secondPredItr != secondPredicateChainItr->rend();) {
						if (firstPredItr->first->getType() == HyperOpEdge::PREDICATE || secondPredItr->first->getType() == HyperOpEdge::PREDICATE) {
							atleastOneControlEdgeInChains = true;
						}
						if (firstPredItr->first->getType() == HyperOpEdge::PREDICATE && secondPredItr->first->getType() == HyperOpEdge::PREDICATE && firstPredItr->first->getValue() == secondPredItr->first->getValue()) {
							if (firstPredItr->first->getPredicateValue() != secondPredItr->first->getPredicateValue()) {
								divergentPaths = true;
								break;
							}
							if (*firstPredItr == firstPredicateChainItr->front() || *secondPredItr == secondPredicateChainItr->front()) {
								break;
							}
							firstPredItr++;
							secondPredItr++;
						} else {
							//Same predicate being delivered elsewhere on a different branch of the HIG tree
							if (*firstPredItr == firstPredicateChainItr->front() || *secondPredItr == secondPredicateChainItr->front() || firstPredItr->second != secondPredItr->second) {
								break;
							}

							if (firstPredItr->first->getType() != HyperOpEdge::PREDICATE) {
								firstPredItr++;
							}
							if (secondPredItr->first->getType() != HyperOpEdge::PREDICATE) {
								secondPredItr++;
							}
						}
					}
					//first and second predicate iterators now point to where they diverge in control
					if (divergentPaths || (!atleastOneControlEdgeInChains && (firstPredItr == firstPredicateChainItr->rend() || secondPredItr == secondPredicateChainItr->rend()))) {
						//Mark both paths for removal and add the prefix
						list<pair<HyperOpEdge*, HyperOp*> > prefixChain;
						for (auto tempItr = firstPredicateChainItr->rbegin(); tempItr != firstPredItr; tempItr++) {
							prefixChain.push_front(*tempItr);
						}
						if (find(additionList.begin(), additionList.end(), prefixChain) == additionList.end() && !prefixChain.empty()) {
							additionList.push_back(prefixChain);
						}
						if (find(removalList.begin(), removalList.end(), *firstPredicateChainItr) == removalList.end()) {
							removalList.push_back(*firstPredicateChainItr);
						}
						if (find(removalList.begin(), removalList.end(), *secondPredicateChainItr) == removalList.end()) {
							removalList.push_back(*secondPredicateChainItr);
						}
					}
				}
			}
		}

//		errs() << "num chains after pairwise compare:" << predicateChains.size() << ", num removal:" << removalList.size() << ", num addition:" << additionList.size() << "\n";
		previousPredChainSize = predicateChains.size();

		for (list<list<pair<HyperOpEdge*, HyperOp*> > >::iterator removalItr = removalList.begin(); removalItr != removalList.end(); removalItr++) {
			predicateChains.remove(*removalItr);
		}

		for (list<list<pair<HyperOpEdge*, HyperOp*> > >::iterator additionItr = additionList.begin(); additionItr != additionList.end(); additionItr++) {
			predicateChains.push_back(*additionItr);
		}

		if (!removalList.empty()) {
			change = true;
		}

		assert(predicateChains.size() <= previousPredChainSize && "Merge of mutually exclusive chains resulted in inconsistent number of chains remaining\n");
	}
	return predicateChains;
}

bool mutuallyExclusiveHyperOps(HyperOp* firstHyperOp, HyperOp* secondHyperOp) {
	list<HyperOp*> firstHyperOpDomf = firstHyperOp->getDominanceFrontier();
	list<HyperOp*> secondHyperOpDomf = firstHyperOp->getDominanceFrontier();
	if ((firstHyperOp->getImmediateDominator() == NULL && secondHyperOp->getImmediateDominator() != NULL) || (firstHyperOp->getImmediateDominator() != NULL && secondHyperOp->getImmediateDominator() == NULL)) {
		return false;
	}
	if (firstHyperOp->getImmediateDominator() == secondHyperOp->getImmediateDominator()) {
		if (find(firstHyperOpDomf.begin(), firstHyperOpDomf.end(), secondHyperOp) == firstHyperOpDomf.end() && find(secondHyperOpDomf.begin(), secondHyperOpDomf.end(), firstHyperOp) == secondHyperOpDomf.end()) {
			//HyperOps are on different paths
			//Check if the HyperOps are on mutually exclusive predicates at all
			list<list<pair<HyperOpEdge*, HyperOp*> > > predicateChains;
			auto firstHyperOpPredicateChain = lastPredicateInput(firstHyperOp);
			auto secondHyperOpPredicateChain = lastPredicateInput(secondHyperOp);

			predicateChains.push_back(firstHyperOpPredicateChain);
			predicateChains.push_back(secondHyperOpPredicateChain);
			predicateChains = mergePredicateChains(predicateChains);
			if (predicateChains.size() > 1) {
				return false;
			}
			return true;
		}
		return false;
	} else if (firstHyperOp->getImmediateDominator() == secondHyperOp || secondHyperOp->getImmediateDominator() == firstHyperOp) {
		if (find(firstHyperOpDomf.begin(), firstHyperOpDomf.end(), secondHyperOp) != firstHyperOpDomf.end() || find(secondHyperOpDomf.begin(), secondHyperOpDomf.end(), firstHyperOp) != secondHyperOpDomf.end()) {
			return true;
		}
		return false;
	}
	return mutuallyExclusiveHyperOps(firstHyperOp->getImmediateDominator(), secondHyperOp->getImmediateDominator());
}
HyperOp::HyperOp(Function* function, HyperOpInteractionGraph* hig) {
	this->hig = hig;
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
	this->staticHyperOp = true;
	list<SyncValue> zeroPredList;
	this->setIncomingSyncCount(0, zeroPredList);
	list<SyncValue> onePredList;
	this->setIncomingSyncCount(1, onePredList);
	list<SyncValue> twoPredList;
	this->setIncomingSyncCount(2, twoPredList);
	this->unrolledInstance = false;
	this->instanceof = NULL;
	this->InRange = false;
	this->setIncomingSyncPredicate(0, NULL);
	this->setIncomingSyncPredicate(1, NULL);
	this->setHasRangeBaseInput(false);
	this->setUpperBoundScope(NULL);
	this->setLowerBoundScope(NULL);
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

unsigned HyperOpEdge::getPredicateValue() {
	return predicateValue;
}

void HyperOpEdge::setPredicateValue(unsigned predicateValue) {
	this->predicateValue = predicateValue;
}

unsigned HyperOpEdge::getDecrementOperandCount() {
	return decrementOperandCount;
}

void HyperOpEdge::setDecrementOperandCount(unsigned decrementOperandCount) {
	this->decrementOperandCount = decrementOperandCount;
}

void HyperOpEdge::setEdgeSource(MachineInstr* edgeSource) {
	this->edgeSource = edgeSource;
}

MachineInstr* HyperOpEdge::getEdgeSource() {
	return edgeSource;
}

int HyperOpEdge::getPositionOfContextSlot() {
	return positionOfContextSlot;
}

void HyperOpEdge::setPositionOfContextSlot(int positionOfInput) {
	this->positionOfContextSlot = positionOfInput;
}

int HyperOpEdge::getMemoryOffsetInTargetFrame() const {
	return memoryOffset;
}

void HyperOpEdge::setMemoryOffsetInTargetFrame(int memoryOffset) {
	this->memoryOffset = memoryOffset;
}

void HyperOpEdge::setValue(Value* value) {
	this->variable = value;
}
Value* HyperOpEdge::getValue() {
	return variable;
}

unsigned int HyperOp::getContextFrame() const {
	return contextFrame;
}

bool HyperOp::isStaticHyperOp() const {
	return staticHyperOp;
}

void HyperOp::setStaticHyperOp(bool staticHyperOp) {
	this->staticHyperOp = staticHyperOp;
}

//void HyperOp::addSyncSource(unsigned predicateValue) {
////	this->numIncomingSyncEdges[predicateValue].push_back(ConstantInt::get(hyperOp->getFunction()->getParent()->getContext(), APInt(32, 1)));
//}

void HyperOp::addIncomingSyncValue(unsigned predicateValue, SyncValue value) {
	int updatedValue = 0;
//	if (!this->numIncomingSyncEdges[predicateValue].empty()) {
//		Value* prevPred = this->numIncomingSyncEdges[predicateValue].front();
//		updatedValue = ((ConstantInt*) prevPred)->getValue().getZExtValue() + 1;
//		this->numIncomingSyncEdges[predicateValue].pop_front();
//	} else {
//		updatedValue = 1;
//	}
	this->numIncomingSyncEdges[predicateValue].push_back(value);
}

void HyperOp::setIncomingSyncCount(unsigned predicateValue, list<SyncValue> syncCountList) {
	this->numIncomingSyncEdges[predicateValue].clear();
	for (auto syncCount : syncCountList) {
		this->numIncomingSyncEdges[predicateValue].push_back(syncCount);
	}
}

void HyperOp::setIncomingSyncPredicate(unsigned predicateValue, Value* predicate) {
	this->predicateForSyncSource[predicateValue] = predicate;
}

Value* HyperOp::getIncomingSyncPredicate(unsigned predicateValue) {
	return this->predicateForSyncSource[predicateValue];
}

void HyperOp::setUpperBoundScope(HyperOp* scope) {
	this->upperBoundScope = scope;
}
void HyperOp::setLowerBoundScope(HyperOp* scope) {
	this->upperBoundScope = scope;
}

HyperOp* HyperOp::getUpperBoundScope() {
	return this->upperBoundScope;
}
HyperOp* HyperOp::getLowerBoundScope() {
	return this->lowerBoundScope;
}

HyperOpInteractionGraph* HyperOp::getParentGraph() {
	return hig;
}

list<SyncValue> HyperOp::getSyncCount(unsigned predicateValue) {
	return this->numIncomingSyncEdges[predicateValue];
}

list<unsigned> HyperOp::getInstanceId() {
	return instanceId;
}

void HyperOp::setInstanceId(list<unsigned> instanceId) {
	this->instanceId = instanceId;
}
unsigned HyperOp::getHyperOpId() const {
	return hyperOpId;
}

Function* HyperOp::getInstanceof() {
	return instanceof;
}

void HyperOp::setInstanceof(Function* instanceof) {
	this->instanceof = instanceof;
}

string HyperOp::asString(bool ignorePeriod) {
	stringstream retVal;
	if (isStaticHyperOp()) {
		retVal << function->getName().data();
	} else if (instanceof != NULL) {
		retVal << function->getName().data() << instanceof->getName().data();
		for (list<unsigned>::iterator idItr = instanceId.begin(); idItr != instanceId.end(); idItr++) {
			retVal << (*idItr);
		}
	}
	if (ignorePeriod) {
		string str = retVal.str();
		str.erase(std::remove(str.begin(), str.end(), '.'), str.end());
		return str;
	} else {
		return retVal.str();
	}
}
bool HyperOp::isUnrolledInstance() {
	return unrolledInstance;
}

void HyperOp::setIsUnrolledInstance(bool isUnrolledInstance) {
	this->unrolledInstance = isUnrolledInstance;
}

void HyperOp::setFunction(Function* function) {
	this->function = function;
}

PHyperOpInteractionGraph HyperOp::getpHyperOpDependenceMap() {
	return this->pHopDependenceMap;
}

void HyperOp::setpHyperOpDependenceMap(PHyperOpInteractionGraph dependenceMap) {
	this->pHopDependenceMap = dependenceMap;
}

bool HyperOp::isHasMutexSyncSources() const {
	return hasMutexSyncSources;
}

void HyperOp::setHasMutexSyncSources(bool hasMutexSyncSources) {
	this->hasMutexSyncSources = hasMutexSyncSources;
}

void HyperOp::setHyperOpId(unsigned hyperOpId) {
	this->hyperOpId = hyperOpId;
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
	this->setBarrierHyperOp();
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

void HyperOp::setPredicatedHyperOp(bool value) {
	this->IsPredicated = value;
}

void HyperOp::setInRange() {
	this->InRange = true;
}
bool HyperOp::getInRange() {
	return this->InRange;
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

list<unsigned int> HyperOp::getTopLevel() {
	return this->topLevel;
}

StrideFunction HyperOp::getInductionVarUpdateFunc() {
	return inductionVarUpdateFunc;
}

void HyperOp::setInductionVarUpdateFunc(StrideFunction inductionVarUpdateFunc) {
	this->inductionVarUpdateFunc = inductionVarUpdateFunc;
}

Value* HyperOp::getRangeUpperBound() {
	return rangeUpperBound;
}

void HyperOp::setRangeUpperBound(Value* rangeUpperBound) {
	this->rangeUpperBound = rangeUpperBound;
}

Value* HyperOp::getRangeLowerBound() {
	return rangeLowerBound;
}

void HyperOp::setRangeLowerBound(Value* rangeLowerBound) {
	this->rangeLowerBound = rangeLowerBound;
}

Value* HyperOp::getStride() {
	return stride;
}

void HyperOp::setStride(Value* stride) {
	this->stride = stride;
}

void HyperOp::setHasRangeBaseInput(bool value) {
	this->rangeBaseInput = value;
}

bool HyperOp::hasRangeBaseInput() {
	return rangeBaseInput;
}

HyperOpEdge::HyperOpEdge() {
	this->Type = SCALAR;
	this->isZeroedOut = false;
	this->isIgnoredEdge = false;
	this->positionOfContextSlot = -1;
	this->variable = 0;
	this->decrementOperandCount = 0;
	this->predicateValue = 0;
	this->multiplicity = "";
	this->memorySize = 0;
	this->memoryOffset = 0;
}
HyperOpEdge::~HyperOpEdge() {

}

void HyperOpEdge::clone(HyperOpEdge** clone) {
	HyperOpEdge* newEdge = new HyperOpEdge();
	newEdge->setIsEdgeIgnored(this->isEdgeIgnored());
	newEdge->setType(this->getType());
	newEdge->zeroOutEdge(this->isEdgeZeroedOut());
	newEdge->setPositionOfContextSlot(this->getPositionOfContextSlot());
	newEdge->setPredicateValue(this->getPredicateValue());
	newEdge->setDecrementOperandCount(this->getDecrementOperandCount());
	newEdge->setEdgeSource(this->getEdgeSource());
	newEdge->setMemoryOffsetInTargetFrame(this->getMemoryOffsetInTargetFrame());
	newEdge->setValue(this->getValue());
	newEdge->setContextFrameAddress(this->getContextFrameAddress());
	*clone = newEdge;
}

void HyperOpEdge::setMemorySize(int memorySize) {
	this->memorySize = memorySize;
}
void HyperOpEdge::setMultiplicity(string multiplicity){
	this->multiplicity = multiplicity;
}
string HyperOpEdge::getMultiplicity(){
	return multiplicity;
}

int HyperOpEdge::getMemorySize() {
	return this->memorySize;
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

SyncValue::SyncValue(HyperOp* rangeHyperOpValue) {
	this->syncVal.rangeHyperOpValue = rangeHyperOpValue;
	this->type = SyncValueType::HYPEROP_SYNC_TYPE;
}
SyncValue::SyncValue(int intValue) {
	this->syncVal.intValue = intValue;
	this->type = SyncValueType::INT_SYNC_TYPE;
}

HyperOp* SyncValue::getHyperOp() {
	assert(this->type == SyncValueType::HYPEROP_SYNC_TYPE);
	return this->syncVal.rangeHyperOpValue;
}
int SyncValue::getInt() {
	assert(this->type == SyncValueType::INT_SYNC_TYPE);
	return this->syncVal.intValue;
}

SyncValueType SyncValue::getType() {
	return this->type;
}
HyperOpInteractionGraph::HyperOpInteractionGraph() {
	columnCount = 1;
	rowCount = 1;
	StridedFunctionKeyValue.insert(make_pair("add", ADD));
//	StridedFunctionKeyValue.insert(make_pair("mul", MUL));
//	StridedFunctionKeyValue.insert(make_pair("sub", SUB));
//	StridedFunctionKeyValue.insert(make_pair("div", DIV));
//	StridedFunctionKeyValue.insert(make_pair("mod", MOD));
}

HyperOpInteractionGraph::~HyperOpInteractionGraph() {
}

void HyperOpInteractionGraph::setDimensions(unsigned int rowCount, unsigned int columnCount) {
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

HyperOp* HyperOpInteractionGraph::getOrCreateHyperOpInstance(Function* function, Function* instanceOf, list<unsigned> instanceId, bool create) {
	for (list<HyperOp*>::iterator vertexItr = Vertices.begin(); vertexItr != Vertices.end(); vertexItr++) {
		if ((*vertexItr)->getInstanceof() == instanceOf) {
			list<unsigned> originalId = (*vertexItr)->getInstanceId();
			if (originalId.size() == instanceId.size()) {
				list<unsigned>::iterator originalIdItr = originalId.begin();
				bool match = true;
				for (list<unsigned>::iterator idItr = instanceId.begin(); idItr != instanceId.end() && originalIdItr != originalId.end(); idItr++, originalIdItr++) {
					if (*idItr != *originalIdItr) {
						match = false;
						break;
					}

				}
				if (match) {
					return (*vertexItr);
				}
			}
		}
	}
	HyperOp* newHyperOp = NULL;
	if (create) {
		newHyperOp = new HyperOp(function, this);
		newHyperOp->setHyperOpId(this->getHyperOp(function)->getHyperOpId());
		newHyperOp->setInstanceof(instanceOf);
		newHyperOp->setInstanceId(instanceId);
		newHyperOp->setStaticHyperOp(false);
		newHyperOp->setIsUnrolledInstance(true);
		this->addHyperOp(newHyperOp);
	}
	return newHyperOp;
}

list<TileCoordinates> HyperOpInteractionGraph::getEdgePathOnNetwork(HyperOp* source, HyperOp* target) {
	list<TileCoordinates> path;
	TileCoordinates sourceCoordinates = make_pair(source->getTargetResource() / columnCount, source->getTargetResource() % columnCount);
	TileCoordinates targetCoordinates = make_pair(source->getTargetResource() / columnCount, source->getTargetResource() % columnCount);
//Set path along column
	unsigned col;
//	errs() << "col from " << sourceCoordinates.second << " to " << targetCoordinates.second << "\n";
	if (columnCount == 1) {
		path.push_back(make_pair(sourceCoordinates.first, 0));
	} else {
		if (fabs(sourceCoordinates.second - targetCoordinates.second) < columnCount / 2) {
			for (col = sourceCoordinates.second; col != (targetCoordinates.second + 1); col = (col + 1) % columnCount) {
				path.push_back(make_pair(sourceCoordinates.first, col));
			}
		} else {
			for (col = sourceCoordinates.second; col != (targetCoordinates.second - 1); col = (col - 1) % columnCount) {
				path.push_back(make_pair(sourceCoordinates.first, col));
				if (col == 0) {
					col = col + columnCount;
				}
			}
		}
	}
	if (rowCount > 1) {
//		errs() << "row from " << sourceCoordinates.first << " to " << targetCoordinates.first << "\n";
		if (fabs(sourceCoordinates.first - targetCoordinates.first) < columnCount / 2) {
			for (unsigned row = sourceCoordinates.first; row != (targetCoordinates.first + 1); row = (row + 1) % rowCount) {
				path.push_back(make_pair(row, col));
			}
		} else {
			for (unsigned row = sourceCoordinates.first; row != (targetCoordinates.first - 1); row = (row - 1) % rowCount) {
				path.push_back(make_pair(row, col));
				if (row == 0) {
					row = row + rowCount;
				}
			}
		}
	}
	return path;
}

void HyperOpInteractionGraph::updateLocalRefEdgeMemSizeAndOffset() {
	DEBUG(dbgs() << "Updating localref edges with the correct offset\n");
	for (auto vertexItr = this->Vertices.begin(); vertexItr != this->Vertices.end(); vertexItr++) {
		list<HyperOpEdge*> edgeProcessingOrder;
		HyperOp* hyperOp = *vertexItr;
		for (auto parentEdgeItr = (*vertexItr)->ParentMap.begin(); parentEdgeItr != (*vertexItr)->ParentMap.end(); parentEdgeItr++) {
			if(parentEdgeItr->second->isUnrolledInstance()){
				continue;
			}
			if (parentEdgeItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF || parentEdgeItr->first->getType() == HyperOpEdge::LOCAL_REFERENCE || parentEdgeItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_BASE_LOCALREF) {
				HyperOpEdge* edge = parentEdgeItr->first;
				AllocaInst* originalEdgeSource = getAllocInstrForLocalReferenceData(edge->getValue(), parentEdgeItr->second);
				int edgeSize = duplicateGetSizeOfType(originalEdgeSource->getAllocatedType());
				edge->setMemorySize(edgeSize);
				edgeProcessingOrder.push_back(edge);
			}
		}
//		Compute the size of each edge before the current edge
		for (auto edge : edgeProcessingOrder) {
			unsigned edgeOffset = 0;
			for (auto secondEdgeItr : edgeProcessingOrder) {
				if (secondEdgeItr == edge || secondEdgeItr->getPositionOfContextSlot() > edge->getPositionOfContextSlot()) {
					continue;
				}
				edgeOffset += secondEdgeItr->getMemorySize();
			}
			edge->setMemoryOffsetInTargetFrame(edgeOffset);
		}
	}

	for (auto vertexItr = this->Vertices.begin(); vertexItr != this->Vertices.end(); vertexItr++) {
		list<HyperOpEdge*> edgeProcessingOrder;
		HyperOp* hyperOp = *vertexItr;
		for (auto parentEdgeItr = (*vertexItr)->ParentMap.begin(); parentEdgeItr != (*vertexItr)->ParentMap.end(); parentEdgeItr++) {
			if (!parentEdgeItr->second->isUnrolledInstance()) {
				continue;
			}
			HyperOp* instanceOfHop = this->getHyperOp(hyperOp->getInstanceof());
			for (auto incomingEdgeItr : instanceOfHop->ParentMap) {
				if (incomingEdgeItr.first->getPositionOfContextSlot() == parentEdgeItr->first->getPositionOfContextSlot()) {
					parentEdgeItr->first->setMemorySize(incomingEdgeItr.first->getMemorySize());
					parentEdgeItr->first->setMemoryOffsetInTargetFrame(incomingEdgeItr.first->getMemoryOffsetInTargetFrame());
					break;
				}
			}
		}
	}
}

void HyperOpInteractionGraph::updateContextFrameForwardingEdges() {
	DEBUG(dbgs() << "Updating context frame address forwarding edges with the right value\n");
	for (auto vertexItr = this->Vertices.begin(); vertexItr != this->Vertices.end(); vertexItr++) {
		HyperOp* hop = *vertexItr;
		for (auto childItr = hop->ChildMap.begin(); childItr != hop->ChildMap.end(); childItr++) {
			if ((childItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_LOWER ||  childItr->first->getType() == HyperOpEdge:: CONTEXT_FRAME_ADDRESS_RANGE_COUNT_UPPER || childItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || childItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF) && childItr->first->getContextFrameAddress()->getImmediateDominator() != hop) {
				int argIndex = -1;
				int lowerboundArgIndex = -1;
				int upperboundArgIndex = -1;
				for (auto parentItr = hop->ParentMap.begin(); parentItr != hop->ParentMap.end(); parentItr++) {
					if (parentItr->first->getContextFrameAddress() == childItr->first->getContextFrameAddress()) {
						if(parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || parentItr->first->getType() ==  HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF){
							argIndex = parentItr->first->getPositionOfContextSlot();
						}else if(parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_LOWER){
							lowerboundArgIndex = parentItr->first->getPositionOfContextSlot();
						}else if(parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_UPPER){
							upperboundArgIndex = parentItr->first->getPositionOfContextSlot();
						}
					}
				}
				assert(argIndex >= 1 && "context frame address not forwarded\n");
				int index = 0;
				for(auto argItr = hop->getFunction()->arg_begin(); argItr!=hop->getFunction()->arg_end(); argItr++, index++){
					if(index == argIndex){
						childItr->first->setValue(argItr);
						break;
					}
					if(index == lowerboundArgIndex){
						childItr->first->setValue(argItr);
					}
					if(index == upperboundArgIndex){
						childItr->first->setValue(argItr);
					}
				}
			}
		}
	}
}

void HyperOpInteractionGraph::addHyperOp(HyperOp *Vertex) {
	this->Vertices.push_back(Vertex);
}

void HyperOpInteractionGraph::removeHyperOp(HyperOp * vertex) {
//Find the parent edges that need to be marked for removal
	for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = vertex->ParentMap.begin(); parentItr != vertex->ParentMap.end(); parentItr++) {
		parentItr->second->removeChildEdge(parentItr->first);
	}

	for (map<HyperOpEdge*, HyperOp*>::iterator childItr = vertex->ChildMap.begin(); childItr != vertex->ChildMap.end(); childItr++) {
		childItr->second->removeParentEdge(childItr->first);
	}

	Vertices.remove(vertex);
}

void HyperOpInteractionGraph::addEdge(HyperOp* SourceVertex, HyperOp * TargetVertex, HyperOpEdge * Edge) {
	TargetVertex->addParentEdge(Edge, SourceVertex);
	SourceVertex->addChildEdge(Edge, TargetVertex);
}

//Returns control flow graph from the cdfg we use, this graph is used to compute mutual exclusion etc
pair<HyperOpInteractionGraph*, map<HyperOp*, HyperOp*> > getCFG(HyperOpInteractionGraph * dfg) {
	HyperOpInteractionGraph* cfg = new HyperOpInteractionGraph();
	cfg->setDimensions(dfg->rowCount, dfg->columnCount);
	map<HyperOp*, HyperOp*> originalToClonedNodesMap;
	for (list<HyperOp*>::iterator vertexItr = dfg->Vertices.begin(); vertexItr != dfg->Vertices.end(); vertexItr++) {
		//Create a new Vertex for each original
		HyperOp* newVertex = new HyperOp((*vertexItr)->getFunction(), dfg);
		newVertex->setStaticHyperOp((*vertexItr)->isStaticHyperOp());
		newVertex->setInstanceof((*vertexItr)->getInstanceof());
		newVertex->setHyperOpId((*vertexItr)->getHyperOpId());
		newVertex->setInstanceId((*vertexItr)->getInstanceId());
		if ((*vertexItr)->isPredicatedHyperOp()) {
			newVertex->setPredicatedHyperOp();
		}
		if ((*vertexItr)->isBarrierHyperOp()) {
			newVertex->setBarrierHyperOp();
		}
		if ((*vertexItr)->isStartHyperOp()) {
			newVertex->setStartHyperOp();
		}
		if ((*vertexItr)->isEndHyperOp()) {
			newVertex->setEndHyperOp();
		}
		cfg->addHyperOp(newVertex);
		originalToClonedNodesMap[*vertexItr] = newVertex;
	}

//Copy only predicate and sync edges
	for (list<HyperOp*>::iterator vertexItr = dfg->Vertices.begin(); vertexItr != dfg->Vertices.end(); vertexItr++) {
		HyperOp* targetHop = originalToClonedNodesMap[*vertexItr];
		for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = (*vertexItr)->ParentMap.begin(); parentItr != (*vertexItr)->ParentMap.end(); parentItr++) {
			HyperOp* sourceHop = originalToClonedNodesMap[parentItr->second];
			if (parentItr->first->getType() == HyperOpEdge::PREDICATE || parentItr->first->getType() == HyperOpEdge::SYNC) {
				HyperOpEdge* edge = new HyperOpEdge();
				edge->Type = parentItr->first->getType();
				edge->setVolume(parentItr->first->getVolume());
				edge->setValue(parentItr->first->getValue());
				edge->setPredicateValue(parentItr->first->getPredicateValue());
				edge->setContextFrameAddress(parentItr->first->getContextFrameAddress());
				edge->setPositionOfContextSlot(parentItr->first->getPositionOfContextSlot());
				targetHop->addParentEdge(edge, sourceHop);
				sourceHop->addChildEdge(edge, targetHop);
			}
		}
	}

//Check if there are hanging hyperops
	for (list<HyperOp*>::iterator vertexItr = dfg->Vertices.begin(); vertexItr != dfg->Vertices.end(); vertexItr++) {
		HyperOp* targetHop = originalToClonedNodesMap[*vertexItr];
		if (!targetHop->isStartHyperOp() && targetHop->ParentMap.empty()) {
			auto parentItr = (*vertexItr)->ParentMap.begin();
			HyperOp* sourceHop = originalToClonedNodesMap[parentItr->second];
			HyperOpEdge* edge;
			parentItr->first->clone(&edge);
			edge->setContextFrameAddress(NULL);
			//Added to ensure that 0th context address is set to the edge, to avoid patching the dot file
			cfg->addEdge(sourceHop, targetHop, edge);
		}
		if (!targetHop->isEndHyperOp() && targetHop->ChildMap.empty()) {
			auto childItr = (*vertexItr)->ChildMap.begin();
			HyperOp* sourceHop = originalToClonedNodesMap[childItr->second];
			HyperOpEdge* edge;
			childItr->first->clone(&edge);
			cfg->addEdge(targetHop, sourceHop, edge);
		}
	}
	return make_pair(cfg, originalToClonedNodesMap);
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
			assert(((*temporaryIdomIterator).second.size() == 1) && "More than one idom of node, aborting\n");
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
					if ((*childItr == vertex || std::find(dominatorOfChild.begin(), dominatorOfChild.end(), vertex) == dominatorOfChild.end()) && find(dominanceFrontier.begin(), dominanceFrontier.end(), *childItr) == dominanceFrontier.end()) {
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
}

//Create a new node between the parentNode and child node and redirect all the direct edges between start and merge through the new join node
void createInterimFunctionToBreakEdge(HyperOp** interimNode, Function** interimFunction, HyperOp* parentNode, HyperOp* childNode) {
	Function* newInterimFunction;
	vector<Type*> funcArgsList;
	Module* M = parentNode->getFunction()->getParent();
	LLVMContext &ctxt = M->getContext();

	vector<Argument*> contextSlotAndArgTypeMap;
	for (auto argItr = childNode->getFunction()->arg_begin(); argItr != childNode->getFunction()->arg_end(); argItr++) {
		contextSlotAndArgTypeMap.push_back(argItr);
	}
	for (auto parentEdgeItr : parentNode->ChildMap) {
		if (parentEdgeItr.second == childNode) {
			funcArgsList.push_back(contextSlotAndArgTypeMap[parentEdgeItr.first->getPositionOfContextSlot()]->getType());
		}
	}
	FunctionType *FT = FunctionType::get(childNode->getFunction()->getReturnType(), funcArgsList, false);
	newInterimFunction = Function::Create(FT, Function::ExternalLinkage, childNode->getFunction()->getName(), childNode->getFunction()->getParent());

	vector<Argument*> contextSlotAndNewArgMap;
	/* Copy attributes of the arguments */
	auto newArgItr = newInterimFunction->arg_begin();
	for (auto parentEdgeItr : parentNode->ChildMap) {
		if (parentEdgeItr.second == childNode) {
			newInterimFunction->setAttributes(childNode->getFunction()->getAttributes().getParamAttributes(contextSlotAndArgTypeMap[parentEdgeItr.first->getPositionOfContextSlot()]->getArgNo()));
			contextSlotAndNewArgMap.push_back(newArgItr);
			newArgItr++;
		}
	}
	BasicBlock* newInterimFunctionBB = BasicBlock::Create(ctxt, "entry", newInterimFunction);

	HyperOp* newInterimNode = new HyperOp(newInterimFunction, childNode->getParentGraph());
	map<HyperOpEdge*, pair<HyperOpEdge*, HyperOpEdge*> > replacementMap;
	int argIndex = 0;
	for (auto edgeItr = parentNode->ChildMap.begin(); edgeItr != parentNode->ChildMap.end(); edgeItr++) {
		if (edgeItr->second == childNode) {
			HyperOpEdge* newParentEdge = new HyperOpEdge();
			edgeItr->first->clone(&newParentEdge);
			newParentEdge->setPositionOfContextSlot(argIndex);
			HyperOpEdge* newChildEdge = new HyperOpEdge();
			edgeItr->first->clone(&newChildEdge);
			newChildEdge->setValue(contextSlotAndNewArgMap[argIndex]);
			newChildEdge->setPositionOfContextSlot(edgeItr->first->getPositionOfContextSlot());
			replacementMap.insert(make_pair(edgeItr->first, make_pair(newParentEdge, newChildEdge)));
			argIndex++;
		}
	}

	for (auto replacementItr : replacementMap) {
		parentNode->removeChildEdge(replacementItr.first);
		childNode->removeParentEdge(replacementItr.first);
		parentNode->addChildEdge(replacementItr.second.first, newInterimNode);
		newInterimNode->addChildEdge(replacementItr.second.second, childNode);
	}

	*interimNode = newInterimNode;
	*interimFunction = newInterimFunction;
}

bool pathExistsInHIG(HyperOp* source, HyperOp* target) {
	if (source == target) {
		return false;
	}
	for (map<HyperOpEdge*, HyperOp*>::iterator childItr = source->ChildMap.begin(); childItr != source->ChildMap.end(); childItr++) {
		if (childItr->second == target || pathExistsInHIG(childItr->second, target)) {
			return true;
		}
	}
	return false;
}

bool pathExistsInHIGExcludingOrderingEdges(HyperOp* source, HyperOp* target) {
	if (source == target) {
		return false;
	}
	for (map<HyperOpEdge*, HyperOp*>::iterator childItr = source->ChildMap.begin(); childItr != source->ChildMap.end(); childItr++) {
		if (childItr->first->getType() != HyperOpEdge::ORDERING && (childItr->second == target || pathExistsInHIG(childItr->second, target))) {
			return true;
		}
	}
	return false;
}

/*
 * Create a new merge node and redirect all the edges coming from siblings of the root node of the subtree that messes stuff up
 * Reroute the edges that go out of the old merge node to go out of the new merge node instead
 */
void createInterimMergeNode(HyperOp** interimNode, Function** interimFunction, HyperOp* violatingNode) {
	Function* newMergeFunction;
	HyperOp* newMergeHyperOp;
	HyperOpInteractionGraph* graph = violatingNode->getParentGraph();
	list<HyperOp*> producerHyperOpsToMergeNode;
	assert(!violatingNode->getImmediatePostDominator()->isPredicatedHyperOp() && "The merge node cannot be a predicated hyperop");
	for (auto parentItr : violatingNode->getImmediatePostDominator()->getParentList()) {
		HyperOp* parentNode = parentItr;
		if (!pathExistsInHIG(violatingNode, parentNode)) {
			continue;
		}
		producerHyperOpsToMergeNode.push_back(parentNode);
	}

	map<HyperOpEdge*, HyperOp*> incomingEdgesToOldMerge;
	list<int> sortedContextSlotsOfEdges;
	// Identify all the edges coming in to the merge node from thethe children of the violating node
	for (auto edgeItr : violatingNode->getImmediatePostDominator()->ParentMap) {
		if (find(producerHyperOpsToMergeNode.begin(), producerHyperOpsToMergeNode.end(), edgeItr.second) != producerHyperOpsToMergeNode.end()) {
			incomingEdgesToOldMerge.insert(make_pair(edgeItr.first, edgeItr.second));
			if (find(sortedContextSlotsOfEdges.begin(), sortedContextSlotsOfEdges.end(), edgeItr.first->getPositionOfContextSlot()) == sortedContextSlotsOfEdges.end()) {
				bool inserted = false;
				for (auto insertItr = sortedContextSlotsOfEdges.begin(); insertItr != sortedContextSlotsOfEdges.end(); insertItr++) {
					if ((*insertItr) > edgeItr.first->getPositionOfContextSlot()) {
						sortedContextSlotsOfEdges.insert(insertItr, edgeItr.first->getPositionOfContextSlot());
						inserted = true;
						break;
					}
				}
				if (!inserted) {
					sortedContextSlotsOfEdges.push_back(edgeItr.first->getPositionOfContextSlot());
				}
			}
		}
	}
	vector<Type*> newfuncArgsList;
	Module *M = violatingNode->getFunction()->getParent();
	LLVMContext &ctxt = M->getContext();
	vector<int> oldToNewSlotMap;
	for (unsigned argIndex = 0; argIndex < sortedContextSlotsOfEdges.size(); argIndex++) {
		for (auto incomingEdgeItr = incomingEdgesToOldMerge.begin(); incomingEdgeItr != incomingEdgesToOldMerge.end(); incomingEdgeItr++) {
			HyperOpEdge* incomingEdge = incomingEdgeItr->first;
			if (incomingEdge->getPositionOfContextSlot() == argIndex) {
				newfuncArgsList.push_back(incomingEdge->getValue()->getType());
				oldToNewSlotMap[incomingEdge->getPositionOfContextSlot()] = argIndex;
				break;
			}
		}
	}
	FunctionType *FT = FunctionType::get(violatingNode->getImmediateDominator()->getFunction()->getReturnType(), newfuncArgsList, false);
	Function *newFunction = Function::Create(FT, Function::ExternalLinkage, violatingNode->getImmediateDominator()->getFunction()->getName(), M);
	newMergeHyperOp = new HyperOp(newMergeFunction, graph);

	vector<Argument*> newMergeArgs;
	int argIndex = 0;
	for (auto argItr = newFunction->arg_begin(); argItr != newFunction->arg_end(); argItr++) {
		newMergeArgs[argIndex++] = argItr;
	}
	/* Add required edges from new merge node to the old merge node */
	HyperOp* oldMergeNode = violatingNode->getImmediatePostDominator();
	for (auto incomingOldNodeMergeItr = incomingEdgesToOldMerge.begin(); incomingOldNodeMergeItr != incomingEdgesToOldMerge.end(); incomingOldNodeMergeItr++) {
		HyperOp* producerHop = incomingOldNodeMergeItr->second;
		producerHop->removeChildEdge(incomingOldNodeMergeItr->first);
		producerHop->addChildEdge(incomingOldNodeMergeItr->first, newMergeHyperOp);
		newMergeHyperOp->addParentEdge(incomingOldNodeMergeItr->first, producerHop);
		/* Add new edge from the new merge node to the old merge node*/

		int slotInNewMerge = oldToNewSlotMap[incomingOldNodeMergeItr->first->getPositionOfContextSlot()];
		HyperOpEdge* newEdge;
		incomingOldNodeMergeItr->first->clone(&newEdge);
		newEdge->setValue(newMergeArgs[slotInNewMerge]);
		newEdge->setPositionOfContextSlot(incomingOldNodeMergeItr->first->getPositionOfContextSlot());
		incomingOldNodeMergeItr->first->setPositionOfContextSlot(slotInNewMerge);
	}

	/* Create a new function */
	*interimNode = newMergeHyperOp;
	*interimFunction = newMergeFunction;
}

//Add nodes to make the HIG structured if necessary
void HyperOpInteractionGraph::makeGraphStructured() {
	bool change = true;
	int i = 0;
	DEBUG(dbgs() << "before making graph structured:");
	this->print(dbgs(), 0);
	while (change) {
		i++;
		change = false;
		//First HyperOp containing the HyperOp that ought to take in all edges instead of the original source
		for (auto vertexItr = Vertices.begin(); vertexItr != Vertices.end(); vertexItr++) {
			HyperOp* vertex = *vertexItr;
			if (vertex->getChildList().size() > 1 && vertex->getImmediateDominator() != NULL
					&& (vertex->getImmediateDominator() != NULL && vertex->getImmediateDominator()->getImmediatePostDominator() != NULL && vertex->getImmediateDominator()->getImmediatePostDominator() == vertex->getImmediatePostDominator())) {
				HyperOp* mergeNode = vertex->getImmediateDominator()->getImmediatePostDominator();
				if (find(mergeNode->getParentList().begin(), mergeNode->getParentList().end(), vertex->getImmediateDominator()) != mergeNode->getParentList().end()) {
					HyperOp* joinNodeStart = vertex->getImmediateDominator();
					HyperOp* joinNodeEnd = mergeNode;
					Function* interimFunction;
					HyperOp* interimNode;
					this->addHyperOp(interimNode);
					createInterimFunctionToBreakEdge(&interimNode, &interimFunction, joinNodeStart, joinNodeEnd);
				} else {
					Function* interimFunction;
					HyperOp* interimNode;
					createInterimMergeNode(&interimNode, &interimFunction, vertex);
				}
			}
		}
		computeDominatorInfo();
	}

	errs() << "after making the graph structured\n";
	this->print(errs());
}

/*
 * Create a clone of a function with 2 new inreg arguments
 */
void cloneFunction(HyperOp** hopForUpdate, list<Type*> additionalNewArgs, bool pushFront) {
	vector<Type*> funcArgsList;
	Function* hopFunction = (*hopForUpdate)->getFunction();
	LLVMContext &ctxt = hopFunction->getParent()->getContext();
	if (pushFront) {
		for (auto argItr : additionalNewArgs) {
			Type* argType = argItr;
			funcArgsList.push_back(argType);
		}
	}
	for (auto oldArgItr = hopFunction->arg_begin(); oldArgItr != hopFunction->arg_end(); oldArgItr++) {
		funcArgsList.push_back(oldArgItr->getType());
	}
	if (!pushFront) {
		for (auto argItr : additionalNewArgs) {
			Type* argType = argItr;
			funcArgsList.push_back(argType);
		}
	}

	FunctionType *FT = FunctionType::get(hopFunction->getReturnType(), funcArgsList, false);
	string newname = hopFunction->getName().data();
	newname.append(itostr(1));
	Function *newFunction = Function::Create(FT, Function::ExternalLinkage, newname, hopFunction->getParent());
	auto newArgItr = newFunction->arg_begin();
	//Advance the pointer twice since two new args are added
	if (pushFront) {
		for (int i = 0; i < additionalNewArgs.size(); i++) {
			newArgItr = newArgItr->getNextNode();
		}
	}
	int newArgIndex = pushFront ? additionalNewArgs.size() : 0;
	int minIndex = newArgIndex;
	map<Value*, Value*> oldToNewValueMap;
	for (auto oldArgItr = hopFunction->arg_begin(); oldArgItr != hopFunction->arg_end(); oldArgItr++, newArgItr++, newArgIndex++) {
		Argument* oldArg = oldArgItr;
		Argument* newArg = newArgItr;
		oldToNewValueMap.insert(make_pair(oldArgItr, newArgItr));
		auto oldAttrSet = hopFunction->getAttributes().getParamAttributes(newArgIndex - minIndex + 1);
		newFunction->addAttributes(newArgIndex + 1, oldAttrSet);
	}

	for (auto funcItr = hopFunction->begin(); funcItr != hopFunction->end(); funcItr++) {
		BasicBlock* oldBB = funcItr;
		BasicBlock* newBB = BasicBlock::Create(ctxt, oldBB->getName(), newFunction);
		oldToNewValueMap.insert(make_pair(oldBB, newBB));
	}
	for (auto bbItr = hopFunction->begin(); bbItr != hopFunction->end(); bbItr++) {
		BasicBlock* oldBB = bbItr;
		assert(oldToNewValueMap.find(oldBB) != oldToNewValueMap.end() && "Basicblock not cloned before");
		BasicBlock* newBB = (BasicBlock*) oldToNewValueMap[oldBB];
		for (auto instItr = oldBB->begin(); instItr != oldBB->end(); instItr++) {
			Instruction* oldInst = instItr;
			Instruction* newInst = oldInst->clone();
			oldToNewValueMap.insert(make_pair(oldInst, newInst));
			newBB->getInstList().insert(newBB->end(), newInst);
		}
	}

	for (auto instItr = oldToNewValueMap.begin(); instItr != oldToNewValueMap.end(); instItr++) {
		if (isa<Instruction>(instItr->first)) {
			Instruction* oldInst = (Instruction*) instItr->first;
			Instruction * newInst = (Instruction*) instItr->second;
			for (int operandIndex = 0; operandIndex < oldInst->getNumOperands(); operandIndex++) {
				Value* oldOperand = oldInst->getOperand(operandIndex);
				if (oldToNewValueMap.find(oldOperand) != oldToNewValueMap.end()) {
					newInst->setOperand(operandIndex, oldToNewValueMap[oldOperand]);
				}
				if (isa<PHINode>(newInst)) {
					((PHINode*) newInst)->setIncomingBlock(operandIndex, (BasicBlock*) oldToNewValueMap[((PHINode*) newInst)->getIncomingBlock(operandIndex)]);
				}
			}
		}
	}
	(*hopForUpdate)->setFunction(newFunction);
	/* Update the outgoing edges with new cloned values from the current HyperOp function */
	for (auto edgeItr = (*hopForUpdate)->ChildMap.begin(); edgeItr != (*hopForUpdate)->ChildMap.end(); edgeItr++) {
		if (edgeItr->first->getValue() != NULL && oldToNewValueMap.find(edgeItr->first->getValue()) != oldToNewValueMap.end()) {
			edgeItr->first->setValue(oldToNewValueMap[edgeItr->first->getValue()]);
		}
	}
	for (auto vertex : (*hopForUpdate)->getParentGraph()->Vertices) {
		if (vertex->getInRange() && (vertex->getUpperBoundScope() == (*hopForUpdate) || vertex->getLowerBoundScope() == (*hopForUpdate))) {
			if (vertex->getUpperBoundScope() == (*hopForUpdate)) {
				vertex->setRangeUpperBound(oldToNewValueMap[vertex->getRangeUpperBound()]);
			}
			if (vertex->getLowerBoundScope() == (*hopForUpdate)) {
				vertex->setRangeLowerBound(oldToNewValueMap[vertex->getRangeLowerBound()]);
			}
		}
	}
}

static bool inline isScalarEdgeTypeWithContextSlot(HyperOpEdge* previouslyAddedEdge) {
	switch (previouslyAddedEdge->getType()) {
		case HyperOpEdge::SCALAR:
		case HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR:
		case HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_BASE:
		case HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_LOWER:
		case HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_UPPER:
			return true;
	}
	return false;
}

//todo This is the same as previous function, check what modification is necessary
static bool inline isEdgeTypeContextFrameSpecific(HyperOpEdge* previouslyAddedEdge) {
	switch (previouslyAddedEdge->getType()) {
	case HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR:
	case HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_BASE:
	case HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_LOWER:
	case HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_UPPER:
		return true;
	}
	return false;
}

/**
 * Indicates additional edges corresponding to WriteCM instructions for forwarding context frame addresses
 */
void HyperOpInteractionGraph::addContextFrameAddressForwardingEdges() {
	/* Context frame edges that get added to a child hop forming the value pair */
	map<HyperOp*, list<HyperOpEdge*> > childHopsAndNewEdges;

	/* Map of HyperOps and the vertices they are in the dominance frontier of */
	map<HyperOp*, list<pair<HyperOp*, HyperOp*> > > vertexInDomfOf;
//Forward addresses to producers that have a HyperOp in their dominance frontier and to the HyperOps that delete the context frame
	for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
		HyperOp* vertex = *vertexIterator;
		list<pair<HyperOp*, HyperOp*> > dominanceFrontierContainingVertex;
		for (list<HyperOp*>::iterator tempItr = Vertices.begin(); tempItr != Vertices.end(); tempItr++) {
			HyperOp* domf = *tempItr;
			auto domfList = domf->getDominanceFrontier();
			HyperOp* fromVertex = domf->getImmediateDominator();
			if (domf != vertex && !domfList.empty() && find(domfList.begin(), domfList.end(), vertex) != domfList.end()) {
				auto parentList = vertex->getParentList();
				if (find(parentList.begin(), parentList.end(), domf) != parentList.end()) {
					dominanceFrontierContainingVertex.push_front(make_pair(domf, fromVertex));
				} else {
					dominanceFrontierContainingVertex.push_back(make_pair(domf, fromVertex));
				}
			}
		}
		if (vertex->isPredicatedHyperOp()) {
			HyperOp* fromVertex = vertex->getImmediateDominator();
			dominanceFrontierContainingVertex.push_back(make_pair(vertex->getImmediateDominator()->getImmediatePostDominator(), fromVertex));
		}
		vertexInDomfOf.insert(make_pair(vertex, dominanceFrontierContainingVertex));
	}

	for (auto dominanceFrontierIterator = vertexInDomfOf.begin(); dominanceFrontierIterator != vertexInDomfOf.end(); dominanceFrontierIterator++) {
		HyperOp* dominanceFrontierHyperOp = dominanceFrontierIterator->first;
		for (auto vertexItr : dominanceFrontierIterator->second) {
			HyperOp* vertex = vertexItr.first;
			HyperOp* immediateDominator = vertexItr.second;
			if (dominanceFrontierHyperOp != vertex) {
				string multiplicity = "";
				for (auto parentItr : dominanceFrontierHyperOp->ParentMap) {
					if (parentItr.second == vertex) {
						multiplicity = parentItr.first->getMultiplicity();
						break;
					}
				}
				HyperOpEdge* contextFrameEdge = new HyperOpEdge();
				contextFrameEdge->setType(HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR);
				contextFrameEdge->setContextFrameAddress(dominanceFrontierHyperOp);
				contextFrameEdge->setMultiplicity(multiplicity);

				HyperOpEdge* upperBoundOfRange = NULL;
				if (dominanceFrontierHyperOp->getInRange() && dominanceFrontierHyperOp->getUpperBoundScope() != NULL && multiplicity.compare(ONE_TO_ONE)) {
					upperBoundOfRange = new HyperOpEdge();
					upperBoundOfRange->setType(HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_UPPER);
					upperBoundOfRange->setContextFrameAddress(dominanceFrontierHyperOp);
					upperBoundOfRange->setValue(dominanceFrontierHyperOp->getRangeUpperBound());
					upperBoundOfRange->setMultiplicity(multiplicity);
				}
				HyperOpEdge* lowerBoundOfRange = NULL;
				if (dominanceFrontierHyperOp->getInRange() && dominanceFrontierHyperOp->getLowerBoundScope() != NULL && multiplicity.compare(ONE_TO_ONE)) {
					lowerBoundOfRange = new HyperOpEdge();
					lowerBoundOfRange->setType(HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_LOWER);
					lowerBoundOfRange->setContextFrameAddress(dominanceFrontierHyperOp);
					lowerBoundOfRange->setValue(dominanceFrontierHyperOp->getRangeLowerBound());
					lowerBoundOfRange->setMultiplicity(multiplicity);
				}

				int freeContextSlot;
				if (dominanceFrontierHyperOp->getImmediateDominator() == vertex->getImmediateDominator()) {
					bool edgeAddedPreviously = false;
					for (map<HyperOpEdge*, HyperOp*>::iterator childMapItr = immediateDominator->ChildMap.begin(); childMapItr != immediateDominator->ChildMap.end(); childMapItr++) {
						if (childMapItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR && childMapItr->first->getContextFrameAddress() == dominanceFrontierHyperOp && childMapItr->second == vertex) {
							edgeAddedPreviously = true;
							break;
						}
					}
					if (!edgeAddedPreviously) {
						int max = -1;
						for (map<HyperOpEdge*, HyperOp*>::iterator parentEdgeItr = vertex->ParentMap.begin(); parentEdgeItr != vertex->ParentMap.end(); parentEdgeItr++) {
							HyperOpEdge* const previouslyAddedEdge = parentEdgeItr->first;
							if (isScalarEdgeTypeWithContextSlot(previouslyAddedEdge) && previouslyAddedEdge->getPositionOfContextSlot() > max) {
								max = previouslyAddedEdge->getPositionOfContextSlot();
							}
						}
						freeContextSlot = max + 1;
						contextFrameEdge->setPositionOfContextSlot(freeContextSlot);

						this->addEdge(immediateDominator, vertex, (HyperOpEdge*) contextFrameEdge);
						/* Add to map so that it can be added to argument list later */
						list<HyperOpEdge*> newEdgesList;
						if (childHopsAndNewEdges.find(vertex) != childHopsAndNewEdges.end()) {
							newEdgesList = childHopsAndNewEdges[vertex];
							childHopsAndNewEdges.erase(vertex);
						}
						newEdgesList.push_back(contextFrameEdge);
						freeContextSlot++;
						if (lowerBoundOfRange != NULL) {
							lowerBoundOfRange->setPositionOfContextSlot(freeContextSlot);
							this->addEdge(immediateDominator, vertex, (HyperOpEdge*) lowerBoundOfRange);
							newEdgesList.push_back(lowerBoundOfRange);
							freeContextSlot++;
						}

						if (upperBoundOfRange != NULL) {
							upperBoundOfRange->setPositionOfContextSlot(freeContextSlot);
							this->addEdge(immediateDominator, vertex, (HyperOpEdge*) upperBoundOfRange);
							newEdgesList.push_back(upperBoundOfRange);
							freeContextSlot++;
						}
						childHopsAndNewEdges.insert(make_pair(vertex, newEdgesList));
					}
				} else {
					list<HyperOp*> immediateDominatorDominanceFrontier = immediateDominator->getDominanceFrontier();
					HyperOp* prevVertex = vertex;
					while (immediateDominator != 0 && !immediateDominatorDominanceFrontier.empty() && std::find(immediateDominatorDominanceFrontier.begin(), immediateDominatorDominanceFrontier.end(), dominanceFrontierHyperOp) != immediateDominatorDominanceFrontier.end()) {
						HyperOpEdge* frameForwardChainEdge = new HyperOpEdge();
						frameForwardChainEdge->setType(HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR);
						frameForwardChainEdge->setContextFrameAddress(dominanceFrontierHyperOp);
						frameForwardChainEdge->setMultiplicity(multiplicity);

						HyperOpEdge* upperBoundOfRangeForward = NULL;
						if (dominanceFrontierHyperOp->getInRange() && dominanceFrontierHyperOp->getUpperBoundScope() != NULL && multiplicity.compare(ONE_TO_ONE)) {
							upperBoundOfRangeForward = new HyperOpEdge();
							upperBoundOfRangeForward->setType(HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_UPPER);
							upperBoundOfRangeForward->setContextFrameAddress(dominanceFrontierHyperOp);
							upperBoundOfRangeForward->setValue(dominanceFrontierHyperOp->getRangeUpperBound());
							frameForwardChainEdge->setMultiplicity(multiplicity);
						}

						HyperOpEdge* lowerBoundOfRangeForward = NULL;
						if (dominanceFrontierHyperOp->getInRange() && dominanceFrontierHyperOp->getLowerBoundScope() != NULL && multiplicity.compare(ONE_TO_ONE)) {
							lowerBoundOfRangeForward = new HyperOpEdge();
							lowerBoundOfRangeForward->setType(HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_LOWER);
							lowerBoundOfRangeForward->setContextFrameAddress(dominanceFrontierHyperOp);
							lowerBoundOfRangeForward->setValue(dominanceFrontierHyperOp->getRangeLowerBound());
							lowerBoundOfRangeForward->setMultiplicity(multiplicity);
						}

						bool edgeAddedPreviously = false;
						for (map<HyperOpEdge*, HyperOp*>::iterator childMapItr = immediateDominator->ChildMap.begin(); childMapItr != immediateDominator->ChildMap.end(); childMapItr++) {
							if (childMapItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR && childMapItr->first->getContextFrameAddress() == dominanceFrontierHyperOp && childMapItr->second == prevVertex) {
								edgeAddedPreviously = true;
								break;
							}
						}
						if (!edgeAddedPreviously) {
							//Find the last free context slot at consumer
							int freeContextSlot;
							int max = -1;
							for (map<HyperOpEdge*, HyperOp*>::iterator parentEdgeItr = prevVertex->ParentMap.begin(); parentEdgeItr != prevVertex->ParentMap.end(); parentEdgeItr++) {
								HyperOpEdge* const previouslyAddedEdge = parentEdgeItr->first;
								if (isScalarEdgeTypeWithContextSlot(previouslyAddedEdge) && previouslyAddedEdge->getPositionOfContextSlot() > max) {
									max = previouslyAddedEdge->getPositionOfContextSlot();
								}
							}
							freeContextSlot = max + 1;
							frameForwardChainEdge->setPositionOfContextSlot(freeContextSlot);

							this->addEdge(immediateDominator, prevVertex, (HyperOpEdge*) frameForwardChainEdge);
							/* Add to map so that it can be added to argument list later */
							list<HyperOpEdge*> newEdgesList;
							if (childHopsAndNewEdges.find(prevVertex) != childHopsAndNewEdges.end()) {
								newEdgesList = childHopsAndNewEdges[prevVertex];
								childHopsAndNewEdges.erase(prevVertex);
							}
							newEdgesList.push_back(frameForwardChainEdge);
							freeContextSlot++;
							if (lowerBoundOfRangeForward != NULL) {
								lowerBoundOfRangeForward->setPositionOfContextSlot(freeContextSlot);
								this->addEdge(immediateDominator, prevVertex, (HyperOpEdge*) lowerBoundOfRangeForward);
								newEdgesList.push_back(lowerBoundOfRangeForward);
								freeContextSlot++;
							}

							if (upperBoundOfRangeForward != NULL) {
								upperBoundOfRangeForward->setPositionOfContextSlot(freeContextSlot);
								this->addEdge(immediateDominator, prevVertex, (HyperOpEdge*) upperBoundOfRangeForward);
								newEdgesList.push_back(upperBoundOfRangeForward);
								freeContextSlot++;
							}
							childHopsAndNewEdges.insert(make_pair(prevVertex, newEdgesList));
						}
						prevVertex = immediateDominator;
						immediateDominator = immediateDominator->getImmediateDominator();
						if (immediateDominator != 0) {
							immediateDominatorDominanceFrontier = immediateDominator->getDominanceFrontier();
						}
					}

					bool edgeAddedPreviously = false;
					for (map<HyperOpEdge*, HyperOp*>::iterator childMapItr = immediateDominator->ChildMap.begin(); childMapItr != immediateDominator->ChildMap.end(); childMapItr++) {
						if (childMapItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR && childMapItr->first->getContextFrameAddress() == dominanceFrontierHyperOp && childMapItr->second == prevVertex) {
							edgeAddedPreviously = true;
							break;
						}
					}
					if (!edgeAddedPreviously) {
						int freeContextSlot;
						int max = -1;
						for (map<HyperOpEdge*, HyperOp*>::iterator parentEdgeItr = prevVertex->ParentMap.begin(); parentEdgeItr != prevVertex->ParentMap.end(); parentEdgeItr++) {
							HyperOpEdge* const previouslyAddedEdge = parentEdgeItr->first;
							if (isScalarEdgeTypeWithContextSlot(previouslyAddedEdge) && previouslyAddedEdge->getPositionOfContextSlot() > max) {
								max = previouslyAddedEdge->getPositionOfContextSlot();
							}
						}
						freeContextSlot = max + 1;
						contextFrameEdge->setPositionOfContextSlot(freeContextSlot);
						this->addEdge(immediateDominator, prevVertex, (HyperOpEdge*) contextFrameEdge);

						/* Add to map so that it can be added to argument list later */
						list<HyperOpEdge*> newEdgesList;
						if (childHopsAndNewEdges.find(prevVertex) != childHopsAndNewEdges.end()) {
							newEdgesList = childHopsAndNewEdges[prevVertex];
							childHopsAndNewEdges.erase(prevVertex);
						}
						newEdgesList.push_back(contextFrameEdge);
						freeContextSlot++;
						if (lowerBoundOfRange != NULL) {
							lowerBoundOfRange->setPositionOfContextSlot(freeContextSlot);
							this->addEdge(immediateDominator, prevVertex, (HyperOpEdge*) lowerBoundOfRange);
							newEdgesList.push_back(lowerBoundOfRange);
							freeContextSlot++;
						}

						if (upperBoundOfRange != NULL) {
							upperBoundOfRange->setPositionOfContextSlot(freeContextSlot);
							this->addEdge(immediateDominator, prevVertex, (HyperOpEdge*) upperBoundOfRange);
							newEdgesList.push_back(upperBoundOfRange);
							freeContextSlot++;
						}
						childHopsAndNewEdges.insert(make_pair(prevVertex, newEdgesList));
					}
				}
			}
		}
	}

//Shuffle the address forward slots for dynamic instances
	map<HyperOp*, list<HyperOpEdge*> > incomingContextAddressFwdEdgeAndSlotMapForDynamicInstances;
	map<HyperOp*, list<HyperOpEdge*> > incomingContextAddressFwdEdgeAndSlotMapForUnrolledInstances;
	for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
		list<HyperOpEdge*> incomingContextEdgeSlotMap;
		for (map<HyperOpEdge*, HyperOp*>::iterator parentMapItr = (*vertexIterator)->ParentMap.begin(); parentMapItr != (*vertexIterator)->ParentMap.end(); parentMapItr++) {
			if (isEdgeTypeContextFrameSpecific(parentMapItr->first) && !(parentMapItr->second->isUnrolledInstance() && (*vertexIterator)->isUnrolledInstance())) {
				incomingContextEdgeSlotMap.push_back(parentMapItr->first);
			}
		}
		if ((*vertexIterator)->isUnrolledInstance()) {
			incomingContextAddressFwdEdgeAndSlotMapForUnrolledInstances[*vertexIterator] = incomingContextEdgeSlotMap;
		} else {
			incomingContextAddressFwdEdgeAndSlotMapForDynamicInstances[*vertexIterator] = incomingContextEdgeSlotMap;
		}
	}

	for (map<HyperOp*, list<HyperOpEdge*> >::iterator unrolledInstItr = incomingContextAddressFwdEdgeAndSlotMapForUnrolledInstances.begin(); unrolledInstItr != incomingContextAddressFwdEdgeAndSlotMapForUnrolledInstances.end(); unrolledInstItr++) {
		HyperOp* unrolledInstance = unrolledInstItr->first;
		list<HyperOpEdge*> dynamicInstanceIncomingEdgeList;
		for (map<HyperOp*, list<HyperOpEdge*> >::iterator dynamicInstItr = incomingContextAddressFwdEdgeAndSlotMapForDynamicInstances.begin(); dynamicInstItr != incomingContextAddressFwdEdgeAndSlotMapForDynamicInstances.end(); dynamicInstItr++) {
			if (dynamicInstItr->first->getFunction() == unrolledInstance->getFunction()) {
				dynamicInstanceIncomingEdgeList = dynamicInstItr->second;
				break;
			}
		}

		if (!dynamicInstanceIncomingEdgeList.empty()) {
			//Check what the edge in the unrolled edge set maps to
			for (list<HyperOpEdge*>::iterator unrolledEdgeMapItr = unrolledInstItr->second.begin(); unrolledEdgeMapItr != unrolledInstItr->second.end(); unrolledEdgeMapItr++) {
				for (list<HyperOpEdge*>::iterator dynamicInstEdgeItr = dynamicInstanceIncomingEdgeList.begin(); dynamicInstEdgeItr != dynamicInstanceIncomingEdgeList.end(); dynamicInstEdgeItr++) {
					if ((*unrolledEdgeMapItr)->getContextFrameAddress()->getInstanceof() == (*dynamicInstEdgeItr)->getContextFrameAddress()->getInstanceof()) {
						(*unrolledEdgeMapItr)->setPositionOfContextSlot((*dynamicInstEdgeItr)->getPositionOfContextSlot());
						break;
					}
				}
			}
		}
	}

	DEBUG(dbgs() << "Fixing args of the functions after adding context frame address forwarding edges\n");
	Module* M = Vertices.front()->getFunction()->getParent();
	/* Update hyperops with newly added args for context frames */
	for (auto hopForUpdateItr : childHopsAndNewEdges) {
		HyperOp* hopForUpdate = hopForUpdateItr.first;
		/* Unrolled instances don't have functions of their own, their functions don't need to be updated a second time */
		if (hopForUpdate->isUnrolledInstance()) {
			continue;
		}
		int numNewEdges = hopForUpdateItr.second.size();
		list<Type*> newArgsList;
		for (int i = 0; i < numNewEdges; i++) {
			newArgsList.push_back(Type::getInt32Ty(M->getContext()));
		}
		Function* prevFunction = hopForUpdate->getFunction();
		cloneFunction(&hopForUpdate, newArgsList, false);

		/* We also need to update any unrolled instance pointing to the same function */
		for (auto vertexItr : this->Vertices) {
			assert((vertexItr->getFunction() != prevFunction || vertexItr->isUnrolledInstance()) && "Only unrolled instances may use the same function reference\n");
			if (vertexItr->getFunction() == prevFunction) {
				vertexItr->setFunction(hopForUpdate->getFunction());
			}
			if (vertexItr->getInstanceof() == prevFunction) {
				vertexItr->setInstanceof(hopForUpdate->getFunction());
			}
		}
		prevFunction->eraseFromParent();
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
//	errs()<<"source:"<<source->asString()<<"\n";
	map<HyperOp*, list<unsigned int> > childNodeWeightMap = source->getChildNodeEdgeWeights();
	bool firstChild = true;
	bool atleastOneChildTraversed = false;
	if (!childNodeWeightMap.empty()) {
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
				if (firstChild || compareHierarchicalVolume(executionTime, newExecutionTime) < 0) {
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
	errs() << "\nDominant sequence computed:\n";
	for (list<HyperOp*>::iterator dominantSequenceItr = dominantSequence.begin(); dominantSequenceItr != dominantSequence.end(); dominantSequenceItr++) {
		errs() << (*dominantSequenceItr)->asString() << "->";
	}
	errs() << "\n";
}

void HyperOpInteractionGraph::clusterNodes() {

//Create a wrapper to HyperOp class to hierarchically cluster nodes
	class HIGSubtree {
	public:
		list<HyperOp*> hyperOpsInCluster;
		list<pair<list<HyperOp*>, unsigned int> > subtreeClusterList;
		list<unsigned int> executionTimeEstimate;
		HyperOp* startHyperOp;
		HyperOp* endHyperOp;
		HIGSubtree(list<HyperOp*> hyperOpList, HyperOp* startHyperOp, HyperOp* endHyperOp) {
			hyperOpsInCluster = hyperOpList;
			this->startHyperOp = startHyperOp;
			this->endHyperOp = endHyperOp;
		}

		void clusterSubTree() {
			errs() << "total number of nodes:" << hyperOpsInCluster.size() << "\n";
			//Compute start and end HyperOps for the subtree
			list<pair<list<HyperOp*>, unsigned int> > computeClusterList;
			for (list<HyperOp*>::iterator vertexIterator = hyperOpsInCluster.begin(); vertexIterator != hyperOpsInCluster.end(); vertexIterator++) {
				list<HyperOp*> hopList;
				hopList.push_back(*vertexIterator);
				estimateExecutionTime(*vertexIterator);
				unsigned int hyperOpType = 2;
				if ((*vertexIterator) == startHyperOp) {
					hyperOpType = 0;
				}
				if ((*vertexIterator) == endHyperOp) {
					hyperOpType = 1;
				}
				//		hierarchicalComputeClusterList.push_back(std::make_pair(newClusterNode, hyperOpType));
				computeClusterList.push_back(std::make_pair(hopList, hyperOpType));
			}
			errs() << "initial number of compute clusters:" << computeClusterList.size() << "\n";

			//Cluster all the nodes in a queue first and replace the nodes with the cluster node
			list<pair<HyperOp*, HyperOp*> > examinedEdges;
			list<list<HyperOp*> > excludeList;
			//Find the initial Dominant Sequence
			list<HyperOp*> partialDominantSequence;
			partialDominantSequence.push_back(startHyperOp);
			pair<list<HyperOp*>, list<unsigned int> > dominantSequencePair = computeDominantSequence(partialDominantSequence);
			//	printDS(dominantSequencePair.first);
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
				//		errs() << "dominant sequence computed:";
				//		printDS(dominantSequence);
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
					}
					if (std::find(cluster.begin(), cluster.end(), target) != cluster.end()) {
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
					computeClusterList.clear();
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

				examinedEdges.push_back(std::make_pair(source, target));
			}
			subtreeClusterList = computeClusterList;
		}
//		printDS(dominantSequence);
	};

//	pair<HyperOpInteractionGraph*, map<HyperOp*, HyperOp*> > controlFlowGraphAndOriginalHopMap = getCFG(this);
//	map<HyperOp*, HyperOp*> originalToCFGVertexMap = controlFlowGraphAndOriginalHopMap.second;
//	HyperOpInteractionGraph* cfg = controlFlowGraphAndOriginalHopMap.first;
//	cfg->computeDominatorInfo();
//	//Find subtrees in cfg
//	for(auto vertex:cfg->Vertices){
//		//Check if any of the children of the vertex is predicated
//		bool hasPredicatedChildren = false;
//		for(auto childVertex:vertex->ChildMap){
//			if(childVertex.second->isPredicatedHyperOp()){
//				hasPredicatedChildren = true;
//				break;
//			}
//		}
//
//		if(hasPredicatedChildren){
//			//Treat the vertex as the root of a subtree and
//		}
//	}

//Add the edge to examined list;

//TODO uncomment the following
//Merge clusters till the number of compute resources matches the number of clusters created
//	while (computeClusterList.size() > (this->rowCount * this->columnCount)) {
//		//Find all pairs of clusters and merge the one that leads to the least execution time
//		//This is expensive but still cheaper when operating on clusters since the number of clusters is way smaller than the number of nodes in the original graph
//		pair<list<HyperOp*>, unsigned int> sourceClusterPair;
//		pair<list<HyperOp*>, unsigned int> targetClusterPair;
//		list<HyperOp*> sourceCluster;
//		list<HyperOp*> targetCluster;
//		list<pair<list<HyperOp*>, unsigned int> > tempClusterList;
//
//		list<unsigned int> minimumExecutionTime;
//		bool first = true;
//		for (list<pair<list<HyperOp*>, unsigned int> >::iterator sourceClusterItr = computeClusterList.begin(); sourceClusterItr != computeClusterList.end(); sourceClusterItr++) {
//			if (*sourceClusterItr != computeClusterList.back()) {
//				list<HyperOp*> sourceCluster = sourceClusterItr->first;
//				list<pair<list<HyperOp*>, unsigned int> >::iterator targetClusterItr = sourceClusterItr;
//				targetClusterItr++;
//				for (; targetClusterItr != computeClusterList.end(); targetClusterItr++) {
//					pair<list<unsigned int>, list<pair<list<HyperOp*>, unsigned int> > > returnValue = mergeNodesAndReturnExecutionTime(startHyperOp, *sourceClusterItr, *targetClusterItr, computeClusterList, true);
////					errs()<<"attempting a merge\n";
//					list<unsigned int> newExecutionTime = returnValue.first;
//					if (first || compareHierarchicalVolume(minimumExecutionTime, newExecutionTime) >= 0) {
//						minimumExecutionTime = newExecutionTime;
//						sourceClusterPair = *sourceClusterItr;
//						sourceCluster = sourceClusterPair.first;
//						targetClusterPair = *targetClusterItr;
//						targetCluster = targetClusterPair.first;
//						first = false;
//					}
//				}
//			}
//		}
//
//		errs()<<"merged clusters:"<<computeClusterList.size()<<"\n";
//		pair<list<unsigned int>, list<pair<list<HyperOp*>, unsigned int> > > returnValue = mergeNodesAndReturnExecutionTime(startHyperOp, sourceClusterPair, targetClusterPair, computeClusterList, false);
//		computeClusterList = returnValue.second;
//	}

	HyperOp* startHyperOp;
	HyperOp* endHyperOp;
	for (auto vertexItr = Vertices.begin(); vertexItr != Vertices.end(); vertexItr++) {
		if ((*vertexItr)->isStartHyperOp()) {
			startHyperOp = *vertexItr;
		}
		if ((*vertexItr)->isEndHyperOp()) {
			endHyperOp = *vertexItr;
		}
	}
	/* Identify the subtrees to be clustered, the root and the idom added here */
	list<pair<HyperOp*, HyperOp*> > clusteringSubtreeList;
	list<HyperOp*> rootNodeForTraversal;
	HyperOp* rootNode = startHyperOp;
	while (rootNode) {
		if (rootNode->getChildList().size() > 1 || (!rootNode->getChildList().empty() && rootNode->getChildList().front()->getInRange())) {
			break;
		}
		rootNode = rootNode->getImmediatePostDominator();
	}

	/* The cluster ought to cover the entire graph, to ensure that all nodes belong to a cluster */
	clusteringSubtreeList.push_front(make_pair(startHyperOp, endHyperOp));
	rootNodeForTraversal.push_front(rootNode);

	list<HyperOp*> traversedNodes;
	while (!rootNodeForTraversal.empty()) {
		HyperOp* currentRoot = rootNodeForTraversal.front();
		rootNodeForTraversal.pop_front();
		//Find all the children of rootNode and add them in traversal queue
		if (currentRoot != NULL) {
			for (auto childItr : currentRoot->getChildList()) {
				HyperOp* child = childItr;
				if(child->isUnrolledInstance() || find(traversedNodes.begin(), traversedNodes.end(), child) != traversedNodes.end() || child->getImmediatePostDominator() == NULL){
					continue;
				}
				if (child->ChildMap.size() > 1) {
					clusteringSubtreeList.push_front(make_pair(child, child->getImmediatePostDominator()));
					rootNodeForTraversal.push_front(child);
					traversedNodes.push_back(child);
				}
			}
		}
	}

//	for(auto subtreeItr:clusteringSubtreeList){
//		list<HyperOp*> nodesCovered;
//		HyperOp* subtreeStart = subtreeItr.first;
//		HyperOp* subtreeEnd = subtreeItr.second;
//		/* Compute the nodes between start and end */
//		nodesCovered.push_back(subtreeStart);
//		nodesCovered.push_back(subtreeEnd);
//		list<HyperOp*> idomSet;
//		idomSet.push_back(subtreeStart);
//		bool change = true;
//
//		while (change) {
//			change = false;
//			for (auto vertexItr : this->Vertices) {
//				if (find(idomSet.begin(), idomSet.end(), vertexItr->getImmediateDominator()) != idomSet.end() && find(nodesCovered.begin(), nodesCovered.end(),vertexItr) == nodesCovered.end()) {
//					nodesCovered.push_back(vertexItr);
//					if (find(idomSet.begin(), idomSet.end(), vertexItr) == idomSet.end()) {
//						idomSet.push_back(vertexItr);
//						change = true;
//					}
//				}
//			}
//		}
//		bool atleastOnePredicatedChild = false;
//		for(auto vertexItr:nodesCovered){
//			if(vertexItr->isPredicatedHyperOp()){
//				 atleastOnePredicatedChild = true;
//				 break;
//			}
//		}
//		if(atleastOnePredicatedChild){
//			HIGSubtree subtree(nodesCovered, subtreeStart, subtreeEnd);
//			errs()<<"\n============\nnew subtree\n";
//			subtree.clusterSubTree();
//			auto subtreeClusterList = subtree.subtreeClusterList;
//			for(auto subclusterListItr:subtreeClusterList){
//				errs()<<"\n----\nhops in cluster "<<subclusterListItr.second<<":";
//				for(auto vertexItr:subclusterListItr.first){
//					errs()<<vertexItr->asString()<<"\n";
//				}
//			}
//		}
//	}
	HIGSubtree subtree(Vertices, startHyperOp, endHyperOp);
	subtree.clusterSubTree();
	auto computeClusterList = subtree.subtreeClusterList;

	clusterList.clear();
	unsigned crId = 0;
	for (list<pair<list<HyperOp*>, unsigned int> >::iterator clusterItr = computeClusterList.begin(); clusterItr != computeClusterList.end(); clusterItr++) {
		clusterList.push_back(clusterItr->first);
	}

	DEBUG(dbgs() << "Minimizing ordering edges\n");
	for (list<HyperOp*>::iterator vertexItr = Vertices.begin(); vertexItr != Vertices.end(); vertexItr++) {
		HyperOp* vertex = *vertexItr;
		list<HyperOp*> children = vertex->getChildList();
		for (list<HyperOp*>::iterator childItr = children.begin(); childItr != children.end(); childItr++) {
			HyperOp* childVertex = *childItr;
			bool edgeTypeOtherThanOrderingExists = true;
			list<HyperOpEdge*> orderingEdges;
			//If there are multiple edges between the vertex and childVertex
			for (map<HyperOpEdge*, HyperOp*>::iterator childEdgeItr = vertex->ChildMap.begin(); childEdgeItr != vertex->ChildMap.end(); childEdgeItr++) {
				if (childEdgeItr->second == childVertex) {
					HyperOpEdge* edge = childEdgeItr->first;
					if (edge->getType() == HyperOpEdge::ORDERING) {
						orderingEdges.push_back(edge);
					} else {
						edgeTypeOtherThanOrderingExists = true;
					}
				}
			}

			if (edgeTypeOtherThanOrderingExists && !orderingEdges.empty()) {
				//					&& pathExistsInHIGExcludingOrderingEdges(vertex, childVertex)) {
				for (list<HyperOpEdge*>::iterator orderingEdgeItr = orderingEdges.begin(); orderingEdgeItr != orderingEdges.end(); orderingEdgeItr++) {
					vertex->removeChildEdge(*orderingEdgeItr);
					childVertex->removeParentEdge(*orderingEdgeItr);
				}
			}
		}
	}
}
static void inline mergeHyperOps(HyperOp* parentHyperOp, HyperOp* childHyperOp, list<pair<HyperOp*, HyperOp*> > hyperOpPairsToBeMerged, HyperOpInteractionGraph* graph){
	vector<Type*> funcArgsList;
	Function* parentHopFunction = parentHyperOp->getFunction();
	LLVMContext &ctxt = parentHopFunction->getParent()->getContext();

	for (auto oldArgItr = parentHopFunction->arg_begin(); oldArgItr != parentHopFunction->arg_end(); oldArgItr++) {
		funcArgsList.push_back(oldArgItr->getType());
	}
	FunctionType *FT = FunctionType::get(parentHopFunction->getReturnType(), funcArgsList, false);
	string newname = parentHopFunction->getName().data();
	newname.append(itostr(1));
	Function *newFunction = Function::Create(FT, Function::ExternalLinkage, newname , parentHopFunction->getParent());
	auto newArgItr = newFunction->arg_begin();

	map<Value*, Value*> oldToNewValueMap;
	int newArgIndex = 0;
	for (auto oldArgItr = parentHopFunction->arg_begin(); oldArgItr != parentHopFunction->arg_end(); oldArgItr++, newArgItr++, newArgIndex++) {
		Argument* oldArg = oldArgItr;
		Argument* newArg = newArgItr;
		oldToNewValueMap.insert(make_pair(oldArgItr, newArgItr));
		auto oldAttrSet = parentHopFunction->getAttributes().getParamAttributes(newArgIndex + 1);
		newFunction->addAttributes((newArgIndex + 1), oldAttrSet);
	}

	list<BasicBlock*> oldBBList;
	for (auto funcItr = parentHopFunction->begin(); funcItr != parentHopFunction->end(); funcItr++) {
		BasicBlock* oldBB = funcItr;
		oldBBList.push_back(oldBB);
		BasicBlock* newBB = BasicBlock::Create(ctxt, oldBB->getName(), newFunction);
		oldToNewValueMap.insert(make_pair(oldBB, newBB));
	}

	BasicBlock* firstBBInChild = NULL;
	Function* childHopFunction = childHyperOp->getFunction();
	for (auto funcItr = childHopFunction->begin(); funcItr != childHopFunction->end(); funcItr++) {
		BasicBlock* oldBB = funcItr;
		oldBBList.push_back(oldBB);
		BasicBlock* newBB = BasicBlock::Create(ctxt, oldBB->getName(), newFunction);
		if(firstBBInChild == NULL){
			firstBBInChild = newBB;
		}
		oldToNewValueMap.insert(make_pair(oldBB, newBB));
	}

	for (auto bbItr = oldBBList.begin(); bbItr != oldBBList.end(); bbItr++) {
		BasicBlock* oldBB = *bbItr;
		assert(oldToNewValueMap.find(oldBB) != oldToNewValueMap.end() && "Basicblock not cloned before");
		BasicBlock* newBB = (BasicBlock*) oldToNewValueMap[oldBB];
		for (auto instItr = oldBB->begin(); instItr != oldBB->end(); instItr++) {
			Instruction* oldInst = instItr;
			Instruction* newInst;
			if (isa<ReturnInst>(oldInst) && oldInst->getParent()->getParent() == parentHopFunction) {
				newInst = BranchInst::Create(firstBBInChild);
			}else{
				newInst = oldInst->clone();
			}
			oldToNewValueMap.insert(make_pair(oldInst, newInst));
			newBB->getInstList().insert(newBB->end(), newInst);
		}
	}

	int argIndex = 0;
	for (auto oldArgItr = childHopFunction->arg_begin(); oldArgItr != childHopFunction->arg_end(); oldArgItr++, argIndex++) {
		Argument* oldArg = oldArgItr;
		Value* newArg;
		for (auto parentItr = childHyperOp->ParentMap.begin(); parentItr != childHyperOp->ParentMap.end(); parentItr++) {
			if (parentItr->first->getPositionOfContextSlot() == argIndex) {
				newArg = parentItr->first->getValue();
				break;
			}
		}
		if (newArg != NULL && oldToNewValueMap.find(newArg) != oldToNewValueMap.end()) {
			oldToNewValueMap.insert(make_pair(oldArg, oldToNewValueMap[newArg]));
		}
	}

	for (auto instItr = oldToNewValueMap.begin(); instItr != oldToNewValueMap.end(); instItr++) {
		if (isa<Instruction>(instItr->first)) {
			Instruction* oldInst = (Instruction*) instItr->first;
			Instruction * newInst = (Instruction*) instItr->second;
			for (int operandIndex = 0; operandIndex < oldInst->getNumOperands(); operandIndex++) {
				Value* oldOperand = oldInst->getOperand(operandIndex);
				if (oldToNewValueMap.find(oldOperand) != oldToNewValueMap.end()) {
					Value* newOperand = oldToNewValueMap[oldOperand];
					if(isa<AllocaInst>(newOperand) && oldInst->getParent()->getParent() == childHopFunction){
						for(auto mapItr:oldToNewValueMap){
							if(mapItr.second == newOperand && isa<AllocaInst>(mapItr.first) && ((AllocaInst*)mapItr.first)->getParent()->getParent() == parentHopFunction){
								newOperand = new LoadInst(oldToNewValueMap[oldOperand], "", newInst);
								break;
							}
						}
					}
					newInst->setOperand(operandIndex, newOperand);
				}
				if (isa<PHINode>(newInst)) {
					((PHINode*) newInst)->setIncomingBlock(operandIndex, (BasicBlock*) oldToNewValueMap[((PHINode*) newInst)->getIncomingBlock(operandIndex)]);
				}
			}
		}
	}

	if(parentHyperOp->isStaticHyperOp() && !childHyperOp->isStaticHyperOp()){
		parentHyperOp->setStaticHyperOp(false);
	}

	hyperOpPairsToBeMerged.push_front(make_pair(parentHyperOp, childHyperOp));
	for (auto instanceItr : hyperOpPairsToBeMerged) {
		HyperOp* instanceParent = instanceItr.first;
		HyperOp* instanceChild = instanceItr.second;
		/* Update the outgoing edges with new cloned values from the current HyperOp function */
		list<HyperOpEdge*> edgesForRemoval;
		for (auto edgeItr = instanceParent->ChildMap.begin(); edgeItr != instanceParent->ChildMap.end(); edgeItr++) {
			if (edgeItr->first->getValue() != NULL && oldToNewValueMap.find(edgeItr->first->getValue()) != oldToNewValueMap.end()) {
				edgeItr->first->setValue(oldToNewValueMap[edgeItr->first->getValue()]);
			}
			if (edgeItr->second == instanceChild) {
				edgesForRemoval.push_back(edgeItr->first);
			}
		}
		for (auto edgeItr = instanceChild->ChildMap.begin(); edgeItr != instanceChild->ChildMap.end(); edgeItr++) {
			edgesForRemoval.push_back(edgeItr->first);
			HyperOpEdge* clonedEdge;
			edgeItr->first->clone(&clonedEdge);
			if (clonedEdge->getValue() != NULL && oldToNewValueMap.find(clonedEdge->getValue()) != oldToNewValueMap.end()) {
				Value* newOperand = oldToNewValueMap[edgeItr->first->getValue()];
				Value* oldOperand = edgeItr->first->getValue();
				if (isa<AllocaInst>(newOperand) && isa<AllocaInst>(oldOperand) && ((AllocaInst*) oldOperand)->getParent()->getParent() == childHopFunction) {
					for (auto mapItr : oldToNewValueMap) {
						if (mapItr.second == newOperand && isa<AllocaInst>(mapItr.first) && ((AllocaInst*) mapItr.first)->getParent()->getParent() == parentHopFunction) {
							newOperand = new LoadInst(oldToNewValueMap[oldOperand], "", newOperand);
							break;
						}
					}
				}
				clonedEdge->setValue(newOperand);
			}
			graph->addEdge(instanceParent, edgeItr->second, clonedEdge);
		}
		instanceParent->setStaticHyperOp(false);
		Function* prevFunction = instanceParent->getFunction();
		instanceParent->setFunction(newFunction);
		instanceParent->setInstanceof(newFunction);

		if (instanceParent != parentHyperOp && instanceChild != childHyperOp) {
			instanceParent->setInstanceId(instanceParent->getInstanceId());
			instanceParent->setIsUnrolledInstance(instanceChild->isUnrolledInstance());
		}
		for (auto edgeForRemovalItr : edgesForRemoval) {
			instanceParent->ChildMap.erase(edgeForRemovalItr);
		}
		graph->removeHyperOp(instanceChild);
	}
	list<Function*> functionsToBeDeleted;
	for(auto functionItr = parentHopFunction->getParent()->begin(); functionItr!=parentHopFunction->getParent()->end(); functionItr++){
		bool functionUse = false;
		for(auto vertexItr:graph->Vertices){
			if(vertexItr->getFunction() == functionItr || vertexItr->getInstanceof() == functionItr){
				functionUse = true;
				break;
			}
		}
		if(!functionUse){
			functionsToBeDeleted.push_back(functionItr);
		}
	}
	for(auto deleteItr:functionsToBeDeleted){
		deleteItr->eraseFromParent();
	}
	if (childHyperOp->isEndHyperOp()) {
		parentHyperOp->setEndHyperOp();
	}
}

void HyperOpInteractionGraph::mergeUnpredicatedNodesInCluster(){
	/* Merge nodes in a cluster if this does not affect parallelism */
	for (auto clusterItr : clusterList) {
		/* Merge a node with its last parent in the cluster if the node isnt predicated and all its parents are in the cluster */
		bool change = true;
		while (change) {
			change = false;
			for (auto hopItr = clusterItr.begin(); hopItr != clusterItr.end(); hopItr++) {
				HyperOp* hop = *hopItr;
				auto parentList = hop->getParentList();
				if (hop->isUnrolledInstance() || hop->isPredicatedHyperOp() || parentList.size() != 1 || find(clusterItr.begin(), clusterItr.end(), parentList.front()) == clusterItr.end() || parentList.front()->isUnrolledInstance() || parentList.front()->isStartHyperOp()) {
					continue;
				}
				HyperOp* parentHyperOp = parentList.front();
				list<pair<HyperOp*, HyperOp*> > hyperOpPairsToBeMerged;
				Function* childHopFunction = hop->getFunction();
				bool canMergeInstances = true;
				for (auto vertexItr = this->Vertices.begin(); vertexItr != this->Vertices.end(); vertexItr++) {
					HyperOp* vertex = *vertexItr;
					HyperOp* parent = vertex->getParentList().front();
					if (vertex->getInstanceof() == hop->getInstanceof() && vertex != hop && vertex->isUnrolledInstance() && vertex->getParentList().size() == 1 && parent->getInstanceof() == parentHyperOp->getFunction() && !parent->isStaticHyperOp()) {
						if (vertex->getParentList().front()->getTargetResource() != vertex->getTargetResource()) {
							canMergeInstances = false;
							break;
						}
						hyperOpPairsToBeMerged.push_back(make_pair(vertex->getParentList().front(), vertex));
					}
				}
				/* Don't want to lose out on load balancing opportunities, hence this check */
				if (!hyperOpPairsToBeMerged.empty() && !canMergeInstances) {
					continue;
				}
				DEBUG(dbgs() << "Merging nodes " << parentHyperOp->asString() << " and " << hop->asString() << "\n");
				mergeHyperOps(parentHyperOp, hop, hyperOpPairsToBeMerged, this);
				change = true;
				clusterItr.erase(hopItr);
				this->print(dbgs());
				break;
			}
		}
	}
}

void HyperOpInteractionGraph::removeCoveredPredicateEdges() {
	//Compute the reaching predicate for each HyperOp
	map<HyperOp*, pair<HyperOpEdge*, HyperOp*> > reachingPredicateForHop;
	for (auto vertexItr = this->Vertices.begin(); vertexItr != this->Vertices.end(); vertexItr++) {
		HyperOp* vertex = *vertexItr;
		if (vertex->getImmediateDominator() != NULL) {
			list<pair<HyperOpEdge*, HyperOp*> > predicateChain = lastPredicateInputUptoParent(vertex, vertex->getImmediateDominator());
			if (!predicateChain.empty()) {
				reachingPredicateForHop[vertex] = make_pair(predicateChain.back().first, predicateChain.back().second);
			}
		}
	}

	bool change = true;
	while (change) {
		change = false;
		for (auto vertexItr = this->Vertices.begin(); vertexItr != this->Vertices.end(); vertexItr++) {
			HyperOp* vertex = *vertexItr;
			if (!vertex->isPredicatedHyperOp()) {
				continue;
			}
			HyperOpEdge* predicateEdge = reachingPredicateForHop[vertex].first;
			HyperOp* predicateProducer = reachingPredicateForHop[vertex].second;
			bool predicateProducerGeneratesData = false;
			HyperOp* otherParent = NULL;
			for (auto parentItr = vertex->ParentMap.begin(); parentItr != vertex->ParentMap.end(); parentItr++) {
				if (parentItr->second == predicateProducer && parentItr->first->getType() != HyperOpEdge::PREDICATE) {
					predicateProducerGeneratesData = true;
				}
				if (parentItr->second != predicateProducer) {
					otherParent = parentItr->second;
				}
			}
			if (predicateProducer != vertex->getImmediateDominator() || predicateProducerGeneratesData || vertex->getParentList().size() != 2) {
				continue;
			}

			assert(otherParent!=NULL);
			if (reachingPredicateForHop[otherParent].first->getValue() != predicateEdge->getValue() || reachingPredicateForHop[otherParent].second != predicateProducer) {
				continue;
			}
			predicateProducer->removeChildEdge(predicateEdge);
			vertex->removeParentEdge(predicateEdge);
			vertex->setPredicatedHyperOp(false);
			reachingPredicateForHop.erase(vertex);
			change = true;
			break;
		}
		if(change){
			this->computeDominatorInfo();
		}
	}
}

void HyperOpInteractionGraph::clusterAllNodesInOne(){
	clusterList.clear();
	clusterList.push_back(Vertices);
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

unsigned inline getExecutionTimeOfCluster(list<HyperOp*> cluster){
	unsigned time = 0;
	for(auto clusterItr = cluster.begin(); clusterItr!=cluster.end(); clusterItr++){
		time+=linearizeTime((*clusterItr)->getExecutionTimeEstimate());
	}
	return time;
}

void HyperOpInteractionGraph::mapClustersToComputeResources() {
	unsigned clusterIndex = 0;
	/* Map of edges between clusters labeled by their index */
	map<HyperOp*, int> hopAndClusterIndex;
	for (auto clusterItr = clusterList.begin(); clusterItr != clusterList.end(); clusterItr++, clusterIndex++) {
		list<HyperOp*> cluster = *clusterItr;
		for (auto hopItr = cluster.begin(); hopItr != cluster.end(); hopItr++) {
			hopAndClusterIndex.insert(make_pair(*hopItr, clusterIndex));
		}
	}

	map<pair<int, int>, int> clusterEdgesList;
	for (auto hopItr = this->Vertices.begin(); hopItr != this->Vertices.end(); hopItr++) {
		HyperOp* hop = *hopItr;
		int clusterIndex = hopAndClusterIndex[hop];
		for (auto hopChildItr = hop->ChildMap.begin(); hopChildItr != hop->ChildMap.end(); hopChildItr++) {
			HyperOp* childHop = hopChildItr->second;
			if (hopAndClusterIndex[childHop] == clusterIndex) {
				continue;
			}
			auto edgeItr = clusterEdgesList.begin();
			int weight = 0;
			for (; edgeItr != clusterEdgesList.end(); edgeItr++) {
				auto edgeKey = edgeItr->first;
				if (edgeKey.first == clusterIndex && edgeKey.second == hopAndClusterIndex[childHop]) {
					weight = edgeItr->second;
					break;
				}
			}

			if (edgeItr != clusterEdgesList.end()) {
				clusterEdgesList.erase(edgeItr);
			}
			weight += linearizeTime(hopChildItr->first->getVolume());
			if (weight == 0) {
				//SYNC/PREDICATE edges
				weight = 1;
			}
			clusterEdgesList.insert(make_pair(make_pair(clusterIndex, hopAndClusterIndex[childHop]), weight));
		}
	}

	SCOTCH_Arch arch;
	SCOTCH_archInit(&arch);
	SCOTCH_archMesh2(&arch, this->rowCount, this->columnCount);

	SCOTCH_Graph* graph = SCOTCH_graphAlloc();

	SCOTCH_Num* vertnbr = (SCOTCH_Num*) malloc(sizeof(SCOTCH_Num));
	vertnbr[0] = clusterList.size();

	/* Execution time of each cluster in an array */
	SCOTCH_Num* velotab = (SCOTCH_Num*) calloc(clusterList.size(), sizeof(SCOTCH_Num));
	clusterIndex = 0;
	for (auto clusterItr = clusterList.begin(); clusterItr != clusterList.end(); clusterItr++, clusterIndex++) {
		list<HyperOp*> cluster = *clusterItr;
		int time = getExecutionTimeOfCluster(cluster);
		assert(time > 0 && "Execution time cant be zero\n");
		velotab[clusterIndex] = time;
	}

	SCOTCH_Num* verttab = (SCOTCH_Num*) calloc(clusterList.size() + 1, sizeof(SCOTCH_Num));
	SCOTCH_Num* vendtab = NULL;
	map<int, map<int, int> > clusterAndTargetEdgesMap;
	int edgeMapSize = 0;
	for (int i = 0; i < clusterList.size(); i++) {
		int numEdges = 0;
		map<int, int> targetClusterWeights;
		for (auto clusterEdgeItr = clusterEdgesList.begin(); clusterEdgeItr != clusterEdgesList.end(); clusterEdgeItr++) {
			if (clusterEdgeItr->first.first == i) {
				targetClusterWeights.insert(make_pair(clusterEdgeItr->first.second, clusterEdgeItr->second));
			}
		}
		verttab[i] = edgeMapSize;
		edgeMapSize += targetClusterWeights.size();
		clusterAndTargetEdgesMap[i] = targetClusterWeights;
	}
	verttab[clusterList.size()] = edgeMapSize;

	SCOTCH_Num* edgetab = NULL;
	SCOTCH_Num* edlotab = NULL;

	if (edgeMapSize > 0) {
		edgetab = (SCOTCH_Num*) malloc(edgeMapSize * sizeof(SCOTCH_Num));
		edlotab = (SCOTCH_Num*) malloc(edgeMapSize * sizeof(SCOTCH_Num));
		int edgeTabIndex = 0;
		for (int i = 0; i < clusterList.size(); i++) {
			map<int, int> targetClusterWeights = clusterAndTargetEdgesMap[i];
			if (targetClusterWeights.size()) {
				for (auto targetClusterItr = targetClusterWeights.begin(); targetClusterItr != targetClusterWeights.end(); targetClusterItr++, edgeTabIndex++) {
					edgetab[edgeTabIndex] = targetClusterItr->first;
					edlotab[edgeTabIndex] = targetClusterItr->second;
				}
			}
		}
	}
	int graphBuildRetVal = SCOTCH_graphBuild(graph, 0, clusterList.size(), verttab, vendtab, velotab, NULL, clusterEdgesList.size(), edgetab, edlotab);
	assert(graphBuildRetVal == 0);

	SCOTCH_Num* parttab = (SCOTCH_Num*) calloc(clusterList.size(), sizeof(SCOTCH_Num));
	SCOTCH_Strat* strat = SCOTCH_stratAlloc();
	SCOTCH_stratInit(strat);
	if (SCOTCH_graphMap(graph, &arch, strat, parttab) == 0) {
		//TODO add assert for map's return value to be correct
		clusterIndex = 0;
		for (auto clusterItr = clusterList.begin(); clusterItr != clusterList.end(); clusterItr++, clusterIndex++) {
			list<HyperOp*> cluster = *clusterItr;
			for (auto hopItr : cluster) {
				if(hopItr->isStartHyperOp()){
					hopItr->setTargetResource(0);
				}else{
					hopItr->setTargetResource(parttab[clusterIndex]);
				}
			}
		}
	} else {
		clusterIndex = 0;
		for (auto clusterItr = clusterList.begin(); clusterItr != clusterList.end(); clusterItr++, clusterIndex++) {
			list<HyperOp*> cluster = *clusterItr;
			for (auto hopItr : cluster) {
				int targetResource = clusterIndex%(rowCount*columnCount);
				hopItr->setTargetResource(targetResource);
			}
		}
	}
	free(edgetab);
	free(edlotab);
	free(velotab);
	free(verttab);
	free(vertnbr);
	free(parttab);
	free(graph);
}

/* Verification passes in that order:
 * 0. Multiple start and end hyperops not allowed
 * 1. Hops cant be both predicated and sync barriers
 * 2. Data arguments from the same producer hyperop dont share the same location
 * 3. There are no cycles in an HIG
 * 4. For each input coming into a context slot, there is at least one parent hyperop producing the data and after self frame argument is added, there are at least 2 arguments in the function
 * 5. Ensure that only start hyperop does not have an immediate dominator and the graph is structured
 * 6. Ensure that there is at least a sync/writecm edge between each producer and consumer
 * 7. There should be no stray functions other than intrinsics or functions attached to hyperops
 * 8. Ensure that the number of inreg arguments per function is less than context frame size
 * 9. Arguments can't be delivered to the first two function arg slots
 * 10. Ensure that context frame base address edges are always at register slot 0
 * 11. Ensure that start hyperop is mapped to CE 0 and is at context frame 0
 * 12. Ensure that no two static hyperops have the same context frame
 */
void HyperOpInteractionGraph::verify(int frameArgsAdded) {
	this->print(dbgs());
	//Check that sync hyperops are not predicated
	HyperOp* startHyperOp = NULL;
	HyperOp* endHyperOp = NULL;
	for (auto vertexItr : Vertices) {
		assert((!(vertexItr->isBarrierHyperOp() && vertexItr->isPredicatedHyperOp())) && "A HyperOp can't be both predicated and sync barrier");
		if (vertexItr->isStartHyperOp()) {
			assert(startHyperOp==NULL && "Multiple Start HyperOps not allowed\n");
			startHyperOp = vertexItr;
		}

		if (vertexItr->isEndHyperOp()) {
			assert(endHyperOp==NULL && "Multiple End HyperOps not allowed\n");
			endHyperOp = vertexItr;
		}
		map<int, list<HyperOp*> > contextSlotAndParentList;
		list<HyperOp*> coveredParents;
		//check to ensure the the incoming edges to a hyperOp don't share the same context slot, and if they do, they come from different hyperops
		for (auto parentEdgeItr : vertexItr->ParentMap) {
			if (parentEdgeItr.first->getType() == HyperOpEdge::SCALAR || parentEdgeItr.first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR) {
				list<HyperOp*> srcList;
				if (contextSlotAndParentList.find(parentEdgeItr.first->getPositionOfContextSlot()) != contextSlotAndParentList.end()) {
					srcList = contextSlotAndParentList.find(parentEdgeItr.first->getPositionOfContextSlot())->second;
					for (auto srcItr : srcList) {
						assert(srcItr != parentEdgeItr.second && "Edges can't share same context slot\n");
					}
				}
				srcList.push_back(parentEdgeItr.second);
				contextSlotAndParentList.insert(make_pair(parentEdgeItr.first->getPositionOfContextSlot(), srcList));
			}
			if (find(coveredParents.begin(), coveredParents.end(), parentEdgeItr.second) == coveredParents.end()
					&& (parentEdgeItr.first->getType() == HyperOpEdge::SCALAR || parentEdgeItr.first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || parentEdgeItr.first->getType() == HyperOpEdge::SYNC || parentEdgeItr.first->getType() == HyperOpEdge::PREDICATE)) {
				coveredParents.push_back(parentEdgeItr.second);
			}
		}
		if (!vertexItr->isUnrolledInstance()) {
			assert(coveredParents.size() == vertexItr->getParentList().size() && "HyperOps must have at least one REDEFINE runtime based communication between every producer and consumer");
		}
	}

	unsigned numVertices = this->Vertices.size();
	bool transitiveClosure[numVertices][numVertices];
	map<HyperOp*, unsigned> hyperOpAndIndexMap;
	unsigned indexMap = 0;
	for (list<HyperOp*>::iterator hopItr = this->Vertices.begin(); hopItr != this->Vertices.end(); hopItr++, indexMap++) {
		hyperOpAndIndexMap[*hopItr] = indexMap;
		for (unsigned i = 0; i < this->Vertices.size(); i++) {
			transitiveClosure[indexMap][i] = 0;
		}
	}
//Populate adjacency matrix
	for (list<HyperOp*>::iterator hopItr = this->Vertices.begin(); hopItr != this->Vertices.end(); hopItr++) {
		list<HyperOp*> children = (*hopItr)->getChildList();
		unsigned producerIndex = hyperOpAndIndexMap[*hopItr];
		for (map<HyperOpEdge*, HyperOp*>::iterator childEdgeItr = (*hopItr)->ChildMap.begin(); childEdgeItr != (*hopItr)->ChildMap.end(); childEdgeItr++) {
			HyperOpEdge::EdgeType edgeType = childEdgeItr->first->getType();
			unsigned consumerIndex = hyperOpAndIndexMap[childEdgeItr->second];
			if (transitiveClosure[producerIndex][consumerIndex] == 0 && (edgeType == HyperOpEdge::SCALAR || edgeType == HyperOpEdge::PREDICATE || edgeType == HyperOpEdge::ORDERING || edgeType == HyperOpEdge::SYNC)) {
				transitiveClosure[producerIndex][consumerIndex] = 1;
			}
		}
	}

//Compute transitive closure
	for (unsigned k = 0; k < numVertices; k++) {
		for (unsigned i = 0; i < numVertices; i++) {
			for (unsigned j = 0; j < numVertices; j++) {
				transitiveClosure[i][j] = transitiveClosure[i][j] || (transitiveClosure[i][k] && transitiveClosure[k][j]);
			}
		}
	}

	for (unsigned i = 0; i < numVertices; i++) {
		for (unsigned j = 0; j < numVertices; j++) {
			assert((!(transitiveClosure[i][j] && transitiveClosure[j][i])) && "Cycle in HIG\n");
		}
	}

	//Ensure that every input to a function has at least one input edge starting at a parent HyperOp
//	for (auto hopItr : this->Vertices) {
//		HyperOp* hop = hopItr;
//		if (hop->isUnrolledInstance()) {
//			continue;
//		}
//		assert((hop->isStartHyperOp()|| (!hop->isStartHyperOp() && hop->getImmediateDominator()!=NULL)) &&"Only start HyperOp does not have an immediate dominator");
//		if (!hop->isStartHyperOp() && !hop->isEndHyperOp() && !hop->isUnrolledInstance()) {
////			assert((hop->getImmediateDominator()->getImmediatePostDominator() == hop->getImmediatePostDominator() || hop->getInRange() || hop->getChildList().size() > 1) && "HIG is not structured");
//		}
//		Function* hopFunction = hop->getFunction();
//
//		int skipArgs = 0;
//		if (frameArgsAdded) {
//			skipArgs++;
//			if (hop->hasRangeBaseInput()) {
//				skipArgs++;
//			}
//		}
//		assert((!frameArgsAdded || hopFunction->getArgumentList().size() >= skipArgs) && "After adding frame and reg args, every hop must have at least 1 argument (2 in case of range hops)");
//
//		int numInRegArgs = 0;
//		int funcArgIndex = 0;
//		for (auto argItr = hopFunction->arg_begin(); argItr != hopFunction->arg_end(); argItr++, funcArgIndex++) {
//			if (hopFunction->getAttributes().hasAttribute(funcArgIndex + 1, Attribute::InReg)) {
//				numInRegArgs++;
//			}
//			if (frameArgsAdded && funcArgIndex < skipArgs) {
//				assert(hopFunction->getAttributes().hasAttribute(funcArgIndex + 1, Attribute::InReg) && "First arg or two args must be marked as in register");
//			}
//			bool funcInputHasValidInput = false;
//			for (auto edgeItr : hop->ParentMap) {
//				assert((!frameArgsAdded || ((edgeItr.first->getType() == HyperOpEdge::SYNC || edgeItr.first->getType() == HyperOpEdge::PREDICATE) || edgeItr.first->getPositionOfContextSlot() >= 1)) && "Arguments can't be delivered to the first arg slot");
//				if (edgeItr.first->getPositionOfContextSlot() == funcArgIndex && std::find(this->Vertices.begin(), this->Vertices.end(), edgeItr.second) != this->Vertices.end()) {
//					funcInputHasValidInput = true;
//					break;
//				}
//			}
//			if (frameArgsAdded) {
//				assert((((funcArgIndex < skipArgs) && !funcInputHasValidInput) || funcInputHasValidInput) && "Invalid input to a function");
//			} else {
//				assert(funcInputHasValidInput && "Invalid input to a function");
//			}
//		}
//		assert(numInRegArgs <= (this->getMaxContextFrameSize() + (frameArgsAdded ? skipArgs : 0)) && "Number of inreg args cannot exceed the number of context frame slots");
//	}

	Module *M = this->Vertices.front()->getFunction()->getParent();
	for (auto funcItr = M->begin(); funcItr != M->end(); funcItr++) {
		Function* func = funcItr;
		assert((!func->hasInternalLinkage()||startHyperOp->getFunction() == func || func->isIntrinsic()|| this->getHyperOp(func)!=NULL || !this->getHyperOpInstances(func).empty()) && "Stray functions are not allowed");
	}

	for (auto hopItr : this->Vertices) {
		if (hopItr->isStartHyperOp()) {
			assert(hopItr->getTargetResource() == 0 && hopItr->getContextFrame() == 0 && "Start hop must be mapped to CE 0 and to context frame 0\n");
			break;
		}
	}
	list<unsigned> occupiedFramesList;
	for (auto hopItr : this->Vertices) {
		assert(hopItr->getTargetResource()<(rowCount*columnCount) && "Target resource can't exceed available hardware");
		if (hopItr->isStaticHyperOp()) {
			assert((occupiedFramesList.empty()|| find(occupiedFramesList.begin(), occupiedFramesList.end(), hopItr->getContextFrame()) == occupiedFramesList.end()) && "Two hyperops mapped to the same context frame in the CR\n");
			occupiedFramesList.push_back(hopItr->getContextFrame());
		}
	}

	/* Ensure that multiplicity strings are supported */
	for (auto hopItr : this->Vertices) {
		for(auto parentItr:hopItr->ParentMap){
			map<HyperOp*, string> multiplicityStrings;
			if(parentItr.first->getMultiplicity().compare("")){
				assert(!parentItr.first->getMultiplicity().compare(ONE_TO_N)|| !parentItr.first->getMultiplicity().compare(N_TO_ONE)|| !parentItr.first->getMultiplicity().compare(ONE_TO_ONE));
				if(parentItr.second->getInRange()){
					assert(!parentItr.first->getMultiplicity().compare(ONE_TO_N)|| !parentItr.first->getMultiplicity().compare(ONE_TO_ONE));
					assert(multiplicityStrings.find(parentItr.second) == multiplicityStrings.end() || multiplicityStrings[parentItr.second] == parentItr.first->getMultiplicity());
				}
			}
		}
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

void HyperOpInteractionGraph::associateStaticContextFrames() {
	for (auto vertexItr : this->Vertices) {
		if(vertexItr->isStartHyperOp()){
			vertexItr->setContextFrame(0);
			break;
		}
	}
	int lastAllocatedFrame = 1;
	/* Allocate a new frame in that cr */
	for (auto vertexItr : this->Vertices) {
		if(!vertexItr->isStartHyperOp() && vertexItr->isStaticHyperOp()){
			/* Compute the new context frame address */
			vertexItr->setContextFrame(lastAllocatedFrame);
			lastAllocatedFrame++;
		}
	}
}

//list<list<pair<HyperOpEdge*, HyperOp*> > > getReachingPredicateChain(HyperOp* currentHyperOp, HyperOp* immediateDominatorHyperOp) {
//	list<list<pair<HyperOpEdge*, HyperOp*> > > predicateChains;
//	//Deliberately commented out for now, testing
////	if (currentHyperOp != immediateDominatorHyperOp) {
//	if (!currentHyperOp->isStartHyperOp()) {
////		errs()<<"traversing "<<currentHyperOp->asString()<<"\n";
//		map<HyperOpEdge*, HyperOp*> traversalMap;
//		list<HyperOp*> parentList = currentHyperOp->getParentList();
//		for (list<HyperOp*>::iterator parentItr = parentList.begin(); parentItr != parentList.end(); parentItr++) {
//			HyperOpEdge* predicateEdge = 0;
//			HyperOpEdge* firstEdge = 0;
//			for (map<HyperOpEdge*, HyperOp*>::iterator edgeItr = currentHyperOp->ParentMap.begin(); edgeItr != currentHyperOp->ParentMap.end(); edgeItr++) {
//				if (edgeItr->second == *parentItr) {
//					if (edgeItr->first->getType() == HyperOpEdge::PREDICATE) {
//						predicateEdge = edgeItr->first;
//						break;
//					} else if (firstEdge == 0) {
//						firstEdge = edgeItr->first;
//					}
//				}
//			}
//
//			if (predicateEdge != 0) {
//				traversalMap.insert(make_pair(predicateEdge, *parentItr));
//			} else if (firstEdge != 0) {
//				traversalMap.insert(make_pair(firstEdge, *parentItr));
//			}
//		}
//
//		for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = traversalMap.begin(); parentItr != traversalMap.end(); parentItr++) {
////		errs() << "considering parent " << parentItr->second->asString() << " for current HyperOp " << currentHyperOp->asString() << "\n";
//			list<list<pair<HyperOpEdge*, HyperOp*> > > reachingPredicates = getReachingPredicateChain(parentItr->second, immediateDominatorHyperOp);
//			if (!parentItr->second->isUnrolledInstance() || !currentHyperOp->isUnrolledInstance()) {
//				if (!reachingPredicates.empty()) {
//					for (list<list<pair<HyperOpEdge*, HyperOp*> > >::iterator reachingPredItr = reachingPredicates.begin(); reachingPredItr != reachingPredicates.end(); reachingPredItr++) {
//						reachingPredItr->push_front(make_pair(parentItr->first, parentItr->second));
//					}
//				} else {
//					list<pair<HyperOpEdge*, HyperOp*> > predPair;
//					predPair.push_front(make_pair(parentItr->first, parentItr->second));
//					reachingPredicates.push_back(predPair);
//				}
//			}
//			for (list<list<pair<HyperOpEdge*, HyperOp*> > >::iterator reachingPredItr = reachingPredicates.begin(); reachingPredItr != reachingPredicates.end(); reachingPredItr++) {
//				if ((!reachingPredItr->empty()) && find(predicateChains.begin(), predicateChains.end(), *reachingPredItr) == predicateChains.end()) {
//					predicateChains.push_back(*reachingPredItr);
//				}
//			}
//		}
//	}
//	return predicateChains;
//}

void setRangeBaseRequired(HyperOp** hop) {
	if ((*hop)->getInRange()) {
		for (auto childItr = (*hop)->ChildMap.begin(); childItr != (*hop)->ChildMap.end(); childItr++) {
			if (childItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || childItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_BASE) {
				(*hop)->setHasRangeBaseInput(true);
				break;
			}
		}
	}
}

void HyperOpInteractionGraph::updateFunctionsForBoundAddress(){
	DEBUG(dbgs() << "Adding args to function if its a range hop with bounds coming in\n");

	for (auto hopItr = this->Vertices.begin(); hopItr != this->Vertices.end(); hopItr++) {
		HyperOp* hop = *hopItr;
		Function* func = hop->getFunction();
		/* Unrolled instances don't have unique functions and don't need updating */
		if (hop->isUnrolledInstance() || !hop->getInRange()) {
			continue;
		}
		list<HyperOpEdge*> incomingBoundEdges;
		for(auto edgeItr = hop->ParentMap.begin(); edgeItr != hop->ParentMap.end(); edgeItr++){
			if(edgeItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_LOWER || edgeItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_UPPER){
				incomingBoundEdges.push_back(edgeItr->first);
			}
		}
		if(incomingBoundEdges.empty()){
			continue;
		}

		Function* replacementFunction;
		list<Type*> newargs;

		for(auto edgeItr:incomingBoundEdges){
			newargs.push_back(Type::getInt32Ty(func->getParent()->getContext()));
		}

		cloneFunction(&hop, newargs, false);
		replacementFunction = hop->getFunction();

		/* Update the function of unrolled instances too */
		for (auto vertexItr : this->Vertices) {
			HyperOp* vertex = vertexItr;
			assert((vertex->getFunction() != func || vertex->isUnrolledInstance()) && "Unrolled instances would be the only ones with stale function\n");
			if (vertex->getFunction() == func) {
				vertex->setFunction(replacementFunction);
			}
			if (vertex->getInstanceof() == func) {
				vertex->setInstanceof(replacementFunction);
			}
		}

		/* Value of context frame args that used in edges to forward are not updated here */
		func->eraseFromParent();
	}
}

/*
 * This method adds 2 register arguments to each function in the beginning of the argument list, indicating that the function gets its own address and the base address of range hop
 */
void HyperOpInteractionGraph::addSelfFrameAddressRegisters() {
	DEBUG(dbgs() << "Adding self context frame addresses and range base address slots, if any\n");
	/* Update range base required flag */
	for (auto hopItr = this->Vertices.begin(); hopItr != this->Vertices.end(); hopItr++) {
		HyperOp* hop = *hopItr;
		Function* func = hop->getFunction();
		setRangeBaseRequired(&hop);
	}

	for (auto hopItr = this->Vertices.begin(); hopItr != this->Vertices.end(); hopItr++) {
		HyperOp* hop = *hopItr;
		Function* func = hop->getFunction();
		/* Unrolled instances don't have unique functions and don't need updating */
		if (hop->isUnrolledInstance()) {
			continue;
		}
		Function* replacementFunction;
		list<Type*> newargs;
		newargs.push_back(Type::getInt32Ty(func->getParent()->getContext()));

		/* Second register is only required in case of range hyperops if they forward any addresses */
		int skipArgs = 1;
		if (hop->hasRangeBaseInput()) {
			newargs.push_back(Type::getInt32Ty(func->getParent()->getContext()));
			skipArgs++;
		}
		cloneFunction(&hop, newargs, true);
		replacementFunction = hop->getFunction();
		replacementFunction->addAttribute(1, Attribute::InReg);
		if (hop->hasRangeBaseInput()) {
			replacementFunction->addAttribute(2, Attribute::InReg);
		}
		/* Update the function of unrolled instances too */
		for (auto vertexItr : this->Vertices) {
			HyperOp* vertex = vertexItr;
			assert((vertex->getFunction() != func || vertex->isUnrolledInstance()) && "Unrolled instances would be the only ones with stale function\n");
			if (vertex->getFunction() == func) {
				vertex->setFunction(replacementFunction);
			}
			if (vertex->getInstanceof() == func) {
				vertex->setInstanceof(replacementFunction);
			}
		}

		/* Update context frame slots of all incoming edges to start from 2 for all args */
		for (auto vertexItr : this->Vertices) {
			HyperOp* vertex = vertexItr;
			for (auto childEdgeItr : vertex->ChildMap) {
				if (childEdgeItr.second->getFunction() == replacementFunction && childEdgeItr.first->getType() != HyperOpEdge::PREDICATE && childEdgeItr.first->getType() != HyperOpEdge::SYNC) {
					childEdgeItr.first->setPositionOfContextSlot(childEdgeItr.first->getPositionOfContextSlot() + skipArgs);
				}
			}
		}

		for (auto hopItr : this->Vertices) {
			if (hopItr->getFunction() == replacementFunction && hop->hasRangeBaseInput()) {
				HyperOpEdge* edge = new HyperOpEdge();
				edge->setType(HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_BASE);
				edge->setPositionOfContextSlot(1);
				edge->setContextFrameAddress(hop);
				edge->setValue(ConstantInt::get(Type::getInt32Ty(hop->getFunction()->getParent()->getContext()), APInt(32, 1)));
				this->addEdge(hop->getImmediateDominator(), hop, edge);
			}
		}
		/* Value of context frame args that used in edges to forward are not updated here */
		func->eraseFromParent();
	}
}

/*
 *  Add synchronization edges between hyperops that communicate only through memory
 */
void HyperOpInteractionGraph::addNecessarySyncEdges() {
	DEBUG(dbgs() << "Computing whether there is a control/scalar path between a pair of HyperOps communicating via memory and adding sync edges if there is no control path\n");
	unsigned numVertices = this->Vertices.size();
	bool transitiveClosure[numVertices][numVertices];
	map<HyperOp*, unsigned> hyperOpAndIndexMap;
	unsigned indexMap = 0;
	for (list<HyperOp*>::iterator hopItr = this->Vertices.begin(); hopItr != this->Vertices.end(); hopItr++, indexMap++) {
		hyperOpAndIndexMap[*hopItr] = indexMap;
		for (unsigned i = 0; i < this->Vertices.size(); i++) {
			transitiveClosure[indexMap][i] = 0;
		}
	}
	//Populate adjacency matrix
	for (list<HyperOp*>::iterator hopItr = this->Vertices.begin(); hopItr != this->Vertices.end(); hopItr++) {
		list<HyperOp*> children = (*hopItr)->getChildList();
		unsigned producerIndex = hyperOpAndIndexMap[*hopItr];
		for (map<HyperOpEdge*, HyperOp*>::iterator childEdgeItr = (*hopItr)->ChildMap.begin(); childEdgeItr != (*hopItr)->ChildMap.end(); childEdgeItr++) {
			HyperOpEdge::EdgeType edgeType = childEdgeItr->first->getType();
			unsigned consumerIndex = hyperOpAndIndexMap[childEdgeItr->second];
			if (transitiveClosure[producerIndex][consumerIndex] == 0
					&& (edgeType == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR || edgeType == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_LOWER || edgeType == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_UPPER || edgeType == HyperOpEdge::SCALAR || edgeType == HyperOpEdge::PREDICATE
							|| edgeType == HyperOpEdge::SYNC)) {
				transitiveClosure[producerIndex][consumerIndex] = 1;
			}
		}
	}

	//Compute transitive closure
	for (unsigned k = 0; k < numVertices; k++) {
		for (unsigned i = 0; i < numVertices; i++) {
			for (unsigned j = 0; j < numVertices; j++) {
				transitiveClosure[i][j] = transitiveClosure[i][j] || (transitiveClosure[i][k] && transitiveClosure[k][j]);
			}
		}
	}

	map<Function*, Function*> originalAndReplacementFunctionMap;
	map<Instruction*, Instruction*> originalInstAndCloneMap;
	list<Function*> functionsForDeletion;
	for (list<HyperOp*>::iterator firstHopItr = this->Vertices.begin(); firstHopItr != this->Vertices.end(); firstHopItr++) {
		HyperOp* producerHyperOp = *firstHopItr;
		unsigned producerIndex = hyperOpAndIndexMap[*firstHopItr];
		list<HyperOp*> previouslyUpdatedChildren;
		map<HyperOp*, HyperOp*> syncBarrierHyperOpMap;

		for (map<HyperOpEdge*, HyperOp*>::iterator childEdgeItr = (*firstHopItr)->ChildMap.begin(); childEdgeItr != (*firstHopItr)->ChildMap.end(); childEdgeItr++) {
			HyperOp* consumerHyperOp = childEdgeItr->second;
			HyperOpEdge::EdgeType edgeType = childEdgeItr->first->getType();
			unsigned consumerIndex = hyperOpAndIndexMap[childEdgeItr->second];
			if (transitiveClosure[producerIndex][consumerIndex] == 0 && edgeType == HyperOpEdge::LOCAL_REFERENCE) {
				//Add a sync edge between producer and consumer since there is no control path between them
				if (!consumerHyperOp->isPredicatedHyperOp()) {
					DEBUG(dbgs() << "adding a sync edge between " << producerHyperOp->asString() << " and " << consumerHyperOp->asString() << "by not doing anything extra\n");
					//Add a sync edge between the source and the consumer
					HyperOpEdge* syncEdge = new HyperOpEdge();
					syncEdge->Type = HyperOpEdge::SYNC;
					producerHyperOp->addChildEdge(syncEdge, consumerHyperOp);
					consumerHyperOp->addParentEdge(syncEdge, producerHyperOp);
					consumerHyperOp->setBarrierHyperOp();
					if (producerHyperOp->getInRange()) {
						consumerHyperOp->addIncomingSyncValue(0, (SyncValue) producerHyperOp);
					} else {
						consumerHyperOp->addIncomingSyncValue(0, (SyncValue) 1);
					}
				}
				//TODO This needs cleaning up later
				else if ((originalAndReplacementFunctionMap.find(consumerHyperOp->getFunction()) == originalAndReplacementFunctionMap.end() && consumerHyperOp->getFunction()->getNumOperands() < this->getMaxMemFrameSize())
						|| originalAndReplacementFunctionMap[consumerHyperOp->getFunction()]->getNumOperands() < this->getMaxContextFrameSize()) {
					DEBUG(dbgs() << "adding sync via context frame from " << producerHyperOp->asString() << " to " << consumerHyperOp->asString() << " by creating a new function\n");

					Function * consumerFunction;
					if (originalAndReplacementFunctionMap.find(consumerHyperOp->getFunction()) == originalAndReplacementFunctionMap.end()) {
						consumerFunction = consumerHyperOp->getFunction();
					} else {
						consumerFunction = originalAndReplacementFunctionMap[consumerHyperOp->getFunction()];
					}

					list<Argument*> scalarArgs;
					list<Argument*> localRefArgs;
					int position = -1;

					map<unsigned, unsigned> originalToReplicatedArgIndexMap;
					map<unsigned, Argument*> replicatedArgIndexMap;
					for (Function::arg_iterator argItr = consumerFunction->arg_begin(); argItr != consumerFunction->arg_end(); argItr++) {
						if (!consumerFunction->getAttributes().hasAttribute(argItr->getArgNo() + 1, Attribute::InReg)) {
							localRefArgs.push_back(argItr);
							continue;
						}
						if (position < argItr->getArgNo()) {
							position = argItr->getArgNo();
						}
						scalarArgs.push_back(argItr);
					}

					if (position == -1) {
						position++;
					}

					vector<Type*> coalescedList;
					unsigned argIndex = 0;
					for (list<Argument*>::iterator scalarArgItr = scalarArgs.begin(); scalarArgItr != scalarArgs.end(); scalarArgItr++, argIndex++) {
						coalescedList.push_back((*scalarArgItr)->getType());
						originalToReplicatedArgIndexMap[argIndex] = argIndex;
					}
					Argument *newArgument = new Argument(Type::getInt32Ty(consumerFunction->getContext()));
					coalescedList.push_back(newArgument->getType());
					unsigned numScalarArgs = scalarArgs.size() + 1;

					for (list<Argument*>::iterator localRefArgItr = localRefArgs.begin(); localRefArgItr != localRefArgs.end(); localRefArgItr++, argIndex++) {
						coalescedList.push_back((*localRefArgItr)->getType());
						originalToReplicatedArgIndexMap[argIndex] = argIndex + 1;
					}

					//TODO
					FunctionType *FT = FunctionType::get(Type::getVoidTy(consumerFunction->getParent()->getContext()), coalescedList, false);
					Function *replacementFunction = Function::Create(FT, Function::ExternalLinkage, consumerFunction->getName(), consumerHyperOp->getFunction()->getParent());

					unsigned replicatedArgIndex = 0;
					for (Function::arg_iterator replicatedArgItr = replacementFunction->arg_begin(); replicatedArgItr != replacementFunction->arg_end(); replicatedArgItr++, replicatedArgIndex++) {
						replicatedArgIndexMap[replicatedArgIndex] = replicatedArgItr;
					}

					for (unsigned i = 1; i <= numScalarArgs; i++) {
						replacementFunction->addAttribute(i, Attribute::InReg);
					}

					map<BasicBlock*, BasicBlock*> originalAndReplicatedBBMap;
					map<Instruction*, Instruction*> localInstCloneMap;
					for (Function::iterator bbItr = consumerFunction->begin(); bbItr != consumerFunction->end(); bbItr++) {
						BasicBlock *newBB = BasicBlock::Create(consumerFunction->getParent()->getContext(), bbItr->getName(), replacementFunction);
						originalAndReplicatedBBMap[bbItr] = newBB;
						for (BasicBlock::iterator instItr = bbItr->begin(); instItr != bbItr->end(); instItr++) {
							Instruction* clonedInst = instItr->clone();
							Instruction* key = 0;
							//Check if the instruction has been inserted previously as a clone of some other instruction
							for (map<Instruction*, Instruction*>::iterator cloneMapItr = originalInstAndCloneMap.begin(); cloneMapItr != originalInstAndCloneMap.end(); cloneMapItr++) {
								if (cloneMapItr->second == instItr) {
									key = cloneMapItr->first;
									break;
								}
							}

							if (key != 0) {
								originalInstAndCloneMap.erase(key);
							} else {
								key = instItr;
							}
							originalInstAndCloneMap[key] = clonedInst;
							localInstCloneMap[instItr] = clonedInst;
							newBB->getInstList().insert(newBB->end(), clonedInst);
						}
					}

					for (Function::iterator bbItr = replacementFunction->begin(); bbItr != replacementFunction->end(); bbItr++) {
						for (BasicBlock::iterator instItr = bbItr->begin(); instItr != bbItr->end(); instItr++) {
							unsigned operatorIndex = 0;
							for (Instruction::op_iterator opItr = instItr->op_begin(); opItr != instItr->op_end(); opItr++, operatorIndex++) {
								unsigned argIndex = 0;
								bool argReplaced = false;
								for (Function::arg_iterator argItr = consumerFunction->arg_begin(); argItr != consumerFunction->arg_end(); argItr++, argIndex++) {
									if (opItr->get() == argItr) {
										Argument* replacementArg = replicatedArgIndexMap[originalToReplicatedArgIndexMap[argIndex]];
										instItr->setOperand(operatorIndex, replacementArg);
										argReplaced = true;
										break;
									}
								}
								if (!argReplaced && !isa<BranchInst>(opItr->get()) && localInstCloneMap.find((Instruction*) opItr->get()) != localInstCloneMap.end()) {
									instItr->setOperand(operatorIndex, localInstCloneMap[(Instruction*) opItr->get()]);
								} else if (originalAndReplicatedBBMap.find((BasicBlock*) opItr->get()) != originalAndReplicatedBBMap.end()) {
									instItr->setOperand(operatorIndex, originalAndReplicatedBBMap[(BasicBlock*) opItr->get()]);
								}
							}
						}
					}

					HyperOpEdge* writecmEdge = new HyperOpEdge();
					writecmEdge->Type = HyperOpEdge::SCALAR;
					writecmEdge->setPositionOfContextSlot(position);
					writecmEdge->setValue(newArgument);

					producerHyperOp->addChildEdge(writecmEdge, consumerHyperOp);
					consumerHyperOp->addParentEdge(writecmEdge, producerHyperOp);
					if (originalAndReplacementFunctionMap.find(consumerHyperOp->getFunction()) != originalAndReplacementFunctionMap.end()) {
						functionsForDeletion.push_back(originalAndReplacementFunctionMap[consumerHyperOp->getFunction()]);
						originalAndReplacementFunctionMap.erase(consumerHyperOp->getFunction());
					}
					originalAndReplacementFunctionMap[consumerHyperOp->getFunction()] = replacementFunction;
				}

				//Update the transitive closure matrix and recompute transitive closure
				transitiveClosure[producerIndex][consumerIndex] = 1;
				for (unsigned k = 0; k < numVertices; k++) {
					for (unsigned i = 0; i < numVertices; i++) {
						for (unsigned j = 0; j < numVertices; j++) {
							transitiveClosure[i][j] = transitiveClosure[i][j] || (transitiveClosure[i][k] && transitiveClosure[k][j]);
						}
					}
				}
			}
		}
	}

	//Update the HyperOps with newly created functions and delete the old ones
	for (map<Function*, Function*>::iterator replacementFunctionItr = originalAndReplacementFunctionMap.begin(); replacementFunctionItr != originalAndReplacementFunctionMap.end(); replacementFunctionItr++) {
		//		HyperOp* hyperOpForUpdate = this->getHyperOp(replacementFunctionItr->first);
		//		errs() << "updating hyperop:" << hyperOpForUpdate->asString() << ", whats in alloca?" << hyperOpForUpdate->loadInstrAndAllocaMap.size() << "\n";
		////Replace the load and alloc map
		//		if (!hyperOpForUpdate->loadInstrAndAllocaMap.empty()) {
		//			errs() << "some entries in allocamap\n";
		//			map<Instruction*, Instruction*> newLoadInstrAndAllocaMap;
		//			//Find the equivalent of each load instruction in the original map and place it in the new one
		//			for (map<Instruction*, Instruction*>::iterator loadItr = hyperOpForUpdate->loadInstrAndAllocaMap.begin(); loadItr != hyperOpForUpdate->loadInstrAndAllocaMap.end(); loadItr++) {
		//				if (originalInstAndCloneMap.find(loadItr->second) != originalInstAndCloneMap.end()) {
		//					newLoadInstrAndAllocaMap[originalInstAndCloneMap[loadItr->first]] = originalInstAndCloneMap[loadItr->second];
		//				} else {
		//					newLoadInstrAndAllocaMap[originalInstAndCloneMap[loadItr->first]] = loadItr->second;
		//				}
		//			}
		//
		//			hyperOpForUpdate->loadInstrAndAllocaMap.clear();
		//			for (map<Instruction*, Instruction*>::iterator updatedMapItr = newLoadInstrAndAllocaMap.begin(); updatedMapItr != newLoadInstrAndAllocaMap.end(); updatedMapItr++) {
		//				hyperOpForUpdate->loadInstrAndAllocaMap.insert(make_pair(updatedMapItr->first, updatedMapItr->second));
		//			}
		//		}
		//
		////replace alloc instructions in other hyperops that possibly use the alloc instruction belonging to the updated the hyperop
		//		for (auto vertexItr : this->Vertices) {
		//			if (vertexItr != hyperOpForUpdate) {
		//				map<Instruction*, Instruction*> tempMap;
		//				for (auto loadInstItr : vertexItr->loadInstrAndAllocaMap) {
		//					if (loadInstItr.second->getParent()->getParent() == hyperOpForUpdate->getFunction()) {
		//						tempMap.insert(make_pair(loadInstItr.first, originalInstAndCloneMap[loadInstItr.second]));
		//					} else {
		//						tempMap.insert(make_pair(loadInstItr.first, loadInstItr.second));
		//					}
		//				}
		//				vertexItr->loadInstrAndAllocaMap = tempMap;
		//			}
		//		}
		//
		//		hyperOpForUpdate->setFunction(replacementFunctionItr->second);
		//
		////Update the edges of outgoing edges of consumer hyperOp
		//		for (map<HyperOpEdge*, HyperOp*>::iterator edgeItr = hyperOpForUpdate->ChildMap.begin(); edgeItr != hyperOpForUpdate->ChildMap.end(); edgeItr++) {
		//			if (edgeItr->first->getValue() != NULL) {
		////				errs() << "edge whose type is " << edgeItr->first->getType() << ", updated edge to contain value from " << ((Instruction*) edgeItr->first->getValue())->getParent()->getParent()->getName() << "\n";
		//				edgeItr->first->setValue(originalInstAndCloneMap[(Instruction*) edgeItr->first->getValue()]);
		////				errs() << "to instead contain value from " << ((Instruction*) edgeItr->first->getValue())->getParent()->getParent()->getName() << "\n";
		//			}
		//		}

		functionsForDeletion.push_back(replacementFunctionItr->first);
	}

	for (list<Function*>::iterator deleteItr = functionsForDeletion.begin(); deleteItr != functionsForDeletion.end(); deleteItr++) {
		errs() << "deleting function:" << (*deleteItr)->getName() << "\n";
		(*deleteItr)->eraseFromParent();
	}
}

/*
 * If the producer and consumer are mapped to different CRs, treat them as localrefs only instead of scalars to avoid reconciles
 */
void HyperOpInteractionGraph::convertRemoteScalarsToStores() {
	DEBUG(dbgs() << "If the producer and consumer are mapped to different CRs, treat them as localrefs only instead of scalars to avoid reconciles\n");
	for (list<HyperOp*>::iterator hopItr = this->Vertices.begin(); hopItr != this->Vertices.end(); hopItr++) {
		HyperOp* hyperOp = *hopItr;
		list<HyperOp*> children = hyperOp->getChildList();
		for (list<HyperOp*>::iterator childHopItr = children.begin(); childHopItr != children.end(); childHopItr++) {
			HyperOp* childHyperOp = *childHopItr;
			if (childHyperOp->getTargetResource() != hyperOp->getTargetResource()) {
				list<HyperOpEdge*> scalarEdgesForConversion;
				bool hasPredOrSyncFromHop = false;
				for (map<HyperOpEdge*, HyperOp*>::iterator childMapItr = hyperOp->ChildMap.begin(); childMapItr != hyperOp->ChildMap.end(); childMapItr++) {
					if(childMapItr->second != childHyperOp){
						continue;
					}
					if (childMapItr->first->getType() == HyperOpEdge::PREDICATE || childMapItr->first->getType() == HyperOpEdge::SYNC){
						hasPredOrSyncFromHop = true;
					}
					if (childMapItr->first->getType() == HyperOpEdge::SCALAR) {
						scalarEdgesForConversion.push_back(childMapItr->first);
					}
				}

				if ((hasPredOrSyncFromHop && scalarEdgesForConversion.size() >= 1) || scalarEdgesForConversion.size() > 1) {
					scalarEdgesForConversion.pop_back();
				}
				for (list<HyperOpEdge*>::iterator edgeItr = scalarEdgesForConversion.begin(); edgeItr != scalarEdgesForConversion.end(); edgeItr++) {
					HyperOpEdge* scalarEdgeForConversion = *edgeItr;
					scalarEdgeForConversion->setType(HyperOpEdge::LOCAL_REFERENCE);
				}
			}
		}
	}
}

/*
 * Once the context frame is full, or a floating point or pointer argument is encountered, arguments must be spilled to memory by converting the edge type
 */
void HyperOpInteractionGraph::convertSpillScalarsToStores() {
	DEBUG(dbgs() << "Convert arguments spilling out of context frames to memory arguments\n");
	for (list<HyperOp*>::iterator hopItr = this->Vertices.begin(); hopItr != this->Vertices.end(); hopItr++) {
		HyperOp* hyperOp = *hopItr;
		Function* hyperOpFunction = hyperOp->getFunction();
		if (hyperOp->isUnrolledInstance()) {
			continue;
		}
		bool firstNonInregEncountered = false;
		int skipArgs = 1;
		if (hyperOp->hasRangeBaseInput()) {
			skipArgs++;
		}
		int argIndex = 0;
		list<int> typeModifiedArgs;
		for (auto argItr = hyperOpFunction->arg_begin(); argItr != hyperOpFunction->arg_end(); argItr++, argIndex++) {
			/* This function must be invoked after the same frame memory args are added to functions, and we must ignore the first two args */
			if (argIndex < skipArgs) {
				continue;
			}
			if (!firstNonInregEncountered && argItr->getType()->isIntegerTy() && (REDEFINEUtils::getSizeOfType(argItr->getType()) / 4) < this->getMaxContextFrameSize() && argIndex < (this->getMaxContextFrameSize() + skipArgs)) {
				//Mark context frame args as inreg
				hyperOpFunction->addAttribute(argIndex + 1, Attribute::InReg);
			} else {
				firstNonInregEncountered = true;
				for (auto oldHopItr : this->Vertices) {
					if (oldHopItr->getFunction() == hyperOpFunction) {
						HyperOp* oldHop = oldHopItr;
						//Change the type of incoming edge to memory based instead of context frame scalar
						for (auto parentItr : oldHop->ParentMap) {
							if (parentItr.first->getPositionOfContextSlot() == argIndex) {
								switch (parentItr.first->getType()) {
								case HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR:
									parentItr.first->setType(HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF);
									break;
								case HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_BASE:
									parentItr.first->setType(HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_BASE_LOCALREF);
									break;
								case HyperOpEdge::SCALAR:
									parentItr.first->setType(HyperOpEdge::LOCAL_REFERENCE);
									break;
								case HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_LOWER:
									parentItr.first->setType(HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_LOCALREF_LOWER);
									break;
								case HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_UPPER:
									parentItr.first->setType(HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_LOCALREF_UPPER);
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	for (auto vertexItr : Vertices) {
		list<int> argTypesForUpdate;
		HyperOp* hop = vertexItr;
		Function* hopFunction = hop->getFunction();
		LLVMContext & ctxt = hopFunction->getParent()->getContext();
		if (hop->isUnrolledInstance()) {
			continue;
		}
		for (auto parentEdgeItr : vertexItr->ParentMap) {
			if (!isScalarEdgeTypeWithContextSlot(parentEdgeItr.first)) {
				argTypesForUpdate.push_back(parentEdgeItr.first->getPositionOfContextSlot());
			}
		}
		if (argTypesForUpdate.empty()) {
			continue;
		}

		int argIndex = 0;
		vector<Type*> newArgsList;
		list<int> updatedArgTypeIndices;
		/* Shuffle all arguments except the first in case of all hyperops and the first two in case of range hyperops */
		for (auto oldArgItr = hopFunction->arg_begin(); oldArgItr != hopFunction->arg_end(); oldArgItr++, argIndex++) {
			if (oldArgItr->getType()->isIntegerTy() || oldArgItr->getType()->isPointerTy()) {
				newArgsList.push_back(oldArgItr->getType());
			} else {
				newArgsList.push_back(oldArgItr->getType()->getPointerTo());
				updatedArgTypeIndices.push_back(argIndex);
			}
		}

		FunctionType *FT = FunctionType::get(hopFunction->getReturnType(), newArgsList, false);
		string newname = hopFunction->getName();
		newname.append(itostr(1));
		Function *newFunction = Function::Create(FT, Function::ExternalLinkage, newname, hopFunction->getParent());

		auto newArgItr = newFunction->arg_begin();
		map<Value*, Value*> oldToNewValueMap;
		int newArgIndex = 0;
		for (auto oldArgItr = hopFunction->arg_begin(); oldArgItr != hopFunction->arg_end(); oldArgItr++, newArgItr++, newArgIndex++) {
			auto oldAttrSet = hopFunction->getAttributes().getParamAttributes(newArgIndex + 1);
			newFunction->addAttributes(newArgIndex + 1, oldAttrSet);
			oldToNewValueMap.insert(make_pair(oldArgItr, newArgItr));
		}
		for (auto funcItr = hopFunction->begin(); funcItr != hopFunction->end(); funcItr++) {
			BasicBlock* oldBB = funcItr;
			BasicBlock* newBB = BasicBlock::Create(ctxt, oldBB->getName(), newFunction);
			oldToNewValueMap.insert(make_pair(oldBB, newBB));
			if(oldBB == &hopFunction->getEntryBlock()){
				for(auto loadArgIndex:updatedArgTypeIndices){
					int argIndex = 0;
					Argument* arg;
					for(auto argItr = newFunction->arg_begin(); argItr!= newFunction->arg_end(); argItr++, argIndex++){
						if(argIndex == loadArgIndex){
							arg = argItr;
							break;
						}
					}
					Value* loadedNewArg = new LoadInst(arg, "", newBB);
					Value* oldArg = NULL;
					argIndex = 0;
					for(auto oldArgItr = hopFunction->arg_begin(); oldArgItr != hopFunction->arg_end(); oldArgItr++, argIndex++){
						if(argIndex == loadArgIndex){
							oldArg = oldArgItr;
							break;
						}
					}
					assert(oldToNewValueMap.find(oldArg) != oldToNewValueMap.end());
					oldToNewValueMap.erase(oldArg);
					oldToNewValueMap[oldArg] = loadedNewArg;
				}
			}
		}
		for (auto bbItr = hopFunction->begin(); bbItr != hopFunction->end(); bbItr++) {
			BasicBlock* oldBB = bbItr;
			//assert(oldToNewValueMap.find(oldBB) != oldToNewValueMap.end() && "Basicblock not cloned before");
			BasicBlock* newBB = (BasicBlock*) oldToNewValueMap[oldBB];
			for (auto instItr = oldBB->begin(); instItr != oldBB->end(); instItr++) {
				Instruction* oldInst = instItr;
				Instruction* newInst = oldInst->clone();
				oldToNewValueMap.insert(make_pair(oldInst, newInst));
				newBB->getInstList().insert(newBB->end(), newInst);
			}
		}

		for (auto instItr = oldToNewValueMap.begin(); instItr != oldToNewValueMap.end(); instItr++) {
			if (isa<Instruction>(instItr->first)) {
				Instruction* oldInst = (Instruction*) instItr->first;
				Instruction * newInst = (Instruction*) instItr->second;
				for (int operandIndex = 0; operandIndex < oldInst->getNumOperands(); operandIndex++) {
					Value* oldOperand = oldInst->getOperand(operandIndex);
					if (oldToNewValueMap.find(oldOperand) != oldToNewValueMap.end()) {
						newInst->setOperand(operandIndex, oldToNewValueMap[oldOperand]);
					}
					if (isa<PHINode>(newInst)) {
						((PHINode*) newInst)->setIncomingBlock(operandIndex, (BasicBlock*) oldToNewValueMap[((PHINode*) newInst)->getIncomingBlock(operandIndex)]);
					}
				}
			}
		}

		/* Update all hops that use the function, including the unrolled ones */
		for (auto vertexItr : this->Vertices) {
			HyperOp* oldHop = vertexItr;
			if (oldHop->getFunction() == hopFunction) {
				oldHop->setFunction(newFunction);
				/* Update the outgoing edges of the hop with new args cloned here */
				for (auto childItr = oldHop->ChildMap.begin(); childItr != oldHop->ChildMap.end(); childItr++) {
					if (childItr->first->getValue() != NULL && oldToNewValueMap.find(childItr->first->getValue()) != oldToNewValueMap.end()) {
						childItr->first->setValue(oldToNewValueMap[childItr->first->getValue()]);
					}
				}

				for (auto vertex : oldHop->getParentGraph()->Vertices) {
					if (vertex->getInRange() && (vertex->getUpperBoundScope() == oldHop || vertex->getLowerBoundScope() == oldHop)) {
						if (vertex->getUpperBoundScope() == oldHop) {
							vertex->setRangeUpperBound(oldToNewValueMap[vertex->getRangeUpperBound()]);
						}
						if (vertex->getLowerBoundScope() == oldHop) {
							vertex->setRangeLowerBound(oldToNewValueMap[vertex->getRangeLowerBound()]);
						}
					}
				}
			}
			if (oldHop->getInstanceof() == hopFunction) {
				oldHop->setInstanceof(newFunction);
			}
		}

		hopFunction->eraseFromParent();
	}
}

/*
 * Shuffle arguments of a HyperOp for better mapping to context frames
 */
void HyperOpInteractionGraph::shuffleHyperOpArguments() {
	Module *M = this->Vertices.front()->getFunction()->getParent();
	LLVMContext &ctxt = M->getContext();
	DEBUG(dbgs() << "Shuffling hyperop arguments to better map args to context frames\n");
	for (auto vertexItr = Vertices.begin(); vertexItr != Vertices.end(); vertexItr++) {
		HyperOp* hop = *vertexItr;
		Function* hopFunction = hop->getFunction();
		if (hop->isUnrolledInstance()) {
			continue;
		}
		map<Argument*, int> oldArgNewIndexMap;
		int argOffset = 0;
		auto oldArgItr = hopFunction->arg_begin();
		oldArgNewIndexMap.insert(make_pair(oldArgItr, 0));
		/* Don't shuffle the first arg */
		oldArgItr++;
		argOffset++;
		if (hop->hasRangeBaseInput()) {
			oldArgNewIndexMap.insert(make_pair(oldArgItr, 1));
			oldArgItr++;
			argOffset++;
		}
		list<Type*> newArgsList;
		/* Shuffle all arguments except the first in case of all hyperops and the first two in case of range hyperops */
		for (; oldArgItr != hopFunction->arg_end(); oldArgItr++) {
			//	funcArgsList.push_back(oldArgItr->getType());
			int newInsertIndex = 0;
			if (oldArgItr->getType()->isIntegerTy()) {
				newArgsList.push_front(oldArgItr->getType());
				newInsertIndex = 0;
				// Update all previously inserted args
				for (auto insertArgItr : oldArgNewIndexMap) {
					if(insertArgItr.second == 0 || (insertArgItr.second == 1 && hop->hasRangeBaseInput())){
						continue;
					}
					oldArgNewIndexMap[insertArgItr.first] = insertArgItr.second + 1;
				}
			} else {
				newArgsList.push_back(oldArgItr->getType());
				newInsertIndex = newArgsList.size() - 1;
			}
			oldArgNewIndexMap.insert(make_pair(oldArgItr, newInsertIndex + argOffset));
		}

		if (hop->hasRangeBaseInput()) {
			newArgsList.push_front(Type::getInt32Ty(ctxt));
		}
		newArgsList.push_front(Type::getInt32Ty(ctxt));

		std::vector<Type*> newArgsVector;
		std::copy(newArgsList.begin(), newArgsList.end(), std::back_inserter(newArgsVector));
		FunctionType *FT = FunctionType::get(hopFunction->getReturnType(), newArgsVector, false);
		string newname = hopFunction->getName();
		newname.append(itostr(1));
		Function *newFunction = Function::Create(FT, Function::ExternalLinkage, newname, hopFunction->getParent());

		auto newArgItr = newFunction->arg_begin();
		map<Value*, Value*> oldToNewValueMap;
		int newArgIndex = 1;
		for (auto oldArgItr = hopFunction->arg_begin(); oldArgItr != hopFunction->arg_end(); oldArgItr++, newArgItr++, newArgIndex++) {
			auto oldAttrSet = hopFunction->getAttributes().getParamAttributes(newArgIndex + 1);
			newFunction->addAttributes(newArgIndex + 1, oldAttrSet);
		}
		newFunction->addAttribute(1, Attribute::InReg);
		if (hop->hasRangeBaseInput()) {
			newFunction->addAttribute(2, Attribute::InReg);
		}
		vector<Argument*> newArgVector;
		for (auto newArgItr = newFunction->arg_begin(); newArgItr != newFunction->arg_end(); newArgItr++) {
			newArgVector.push_back(newArgItr);
		}

		for (auto argItr = hopFunction->arg_begin(); argItr != hopFunction->arg_end(); argItr++) {
			oldToNewValueMap.insert(make_pair(argItr, newArgVector[oldArgNewIndexMap[argItr]]));
		}
		for (auto funcItr = hopFunction->begin(); funcItr != hopFunction->end(); funcItr++) {
			BasicBlock* oldBB = funcItr;
			BasicBlock* newBB = BasicBlock::Create(ctxt, oldBB->getName(), newFunction);
			oldToNewValueMap.insert(make_pair(oldBB, newBB));
		}
		for (auto bbItr = hopFunction->begin(); bbItr != hopFunction->end(); bbItr++) {
			BasicBlock* oldBB = bbItr;
			//assert(oldToNewValueMap.find(oldBB) != oldToNewValueMap.end() && "Basicblock not cloned before");
			BasicBlock* newBB = (BasicBlock*) oldToNewValueMap[oldBB];
			for (auto instItr = oldBB->begin(); instItr != oldBB->end(); instItr++) {
				Instruction* oldInst = instItr;
				Instruction* newInst = oldInst->clone();
				oldToNewValueMap.insert(make_pair(oldInst, newInst));
				newBB->getInstList().insert(newBB->end(), newInst);
			}
		}

		for (auto instItr = oldToNewValueMap.begin(); instItr != oldToNewValueMap.end(); instItr++) {
			if (isa<Instruction>(instItr->first)) {
				Instruction* oldInst = (Instruction*) instItr->first;
				Instruction * newInst = (Instruction*) instItr->second;
				for (int operandIndex = 0; operandIndex < oldInst->getNumOperands(); operandIndex++) {
					Value* oldOperand = oldInst->getOperand(operandIndex);
					if (oldToNewValueMap.find(oldOperand) != oldToNewValueMap.end()) {
						newInst->setOperand(operandIndex, oldToNewValueMap[oldOperand]);
					}
					if (isa<PHINode>(newInst)) {
						((PHINode*) newInst)->setIncomingBlock(operandIndex, (BasicBlock*) oldToNewValueMap[((PHINode*) newInst)->getIncomingBlock(operandIndex)]);
					}
				}
			}
		}

		/* Update all hops that use the function, including the unrolled ones */
		for (auto vertexItr : this->Vertices) {
			HyperOp* oldHop = vertexItr;
			if (oldHop->getFunction() == hopFunction) {
				oldHop->setFunction(newFunction);
				/* Shuffle all context slots for the updated function */
				for (auto parentEdgeItr = oldHop->ParentMap.begin(); parentEdgeItr != oldHop->ParentMap.end(); parentEdgeItr++) {
					auto oldArgItr = hopFunction->arg_begin();
					for (int i = 0; i < parentEdgeItr->first->getPositionOfContextSlot(); i++, oldArgItr++) {
					}
					Argument* oldArg = oldArgItr;
					parentEdgeItr->first->setPositionOfContextSlot(oldArgNewIndexMap[oldArg]);
				}

				/* Update the outgoing edges of the hop with new args cloned here */
				for (auto childItr = oldHop->ChildMap.begin(); childItr != oldHop->ChildMap.end(); childItr++) {
					if (childItr->first->getValue() != NULL && oldToNewValueMap.find(childItr->first->getValue()) != oldToNewValueMap.end()) {
						childItr->first->setValue(oldToNewValueMap[childItr->first->getValue()]);
					}
				}

				for (auto vertex : oldHop->getParentGraph()->Vertices) {
					if (vertex->getInRange() && (vertex->getUpperBoundScope() == oldHop || vertex->getLowerBoundScope() == oldHop)) {
						if (vertex->getUpperBoundScope() == oldHop) {
							vertex->setRangeUpperBound(oldToNewValueMap[vertex->getRangeUpperBound()]);
						}
						if (vertex->getLowerBoundScope() == oldHop) {
							vertex->setRangeLowerBound(oldToNewValueMap[vertex->getRangeLowerBound()]);
						}
					}
				}
			}
			if(oldHop->getInstanceof() == hopFunction){
				oldHop->setInstanceof(newFunction);
			}
		}

		hopFunction->eraseFromParent();
	}
}

/*
 * Computes the reaching predicate with decrement count in case operands to be delivered are on the non taken path
 */
void HyperOpInteractionGraph::addArgDecrementCountOnControlPaths() {
	DEBUG(dbgs() << "Adding argument decrement count on paths when they arent taken\n");
	pair<HyperOpInteractionGraph*, map<HyperOp*, HyperOp*> > controlFlowGraphAndOriginalHopMap = getCFG(this);
	HyperOpInteractionGraph* cfg = controlFlowGraphAndOriginalHopMap.first;
	cfg->computeDominatorInfo();

	unsigned numVertices = this->Vertices.size();
	bool transitiveClosure[numVertices][numVertices];
	map<HyperOp*, unsigned> hyperOpAndIndexMap;
	unsigned indexMap = 0;
	for (list<HyperOp*>::iterator hopItr = this->Vertices.begin(); hopItr != this->Vertices.end(); hopItr++, indexMap++) {
		hyperOpAndIndexMap[*hopItr] = indexMap;
		for (unsigned i = 0; i < this->Vertices.size(); i++) {
			transitiveClosure[indexMap][i] = 0;
		}
	}
	//Populate adjacency matrix
	for (list<HyperOp*>::iterator hopItr = this->Vertices.begin(); hopItr != this->Vertices.end(); hopItr++) {
		list<HyperOp*> children = (*hopItr)->getChildList();
		unsigned producerIndex = hyperOpAndIndexMap[*hopItr];
		for (map<HyperOpEdge*, HyperOp*>::iterator childEdgeItr = (*hopItr)->ChildMap.begin(); childEdgeItr != (*hopItr)->ChildMap.end(); childEdgeItr++) {
			HyperOpEdge::EdgeType edgeType = childEdgeItr->first->getType();
			unsigned consumerIndex = hyperOpAndIndexMap[childEdgeItr->second];
			if (transitiveClosure[producerIndex][consumerIndex] == 0 && (edgeType == HyperOpEdge::SCALAR || edgeType == HyperOpEdge::PREDICATE || edgeType == HyperOpEdge::ORDERING || edgeType == HyperOpEdge::SYNC)) {
				transitiveClosure[producerIndex][consumerIndex] = 1;
			}
		}
	}

	//Compute transitive closure
	for (unsigned k = 0; k < numVertices; k++) {
		for (unsigned i = 0; i < numVertices; i++) {
			for (unsigned j = 0; j < numVertices; j++) {
				transitiveClosure[i][j] = transitiveClosure[i][j] || (transitiveClosure[i][k] && transitiveClosure[k][j]);
			}
		}
	}

	DEBUG(dbgs() << "Delivering reaching predicate with decrement count in case operands to be delivered are on the non taken path\n");
	//	Add predicate delivery edges to HyperOps that are on non taken paths but may have data coming from a HyperOp that precedes the HyperOp producing the predicate
	for (list<HyperOp*>::iterator hopItr = this->Vertices.begin(); hopItr != this->Vertices.end(); hopItr++) {
		HyperOp* hyperOp = *hopItr;
		if (hyperOp->isPredicatedHyperOp()) {
			HyperOp* immediateDominator = hyperOp->getImmediateDominator();
			list<pair<HyperOpEdge*, HyperOp*> > parentPredicateChain = lastPredicateInput(controlFlowGraphAndOriginalHopMap.second[hyperOp]);
			pair<HyperOpEdge*, HyperOp*> parentPredicate;
			if (parentPredicateChain.empty()) {
				continue;
			} else {
				parentPredicate = parentPredicateChain.front();
			}

			list<HyperOp*> parentList = hyperOp->getParentList();
			//				if (immediateDominator == parentPredicate.second || pathExistsInHIG(immediateDominator, parentPredicate.second)) {
			//Add a predicate edge from the predicate parent to the current HyperOp
			HyperOp* parentProducingPredicate;
			for (auto mapItr = controlFlowGraphAndOriginalHopMap.second.begin(); mapItr != controlFlowGraphAndOriginalHopMap.second.end(); mapItr++) {
				if (mapItr->second == parentPredicate.second) {
					parentProducingPredicate = mapItr->first;
					break;
				}
			}
			unsigned decByValue = 0;
			//Count number of inputs coming from other parent nodes which are also predicated by the same HyperOp
			for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = hyperOp->ParentMap.begin(); parentItr != hyperOp->ParentMap.end(); parentItr++) {
				if ((parentItr->first->getType() == HyperOpEdge::SCALAR || parentItr->first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR) && parentItr->second != immediateDominator && transitiveClosure[hyperOpAndIndexMap[parentProducingPredicate]][hyperOpAndIndexMap[parentItr->second]]) {
					decByValue++;
				}
			}
			if (decByValue > 0) {
				HyperOpEdge* predicateEdge;
				if (find(parentList.begin(), parentList.end(), parentProducingPredicate) == parentList.end()) {
					//	Add the edge delivering the reaching predicate
					predicateEdge = new HyperOpEdge();
					predicateEdge->setValue(parentPredicate.first->getValue());
					predicateEdge->setType(HyperOpEdge::PREDICATE);
					predicateEdge->setPredicateValue(parentPredicate.first->getPredicateValue());
				} else {
					//Find the predicate edge originating from parentProducingPredicate
					for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = hyperOp->ParentMap.begin(); parentItr != hyperOp->ParentMap.end(); parentItr++) {
						if (parentItr->second == parentProducingPredicate && parentItr->first->getType() == HyperOpEdge::PREDICATE) {
							predicateEdge = parentItr->first;
							break;
						}
					}
				}

				predicateEdge->setDecrementOperandCount(decByValue);
				hyperOp->setPredicatedHyperOp();
				this->addEdge(parentPredicate.second, hyperOp, predicateEdge);
			}
		}
	}
}

/*
 * This method computes sync count on each path of control for barrier hyperops
 */
void HyperOpInteractionGraph::addSyncCountDecrementOnControlPaths() {
	DEBUG(dbgs() << "Decrementing sync count for nodes with sync edges coming from mutually exclusive paths\n");
//Update the sync count of nodes with sync edges incoming from mutually exclusive paths
	for (list<HyperOp*>::iterator hopItr = this->Vertices.begin(); hopItr != this->Vertices.end(); hopItr++) {
		HyperOp* hyperOp = *hopItr;
		if (hyperOp->isBarrierHyperOp()) {
			//Initialize every incoming path with the same count so that decrements can be performed later
			list<HyperOp*> syncSourceList;
			for (map<HyperOpEdge*, HyperOp*>::iterator parentItr = hyperOp->ParentMap.begin(); parentItr != hyperOp->ParentMap.end(); parentItr++) {
				if (parentItr->first->getType() == HyperOpEdge::SYNC && find(syncSourceList.begin(), syncSourceList.end(), parentItr->second) == syncSourceList.end()) {
					syncSourceList.push_back(parentItr->second);
				}
			}
			//Identify the number of sync tokens expected on each path to a sync barrier hyperop
			list<SyncValue> incomingSyncAlongZeroPred;
			list<SyncValue> incomingSyncAlongOnePred;
			list<SyncValue> incomingSyncAlongNoPred;
			bool syncFromPredicatedSources = false;
			for (list<HyperOp*>::iterator syncSourceItr = syncSourceList.begin(); syncSourceItr != syncSourceList.end(); syncSourceItr++) {
				auto predicateChain = lastPredicateInputUptoParent(*syncSourceItr, hyperOp->getImmediateDominator());
				while (!predicateChain.empty() && predicateChain.front().first->getType() != HyperOpEdge::PREDICATE) {
					predicateChain.pop_front();
				}
				if (!predicateChain.empty()) {
					assert(predicateChain.front().first->getType() == HyperOpEdge::PREDICATE && "Non predicate edge in predicate chain");
					if (!predicateChain.front().first->getPredicateValue()) {
						if ((*syncSourceItr)->getInRange()) {
							//TODO
							incomingSyncAlongZeroPred.push_back((SyncValue) (*syncSourceItr));
						} else {
							incomingSyncAlongZeroPred.push_back((SyncValue) 1);
						}
					} else if (predicateChain.front().first->getPredicateValue()) {
						if ((*syncSourceItr)->getInRange()) {
							//TODO
							incomingSyncAlongOnePred.push_back((SyncValue) (*syncSourceItr));
						} else {
							incomingSyncAlongOnePred.push_back((SyncValue) 1);
						}
					} else {
						if ((*syncSourceItr)->getInRange()) {
							//TODO
							incomingSyncAlongNoPred.push_back((SyncValue) (*syncSourceItr));
						} else {
							incomingSyncAlongNoPred.push_back((SyncValue) 1);
						}
					}
					hyperOp->setIncomingSyncPredicate(predicateChain.front().first->getPredicateValue(), predicateChain.front().first->getValue());
					syncFromPredicatedSources = true;
				} else {
					if ((*syncSourceItr)->getInRange()) {
						//TODO
						incomingSyncAlongNoPred.push_back((SyncValue) (*syncSourceItr));
					} else {
						incomingSyncAlongNoPred.push_back((SyncValue) 1);
					}
				}
			}
			if (syncFromPredicatedSources && hyperOp->getParentList().size() > syncSourceList.size()) {
				hyperOp->setHasMutexSyncSources(true);
			} else {
				hyperOp->setHasMutexSyncSources(false);
			}

			hyperOp->setIncomingSyncCount(0, incomingSyncAlongZeroPred);
			hyperOp->setIncomingSyncCount(1, incomingSyncAlongOnePred);
			hyperOp->setIncomingSyncCount(2, incomingSyncAlongNoPred);
		}
	}
}

HyperOp * HyperOpInteractionGraph::getHyperOp(Function * F) {
	for (list<HyperOp*>::iterator vertexItr = Vertices.begin(); vertexItr != Vertices.end(); vertexItr++) {
		if ((*vertexItr)->getFunction() == F) {
			return (*vertexItr);
		}
	}
	return 0;
}

list<HyperOp*> HyperOpInteractionGraph::getHyperOpInstances(Function * F) {
	list<HyperOp*> instances;
	for (list<HyperOp*>::iterator vertexItr = Vertices.begin(); vertexItr != Vertices.end(); vertexItr++) {
		if ((*vertexItr)->getInstanceof() == F) {
			instances.push_back(*vertexItr);
		}
	}
	return instances;
}

void HyperOpInteractionGraph::print(raw_ostream &os, int debug) {
	os << "digraph{\n";
	if (!this->Vertices.empty()) {
		for (list<HyperOp*>::iterator vertexIterator = Vertices.begin(); vertexIterator != Vertices.end(); vertexIterator++) {
			HyperOp* vertex = *vertexIterator;
			os << vertex->asString(true);
			os << "[";
			if (!vertex->isStaticHyperOp()) {
				os << "style=filled,";
				if (vertex->isUnrolledInstance()) {
					os << "fillcolor=yellow,";
				} else {
					os << "fillcolor=gray,";
				}
				if(vertex->getInRange()){
					os << "color=blue,";
				}
			}
			if (vertex->isPredicatedHyperOp()) {
				os << "shape=polygon,";
			}
			if(vertex->isBarrierHyperOp()){
				os<< "shape=invhouse, ";
			}
			os << "label=\"Name:" << vertex->asString();
			if (debug) {
				os << ",";
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

				os << "Dom:" << dom << ", PostDom:" << postdom << ",";
				os << "Map:" << ((*vertexIterator)->getTargetResource() / columnCount) << ":" << ((*vertexIterator)->getTargetResource() % columnCount) << ", Context frame:" << (*vertexIterator)->getContextFrame() << ",";
//			os << "SyncCount:" << (*vertexIterator)->getSyncCount(0);
				os << "Domf:" << vertex->getDominanceFrontier().size() << ":";
				if (!vertex->getDominanceFrontier().empty()) {
					list<HyperOp*> domf = vertex->getDominanceFrontier();
					for (list<HyperOp*>::iterator domfItr = domf.begin(); domfItr != domf.end(); domfItr++) {
						os << (*domfItr)->asString() << ";";
					}
				}
			}
			os << "\"];\n";

			map<HyperOpEdge*, HyperOp*> children = vertex->ChildMap;
			for (map<HyperOpEdge*, HyperOp*>::iterator childItr = children.begin(); childItr != children.end(); childItr++) {
				os << vertex->asString() << "->" << childItr->second->asString() << "[label=";
				HyperOpEdge* edge = (*childItr).first;
				if (edge->Type == HyperOpEdge::SCALAR) {
					os << "scalar"<<edge->getPositionOfContextSlot();
				} else if (edge->Type == HyperOpEdge::LOCAL_REFERENCE) {
					os << "localref"<<edge->getPositionOfContextSlot();
				} else if (edge->Type == HyperOpEdge::CONTEXT_FRAME_ADDRESS_SCALAR) {
					os << "frameAddress"<<edge->getPositionOfContextSlot()<<edge->getContextFrameAddress()->asString();
				} else if (edge->Type == HyperOpEdge::CONTEXT_FRAME_ADDRESS_LOCALREF) {
					os << "frameAddressmem"<<edge->getContextFrameAddress()->asString();
				} else if (edge->Type == HyperOpEdge::PREDICATE) {
					os << "control"<<edge->getPredicateValue();
				} else if (edge->Type == HyperOpEdge::ORDERING) {
					os << "order";
				} else if (edge->Type == HyperOpEdge::SYNC) {
					os << "sync";
				} else if (edge->Type == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_BASE) {
					os << "cfaddrbase"<<edge->getPositionOfContextSlot()<<edge->getMultiplicity();
				} else if (edge->Type == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_BASE_LOCALREF) {
					os << "cfaddrbaselocalref"<<edge->getMultiplicity();
				}else if(edge->Type == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_LOWER){
					os<<"lowerbound";
				}else if(edge->Type == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_UPPER){
					os<<"upperbound"<<edge->getPositionOfContextSlot()<<edge->getContextFrameAddress()->asString()<<edge->getMultiplicity();
//					edge->getValue()->print(os);
				}else if(edge->Type == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_LOCALREF_LOWER){
					os<<"lowerboundlocaref";
				}else if(edge->Type == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_LOCALREF_UPPER){
					os<<"upperboundlocaref";
				}
				os << "];\n";
			}
		}
	}

	os << "}\n";
}

void setUpdatedMetadata(HyperOp* nodeForRemoval, Instruction** instr, StringRef mdkind) {
	vector<Value*> updatedConsumerList;
	MDNode* consumedByMDNode = (*instr)->getMetadata(mdkind);
	if (consumedByMDNode != NULL) {
		for (unsigned consumerMDNodeIndex = 0; consumerMDNodeIndex != consumedByMDNode->getNumOperands(); consumerMDNodeIndex++) {
			MDNode* consumedByNode = (MDNode*) consumedByMDNode->getOperand(consumerMDNodeIndex);
			if (!(consumedByNode != NULL && !((MDNode*) consumedByNode->getOperand(0))->getOperand(0)->getName().compare(nodeForRemoval->getFunction()->getName()))) {
				updatedConsumerList.push_back(consumedByMDNode);
			}
		}
		if (!updatedConsumerList.empty()) {
			(*instr)->setMetadata(mdkind, MDNode::get(nodeForRemoval->getFunction()->getParent()->getContext(), updatedConsumerList));
		} else {
			(*instr)->setMetadata(mdkind, NULL);
		}
	}
}

/* Ideally, this shouldn't happen, but the previous pass may generate hops incorrectly, this is more a sanity check than anything else */
void HyperOpInteractionGraph::removeUnreachableHops() {
	//This had to be written as follows because removal of one node may cause other nodes to go hanging
	Module* M = this->Vertices.front()->getFunction()->getParent();
	LLVMContext & ctxt = M->getContext();
	while (true) {
		bool updatedGraph = false;
		HyperOp* nodeForRemoval = NULL;
		list<HyperOp*> vertices = this->Vertices;
		for (list<HyperOp*>::iterator vertexItr = vertices.begin(); vertexItr != vertices.end(); vertexItr++) {
			if (!(*vertexItr)->isEndHyperOp() && (*vertexItr)->ChildMap.empty()) {
				nodeForRemoval = *vertexItr;
				break;
			} else if (!(*vertexItr)->isStartHyperOp() && (*vertexItr)->ParentMap.empty()) {
				nodeForRemoval = *vertexItr;
				break;
			}
		}

		if (nodeForRemoval != NULL) {
			/* Update the code of the module by removing the function and all the incoming/outgoing metadata */
			if (!nodeForRemoval->isUnrolledInstance()) {
				nodeForRemoval->getFunction()->eraseFromParent();
			}

			/* Remove the node from the graph, whether unrolled or otherwise, just so the loop can keep continuing */
			this->removeHyperOp(nodeForRemoval);
			updatedGraph = true;
		}
		if (!updatedGraph) {
			break;
		}
	}
}

AllocaInst* HyperOpInteractionGraph::getAllocInstrForLocalReferenceData(Value* sourceInstr, HyperOp* parentInstrContainingSource) {
	if (isa<AllocaInst>(sourceInstr)) {
		return (AllocaInst*) sourceInstr;
	}
	assert(isa<Argument>(sourceInstr) && "Data being passed to another HyperOp must either be created locally or from another HyperOp that passed it as an argument\n");
	int argIndex = -1;
	for (auto argItr = parentInstrContainingSource->getFunction()->arg_begin(); argItr != parentInstrContainingSource->getFunction()->arg_end(); argItr++) {
		argIndex++;
		Argument* arg = argItr;
		if (arg == sourceInstr) {
			break;
		}
	}
	assert(argIndex >= 0 && "Argument not passed to the HyperOp");
	for (auto parentItr = parentInstrContainingSource->ParentMap.begin(); parentItr != parentInstrContainingSource->ParentMap.end(); parentItr++) {
		if (parentItr->first->getPositionOfContextSlot() == argIndex) {
			HyperOp* parent = parentItr->second;
			return getAllocInstrForLocalReferenceData(parentItr->first->getValue(), parent);
		}
	}
	return NULL;
}

void HyperOpInteractionGraph::removeRangeBoundEdges(){
	for(auto vertexItr:this->Vertices){
		map<HyperOpEdge*, HyperOp*> childEdgesForDeletion;
		for(auto childEdgeItr:vertexItr->ChildMap){
			if(childEdgeItr.first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_LOWER || childEdgeItr.first->getType() == HyperOpEdge::CONTEXT_FRAME_ADDRESS_RANGE_COUNT_UPPER){
				childEdgesForDeletion.insert(childEdgeItr);
			}
		}
		for(auto deleteItr:childEdgesForDeletion){
			vertexItr->removeChildEdge(deleteItr.first);
			deleteItr.second->removeParentEdge(deleteItr.first);
			free(deleteItr.first);
		}
	}
}

