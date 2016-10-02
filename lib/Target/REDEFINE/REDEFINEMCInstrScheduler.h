/*
 * REDEFINEMCInstrScheduler.h
 *
 *  Created on: 06-Dec-2015
 *      Author: kavitha
 */

#ifndef LIB_TARGET_REDEFINE_REDEFINEMCINSTRSCHEDULER_H_
#define LIB_TARGET_REDEFINE_REDEFINEMCINSTRSCHEDULER_H_

#include <stddef.h>
#include <cassert>
#include <queue>
#include <vector>

#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/CodeGen/ScheduleDFS.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/CodeGen/FunctionLoweringInfo.h"
#include "REDEFINE.h"
//#include "metis.h"

using namespace llvm;


namespace llvm {

class REDEFINEMCInstrScheduler: public llvm::ScheduleDAGMI {
	static const unsigned SPLOCATIONS = 256;
	//Number of bytes in an addressable location
	static const unsigned datawidth = 4;
	unsigned ceCount;
	unsigned frameSize;
	//This is introduced to spill all the liveout registers in a basic block to be used by successive basic blocks
	map<unsigned, unsigned> registerAndFrameLocation;

	//Instruction and the pHyperOp it belongs to
	list<pair<SUnit*, unsigned> > instructionAndPHyperOpMapForRegion;

	/* Memory locations accessed in a ce are cached as alloca or getelementptr instructions(in Value* form) to ensure that memory accesses are chained across basic blocks.
	This is required since l1 caches are coherent and hence, memory accesses need to be chained
	*/
	vector<const Value*> memoryLocationsAccessedInCE[4];

	//Instruction in a region, the pHyperOp it belongs to and its position in the region
	list<pair<MachineInstr*, pair<unsigned, unsigned> > > allInstructionsOfRegion;

	list<pair<MachineInstr*, pair<unsigned, unsigned> > > allInstructionsOfBB;

	//We need this to do additional code motion and ease creation of pHyperOp bundles
	//TODO
	vector<MachineInstr*> firstInstructionOfpHyperOpInRegion;

	vector<unsigned> registersUsedInBB;

	//Contains t4 and t5 registers, a copy of t5 and their allocated virtual registers in each pHyperOp
	pair<unsigned, unsigned > virtualRegistersForInstAddr[4];

	//Position tracking a new insertion
	unsigned insertPosition = 0;

	//TODO
	//Used to track the SP location in use
	vector<unsigned> faninOfHyperOp;

	//TODO
	//First index corresponds to the CE and the value corresponds to the register containing the base address of the scratch pad location of the consumer CE to which the producer CE is writing to
	int registerContainingBaseAddress[4][4];

	//Register containing code segment+global data segment address
	int memoryFrameBaseAddress[4];

	int memoryFrameMaxSizeReg[4];

	//Tracks the writecm instructions that have already been added in a different machine function; this is required to patch the writecm instructions once the registers corresponding to HyperOp inputs are shuffled
	map<Function*, list<MachineInstr*> > writeInstrToContextFrame;

	const char* HYPEROP_ID_PREFIX = "HyOp#";

	unsigned copyOfInstanceId;
	unsigned REDEFINEphysRegs[16] = {REDEFINE::ra,REDEFINE::sp,REDEFINE::gp,REDEFINE::tp,REDEFINE::t0,REDEFINE::t1,REDEFINE::t2,REDEFINE::t3,REDEFINE::fp,REDEFINE::s0,REDEFINE::s1,REDEFINE::a0,REDEFINE::a1,REDEFINE::a2,REDEFINE::a3,REDEFINE::a4 };
	static const int FRAMES_PER_CR = 832;
	static const int FRAMES_PER_PAGE = 52;

public:
	REDEFINEMCInstrScheduler(MachineSchedContext *C, MachineSchedStrategy *S);
	virtual ~REDEFINEMCInstrScheduler();
	virtual void schedule();
	virtual void scheduleMI(SUnit *SU, bool IsTopNode);
	virtual void enterRegion(MachineBasicBlock *bb, MachineBasicBlock::iterator begin, MachineBasicBlock::iterator end, unsigned endcount);
	virtual void startBlock(MachineBasicBlock *bb);
	virtual void exitRegion();
	virtual void finishBlock();
	virtual void finalizeSchedule();
};

/// \brief Order nodes by the ILP metric.
struct ILPOrder {
	const SchedDFSResult *DFSResult;
	const BitVector *ScheduledTrees;
	bool MaximizeILP;

