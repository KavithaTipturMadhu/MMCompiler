/*
 * Edge.cpp
 *
 *  Created on: 03-Mar-2013
 *      Author: kavitha
 */

#include "Edge.h"
using namespace llvm;

Edge::Edge() {
	this->source = NULL;
	this->target = NULL;
	this->executed = false;
}

Edge::Edge(BasicBlock *source, BasicBlock *target) {
	this->source = source;
	this->target = target;
	this->executed = false;
}

Edge::~Edge() {
}

BasicBlock *Edge::getSource() {
	return this->source;
}
BasicBlock *Edge::getTarget() {
	return this->target;
}

void Edge::markEdgeExecutable() {
	this->executed = true;
}

bool Edge::isExecuted() {
	return this->executed;
}
