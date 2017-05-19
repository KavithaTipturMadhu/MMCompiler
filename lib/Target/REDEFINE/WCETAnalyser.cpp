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
	errs() << "------------------------------------CACHE ANALYSIS ALGO--------------------------------------------\n";
	errs() << "---------------------------------------------------------------------------------------------------\n";
	const TargetInstrInfo* TII;
	const InstrItineraryData *InstrItins;
	const TargetMachine &TM = MF.getTarget();
	TII = MF.getTarget().getInstrInfo();

	for ( auto bitr = MF.begin() ; bitr != MF.end() ; ++bitr )
	{
		errs() << "MBB Name: " << bitr->getName() << " , BB Name: " << bitr->getBasicBlock()->getName() << "\n";
		for ( auto itr = bitr->instr_begin() ; itr != bitr->instr_end() ; ++itr )
		{

			if( !itr->memoperands_empty() )
			{
				for ( auto mitr = itr->memoperands_begin() ; mitr != itr->memoperands_end() ; ++mitr )
				{
					errs() << "\t\tVAR VAL: " << "Ptr: " << ( *mitr )->getValue() << " , IR Instruction " << * ( ( *mitr )->getValue() ) << " , OPCODE: " << itr->getOpcode() << "\n\t\t";
				}
			}
			else
			{
				itr->dump();
			}
			if( itr->mayLoad( llvm::MachineInstr::QueryType::AnyInBundle ) || itr->mayStore( llvm::MachineInstr::QueryType::AnyInBundle ) )
			{
				errs() << "Inside: load&store: ";
				itr->dump();
			}

		}
	}
	errs() << "---------------------------------------------------------------------------------------------------\n";
	errs() << "--------------------------------------Global ANALYSIS ALGO--------------------------------------------\n";
	errs() << "---------------------------------------------------------------------------------------------------\n";

	std::map< StringRef , long int > globalVarStartAddressMap;
	long int maxGlobalSize = 0;
	const Module* parentModule = MF.getFunction()->getParent(); //MO.getParent()->getParent()->getParent()->getFunction()->getParent();
	for ( Module::const_global_iterator globalArgItr = parentModule->global_begin() ; globalArgItr != parentModule->global_end() ; globalArgItr++ )
	{
		const GlobalVariable *globalVar = &*globalArgItr;
		globalVar->dump();
		errs() << "Global Variable: " << globalVar->getName() << " , Type" << globalVar->getType()->getTypeID() << " , ID " << globalVar->getValueID() << "\n";
		globalVarStartAddressMap.insert( std::make_pair( globalVar->getName() , maxGlobalSize ) );
		maxGlobalSize += REDEFINEUtils::getAlignedSizeOfType( globalVar->getType() );
	}
	errs() << "maxGlobal Size " << maxGlobalSize << "\n";

	errs() << "---------------------------------------------------------------------------------------------------\n";
	errs() << "--------------------------------------Local ANALYSIS ALGO--------------------------------------------\n";
	errs() << "---------------------------------------------------------------------------------------------------\n";

	//Get the location of the stack allocated object in the basic block containing the load instruction and not the alloca instruction because alloca might belong
	//Arguments have negative index and are added in memory locations that succeed the locals of the stack frame
	unsigned frameLocationOfSourceData = 0;
	if( MF.getFrameInfo()->getObjectIndexEnd() > 0 )
	{
		for ( unsigned i = 0 ; i < MF.getFrameInfo()->getObjectIndexEnd() ; i++ )
		{
			frameLocationOfSourceData += REDEFINEUtils::getSizeOfType( MF.getFrameInfo()->getObjectAllocation( i )->getType() );
		}
	}
	MF.getFrameInfo()->dump( MF );
	errs() << "---------------------------------------------------------------------------------------------------\n";
	errs() << "--------------------------------------ARG ANALYSIS ALGO--------------------------------------------\n";
	errs() << "---------------------------------------------------------------------------------------------------\n";
	Function* consumerFunction = const_cast< Function* >( MF.getFunction() ) ;
	int beginArgIndex = 0;
	unsigned frameLocationOfTargetData = 0;
	for ( Function::arg_iterator funcArgItr = consumerFunction->arg_begin() ; funcArgItr != consumerFunction->arg_end() ; funcArgItr++ , beginArgIndex++ )
	{
		Argument* argument = &*funcArgItr;
		argument->dump();
		if( !consumerFunction->getAttributes().hasAttribute( beginArgIndex + 1 , Attribute::InReg ) )
		{
			frameLocationOfTargetData += REDEFINEUtils::getSizeOfType( funcArgItr->getType() );
		}
	}





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
				if( HigWcet.Is_Edge( currentHyperOp , childMapItr->second ) )
				{
					if( HigWcet.get_EdgeWeight( currentHyperOp , childMapItr->second ) < SingleHyperOpMap.at( ( currentHyperOp )->getHyperOpId() )->get_WcetNode( edgeSourceInstr ) )
					{
						HigWcet.set_EdgeWeight( currentHyperOp , childMapItr->second , SingleHyperOpMap.at( ( currentHyperOp )->getHyperOpId() )->get_WcetNode( edgeSourceInstr ) );
					}
				}
				else
				{
					HigWcet.add_Edge( currentHyperOp , childMapItr->second , SingleHyperOpMap.at( ( currentHyperOp )->getHyperOpId() )->get_WcetNode( edgeSourceInstr ) );
				}

				if( HigBcet.Is_Edge( currentHyperOp , childMapItr->second ) )
				{
					if( HigBcet.get_EdgeWeight( currentHyperOp , childMapItr->second ) > SingleHyperOpMap.at( ( currentHyperOp )->getHyperOpId() )->get_Bcet() )
					{
						HigBcet.set_EdgeWeight( currentHyperOp , childMapItr->second , SingleHyperOpMap.at( ( currentHyperOp )->getHyperOpId() )->get_Bcet() );
					}
				}
				else
				{
					HigBcet.add_Edge( currentHyperOp , childMapItr->second , SingleHyperOpMap.at( ( currentHyperOp )->getHyperOpId() )->get_Bcet() );
				}
				/*
				 if( 1 )
				 {
				 HigBcet.add_Edge( currentHyperOp , childMapItr->second , SingleHyperOpMap.at( ( currentHyperOp )->getHyperOpId() )->get_Bcet() );
				 }*/
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
		HigWcet.xdot_CriticalPath();
		HigBcet.xdot_ShortestPath();
		errs() << "HIG WCET : " << HigWcet.get_Wcet() << "\n";
		errs() << "HIG BCET : " << HigBcet.get_Bcet() << "\n";

		errs() << "HIG graph Created!!!\n";
	}

	errs() << "###################################################################################################" << "\n";
	errs() << "########################################WCET Finished##############################################" << "\n";
	errs() << "###################################################################################################" << "\n";

	return false;

}
