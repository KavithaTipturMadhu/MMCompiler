#include "WCET.h"
#include "WCET.cpp"
using namespace std;
using namespace WCET;
using namespace llvm;

namespace llvm
{
	MachineFunctionPass *createWcetAnalyzer();
	void initializeWcetAnalyzerPass( PassRegistry& );
}
namespace
{
	struct WcetAnalyzer : public MachineFunctionPass
	{
			static char ID; // Pass identification, replacement for typeid

			WcetAnalyzer() :
					MachineFunctionPass( ID )
			{
				initializeWcetAnalyzerPass( *PassRegistry::getPassRegistry() );
			}

			const char *getPassName() const override
			{
				return " Wcet analyzer pass";
			}

			void getAnalysisUsage( AnalysisUsage &AU ) const override
			{
				AU.setPreservesCFG();
				AU.setPreservesAll();
				MachineFunctionPass::getAnalysisUsage( AU );
				//AU.addRequired<AliasAnalysis>();
				//AU.addPreservedID(MachineLoopInfoID);
				AU.addRequired< MachineDominatorTree >();
				AU.addRequired< MachineLoopInfo >();
				//AU.addRequired<MemoryDependenceAnalysis>();

			}

			virtual bool runOnMachineFunction( MachineFunction &MF ) override;

	};
}

char WcetAnalyzer::ID = 0;
INITIALIZE_PASS_BEGIN(WcetAnalyzer, "Wcet Analyzer"," Wcet analyzer pass", false, false)
	INITIALIZE_PASS_END( WcetAnalyzer , "Wcet Analyzer" , " Wcet analyzer pass" , false , false )

MachineFunctionPass *llvm::createWcetAnalyzer()
{
	return new WcetAnalyzer();
}

