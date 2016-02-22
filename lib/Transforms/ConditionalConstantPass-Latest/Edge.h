/*
 * Edge.h
 *
 *  Created on: 03-Mar-2013
 *      Author: kavitha
 */

#ifndef GRAPHEDGE_H_
#define GRAPHEDGE_H_
#include "llvm/IR/BasicBlock.h"
using namespace llvm;

namespace llvm {
class Edge {
private:
	BasicBlock * source;
	BasicBlock *target;
	bool executed;
public:
	Edge(BasicBlock *source, BasicBlock *target);
	Edge();
	virtual ~Edge();
	BasicBlock * getSource();
	BasicBlock * getTarget();
	void markEdgeExecutable();
	bool isExecuted();
};
}
#endif /* GRAPHEDGE_H_ */
