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
#include "llvm/IR/Instructions.h"
using namespace llvm;

#include <list>
#include <sstream>
#include<algorithm>
#include <map>
using namespace std;

#include "llvm/IR/InterHyperOpData.h"

static string REDEFINE_ANNOTATIONS = "redefine.annotations";
static string HYPEROP_ANNOTATIONS = "hyperop.annotations";
static string HYPEROP = "HyperOp";
static string HYPEROP_CONSUMED_BY = "ConsumedBy";
static string HYPEROP_CONTROLS = "Controls";
const string HYPEROP_SYNC = "Sync";
const string HYPEROP_RANGE = "Range";
static string HYPEROP_ENTRY = "Entry";
static string HYPEROP_EXIT = "Exit";
static string HYPEROP_PREDICATED = "Predicated";
static string HYPEROP_BARRIER = "Barrier";
static string HYPEROP_INTERMEDIATE = "Intermediate";
static string SCALAR = "Scalar";
static string LOCAL_REFERENCE = "LocalReference";
static string STATIC_HYPEROP = "Static";
static string DYNAMIC_HYPEROP = "Dynamic";
static string HYPEROP_AFFINITY = "Affinity";
static string HYPEROP_FRAME = "Frame";
static string HYPEROP_ID = "Id";

class HyperOp;
class HyperOpInteractionGraph;
#include "llvm/IR/REDEFINEUtils.h"

//(X,Y) coordinates of Tile
typedef pair<unsigned, unsigned> TileCoordinates;

class HyperOpEdge {
	bool isZeroedOut;
	bool isIgnoredEdge;
	int positionOfContextSlot;
	unsigned predicateValue;
	unsigned decrementOperandCount;
	MachineInstr* edgeSource;
	int memoryOffset;
	int memorySize;

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
		CONTEXT_FRAME_ADDRESS_SCALAR,
		CONTEXT_FRAME_ADDRESS_LOCALREF,
		/* This is required in order to avoid address forwarding from all hyperops in a range to the hyperops in the dominance frontier and restrict it to the first hyperop in the range of hops created*/
		CONTEXT_FRAME_ADDRESS_RANGE_BASE,
		CONTEXT_FRAME_ADDRESS_RANGE_BASE_LOCALREF,
		//Edge used for ordering HyperOps to maintain partial order
		ORDERING,
		//Edge to ensure completion of the hyperOp by inserting equivalent delay instruction in the end HyperOp
		SYNC
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
	unsigned getDecrementOperandCount();
	void setDecrementOperandCount(unsigned decrementOperandCount);
	void setEdgeSource(MachineInstr* edgeSource);
	MachineInstr* getEdgeSource();
	int getMemoryOffsetInTargetFrame() const;
	void setMemoryOffsetInTargetFrame(int memoryOffset);
	void clone(HyperOpEdge** clone);
	void setMemorySize(int memorySize);
	int getMemorySize();
};
typedef list<pair<MachineInstr*, MachineInstr*> > PHyperOpInteractionGraph;

enum StrideFunction {
	ADD, MUL, SUB, DIV, MOD
};

enum SyncValueType{
	HYPEROP_SYNC_TYPE,
	INT_SYNC_TYPE
};

struct SyncValue{
	union{
		HyperOp* rangeHyperOpValue;
		int intValue;
	} syncVal;

	SyncValueType type;

	SyncValue(HyperOp*);
	SyncValue(int);
	HyperOp* getHyperOp();
	int getInt();
	SyncValueType getType();
};

class HyperOp {
	/**
	 * Bundle instruction corresponding to the HyperOp
	 */
	Function *function;
	HyperOp* ImmediateDominator;
	HyperOp* ImmediatePostDominator;
	list<HyperOp*> DominanceFrontier;
	bool IsBarrier;
	bool IsStart;
	bool IsEnd;
	bool IsIntermediate;
	bool IsPredicated;
	bool InRange;
	Value* rangeUpperBound;
	Value* rangeLowerBound;
	Value* stride;
	StrideFunction inductionVarUpdateFunc;
	unsigned int TargetResource;
	unsigned contextFrame;
	list<unsigned int> executionTimeEstimate;
	list<unsigned int> topLevel;
	list<unsigned> instanceId;
	bool unrolledInstance;
// The function is an instanceof another function,
//	it is replicated into a newer function because it lets us associate different metadata with the static and dynamic instances
	Function* instanceof;
	bool staticHyperOp;
	unsigned hyperOpId;
	//map of predicate value to sync count
	list<SyncValue> numIncomingSyncEdges[3];
	bool hasMutexSyncSources;
	Value* predicateForSyncSource[2];
	//Map of source instruction in a CE and the first consumer instruction in a different CE
	PHyperOpInteractionGraph pHopDependenceMap;
	HyperOpInteractionGraph* hig;
	bool rangeBaseInput;

public:
//	//Map to cache local reference object sizes that have  an alloc instruction in a different HyperOp
//	map<int, int> localRefSizeMap;
//Map to cache local reference objects that have an alloc instruction in a different HyperOp
	map<Instruction*, Instruction*> loadInstrAndAllocaMap;
	map<HyperOpEdge*, HyperOp*> ParentMap;
	map<HyperOpEdge*, HyperOp*> ChildMap;
	map<unsigned, unsigned> syncCountOnPredicates;
	HyperOp(Function* function, HyperOpInteractionGraph* hig);
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
	void setInRange();
	bool getInRange();
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
	unsigned getHyperOpId() const;
	void setHyperOpId(unsigned hyperOpId);