bool WcetAnalyzer::runOnMachineFunction( MachineFunction &MF )
{
	errs() << "###################################################################################################" << "\n";
	errs() << "########################################Start WCET#################################################" << "\n";
	errs() << "###################################################################################################" << "\n";
	/*
	 errs() << "---------------------------------------------------------------------------------------------------\n";
	 errs() << "----------------------------------------LATENCY ALGO-----------------------------------------------\n";
	 errs() << "---------------------------------------------------------------------------------------------------\n";

	 errs() << "I saw a function called " << MF.getName() << "\n";
	 for (MachineFunction::iterator bbItr = MF.begin(); bbItr != MF.end(); bbItr++)
	 {
	 errs() << "BB: " << bbItr->getNumber() << "\n";
	 for (MachineBasicBlock::instr_iterator IItr = bbItr->instr_begin(); IItr != bbItr->instr_end(); IItr++)
	 {
	 //DEFINE VARS
	 const TargetInstrInfo* TII;
	 const InstrItineraryData *InstrItins;

	 //USE MF to GET Target Machine
	 TII = MF.getTarget().getInstrInfo();
	 const TargetMachine &TM = MF.getTarget();

	 //CALL getInstrItineraryData() to instruction Info
	 InstrItins = ((const REDEFINETargetMachine&) TM).getInstrItineraryData();
	 unsigned latency = TII->getInstrLatency(InstrItins, &*IItr);

	 errs() << TII->getName(IItr->getOpcode()) << ": \r\t";
	 if (InstrItins && !InstrItins->isEmpty())
	 {
	 const InstrStage *IS = InstrItins->beginStage(IItr->getDesc().getSchedClass());
	 const InstrStage *E = InstrItins->endStage(IItr->getDesc().getSchedClass());
	 for (; IS != E; ++IS)
	 {
	 unsigned StageDepth = IS->getCycles();
	 errs() << StageDepth << "\t";
	 }
	 }

	 errs() << " Latency:" << latency << "\n";
	 }
	 }
	 */

	errs() << "---------------------------------------------------------------------------------------------------\n";
	errs() << "-------------------------------------------LOOP ALGO-----------------------------------------------\n";
	errs() << "---------------------------------------------------------------------------------------------------\n";
	MachineLoopInfo &GA = getAnalysis< MachineLoopInfo >();
	MachineDominatorTree &DT = getAnalysis< MachineDominatorTree >();
	/*	LoopAnalysis LA(&GA);
	 LA.xdot();
	 LA.dump();*/

	/*	MachineLoopInfo::iterator LIT = GA.begin();
	 MachineLoop* l = *LIT;
	 LoopSingleIterationWcet LWcet(l);
	 */

	WCET::SingleHyperOpWcet SHO( &MF , &GA , &DT );
	errs() << "HyperOp WCET: " << SHO.get_Wcet() << "\n";

	errs() << "---------------------------------------------------------------------------------------------------\n";
	errs() << "----------------------------------------DOMINATOR TREE---------------------------------------------\n";
	errs() << "---------------------------------------------------------------------------------------------------\n";
	/*
	 errs() << "Machine Function Name: " << MF.getName() << "\n";
	 MachineDominatorTree &DT = getAnalysis<MachineDominatorTree>();
	 WCET::DWAGraph<int> g;
	 for (auto& B : MF)
	 {
	 g.add_Vertex(B.getNumber());
	 }
	 for (auto& B : MF)
	 {
	 //errs() << "Basic block:" << B.getNumber() << "\n";
	 for (MachineBasicBlock::succ_iterator succItr = B.succ_begin(); succItr != B.succ_end(); succItr++)
	 {
	 //errs() << "Successor: " << (*succItr)->getNumber() << "\n";
	 g.add_Edge(B.getNumber(), (*succItr)->getNumber(), 0);
	 }
	 }
	 g.xdot;*/
	/*
	 errs() << "---------------------------------------------------------------------------------------------------\n";
	 errs() << "-------------------------------------------TEST Graph----------------------------------------------\n";
	 errs() << "---------------------------------------------------------------------------------------------------\n";
	 MachineFunction::iterator bbItr = MF.begin();
	 WCET::pHyperOpBasicBlock pbb(bbItr);
	 WCET::pHyperOpBB hbb=pbb.get_pHyperOpBB(0);
	 //hbb.xdot();
	 WCET::pHyperOpControlFlowGraph pcfg(MF);

	 errs() << "---------------------------------------------------------------------------------------------------\n";
	 errs() << "----------------------------------------HyperOp Graph----------------------------------------------\n";
	 errs() << "---------------------------------------------------------------------------------------------------\n";
	 typedef WCET::DWAGraph<unsigned long int> HyperOpGraph;
	 HyperOpGraph HyperGraph;

	 const TargetInstrInfo* TII;
	 TII = MF.getTarget().getInstrInfo();
	 const TargetMachine &TM = MF.getTarget();
	 HyperOpInteractionGraph * HIG = ((REDEFINETargetMachine&) TM).HIG;
	 HyperOp* currentHyperOp = HIG->getHyperOp(const_cast<Function*>(MF.getFunction()));
	 PHyperOpInteractionGraph phopDependence = currentHyperOp->getpHyperOpDependenceMap();
	 for (auto pHopDependenceItr = phopDependence.begin(); pHopDependenceItr != phopDependence.end(); pHopDependenceItr++)
	 {
	 errs() << "dependence between instructions :";
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
	 //RUN ONLY FOR LAST PASS Machine Function
	 if (MF.getFunctionNumber() == ((MF.getFunction())->getParent()->getFunctionList().size() - 2))
	 {
	 WCET::DWAGraph<HyperOp *> H;

	 for (list<HyperOp*>::iterator HyperopItr = HIG->Vertices.begin(); HyperopItr != HIG->Vertices.end(); ++HyperopItr)
	 {

	 for (auto childMapItr = (*HyperopItr)->ChildMap.begin(); childMapItr != (*HyperopItr)->ChildMap.end(); childMapItr++)
	 {
	 HyperOpEdge* childEdge = childMapItr->first;
	 MachineInstr* edgeSourceInstr = childEdge->getEdgeSource();
	 list<TileCoordinates> edgePath = HIG->getEdgePathOnNetwork((*HyperopItr), childMapItr->second);
	 H.add_Edge((*HyperopItr), childMapItr->second, 0);
	 TileCoordinates sourceTile = edgePath.front();
	 TileCoordinates targetTile = edgePath.back();
	 }
	 }
	 //H.xdot();
	 errs() << "HIG graph Created!!!\n";
	 }
	 */

	errs() << "###################################################################################################" << "\n";
	errs() << "########################################WCET Finished##############################################" << "\n";
	errs() << "###################################################################################################" << "\n";

	return false;

}
