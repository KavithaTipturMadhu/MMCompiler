#define DEBUG_TYPE "analyzer"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/PassSupport.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetInstrInfo.h"
#include <algorithm>
#include <vector>
#include "llvm/Support/raw_ostream.h"
#include "iostream"
#include "llvm/Pass.h"
#include "llvm/Target/TargetMachine.h"
#include <llvm/Analysis/LoopInfo.h>

#include "WCET.h"
#include "WCET.cpp"
#include "HyperOpInteractionGraph.h"
#include "REDEFINETargetMachine.h"
using namespace std;
using namespace WCET;
using namespace llvm;

namespace llvm
{
	MachineFunctionPass *createWcetAnalyzer();
	void initializeWcetAnalyzerPass(PassRegistry&);
}
namespace
{
struct WcetAnalyzer: public MachineFunctionPass
{
	static char ID; // Pass identification, replacement for typeid

	WcetAnalyzer() :
			MachineFunctionPass(ID) {
		initializeWcetAnalyzerPass(*PassRegistry::getPassRegistry());
	}

	const char *getPassName() const override
	{
		return " Wcet analyzer pass";
	}

	void getAnalysisUsage(AnalysisUsage &AU) const override
	{
		AU.setPreservesCFG();
		AU.setPreservesAll();
		MachineFunctionPass::getAnalysisUsage(AU);
		//AU.addRequired<AliasAnalysis>();
		//AU.addPreservedID(MachineLoopInfoID);
		AU.addRequired<MachineDominatorTree>();
		AU.addRequired<MachineLoopInfo>();
		//AU.addRequired<MemoryDependenceAnalysis>();

	}

	void countBlocksInLoop(MachineLoop *L, unsigned nest)
	{
		unsigned num_Blocks = 0;
		MachineLoop::block_iterator bb;
		L->getTopBlock()->dump();
		L->getBottomBlock()->dump();
		for (bb = L->block_begin(); bb != L->block_end(); ++bb) {
			num_Blocks++;
		}
		errs() << "Loop level " << nest << " has " << num_Blocks << " blocks\n";
		std::vector<MachineLoop*> subLoops = L->getSubLoops();
		MachineLoop::iterator j, f;
		for (j = subLoops.begin(), f = subLoops.end(); j != f; ++j)
			countBlocksInLoop(*j, nest + 1);
	}
	int get_BasicBlockWcet(MachineBasicBlock *B, MachineFunction &MF, bool XDOT)
	{
		WCET::DWAGraph<unsigned int> FUG;
		unsigned int i = 0;
		for (MachineBasicBlock::instr_iterator IItr = B->instr_begin(); IItr != B->instr_end(); IItr++) {
			//DEFINE VARS
//			const TargetMachine *TM;
			const InstrItineraryData *InstrItins;

			//USE MF to GET Target Machine
			const TargetMachine &TM = MF.getTarget();
			//CALL getInstrItineraryData() to instruction Info
			//InstrItins = TM->getInstrItineraryData();
			InstrItins = ((const REDEFINETargetMachine&)TM).getInstrItineraryData();
			if (InstrItins && !InstrItins->isEmpty()) {
				const InstrStage *IS = InstrItins->beginStage(IItr->getDesc().getSchedClass());
				const InstrStage *E = InstrItins->endStage(IItr->getDesc().getSchedClass());
				for (; IS != E; ++IS) {
					int StageDepth = IS->getCycles();
					FUG.add_Edge(i, i + 1, StageDepth);
					i++;
				}
				i++;

			}

		}
		if (i > 6) {
			for (unsigned int k = 0; k < (i - 6); k = k + 6) {
				for (unsigned j = 0; j < 6; j++) {
					FUG.add_Edge(k + j, k + j + 5, 0);
				}
			}
			if (XDOT)
				FUG.xdot();
			return FUG.get_Wcet();
		}
		return 0;

	}

	virtual bool runOnMachineFunction(MachineFunction &MF) override;

};
}

char WcetAnalyzer::ID = 0;
INITIALIZE_PASS_BEGIN(WcetAnalyzer, "Wcet Analyzer"," Wcet analyzer pass", false, false)
	INITIALIZE_PASS_END(WcetAnalyzer, "Wcet Analyzer", " Wcet analyzer pass", false, false)

MachineFunctionPass *llvm::createWcetAnalyzer() {
	return new WcetAnalyzer();
}