	ILPOrder(bool MaxILP) :
			DFSResult(0), ScheduledTrees(0), MaximizeILP(MaxILP) {
	}

	/// \brief Apply a less-than relation on node priority.
	///
	/// (Return true if A comes after B in the Q.)
	bool operator()(const SUnit *A, const SUnit *B) const {
		unsigned SchedTreeA = DFSResult->getSubtreeID(A);
		unsigned SchedTreeB = DFSResult->getSubtreeID(B);
		if (SchedTreeA != SchedTreeB) {
			// Unscheduled trees have lower priority.
			if (ScheduledTrees->test(SchedTreeA) != ScheduledTrees->test(SchedTreeB))
				return ScheduledTrees->test(SchedTreeB);

			// Trees with shallower connections have have lower priority.
			if (DFSResult->getSubtreeLevel(SchedTreeA) != DFSResult->getSubtreeLevel(SchedTreeB)) {
				return DFSResult->getSubtreeLevel(SchedTreeA) < DFSResult->getSubtreeLevel(SchedTreeB);
			}
		}
		if (MaximizeILP)
			return DFSResult->getILP(A) < DFSResult->getILP(B);
		else
			return DFSResult->getILP(A) > DFSResult->getILP(B);
	}
};

class REDEFINEILPScheduler: public MachineSchedStrategy {
	/// In case all subtrees are eventually connected to a common root through
	/// data dependence (e.g. reduction), place an upper limit on their size.
	///
	/// FIXME: A subtree limit is generally good, but in the situation commented
	/// above, where multiple similar subtrees feed a common root, we should
	/// only split at a point where the resulting subtrees will be balanced.
	/// (a motivating test case must be found).
	static const unsigned SubtreeLimit = 16;

	ScheduleDAGMI *DAG;
	ILPOrder Cmp;

	std::vector<SUnit*> ReadyQ;
public:
	REDEFINEILPScheduler(bool MaximizeILP) :
			DAG(0), Cmp(MaximizeILP) {
	}

	virtual void initialize(ScheduleDAGMI *dag) {
		DAG = dag;
		DAG->computeDFSResult();
		Cmp.DFSResult = DAG->getDFSResult();
		Cmp.ScheduledTrees = &DAG->getScheduledTrees();
		ReadyQ.clear();
	}

	virtual void registerRoots() {
		// Restore the heap in ReadyQ with the updated DFS results.
		std::make_heap(ReadyQ.begin(), ReadyQ.end(), Cmp);
	}

	/// Implement MachineSchedStrategy interface.
	/// -----------------------------------------

	/// Callback to select the highest priority node from the ready Q.
	virtual SUnit *pickNode(bool &IsTopNode) {
		if (ReadyQ.empty())
			return NULL;
		std::pop_heap(ReadyQ.begin(), ReadyQ.end(), Cmp);
		SUnit *SU = ReadyQ.back();
		ReadyQ.pop_back();
		IsTopNode = true;
		DEBUG(
				dbgs() << "Pick node " << "SU(" << SU->NodeNum << ") " << " ILP: " << DAG->getDFSResult()->getILP(SU) << " Tree: " << DAG->getDFSResult()->getSubtreeID(SU) << " @" << DAG->getDFSResult()->getSubtreeLevel(DAG->getDFSResult()->getSubtreeID(SU)) << '\n' << "Scheduling "
						<< *SU->getInstr());
		return SU;

	}

	/// \brief Scheduler callback to notify that a new subtree is scheduled.
	virtual void scheduleTree(unsigned SubtreeID) {
		std::make_heap(ReadyQ.begin(), ReadyQ.end(), Cmp);
	}

	/// Callback after a node is scheduled. Mark a newly scheduled tree, notify
	/// DFSResults, and resort the priority Q.
	virtual void schedNode(SUnit *SU, bool IsTopNode) {
		assert(IsTopNode && "SchedDFSResult needs top-down");
	}

	virtual void releaseTopNode(SUnit *SU) { /*only called for top roots*/
		ReadyQ.push_back(SU);
		std::push_heap(ReadyQ.begin(), ReadyQ.end(), Cmp);
	}

	virtual void releaseBottomNode(SUnit *SU) {
	}
};

}
#endif /* LIB_TARGET_REDEFINE_REDEFINEMCINSTRSCHEDULER_H_ */
