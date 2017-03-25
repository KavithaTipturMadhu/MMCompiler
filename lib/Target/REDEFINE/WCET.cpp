/*
 * wcet.cpp
 *
 *  Created on: 06-Jan-2017
 *      Author: tarun
 */
#include "WCET.h"
using namespace WCET;
using namespace std;

//GET ID
static unsigned int gidx;
unsigned int WCET::get_gidx()
{
	unsigned int id = gidx;
	gidx = gidx + 1;
	return id;
}
WCET::FuncUnit< const llvm::MachineInstr * > FunctionUnit;
WCET::LoopBound LoopBounds;
//pHyperOpBasicBlock
pHyperOpBasicBlock::pHyperOpBasicBlock( MachineBasicBlock * B )
{

	MachineFunction *MF = ( const_cast< MachineFunction* >( B->getParent() ) );
	const TargetInstrInfo* TII;
	const InstrItineraryData *InstrItins;
	const TargetMachine &TM = MF->getTarget();
	TII = MF->getTarget().getInstrInfo();
	InstrItins = ( ( const REDEFINETargetMachine& ) TM ).getInstrItineraryData();

	//Creating Function unit Graph
	int pHyperOpIndex = -1;
	int pHyperOpIndexold = -2;
	MachineBasicBlock::const_instr_iterator instrItr = B->instr_begin();
	MachineBasicBlock::const_instr_iterator instrItrold;
	for ( ; instrItr != B->instr_end() ; ++instrItr )
	{
		FunctionUnit.Iinsert( &*instrItr );
		if( !instrItr->isInsideBundle() )
		{
			pHyperOpIndex++;
		}

		if( InstrItins && !InstrItins->isEmpty() )
		{
			const InstrStage *IS = InstrItins->beginStage( instrItr->getDesc().getSchedClass() );
			const InstrStage *E = InstrItins->endStage( instrItr->getDesc().getSchedClass() );
			int unit = 0;
			for ( ; IS != E ; ++IS )
			{
				unsigned StageDepth = IS->getCycles();
				( this->pHyperBB[pHyperOpIndex] ).add_Edge( FunctionUnit.get_FuncUnitNumber( &*instrItr , unit ) , FunctionUnit.get_FuncUnitNumber( &*instrItr , unit + 1 ) , StageDepth );
				unit++;
			}
		}
		if( pHyperOpIndex == pHyperOpIndexold )
		{

			for ( int unit = 0 ; unit < PIPEDEPTH ; unit++ )
			{
				( this->pHyperBB[pHyperOpIndex] ).add_Edge( FunctionUnit.get_FuncUnitNumber( &*instrItrold , unit + 1 ) , FunctionUnit.get_FuncUnitNumber( &*instrItr , unit ) , 0 );
			}
		}
		pHyperOpIndexold = pHyperOpIndex;
		instrItrold = instrItr;
	}
	//Handling Instruction Function Unit Dependency
	//TODO Include Parallelism by Floating and Integer Instruction
	//errs()<<"WCET: pH0: "<<this->pHyperBB[0].get_Wcet()<<" WCET: pH1: "<<this->pHyperBB[1].get_Wcet()<<\
			" WCET: pH2: "<<this->pHyperBB[2].get_Wcet()<<" WCET: pH3: "<<this->pHyperBB[3].get_Wcet()<<"\n";
}
WCET::pHyperOpBB pHyperOpBasicBlock::get_pHyperOpBB( int pHyperOpIndex )
{
	WCET::pHyperOpBB phmbb = this->pHyperBB[pHyperOpIndex];
	return phmbb;
}

//pHyperOpControlFlowGraph
pHyperOpControlFlowGraph::pHyperOpControlFlowGraph( llvm::MachineFunction &MF , llvm::MachineDominatorTree *DT )
{
	for ( MachineFunction::iterator bbItr = MF.begin() ; bbItr != MF.end() ; bbItr++ )
	{
		//bbItr->getNumber();
		WCET::pHyperOpBasicBlock pHyperMBB( bbItr );
		for ( int pHyperCFGindex = 0 ; pHyperCFGindex < 4 ; pHyperCFGindex++ )
		{
			WCET::pHyperOpBB *obj = new WCET::pHyperOpBB;
			*obj = pHyperMBB.get_pHyperOpBB( pHyperCFGindex );
			this->LookUp[pHyperCFGindex].push_back( obj );
			this->pHyperCFG[pHyperCFGindex].add_Vertex( obj );
		}
	}

	for ( auto& B : MF )
	{
		for ( MachineBasicBlock::succ_iterator succItr = B.succ_begin() ; succItr != B.succ_end() ; succItr++ )
		{
			for ( int pHyperCFGindex = 0 ; pHyperCFGindex < 4 ; pHyperCFGindex++ )
			{
				if( !DT->dominates( ( *succItr ) , ( &B ) ) )
				{
					this->pHyperCFG[pHyperCFGindex].add_Edge( this->LookUp[pHyperCFGindex][B.getNumber()] , this->LookUp[pHyperCFGindex][ ( *succItr )->getNumber()] , ( this->LookUp[pHyperCFGindex][B.getNumber()] )->get_Wcet() );
				}
			}
		}
	}
}

