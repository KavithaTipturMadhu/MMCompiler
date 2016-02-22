/*
 * RISCVMCInstrScheduler.h
 *
 *  Created on: 06-Dec-2015
 *      Author: kavitha
 */

#ifndef LIB_TARGET_RISCV_RISCVMCINSTRSCHEDULER_H_
#define LIB_TARGET_RISCV_RISCVMCINSTRSCHEDULER_H_

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
using namespace llvm;

namespace llvm {

class RISCVMCInstrScheduler: public llvm::ScheduleDAGMI {
	static const unsigned SPLOCATIONS = 1024;
	static const unsigned ceCount = 4;

public:
	RISCVMCInstrScheduler(MachineSchedContext *C, MachineSchedStrategy *S);
	virtual ~RISCVMCInstrScheduler();
	virtual void schedule();
	virtual void scheduleMI(SUnit *SU, bool IsTopNode);
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
	      if (DFSResult->getSubtreeLevel(SchedTreeA)
	          != DFSResult->getSubtreeLevel(SchedTreeB)) {
	        return DFSResult->getSubtreeLevel(SchedTreeA)
	          < DFSResult->getSubtreeLevel(SchedTreeB);
	      }
	    }
	    if (MaximizeILP)
	      return DFSResult->getILP(A) < DFSResult->getILP(B);
	    else
	      return DFSResult->getILP(A) > DFSResult->getILP(B);
	}
};

class RISCVILPScheduler: public MachineSchedStrategy {
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
	RISCVILPScheduler(bool MaximizeILP) :
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
#endif /* LIB_TARGET_RISCV_RISCVMCINSTRSCHEDULER_H_ */
