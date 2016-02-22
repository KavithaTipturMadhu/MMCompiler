/*
 * EdgePile.h
 *
 *  Created on: 03-Mar-2013
 *      Author: kavitha
 */

#ifndef EDGEPILE_H_
#define EDGEPILE_H_
#include <list>
using namespace std;
#include "Edge.h"
#include "llvm/IR/BasicBlock.h"
using namespace llvm;

namespace llvm {

class EdgePile {
private:
	list<Edge *> edgeList;
public:
	EdgePile();
	virtual ~EdgePile();
	bool isEmpty();
	void clear();
	void addEdge(Edge * edge);
	void removeEdge(Edge * edge);
	Edge * getNextExecutableEdge();
	bool hasEnabledInputEdge(BasicBlock *);
	Edge * getNextEdge(BasicBlock * source);
	bool hasMoreThanOneEnabledInputs(BasicBlock *target);
	list<BasicBlock *> getEnabledPredecessors(BasicBlock * source);
	bool hasEdge(Edge *executableEdge);
	int getSize();
	bool hasEnabledOutputEdge(BasicBlock *);


};
}
#endif /* EDGEPILE_H_ */