//LoopSingleIterationWcet
LoopSingleIterationWcet::LoopSingleIterationWcet( MachineLoop* ml )
{
	this->L = ml;
	for ( auto BBitr = ml->block_begin() ; BBitr != ml->block_end() ; ++BBitr )
	{
		this->qt.push( *BBitr );
	}

}
int LoopSingleIterationWcet::get_LoopSingleIterationWcet( int LoopSingleIterationWcetindex )
{
	return this->LoopSIWcet[LoopSingleIterationWcetindex];
}

//LoopAnalysis
LoopAnalysis::LoopAnalysis( MachineLoopInfo *LI )
{
	this->LS.add_Vertex( LoopScopeRoot );

	for ( MachineLoopInfo::iterator LIT = LI->begin() ; LIT != LI->end() ; ++LIT )
	{
		MachineLoop* l = *LIT;
		set_LoopBounds( l , MAXLOOPBOUND );
		this->LS.add_Edge( LoopScopeRoot , l , get_LoopBounds( l ) );

		LoopDFS( l );
	}
}
void LoopAnalysis::LoopDFS( MachineLoop *L )
{
	this->LS.add_Vertex( L );
	errs() << "getLD entry " << L->getLoopDepth() << "\n";
	for ( auto BBitr = L->block_begin() ; BBitr != L->block_end() ; ++BBitr )
	{
		errs() << ( *BBitr )->getNumber() << ", ";
	}
	errs() << "\n";
	std::vector< MachineLoop* > subLoops = L->getSubLoops();
	for ( MachineLoop::iterator subitr = subLoops.begin() ; subitr != subLoops.end() ; ++subitr )
	{
		//TODO Centrally Set Loop bounds
		set_LoopBounds( L , MAXLOOPBOUND );
		this->LS.add_Edge( L , *subitr , get_LoopBounds( L ) );
		LoopDFS( *subitr );
	}

}
void LoopAnalysis::dumpDFS( MachineLoop *L )
{
	list< MachineLoop * > l = this->LS.get_AdjList( L );
	for ( auto itr = l.begin() ; itr != l.end() ; ++itr )
	{
		errs() << "---------------------------------------------------------------------------------------------------\n";
		( *itr )->dump();
		this->dumpDFS( *itr );
	}
}
void LoopAnalysis::dump()
{
	this->dumpDFS( LoopScopeRoot );
}
void LoopAnalysis::set_LoopBounds( MachineLoop * l , unsigned int max )
{
	auto itr = LoopBounds.find( l );
	if( itr != LoopBounds.end() )
		LoopBounds.at( l ) = max;
	else
		LoopBounds.insert( std::make_pair( l , max ) );

}
unsigned int LoopAnalysis::get_LoopBounds( MachineLoop * l )
{
	auto itr = LoopBounds.find( l );
	if( itr != LoopBounds.end() )
		return LoopBounds.at( l );
	else
		return 0;
}