	//TODO need to create a seperate API for this, the following methods are dependent on the target machine
	unsigned computeDepthInGraph();
	unsigned int getTargetResource() const;
	void setTargetResource(unsigned int targetResource);
	void setContextFrame(unsigned int contextFrame);
	unsigned int getContextFrame() const;
	bool isFbindRequired() const;
	void setFbindRequired(bool fbindRequired);
	bool isStaticHyperOp() const;
	void setStaticHyperOp(bool staticHyperOp);
	void setIncomingSyncCount(unsigned predicateValue, list<SyncValue> syncCountList);
	void addIncomingSyncValue(unsigned predicateValue, SyncValue value);
	list<SyncValue> getSyncCount(unsigned predicateValue);
	list<unsigned> getInstanceId();
	void setInstanceId(list<unsigned> instanceId);
	Function* getInstanceof();
	void setInstanceof(Function* instanceof);
	string asString(bool ignorePeriod = true);
	bool isUnrolledInstance();
	void setIsUnrolledInstance(bool isUnrolledInstance);
	void setFunction(Function* function);
	PHyperOpInteractionGraph getpHyperOpDependenceMap();
	void setpHyperOpDependenceMap(PHyperOpInteractionGraph);
	StrideFunction getInductionVarUpdateFunc();
	void setInductionVarUpdateFunc(StrideFunction inductionVarUpdateFunc);
	Value* getRangeUpperBound();
	void setRangeUpperBound(Value* rangeUpperBound);
	Value* getRangeLowerBound();
	void setRangeLowerBound(Value* rangeLowerBound);
	Value* getStride();
	void setStride(Value* stride);
	bool isHasMutexSyncSources() const;
	void setHasMutexSyncSources(bool hasMutexSyncSources);
	void setIncomingSyncPredicate(unsigned predicateValue, Value* predicate);
	Value* getIncomingSyncPredicate(unsigned predicateValue);
	HyperOpInteractionGraph* getParentGraph();
	void setHasRangeBaseInput(bool);
	bool hasRangeBaseInput();
};

class HyperOpInteractionGraph {
	//list of clusters, each cluster is identified as a list of HyperOps
	list<list<HyperOp*> > clusterList;
	void computeImmediateDominatorInfo();
	void computePostImmediateDominatorInfo();

public:
	unsigned int numContextFrames;
	unsigned int rowCount;
	unsigned int columnCount;
	unsigned int maxMemFrameSize;
	unsigned int maxContextFrameSize;
	list<HyperOp*> Vertices;

	map<StringRef, StrideFunction> StridedFunctionKeyValue;

	HyperOpInteractionGraph();

	void addHyperOp(HyperOp *);

	void removeHyperOp(HyperOp *);

	void addEdge(HyperOp* source, HyperOp * target, HyperOpEdge * edge);
	/**
	 * Associate immediate dominator, immediate post-dominator and dominance frontier information with vertices
	 */
	void computeDominatorInfo();

	void makeGraphStructured();
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

	/*
	 * Adds 2 register arguments to each function in the beginning of the argument list, indicating that the function gets its own address and the base address of range hop
	 */
	void addSelfFrameAddressRegisters();

	/**
	 * Prints in dot format
	 */
	void print(raw_ostream &, int debug = 0);

	HyperOp * getHyperOp(Function* F);

	/**
	 * 	Clusters HyperOps together with the objective of minimizing execution time
	 */
	void clusterNodes();

	/**
	 * 	Map HyperOp Clusters onto compute resources
	 */
	void mapClustersToComputeResources();

	/*
	 * Add all correctness checks on HIG here
	 */
	void verify(int frameArgsAdded = 0);

	virtual ~HyperOpInteractionGraph();

	void setDimensions(unsigned int columnCount, unsigned int rowCount);

	void setNumContextFrames(unsigned int numContextFrames);

	unsigned int getMaxMemFrameSize() const;

	void setMaxMemFrameSize(unsigned int maxFrameSize);

	unsigned int getMaxContextFrameSize() const;

	void setMaxContextFrameSize(unsigned int maxFrameSize);

	HyperOp* getOrCreateHyperOpInstance(Function* function, Function* instanceOf, list<unsigned> instanceId, bool create = true);

	//Returns source-destination tile coordinates for an edge
	list<TileCoordinates> getEdgePathOnNetwork(HyperOp* source, HyperOp* target);

	//Update all the localref edges with memory offsets wrt base 0, needs updating when the functions are lowered to machine functions
	void updateLocalRefEdgeMemSizeAndOffset();

	void removeUnreachableHops();

	void addNecessarySyncEdges();

	void convertRemoteScalarsToStores();

	void addArgDecrementCountOnControlPaths();

	void addSyncCountDecrementOnControlPaths();

	void convertSpillScalarsToStores();

	void shuffleHyperOpArguments();

	AllocaInst* getAllocInstrForLocalReferenceData(Value* sourceInstr, HyperOp* hyperOp);
};
#endif /* LIB_TARGET_RISCV_HYPEROPINTERACTIONGRAPH_H_ */
