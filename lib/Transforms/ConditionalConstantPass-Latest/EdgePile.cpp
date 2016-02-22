/*
 * EdgePile.cpp
 *
 *  Created on: 03-Mar-2013
 *      Author: kavitha
 */

#include "EdgePile.h"
using namespace llvm;

EdgePile::EdgePile() {
}

EdgePile::~EdgePile() {
}

bool EdgePile::isEmpty() {
	return (this->edgeList.size() == 0);
}

void EdgePile::clear() {
	this->edgeList.clear();
}

int EdgePile::getSize() {
	return this->edgeList.size();
}
void EdgePile::addEdge(Edge * edge) {
	for (list<Edge *>::const_iterator it = edgeList.begin(); it != edgeList.end(); it++) {
		if ((*it)->getSource() == edge->getSource() && (*it)->getTarget() == edge->getTarget()) {
			return;
		}
	}
	this->edgeList.push_back(edge);
}

void EdgePile::removeEdge(Edge * edge) {
	list<Edge *>::iterator itr;
	for (itr = edgeList.begin(); itr != edgeList.end(); itr++) {
		if (*itr == edge) {
			edgeList.remove(*itr);
			return;
		}
	}
}

Edge* EdgePile::getNextExecutableEdge() {
	Edge *edge = 0;
	for (list<Edge *>::const_iterator it = edgeList.begin(); it != edgeList.end(); it++) {
		if (!(*it)->isExecuted()) {
			edge = (*it);
			break;
		}
	}

	if (edge) {
		removeEdge(edge);
	}
	return edge;
}

Edge* EdgePile::getNextEdge(BasicBlock * source) {
	for (list<Edge *>::const_iterator it = edgeList.begin(); it != edgeList.end(); it++) {
		Edge * edge = (*it);
		removeEdge(edge);
		return edge;
	}
	return 0;
}

bool EdgePile::hasEnabledInputEdge(BasicBlock *target) {
	for (list<Edge *>::const_iterator it = edgeList.begin(); it != edgeList.end(); it++) {
		if ((*it)->getTarget() == target && (*it)->isExecuted()) {
			return true;
		}
	}

	return false;
}

bool EdgePile::hasMoreThanOneEnabledInputs(BasicBlock *target) {
	int count = 0;
	for (list<Edge *>::const_iterator it = edgeList.begin(); it != edgeList.end(); it++) {
		if ((*it)->getTarget() == target && (*it)->isExecuted()) {
			count++;
			if (count > 1) {
				return true;
			}
		}
	}
	return false;
}

list<BasicBlock *> EdgePile::getEnabledPredecessors(BasicBlock * target) {
	list<BasicBlock *> enabledInputList;
	for (list<Edge *>::const_iterator it = edgeList.begin(); it != edgeList.end(); it++) {
		if ((*it)->getTarget() == target && (*it)->isExecuted()) {
			enabledInputList.push_back((*it)->getSource());
		}
	}
	return enabledInputList;
}

bool EdgePile::hasEdge(Edge *executableEdge) {
	if (!this->isEmpty()) {
		for (list<Edge *>::const_iterator it = edgeList.begin(); it != edgeList.end(); it++) {
			if ((*it)->getTarget() == executableEdge->getTarget() && (*it)->getSource() == executableEdge->getSource()) {
				return true;
			}
		}
	}
	return false;
}

bool EdgePile::hasEnabledOutputEdge(BasicBlock * source) {
	for (list<Edge *>::const_iterator it = edgeList.begin(); it != edgeList.end(); it++) {
		if ((*it)->getSource() == source && (*it)->isExecuted()) {
			return true;
		}
	}

	return false;
}