bool WcetAnalyzer::runOnMachineFunction(MachineFunction &MF)
{
	errs() << "###################################################################################################" << "\n";
	errs() << "########################################Start WCET#################################################" << "\n";
	errs() << "###################################################################################################" << "\n";

	errs() << "---------------------------------------------------------------------------------------------------\n";
	errs() << "----------------------------------------LATENCY ALGO-----------------------------------------------\n";
	errs() << "---------------------------------------------------------------------------------------------------\n";
	errs() << "I saw a function called " << MF.getName() << "\n";
	for (MachineFunction::iterator bbItr = MF.begin(); bbItr != MF.end(); bbItr++) {
		errs() << "BB: " << bbItr->getNumber() << "\n";
		for (MachineBasicBlock::instr_iterator IItr = bbItr->instr_begin(); IItr != bbItr->instr_end(); IItr++) {
			//DEFINE VARS
			const TargetInstrInfo* TII;
			const InstrItineraryData *InstrItins;

			//USE MF to GET Target Machine
			TII = MF.getTarget().getInstrInfo();
			const TargetMachine &TM = MF.getTarget();

			//CALL getInstrItineraryData() to instruction Info
			InstrItins = ((const REDEFINETargetMachine&)TM).getInstrItineraryData();
			unsigned latency = TII->getInstrLatency(InstrItins, &*IItr);

			errs() << TII->getName(IItr->getOpcode()) << ": \r\t";
			if (InstrItins && !InstrItins->isEmpty()) {
				const InstrStage *IS = InstrItins->beginStage(IItr->getDesc().getSchedClass());
				const InstrStage *E = InstrItins->endStage(IItr->getDesc().getSchedClass());
				for (; IS != E; ++IS) {
					unsigned StageDepth = IS->getCycles();
					errs() << StageDepth << "\t";
				}
			}

			errs() << " Latency:" << latency << "\n";
			//IItr->dump();
		}
	}

	errs() << "---------------------------------------------------------------------------------------------------\n";
	errs() << "-------------------------------------------LOOP ALGO-----------------------------------------------\n";
	errs() << "---------------------------------------------------------------------------------------------------\n";
	MachineLoopInfo &LI = getAnalysis<MachineLoopInfo>();
	for (MachineLoopInfo::iterator LIT = LI.begin(); LIT != LI.end(); ++LIT)
	{
		MachineLoop* ll = *LIT;
		errs() << "Loop: \n";
		ll->dump();
		countBlocksInLoop(ll, 0);
	}

	errs() << "---------------------------------------------------------------------------------------------------\n";
	errs() << "-------------------------------------------Basic Block WCET----------------------------------------\n";
	errs() << "---------------------------------------------------------------------------------------------------\n";
	for (MachineFunction::iterator bbItr = MF.begin(); bbItr != MF.end(); ++bbItr) {
		errs() << "BB #" << bbItr->getNumber() << " WCET:" << get_BasicBlockWcet(bbItr, MF, 0) << "\n";
	}

	errs() << "---------------------------------------------------------------------------------------------------\n";
	errs() << "-------------------------------------------MEMORY ALGO---------------------------------------------\n";
	errs() << "---------------------------------------------------------------------------------------------------\n";
	//MemoryDependenceAnalysis &MDA = getAnalysis<MemoryDependenceAnalysis>();
	//MDA.dump();

	errs() << "---------------------------------------------------------------------------------------------------\n";
	errs() << "----------------------------------------DOMINATOR TREE---------------------------------------------\n";
	errs() << "---------------------------------------------------------------------------------------------------\n";

	errs() << "Function Name " << MF.getName() << "\n";
//CFG START
	MachineDominatorTree &DT = getAnalysis<MachineDominatorTree>();
	DT.dump();
	errs() << "=============================--------------------------------\n";
	for (auto& B : MF) {
		errs() << "Basic block:" << B.getNumber() << "\n";
		for (MachineBasicBlock::succ_iterator succItr = B.succ_begin(); succItr != B.succ_end(); succItr++) {
			errs() << "Successor: " << (*succItr)->getNumber() << "\n";
		}
	}
//CFG ENDS
	errs() << "---------------------------------------------------------------------------------------------------\n";
	errs() << "----------------------------------------GRAPH ALGO-------------------------------------------------\n";
	errs() << "---------------------------------------------------------------------------------------------------\n";

	WCET::DWXGraph<const BasicBlock *> g;
//SOURCE
	g.add_Vertex(DT.getRoot()->getBasicBlock());

	for (auto& B : MF)
	{
		g.add_Vertex(B.getBasicBlock());
	}
	g.add_Vertex((const BasicBlock *) 0);
	int num = 1;
	for (auto& B : MF)
	{
		for (MachineBasicBlock::succ_iterator succItr = B.succ_begin(); succItr != B.succ_end(); succItr++)
		{
			g.add_Edge(B.getBasicBlock(), (*succItr)->getBasicBlock(), get_BasicBlockWcet(&B, MF, 0));
		}
		if (B.succ_empty())
		{
			g.add_Edge(B.getBasicBlock(), (const BasicBlock *) 0, get_BasicBlockWcet(&B, MF, 0));
		}
	}
	//pHyperOp Map

	//g.dump();
	g.xdot();
	errs()<<"HyperOp graph generated\n";
	//g.xdot_CriticalPath();
	//errs() << "\nCritical Path Graph Generated!" << "\n";
//g.GraphProfile();
//g.xdot_DFS();


	errs() << "---------------------------------------------------------------------------------------------------\n";
	errs() << "----------------------------------------HyperOp Graph----------------------------------------------\n";
	errs() << "---------------------------------------------------------------------------------------------------\n";
	typedef WCET::DWAGraph<unsigned long int> HyperOpGraph;
	HyperOpGraph HyperGraph;

	const TargetInstrInfo* TII;
	TII = MF.getTarget().getInstrInfo();
	const TargetMachine &TM = MF.getTarget();
	HyperOpInteractionGraph * HIG = ((REDEFINETargetMachine&) TM).HIG;
	HyperOp* currentHyperOp =	HIG->getHyperOp(const_cast<Function*>(MF.getFunction()));
	PHyperOpInteractionGraph phopDependence = currentHyperOp->getpHyperOpDependenceMap();
	for(auto pHopDependenceItr = phopDependence.begin();pHopDependenceItr!=phopDependence.end();pHopDependenceItr++){
		errs()<<"dependence between instructions :";
		MachineInstr* sourceMI = pHopDependenceItr->first;
		MachineInstr* targetMI = pHopDependenceItr->second;
		sourceMI->dump();
		targetMI->dump();
	}

	for (MachineFunction::const_iterator B = MF.begin(), E = MF.end(); B != E; ++B)
	{
		int pHyperOpIndex = -1;
		for (MachineBasicBlock::const_instr_iterator instrItr = B->instr_begin(); instrItr != B->instr_end(); ++instrItr)
		{

			if (!instrItr->isInsideBundle())
			{
				pHyperOpIndex++;
				//UID uniqueId(pHyperOpIndex,IUID);
				//ILookup.insert(std::make_pair(instrItr,uniqueId))
				if (instrItr->getNumOperands() > 2)
					errs() << "\n\nBasicBlock#" << B->getNumber() << ", pHyperOp#" << pHyperOpIndex << " : " <<\
					TII->getName(instrItr->getOpcode()) << " (" << instrItr->getOpcode() << ") -> " << instrItr->getOperand(2) << "\n";
				else
					errs() << "\n\nBasicBlock#" << B->getNumber() << ", pHyperOp#" << pHyperOpIndex << " : " <<\
					TII->getName(instrItr->getOpcode()) << instrItr->getOpcode() << "\n";
			}
			if (instrItr->getNumOperands() > 2)
				errs() << "BasicBlock#" << B->getNumber() << ", pHyperOp#" << pHyperOpIndex << " : " <<\
				TII->getName(instrItr->getOpcode()) << " (" << instrItr->getOpcode() << ") -> " << instrItr->getOperand(2) << "\n";
			else
				errs() << "BasicBlock#" << B->getNumber() << ", pHyperOp#" << pHyperOpIndex << " : " <<\
				TII->getName(instrItr->getOpcode()) << instrItr->getOpcode() << "\n";
		}
	}
	errs() << "---------------------------------------------------------------------------------------------------\n";
	errs() << "------------------------------------------HIG ALGO-------------------------------------------------\n";
	errs() << "---------------------------------------------------------------------------------------------------\n";

	WCET::DWAGraph<HyperOp *> H;

	for (list<HyperOp*>::iterator HyperopItr = HIG->Vertices.begin(); HyperopItr != HIG->Vertices.end(); ++HyperopItr)
	{

		for(auto childMapItr = (*HyperopItr)->ChildMap.begin();childMapItr!=(*HyperopItr)->ChildMap.end();childMapItr++)
		{
			HyperOpEdge* childEdge = childMapItr->first;
			MachineInstr* edgeSourceInstr=  childEdge->getEdgeSource();
			list<TileCoordinates> edgePath = HIG->getEdgePathOnNetwork((*HyperopItr), childMapItr->second);
			H.add_Edge((*HyperopItr),  childMapItr->second, 0);
			TileCoordinates sourceTile = edgePath.front();
			TileCoordinates targetTile = edgePath.back();
		}
	}
	//H.xdot();
	errs() << "###################################################################################################" << "\n";
	errs() << "########################################WCET Finished##############################################" << "\n";
	errs() << "###################################################################################################" << "\n";

	return false;

}

