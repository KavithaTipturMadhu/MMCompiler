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
				AU.addRequired< MachineDominatorTree >();
				AU.addRequired< MachineLoopInfo >();

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
	static std::map< unsigned , SingleHyperOpWcet * > SingleHyperOpMap;
	static HyperOpGraph HigWcet;
	static HyperOpGraph HigBcet;
	errs() << "###################################################################################################" << "\n";
	errs() << "########################################Start WCET#################################################" << "\n";
	errs() << "###################################################################################################" << "\n";

	errs() << "---------------------------------------------------------------------------------------------------\n";
	errs() << "------------------------------------------HIG ALGO-------------------------------------------------\n";
	errs() << "---------------------------------------------------------------------------------------------------\n";
	//RUN ONLY FOR LAST PASS Machine Function
	if( MF.getFunctionNumber() >= 0 )
	{
		const TargetInstrInfo* TII;
		TII = MF.getTarget().getInstrInfo();
		const TargetMachine &TM = MF.getTarget();
		HyperOpInteractionGraph * HIG = ( ( REDEFINETargetMachine& ) TM ).HIG;
		int rows = ( ( REDEFINETargetMachine& ) TM ).getSubtargetImpl()->getM();
		int column = ( ( REDEFINETargetMachine& ) TM ).getSubtargetImpl()->getN();
		MachineLoopInfo &GA = getAnalysis< MachineLoopInfo >();
		MachineDominatorTree &DT = getAnalysis< MachineDominatorTree >();
		HyperOp* currentHyperOp = HIG->getHyperOp( const_cast< Function* >( MF.getFunction() ) );
		SingleHyperOpWcet *SHO = new SingleHyperOpWcet( &MF , &GA , &DT );
		SingleHyperOpMap.insert( std::make_pair( currentHyperOp->getHyperOpId() , SHO ) );

		errs() << "Completed HyperOp: " << MF.getFunction()->getName() << " , Single HyperOps Covered: " << SingleHyperOpMap.size() << "\n";
		for ( auto itr = SingleHyperOpMap.begin() ; itr != SingleHyperOpMap.end() ; ++itr )
		{
			errs() << "SingleHypeOpWCET: " << itr->first << " , WCET: " << ( itr->second )->get_Wcet() << " , BCET: " << ( itr->second )->get_Bcet() << "\n";
		}

		HigWcet.add_Vertex( ( currentHyperOp ) );
		HigBcet.add_Vertex( ( currentHyperOp ) );

		errs() << "HyperOpID : " << ( currentHyperOp )->getHyperOpId() << "\n";

		for ( auto childMapItr = ( currentHyperOp )->ChildMap.begin() ; childMapItr != ( currentHyperOp )->ChildMap.end() ; childMapItr++ )
		{
			HyperOpEdge* childEdge = childMapItr->first;
			MachineInstr* edgeSourceInstr = childEdge->getEdgeSource();
			if( edgeSourceInstr != NULL )
			{
				HigWcet.add_Edge( currentHyperOp , childMapItr->second , SingleHyperOpMap.at( ( currentHyperOp )->getHyperOpId() )->get_WcetNode( edgeSourceInstr ) );
				HigBcet.add_Edge( currentHyperOp , childMapItr->second , SingleHyperOpMap.at( ( currentHyperOp )->getHyperOpId() )->get_BcetNode( edgeSourceInstr ) );
			}

		}

	}

	if( MF.getFunctionNumber() == ( ( MF.getFunction() )->getParent()->getFunctionList().size() - 2 ) )
	{

		auto ls = HigWcet.get_VertexList();
		for ( auto itr = ls.begin() ; itr != ls.end() ; ++itr )
		{
			if( HigWcet.get_VertexDegree( *itr ) == 0 )
			{
				HigWcet.add_Edge( ( *itr ) , HyperGND , SingleHyperOpMap.at( ( *itr )->getHyperOpId() )->get_Wcet() );
				HigBcet.add_Edge( ( *itr ) , HyperGND , SingleHyperOpMap.at( ( *itr )->getHyperOpId() )->get_Bcet() );
			}
		}
		HigWcet.xdot();
		HigBcet.xdot();
		errs() << "HIG graph Created!!!\n";
	}

	errs() << "###################################################################################################" << "\n";
	errs() << "########################################WCET Finished##############################################" << "\n";
	errs() << "###################################################################################################" << "\n";

	return false;

}
