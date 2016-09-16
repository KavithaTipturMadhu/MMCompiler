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

static string REDEFINE_ANNOTATIONS = "redefine.annotations";
static string HYPEROP = "HyperOp";
static string HYPEROP_CONSUMED_BY = "ConsumedBy";
static string HYPEROP_CONTROLS = "Controls";
const string HYPEROP_SYNC = "Sync";
static string HYPEROP_AFFINITY = "Affinity";
static string HYPEROP_ENTRY = "Entry";
static string HYPEROP_EXIT = "Exit";
static string HYPEROP_INTERMEDIATE = "Intermediate";
static string SCALAR = "Scalar";
static string LOCAL_REFERENCE = "LocalReference";

class HyperOp;

class HyperOpEdge {
	bool isZeroedOut;
	bool isIgnoredEdge;
	int positionOfContextSlot;
	unsigned predicateValue;

protected:
	Value* variable;
	HyperOp* contextFrameAddress;
public:
	list<unsigned int> volume;
	enum EdgeType {
		SCALAR,
		//Data that cannot be passed as a scalar is passed as reference instead through memory
		LOCAL_REFERENCE,
		PREDICATE,
		CONTEXT_FRAME_ADDRESS,
		//Edge used for ordering HyperOps such that
		ORDERING,
		//Edge to ensure completion of the hyperOp by inserting equivalent delay instruction in the end HyperOp
		SYNC,
	} Type;
	HyperOpEdge();
	virtual ~HyperOpEdge();
	list<unsigned int> getVolume();
	void setVolume(list<unsigned> volume);
	void zeroOutEdge(bool zeroOut);
	bool isEdgeZeroedOut();
	bool isEdgeIgnored() const;
	void setIsEdgeIgnored(bool isIgnoredEdge);
	int getPositionOfContextSlot();
	void setPositionOfContextSlot(int positionOfInput);
	void setValue(Value*);
	Value* getValue();
	EdgeType getType();
	void setType(EdgeType type);
	HyperOp* getContextFrameAddress();
	void setContextFrameAddress(HyperOp* contextFrameAddress);
	unsigned getPredicateValue();
	void setPredicateValue(unsigned predicateValue);
};

class HyperOp {
	/**
	 * Bundle instruction corresponding to the HyperOp
	 */
	Function *function;
	HyperOp* ImmediateDominator;
	HyperOp* ImmediatePostDominator;
	list<HyperOp*> DominanceFrontier;
	list<HyperOp*> CreateFrameList;
	bool IsBarrier;
	bool IsStart;
	bool IsEnd;
	bool IsIntermediate;
	bool IsPredicated;
	unsigned int TargetResource;
	unsigned contextFrame;
	list<unsigned int> executionTimeEstimate;
	list<unsigned int> topLevel;
	list<unsigned> instanceId;
	bool unrolledInstance;
// The function is an instanceof another function,
//	it is replicated into a newer function because it lets us associate different metadata with the static and dynamic instances
	Function* instanceof;
	bool fbindRequired;
	bool staticHyperOp;
	//Frame to be garbage collected?
	bool gcRequired;
	unsigned hyperOpId;
	vector<unsigned> numInputsPerCE;
	unsigned int numIncomingSyncEdges;

public:
	//Map to cache local reference objects that have an alloc instruction in a different HyperOp
	map<Instruction*, Instruction*> loadInstrAndAllocaMap;
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
	void setStartHyperOp();
	void setEndHyperOp();
	void setIntermediateHyperOp();
	void setBarrierHyperOp();
	void setPredicatedHyperOp();
	bool isBarrierHyperOp();
	bool isStartHyperOp();
	bool isEndHyperOp();
	bool isPredicatedHyperOp();
	bool isIntermediateHyperOp();
	Function* getFunction();
	bool isPredecessor(HyperOp*);
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
	unsigned getHyperOpId() const;
	void setHyperOpId(unsigned hyperOpId);

	//TODO need to create a seperate API for this, the following methods are dependent on the target machine
	void setNumCEInputs(unsigned pHyperOpId, unsigned numInputs);
	void setNumCEs(unsigned numPHyperOps);
	unsigned getNumInputsPerCE(unsigned pHyperOpId);
	unsigned computeDepthInGraph();
	bool frameNeedsGC() const;
	void setFrameNeedsGC(bool intrinsicFrame);
	unsigned int getTargetResource() const;
	void setTargetResource(unsigned int targetResource);
	void setContextFrame(unsigned int contextFrame);
	unsigned int getContextFrame() const;
	bool isFbindRequired() const;
	void setFbindRequired(bool fbindRequired);
	bool isStaticHyperOp() const;
	void setStaticHyperOp(bool staticHyperOp);
	void incrementIncomingSyncCount();
	unsigned getSyncCount();
	list<unsigned> getInstanceId();
	void setInstanceId(list<unsigned> instanceId);
	Function* getInstanceof();
	void setInstanceof(Function* instanceof);
	string asString();
	bool isUnrolledInstance();
	void setIsUnrolledInstance(bool isUnrolledInstance);
};

class HyperOpInteractionGraph {
	unsigned int numContextFrames;
	unsigned int rowCount;
	unsigned int columnCount;
	unsigned int maxMemFrameSize;
	unsigned int maxContextFrameSize;
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
	 *Minimize the edges introduced due to clustering since we don't need (nor can we support) so many control edges flowing around
	 */
	void minimizeControlEdges();

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

	unsigned int getMaxMemFrameSize() const;

	void setMaxMemFrameSize(unsigned int maxFrameSize);

	unsigned int getMaxContextFrameSize() const;

	void setMaxContextFrameSize(unsigned int maxFrameSize);

	HyperOp* getOrCreateHyperOpInstance(Function* function, Function* instanceOf, list<unsigned> instanceId);

};
#endif /* LIB_TARGET_RISCV_HYPEROPINTERACTIONGRAPH_H_ */