//IntraHyperOp
SingleHyperOpWcet::SingleHyperOpWcet( llvm::MachineFunction *MFI , MachineLoopInfo *LI , MachineDominatorTree *DT )
{
	this->MF = MFI;

	WCET::pHyperOpControlFlowGraph mycfg( *MFI , DT );
	this->pHopCFG[0] = mycfg.pHyperCFG[0];
	this->pHopCFG[1] = mycfg.pHyperCFG[1];
	this->pHopCFG[2] = mycfg.pHyperCFG[2];
	this->pHopCFG[3] = mycfg.pHyperCFG[3];

	this->LookUp[0] = mycfg.LookUp[0];
	this->LookUp[1] = mycfg.LookUp[1];
	this->LookUp[2] = mycfg.LookUp[2];
	this->LookUp[3] = mycfg.LookUp[3];

	WCET::LoopAnalysis myla( LI );
	this->LScope[0] = myla.LS;
	this->LScope[1] = myla.LS;
	this->LScope[2] = myla.LS;
	this->LScope[3] = myla.LS;

	//MERGING pHop into HyerOp
	this->IntraHop.add_Vertex( CFGSTART );
	for ( int pHopindex = 0 ; pHopindex < 4 ; pHopindex++ )
	{
		auto ls = this->pHopCFG[pHopindex].get_VertexList();
		for ( auto lsitr = ls.begin() ; lsitr != ls.end() ; ++lsitr )
		{
			this->IntraHop.add_Vertex( *lsitr );
		}
	}
	this->IntraHop.add_Vertex( CFGEND );

	for ( int pHopindex = 0 ; pHopindex < 4 ; pHopindex++ )
	{
		auto ls = this->pHopCFG[pHopindex].get_VertexList();
		auto lsitr = ls.begin();
		this->IntraHop.add_Edge( CFGSTART , mycfg.LookUp[pHopindex].at( 0 ) , 4 + ( pHopindex * 3 ) );
		for ( ; lsitr != ls.end() ; ++lsitr )
		{

			auto adjls = this->pHopCFG[pHopindex].get_AdjList( *lsitr );
			for ( auto adjlsitr = adjls.begin() ; adjlsitr != adjls.end() ; ++adjlsitr )
			{
				this->IntraHop.add_Edge( *lsitr , *adjlsitr , ( *lsitr )->get_Wcet() );
			}

			if( adjls.size() == 0 )
			{
				this->IntraHop.add_Edge( *lsitr , CFGEND , ( *lsitr )->get_Wcet() );
			}
		}

	}

	this->IntraHop.xdot_CriticalPath();
	//TODO INCLUDING LOOP ANALYSIS

	//ADDING MPI

	//CREATING A LOOKUP
	const TargetInstrInfo* TII;
	TII = this->MF->getTarget().getInstrInfo();
	const TargetMachine &TM = this->MF->getTarget();
	HyperOpInteractionGraph * HIG = ( ( REDEFINETargetMachine& ) TM ).HIG;
	HyperOp* currentHyperOp = HIG->getHyperOp( const_cast< Function* >( this->MF->getFunction() ) );

	PHyperOpInteractionGraph phopDependence = currentHyperOp->getpHyperOpDependenceMap();
	for ( auto pHopDependenceItr = phopDependence.rbegin() ; pHopDependenceItr != phopDependence.rend() ; ++pHopDependenceItr )
	{
		MachineInstr* s = pHopDependenceItr->first;
		MachineInstr* t = pHopDependenceItr->second;
		pHyperOpBB* pstart;
		pHyperOpBB* pend;

		//Dummy Nodes for MPI
		pHyperOpBB *ptrs = new pHyperOpBB;
		pHyperOpBB *ptre = new pHyperOpBB;
		this->MPILookup.insert( make_pair( s , ptrs ) );
		this->MPILookup.insert( make_pair( t , ptre ) );
	}

	//ADDING NODES
	std::map< llvm::MachineBasicBlock * , bool > Q;
	Q.clear();
	for ( auto itr = MPILookup.begin() ; itr != MPILookup.end() ; ++itr )
	{
		auto Iptr = itr->first;
		auto MBB = Iptr->getParent();
		auto findItr = Q.find( MBB );

		if( findItr == Q.end() )
		{
			for ( auto litr = MBB->instr_rbegin() ; litr != MBB->instr_rend() ; ++litr )
			{
				auto MessageInstrItr = MPILookup.find( &*litr );
				if( MessageInstrItr != MPILookup.end() )
				{
					for ( int pLookindex = 0 ; pLookindex < 4 ; pLookindex++ )
					{
						pHyperOpBB* pbbs = this->LookUp[pLookindex][MBB->getNumber()];
						if( pbbs->Is_Vertex( FunctionUnit.get_FuncUnitNumber( ( &*litr ) , 0 ) ) )
						{
							this->IntraHop.split_Vertex( pbbs , MessageInstrItr->second , pbbs->get_NodeWcet( FunctionUnit.get_FuncUnitNumber( &*litr , 4 ) ) );
						}

					}

				}

			}
			Q.insert( make_pair( MBB , true ) );
		}
	}

	/*	for ( int pLookindex = 0 ; pLookindex < 4 ; pLookindex++ )
	 {
	 pHyperOpBB* pbbs = ( this->LookUp[pLookindex][s->getParent()->getNumber()] );
	 if( pbbs->Is_Vertex( FunctionUnit.get_FuncUnitNumber( s , 0 ) ) )
	 {
	 pstart = pbbs;
	 this->IntraHop.split_Vertex( pstart , ptrs , pstart->get_NodeWcet( FunctionUnit.get_FuncUnitNumber( s , 4 ) ) );
	 this->pHopMPI.add_Edge( pstart , ptrs , pstart->get_NodeWcet( FunctionUnit.get_FuncUnitNumber( s , 4 ) ) );
	 }

	 pHyperOpBB* pbbt = ( this->LookUp[pLookindex][t->getParent()->getNumber()] );
	 if( pbbt->Is_Vertex( FunctionUnit.get_FuncUnitNumber( t , 0 ) ) )
	 {
	 pend = pbbt;
	 this->IntraHop.split_Vertex( pend , ptre , pend->get_NodeWcet( FunctionUnit.get_FuncUnitNumber( t , 0 ) ) );
	 this->pHopMPI.add_Edge( pend , ptre , pend->get_NodeWcet( FunctionUnit.get_FuncUnitNumber( t , 0 ) ) );
	 }
	 }*/

	//this->pHopMPI.add_Edge( ptrs , ptre , pstart->get_NodeWcet( FunctionUnit.get_FuncUnitNumber( s , 4 ) ) );
	//ADD EDGES
	for ( auto pHopDependenceItr = phopDependence.rbegin() ; pHopDependenceItr != phopDependence.rend() ; ++pHopDependenceItr )
	{
		MachineInstr* s = pHopDependenceItr->first;
		MachineInstr* t = pHopDependenceItr->second;
		this->IntraHop.add_Edge( this->MPILookup.at( s ) , this->MPILookup.at( t ) , pHyperMessageLatency );
	}

	//this->pHopMPI.xdot();
	//	this->pHopCFG[0].xdot();
	//	this->pHopMPI.xdot();
	//  this->LScope[0].xdot ();
}
int SingleHyperOpWcet::get_Wcet()
{
	this->IntraHop.xdot_CriticalPath();
	return this->IntraHop.get_Wcet();

}
int SingleHyperOpWcet::get_WcetNode( int i )
{
	return 0;
}

