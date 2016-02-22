/**
 * HyperOpInteractionGraph.h
 *
 * Header for describing an application as a HyperOp Interaction Graph and its components.
 * Vertices represent a HyperOp while Edges represent data/control dependence.
 * This class has been created to parse the HIG meta-data into a graph and use it for dominance frontier analysis and subsequently, code generation.
 *
 * Created on: 02-Jul-2015
 *      Author: kavitha
 */

#ifndef LIB_TARGET_RISCV_HYPEROPINTERACTIONGRAPH_H_
#define LIB_TARGET_RISCV_HYPEROPINTERACTIONGRAPH_H_

#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#include <list>
#include <sstream>
#include<algorithm>
#include <map>
using namespace std;

#include "llvm/IR/InterHyperOpData.h"

class HyperOpEdge {
		bool isZeroedOut;
		bool isIgnoredEdge;
		int positionOfInput;
	protected:
		string variable;
	public:
		list<unsigned int> volume;
		enum type {
			DATA, CONTROL, ADDRESS
		} Type;
		HyperOpEdge();
		virtual ~HyperOpEdge();
		virtual list<unsigned int> getVolume();
		void zeroOutEdge(bool zeroOut);
		bool isEdgeZeroedOut();
		bool isEdgeIgnored() const;
		void setIsEdgeIgnored(bool isIgnoredEdge);
		int getPositionOfInput() const;
		void setPositionOfInput(int positionOfInput);
		void setName(string name);
		string getName();
};

/**
 * HyperOp properties
 */
#define START_HYPEROP 0
#define	END_HYPEROP 1
#define PREDICATED_HYPEROP 2
#define	LEAF_HYPEROP 3

//No load-stores
#define	CONTENT_HYPEROP 4
#define	BARRIER_HYPEROP 5
#define	CONTINUATION_HYPEROP 6
#define NATIVE_HYPEROP 7
#define STREAMING_HYPEROP 8
#define TASK_HYPEROP 9

class HyperOp {
		/**
		 * Bundle instruction corresponding to the HyperOp
		 */
		Function *function;
		HyperOp* ImmediateDominator;
		HyperOp* ImmediatePostDominator;
		list<HyperOp*> DominanceFrontier;
		list<HyperOp*> CreateFrameList;
		bool InLoop;
		bool IsStart;
		bool IsEnd;
		unsigned int TargetResource;
		unsigned contextFrame;
		list<unsigned int> executionTimeEstimate;
		list<unsigned int> topLevel;
		bool fbindRequired;
		unsigned hyperOpId;

	public:
		map<HyperOpEdge*, HyperOp*> ParentMap;
		map<HyperOpEdge*, HyperOp*> ChildMap;
		HyperOp(Function* function);
		void addParentEdge(HyperOpEdge*, HyperOp *);
		void addChildEdge(HyperOpEdge*, HyperOp*);
		virtual ~HyperOp();
		list<HyperOp*> getParentList();
		list<HyperOp*> getChildList();
		void setImmediateDominator(HyperOp*);
		void setImmediatePostDominator(HyperOp*);
		void setDominanceFrontier(list<HyperOp*>);
		HyperOp* getImmediateDominator();
		HyperOp* getImmediatePostDominator();
		list<HyperOp*> getDominanceFrontier();
		void setStart();
		void setEnd();
		void setInLoop();
		bool isInLoop();
		bool isStartHyperOp();
		bool isEndHyperOp();
		unsigned int getTargetResource() const;
		void setTargetResource(unsigned int targetResource);
		Function* getFunction();
		bool isPredecessor(HyperOp*);
		void setContextFrame(unsigned int contextFrame);
		map<HyperOp*, list<unsigned int> > getChildNodeEdgeWeights();
		void setExecutionTimeEstimate(list<unsigned int> estimate);
		list<unsigned int> getExecutionTimeEstimate();
		void zeroOutChildEdge(HyperOp* childNode, bool zeroOut);
		list<HyperOpEdge*> ignoreChildEdge(HyperOp* childNode, bool ignore);
		void zeroOutParentEdge(HyperOp* parentNode, bool zeroOut);
		list<HyperOpEdge*> ignoreParentEdge(HyperOp* parentNode, bool ignore);
		void removeChildEdge(HyperOpEdge*);
		void removeParentEdge(HyperOpEdge*);
		map<HyperOp*, list<unsigned int> > getParentNodeEdgeWeights();
		list<unsigned int> getTopLevel();
		void setTopLevel(list<unsigned int> topLevel);
		const list<HyperOp*>& getCreateFrameList() const;
		void setCreateFrameList(const list<HyperOp*>& createFrameList);
		unsigned int getContextFrame() const;
		bool isFbindRequired() const;
		void setFbindRequired(bool fbindRequired);
		unsigned getHyperOpId() const;
		void setHyperOpId(unsigned hyperOpId);
};

class DataDependenceEdge: public HyperOpEdge {
	public:
		AggregateData * Data;
		DataDependenceEdge(AggregateData *, string variable);
		list<unsigned int> getVolume();
};

class ControlDependenceEdge: public HyperOpEdge {
	public:
		ControlDependenceEdge();
		list<unsigned int> getVolume();
};

/**
 * Required in case of dynamic mapping where address of a context frame needs to be forwarded to a HyperOp
 */
class ContextFrameAddressFordwardEdge: public HyperOpEdge {
		HyperOp* HyperOpContextFrameForward;
	public:
		ContextFrameAddressFordwardEdge(HyperOp*);
		list<unsigned int> getVolume();
};

class HyperOpInteractionGraph {
		unsigned int numContextFrames;
		unsigned int rowCount;
		unsigned int columnCount;
		//list of clusters, each cluster is identified as a list of HyperOps
		list<list<HyperOp*> > clusterList;
		void computeImmediateDominatorInfo();
		void computePostImmediateDominatorInfo();
	public:
		list<HyperOp*> Vertices;
		HyperOpInteractionGraph();
		void addHyperOp(HyperOp *);
		void addEdge(HyperOp* source, HyperOp * target, HyperOpEdge * edge);
		/**
		 * Associate immediate dominator, immediate post-dominator and dominance frontier information with vertices
		 */
		void computeDominatorInfo();

		/**
		 * Add edges indicating the HyperOp whose context frame address that needs to be forwarded for either writing or deleting frames
		 */
		void addContextFrameAddressForwardingEdges();

		void mapHyperOpsToTarget();
		/**
		 * Static Map of context frames to HyperOps based on liveness of frames.
		 * This needs creating a conflict graph from the liveness information of frames and coloring the graph.
		 */
		void associateStaticContextFrames();

		/**
		 * Prints in dot format
		 */
		void print(raw_ostream &);

		HyperOp * getHyperOp(Function* F);

		/**
		 * 	Clusters HyperOps together with the objective of minimizing execution time
		 */
		void clusterNodes();

		/**
		 * 	Map HyperOp Clusters onto compute resources
		 */
		void mapClustersToComputeResources();

		virtual ~HyperOpInteractionGraph();

		void setDimensions(unsigned int columnCount, unsigned int rowCount);

		void setNumContextFrames(unsigned int numContextFrames);
};
#endif /* LIB_TARGET_RISCV_HYPEROPINTERACTIONGRAPH_H_ */