//CLASS Functional Unit
template<class T> unsigned long int FuncUnit< T >::ID;
template<class T> FuncUnit< T >::FuncUnit()
{
	this->TLookup.clear();
}
template<class T> void FuncUnit< T >::Ireset()
{
	this->TLookup.clear();
	this->ID = 0;
}
template<class T> void FuncUnit< T >::Iinsert( T _u )
{
	if( this->TLookup.insert( std::make_pair( _u , this->ID ) ).second )
		this->ID = this->ID + 1;
}
template<class T> void FuncUnit< T >::Idelete( T _u )
{
	typename FuncMap::iterator itr = this->TLookup.find( _u );
	if( itr != this->TLookup.end() )
		this->TLookup.erase( itr );

}
template<class T> void FuncUnit< T >::dump()
{
	typename FuncMap::iterator itr;
	for ( itr = this->TLookup.begin(); itr != this->TLookup.end() ; ++itr )
		std::cout << "Key: " << itr->first << ", Value: " << itr->second << "\n";
}
template<class T> unsigned long int FuncUnit< T >::get_FuncUnitNumber( T _u , unsigned _stage )
{
//_stage should be <= PIPEDEPTH
	unsigned long int i;
	typename FuncMap::iterator itr = this->TLookup.find( _u );
	if( itr != this->TLookup.end() )
	{
		i = this->TLookup.at( _u ) * ( PIPEDEPTH + 1 ) + _stage;
	}
	else
	{
		i = -1;
	}

//*
	return i;
}

//DONE CLASS AdjEdge
template<class T> AdjEdge< T >::AdjEdge( T _v , int _w )
{
	Vertex = _v;
	Weight = _w;
}
template<class T> int AdjEdge< T >::get_weight()
{
	return this->Weight;
}
template<class T> T AdjEdge< T >::get_vertex()
{
	return this->Vertex;
}
template<class T> void AdjEdge< T >::set_weight( int _w )
{
	this->Weight = _w;
}

//CLASS DFSNode
template<class T> DFSNode< T >::DFSNode( T _u )
{
	this->Parent = _u;
	this->Post = -1;
	this->Pre = -1;
	this->Visited = 0;
}
template<class T> unsigned long int DFSNode< T >::get_Pre()
{
	return this->Pre;
}
template<class T> unsigned long int DFSNode< T >::get_Post()
{
	return this->Post;
}
template<class T> T DFSNode< T >::get_Parent()
{
	return this->Parent;
}
template<class T> void DFSNode< T >::set_Pre( unsigned long int _Pre )
{
	this->Pre = _Pre;
}
template<class T> void DFSNode< T >::set_Post( unsigned long int _Post )
{
	this->Post = _Post;
}
template<class T> void DFSNode< T >::set_Parent( T _Parent )
{
	this->Parent = _Parent;
}
template<class T> void DFSNode< T >::set_Visited()
{
	this->Visited = true;
}
template<class T> bool DFSNode< T >::IsVisited()
{
	return this->Visited;
}

//DONE CLASS DWGraph
template<class T> DWGraph< T >::DWGraph()
{
	this->G.clear();
	this->n = 0;
}
template<class T> unsigned long int DWGraph< T >::get_SizeOfGraph()
{
	unsigned long int size = 0;
	for ( typename DWG::iterator itr = this->G.begin() ; itr != this->G.end() ; ++itr )
	{
		size += get_VertexDegree( itr->first );
	}
	return size;

}
template<class T> bool DWGraph< T >::add_Vertex( T _u )
{

	if( this->G.insert( make_pair( _u , new AdjList ) ).second )
	{
		this->Lookup.insert( std::make_pair( _u , n ) );
		this->n++;
		return true;
	}
	else
	{
		return false;
	}
}
template<class T> bool DWGraph< T >::split_Vertex( T _u , T _v , int delta )
{
	typename DWG::iterator itr = this->G.find( _u );
	typename DWG::iterator jitr;
	if( itr == this->G.end() )
	{
		return false;
	}
	else
	{

		if( this->G.insert( make_pair( _v , this->G.at( _u ) ) ).second )
		{
			this->Lookup.insert( std::make_pair( _v , n ) );
			this->n++;

			this->G.at( _u ) = new AdjList;
			AdjEdge< T > *E = new AdjEdge< T >( _v , delta );
			this->G.at( _u )->push_back( *E );

			jitr = this->G.find( _v );
			for ( typename AdjList::iterator litr = ( ( *jitr ).second )->begin() ; litr != ( ( *jitr ).second )->end() ; ++litr )
			{
				litr->set_weight( litr->get_weight() - delta );
			}

			return true;
		}
		else
		{
			return false;
		}
	}
}
template<class T> bool DWGraph< T >::Is_Vertex( T _u )
{
	typename DWG::iterator itr = this->G.find( _u );
	return ( itr != this->G.end() );

}
template<class T> bool DWGraph< T >::Is_Edge( T _u , T _v )
{
	bool ue , ve;
	typename DWG::iterator itr = this->G.find( _u );
	ue = ( itr == this->G.end() );
	ve = ( this->G.find( _v ) == this->G.end() );

	if( ue || ve )
		return false;

	for ( typename AdjList::iterator litr = ( ( *itr ).second )->begin() ; litr != ( ( *itr ).second )->end() ; ++litr )
	{
		if( litr->get_vertex() == _v )
			return true;
	}
	return false;
}
template<class T> bool DWGraph< T >::set_EdgeWeight( T _u , T _v , int _w )
{
	bool ue , ve;
	typename DWG::iterator itr = this->G.find( _u );
	ue = ( itr != this->G.end() );
	ve = ( this->G.find( _v ) != this->G.end() );
	if( ue && ve )
	{

		for ( typename AdjList::iterator litr = ( ( *itr ).second )->begin() ; litr != ( ( *itr ).second )->end() ; ++litr )
		{
			if( litr->get_vertex() == _v )
				litr->set_weight( _w );
		}
		return true;
	}
	else
	{
		return false;
	}

}
template<class T> bool DWGraph< T >::add_Edge( T _u , T _v , int _w )
{
	AdjEdge< T > *E = new AdjEdge< T >( _v , _w );
	bool ue , ve;
	ue = ( this->G.find( _u ) != this->G.end() );
	ve = ( this->G.find( _v ) != this->G.end() );
	if( ue && ve )
		this->G.at( _u )->push_back( *E );
	else
	{
		add_Vertex( _u );
		add_Vertex( _v );
		this->G.at( _u )->push_back( *E );
	}
	return true;
}
template<class T> bool DWGraph< T >::remove_Edge( T _u , T _v )
{
	bool ue , ve;
	ue = ( this->G.find( _u ) != this->G.end() );
	ve = ( this->G.find( _v ) != this->G.end() );
	if( ue && ve )
	{
		typename DWG::iterator itr = this->G.find( _u );
		for ( typename AdjList::iterator litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
		{
			if( ( this->Lookup.at( litr->get_vertex() ) ) == ( this->Lookup.at( _v ) ) )
			{
				( this->G.at( _u ) )->erase( litr-- );
			}
		}
		return true;
	}

	return false;

}
template<class T> void DWGraph< T >::dump()
{
	for ( typename DWG::iterator itr = this->G.begin() ; itr != this->G.end() ; ++itr )
	{
		std::cout << this->Lookup.at( ( *itr ).first ) << ": ";
		for ( typename AdjList::iterator litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
		{
			std::cout << " " << this->Lookup.at( litr->get_vertex() ) << "(" << litr->get_weight() << "),";
		}
		std::cout << std::endl;
	}
}
template<class T> void DWGraph< T >::xdot()
{
	std::ofstream df;
	std::string file( "graph" );
	file.append( std::to_string( WCET::get_gidx() ) );
	file.append( ".dot" );
	df.open( file );
	df << "digraph{\n";
	for ( typename DWG::iterator itr = this->G.begin() ; itr != this->G.end() ; ++itr )
	{
		df << this->Lookup.at( ( *itr ).first ) << "[label=\"" << this->Lookup.at( ( *itr ).first ) << "\"];\n";
	}
	for ( typename DWG::iterator itr = this->G.begin() ; itr != this->G.end() ; ++itr )
	{
		for ( typename AdjList::iterator litr = ( ( *itr ).second )->begin() ; litr != ( ( *itr ).second )->end() ; ++litr )
		{
			df << " " << this->Lookup.at( ( *itr ).first ) << "->" << this->Lookup.at( litr->get_vertex() ) << "[label=" << litr->get_weight() << "];\n";
		}
	}
	df << "}\n";
	df.close();
}
template<class T> unsigned long int DWGraph< T >::get_Cardinality()
{
	return this->n;
}
template<class T> unsigned long int DWGraph< T >::get_VertexDegree( T _u )
{
	return this->G.at( _u )->size();
}
template<class T> int DWGraph< T >::get_EdgeWeight( T _u , T _v )
{
	bool ue , ve;
	typename DWG::iterator itr = this->G.find( _u );
	ue = ( itr == this->G.end() );
	ve = ( this->G.find( _v ) == this->G.end() );

	if( ue || ve )
		return INT_MIN;

	for ( typename AdjList::iterator litr = ( ( *itr ).second )->begin() ; litr != ( ( *itr ).second )->end() ; ++litr )
	{
		if( litr->get_vertex() == _v )
			return litr->get_weight();
	}

	return INT_MIN;
}
template<class T> list< T > DWGraph< T >::get_VertexList()
{
	list< T > l;
	l.clear();
	for ( typename DWG::iterator itr = this->G.begin() ; itr != this->G.end() ; ++itr )
	{
		l.push_back( itr->first );
	}
	return l;
}
template<class T> list< T > DWGraph< T >::get_AdjList( T _u )
{
	list< T > l;
	l.clear();
	typename DWG::iterator itr = this->G.find( _u );
	if( itr != this->G.end() )
	{
		for ( typename AdjList::iterator litr = ( ( *itr ).second )->begin() ; litr != ( ( *itr ).second )->end() ; ++litr )
		{
			l.push_back( litr->get_vertex() );
		}
	}
	return l;
}

//CLASS DWAGraph
template<class T> DWAGraph< T >::DWAGraph()
{
	this->G.clear();
	this->n = 0;
	this->TSL.clear();
}
template<class T> void DWAGraph< T >::TopologicalSorting()
{
	std::queue< T > Q;
	std::map< T , int > indegree;
	T _u;
	typename DWGraph< T >::DWG::iterator _uitr;
	typename DWGraph< T >::DWG tsg( this->G );

	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
	{
		indegree.insert( std::make_pair( ( itr->first ) , 0 ) );
	}

	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
	{
		for ( typename DWGraph< T >::AdjList::iterator litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
		{
			indegree.at( litr->get_vertex() ) += 1;
		}
	}

	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
	{
		if( indegree.at( itr->first ) == 0 )
		{
			Q.push( itr->first );
			indegree.at( itr->first ) -= 1;
		}
	}
	while ( !Q.empty() )
	{
		_u = Q.front();
		Q.pop();
		this->TSL.push_back( _u );
		_uitr = tsg.find( _u );

		for ( typename DWGraph< T >::AdjList::iterator litr = _uitr->second->begin() ; litr != _uitr->second->end() ; ++litr )
		{
			indegree.at( litr->get_vertex() ) -= 1;
			if( indegree.at( litr->get_vertex() ) == 0 )
			{
				Q.push( litr->get_vertex() );
			}
		}
		tsg.erase( _u );
	}

}
template<class T> void DWAGraph< T >::dump_TSL()
{
	if( this->TSL.empty() )
		this->TopologicalSorting();

	for ( typename std::list< T >::iterator itr = this->TSL.begin() ; itr != this->TSL.end() ; ++itr )
	{
		std::cout << this->Lookup.at( *itr ) << ",";
	}
	std::cout << std::endl;

}
template<class T> void DWAGraph< T >::CriticalPath()
{
	std::queue< T > Q;
//Vertex , Degree , Weight
	std::map< T , std::pair< int , int > > indegree;
	T _u;
	typename DWGraph< T >::DWG::iterator _uitr;
	typename DWGraph< T >::DWG tsg( this->G );
	std::multimap< int , T > CTVM;
	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
	{
		indegree.insert( std::make_pair( ( itr->first ) , std::make_pair( 0 , 0 ) ) );
	}

	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
	{
		for ( typename DWGraph< T >::AdjList::iterator litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
		{
			indegree.at( litr->get_vertex() ).first += 1;
		}
	}

	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
	{
		if( indegree.at( itr->first ).first == 0 )
		{
			Q.push( itr->first );
			indegree.at( itr->first ).first -= 1;
		}
	}

	while ( !Q.empty() )
	{
		_u = Q.front();
		Q.pop();
		CTVM.insert( std::make_pair( indegree.at( _u ).second , _u ) );
		_uitr = tsg.find( _u );

		for ( typename DWGraph< T >::AdjList::iterator litr = _uitr->second->begin() ; litr != _uitr->second->end() ; ++litr )
		{
			indegree.at( litr->get_vertex() ).first -= 1;
			indegree.at( litr->get_vertex() ).second = std::max( indegree.at( litr->get_vertex() ).second , indegree.at( _u ).second + litr->get_weight() );
			if( indegree.at( litr->get_vertex() ).first == 0 )
			{
				Q.push( litr->get_vertex() );
			}
		}
		tsg.erase( _u );
	}

//Critical Path

	typename std::multimap< int , T >::reverse_iterator iitr = CTVM.rbegin();
	typename std::multimap< int , T >::reverse_iterator jitr = CTVM.rbegin();
	this->CrticalPath.insert( std::make_pair( ( iitr->second ) , ( iitr->first ) ) );
	while ( jitr != CTVM.rend() )
	{
		if( iitr->first == jitr->first + this->get_EdgeWeight( jitr->second , iitr->second ) )
		{
			this->CrticalPath.insert( std::make_pair( ( jitr->second ) , ( jitr->first ) ) );
			iitr = jitr;
		}
		++jitr;
	}

}
template<class T> int DWAGraph< T >::get_Wcet()
{
	if( this->G.empty() )
		return 0;

	std::queue< T > Q;
//Vertex , Degree , Weight
	std::map< T , std::pair< int , int > > indegree;
	T _u;
	typename DWGraph< T >::DWG::iterator _uitr;
	typename DWGraph< T >::DWG tsg( this->G );
	this->CTVM.clear();
	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
	{
		indegree.insert( std::make_pair( ( itr->first ) , std::make_pair( 0 , 0 ) ) );
	}

	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
	{
		for ( typename DWGraph< T >::AdjList::iterator litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
		{
			indegree.at( litr->get_vertex() ).first += 1;
		}
	}

	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
	{
		if( indegree.at( itr->first ).first == 0 )
		{
			Q.push( itr->first );
			indegree.at( itr->first ).first -= 1;
		}
	}

	while ( !Q.empty() )
	{
		_u = Q.front();
		Q.pop();
		this->CTVM.insert( std::make_pair( indegree.at( _u ).second , _u ) );
		_uitr = tsg.find( _u );

		for ( typename DWGraph< T >::AdjList::iterator litr = _uitr->second->begin() ; litr != _uitr->second->end() ; ++litr )
		{
			indegree.at( litr->get_vertex() ).first -= 1;
			indegree.at( litr->get_vertex() ).second = std::max( indegree.at( litr->get_vertex() ).second , indegree.at( _u ).second + litr->get_weight() );
			if( indegree.at( litr->get_vertex() ).first == 0 )
			{
				Q.push( litr->get_vertex() );
			}
		}
		tsg.erase( _u );
	}

	return this->CTVM.rbegin()->first;

}
template<class T> int DWAGraph< T >::get_NodeWcet( T Node )
{
	if( this->G.empty() )
		return 0;

	if( this->CTVM.empty() )
	{
		std::queue< T > Q;
		//Vertex , Degree , Weight
		std::map< T , std::pair< int , int > > indegree;
		T _u;
		typename DWGraph< T >::DWG::iterator _uitr;
		typename DWGraph< T >::DWG tsg( this->G );
		for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
		{
			indegree.insert( std::make_pair( ( itr->first ) , std::make_pair( 0 , 0 ) ) );
		}

		for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
		{
			for ( typename DWGraph< T >::AdjList::iterator litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
			{
				indegree.at( litr->get_vertex() ).first += 1;
			}
		}

		for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
		{
			if( indegree.at( itr->first ).first == 0 )
			{
				Q.push( itr->first );
				indegree.at( itr->first ).first -= 1;
			}
		}

		while ( !Q.empty() )
		{
			_u = Q.front();
			Q.pop();
			CTVM.insert( std::make_pair( indegree.at( _u ).second , _u ) );
			_uitr = tsg.find( _u );

			for ( typename DWGraph< T >::AdjList::iterator litr = _uitr->second->begin() ; litr != _uitr->second->end() ; ++litr )
			{
				indegree.at( litr->get_vertex() ).first -= 1;
				indegree.at( litr->get_vertex() ).second = std::max( indegree.at( litr->get_vertex() ).second , indegree.at( _u ).second + litr->get_weight() );
				if( indegree.at( litr->get_vertex() ).first == 0 )
				{
					Q.push( litr->get_vertex() );
				}
			}
			tsg.erase( _u );
		}
	}
	for ( typename std::multimap< int , T >::iterator mitr = this->CTVM.begin() ; mitr != this->CTVM.end() ; ++mitr )
	{
		if( ( *mitr ).second == Node )
			return ( *mitr ).first;
	}
	return 0;

}
template<class T> void DWAGraph< T >::dump_CriticalPath()
{
	if( this->CrticalPath.empty() )
		this->CriticalPath();

	for ( typename std::map< T , int >::iterator itr = this->CrticalPath.begin() ; itr != this->CrticalPath.end() ; ++itr )
	{
		std::cout << this->Lookup.at( itr->first ) << "(" << itr->second << "), ";
	}
	std::cout << std::endl;
}
template<class T> void DWAGraph< T >::xdot_CriticalPath()
{
	std::queue< T > Q;
//Vertex , Degree , Weight
	std::map< T , std::pair< int , int > > indegree;
	T _u;
	typename DWGraph< T >::DWG::iterator _uitr;
	typename DWGraph< T >::DWG tsg( this->G );
	std::map< T , int > VCTM;
	std::multimap< int , T > CTVM;
	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
	{
		indegree.insert( std::make_pair( ( itr->first ) , std::make_pair( 0 , 0 ) ) );
	}

	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
	{
		for ( typename DWGraph< T >::AdjList::iterator litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
		{
			indegree.at( litr->get_vertex() ).first += 1;
		}
	}

	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
	{
		if( indegree.at( itr->first ).first == 0 )
		{
			Q.push( itr->first );
			indegree.at( itr->first ).first -= 1;
		}
	}

	while ( !Q.empty() )
	{
		_u = Q.front();
		Q.pop();
		VCTM.insert( std::make_pair( _u , indegree.at( _u ).second ) );
		CTVM.insert( std::make_pair( indegree.at( _u ).second , _u ) );
		_uitr = tsg.find( _u );

		for ( typename DWGraph< T >::AdjList::iterator litr = _uitr->second->begin() ; litr != _uitr->second->end() ; ++litr )
		{
			indegree.at( litr->get_vertex() ).first -= 1;
			indegree.at( litr->get_vertex() ).second = std::max( indegree.at( litr->get_vertex() ).second , indegree.at( _u ).second + litr->get_weight() );
			if( indegree.at( litr->get_vertex() ).first == 0 )
			{
				Q.push( litr->get_vertex() );
			}
		}
		tsg.erase( _u );
	}

	std::ofstream df;
	std::string file( "graph" );
	file.append( std::to_string( WCET::get_gidx() ) );
	file.append( ".dot" );
	df.open( file );
	df << "digraph{\n";
	if( XDOTRANK )
		df << "\trankdir=LR;\n";

	for ( typename DWGraph< T >::DWG::iterator itr = this->G.begin() ; itr != this->G.end() ; ++itr )
	{
		df << this->Lookup.at( ( *itr ).first ) << "[label=\"" << this->Lookup.at( ( *itr ).first ) << " (@" << VCTM.at( itr->first ) << ")" << "\"];\n";
	}
	for ( typename DWGraph< T >::DWG::iterator itr = this->G.begin() ; itr != this->G.end() ; ++itr )
	{
		for ( typename DWGraph< T >::AdjList::iterator litr = ( ( *itr ).second )->begin() ; litr != ( ( *itr ).second )->end() ; ++litr )
		{
			df << " " << this->Lookup.at( ( *itr ).first ) << "->" << this->Lookup.at( litr->get_vertex() ) << "[label=" << litr->get_weight() << "];\n";
		}
	}

//Critical Path

	typename std::multimap< int , T >::reverse_iterator iitr = CTVM.rbegin();
	typename std::multimap< int , T >::reverse_iterator jitr = CTVM.rbegin();
	while ( jitr != CTVM.rend() )
	{
		if( iitr->first == jitr->first + this->get_EdgeWeight( jitr->second , iitr->second ) )
		{
			df << this->Lookup.at( iitr->second ) << "[fillcolor=red, style=filled];\n";
			df << this->Lookup.at( jitr->second ) << "[fillcolor=red, style=filled];\n";
			iitr = jitr;
		}
		++jitr;
	}
	if( XDOTRANK )
	{
		df << "{rank=same; ";
		iitr = CTVM.rbegin();
		jitr = CTVM.rbegin();
		while ( jitr != CTVM.rend() )
		{
			if( iitr->first == jitr->first + this->get_EdgeWeight( jitr->second , iitr->second ) )
			{
				df << this->Lookup.at( iitr->second ) << " " << this->Lookup.at( jitr->second ) << " ";
				iitr = jitr;
			}
			++jitr;
		}

		df << "}\n";
	}
	df << "}\n";
	df.close();
}

