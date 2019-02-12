///*
// * wcet.cpp
// *
// *  Created on: 06-Jan-2017
// *      Author: tarun
// */
//#include "WCET.h"
//#include "llvm/ADT/StringExtras.h"
//using namespace WCET;
//using namespace std;
//
////GLOBAL VARIABLE
//static unsigned int gidx;
//WCET::FuncUnit< const llvm::MachineInstr * > FunctionUnit;
//WCET::LoopBound LoopBounds;
//MemoryAliasMap MA;
//
////GET ID
//unsigned int WCET::get_gidx()
//{
//	unsigned int id = gidx;
//	gidx = gidx + 1;
//	return id;
//}
//
////GLOBAL MF CACHE
//void WCET::WCETMemoryAliasing( MachineFunction *MF )
//{
//	for ( auto bitr = MF->begin() ; bitr != MF->end() ; ++bitr )
//	{
//		for ( auto itr = bitr->instr_begin() ; itr != bitr->instr_end() ; ++itr )
//		{
////		if( itr->mayLoad( llvm::MachineInstr::QueryType::AnyInBundle ) || itr->mayStore( llvm::MachineInstr::QueryType::AnyInBundle ) )
//			if( !itr->memoperands_empty() )
//			{
//				for ( auto mitr = itr->memoperands_begin() ; mitr != itr->memoperands_end() ; ++mitr )
//				{
//					MA.insert( std::make_pair( itr , std::make_pair( ( void * ) ( ( *mitr )->getValue() ) , ( unsigned int ) ( ( *mitr )->getSize() ) ) ) );
//				}
//			}
//		}
//	}
//}
//CacheAnalysis * WCET::Union( CacheAnalysis * CAA , CacheAnalysis * CAB )
//{
//	CacheAnalysis *CA = new CacheAnalysis[pHyperOpNumber];
//	for ( int pindx = 0 ; pindx < pHyperOpNumber ; pindx++ )
//	{
//		auto Aitr = CAA[pindx].LRU.begin();
//		auto Bitr = CAB[pindx].LRU.begin();
//		bool flagA = true;
//		bool flagB = true;
//		CacheNode *Node;
//
//		while ( ( Aitr != CAA[pindx].LRU.end() ) && ( Bitr != CAB[pindx].LRU.end() ) )
//		{
//			for ( auto itr = CA[pindx].LRU.begin() ; itr != CA[pindx].LRU.end() ; ++itr )
//			{
//				if( itr->get_MemoryLocation() == Aitr->get_MemoryLocation() )
//				{
//					flagA = false;
//					break;
//				}
//			}
//			if( flagA && ( Aitr != CAA[pindx].LRU.end() ) )
//			{
//				Node = new CacheNode( Aitr->get_MemoryLocation() , Aitr->get_memsize() );
//				CA[pindx].LRU.push_back( *Node );
//				++Aitr;
//
//			}
//			flagA = true;
//			if( CA[pindx].LRU.size() >= CA[pindx].get_line() )
//			{
//				break;
//			}
//			for ( auto itr = CA[pindx].LRU.begin() ; itr != CA[pindx].LRU.end() ; ++itr )
//			{
//				if( itr->get_MemoryLocation() == Bitr->get_MemoryLocation() )
//				{
//					flagB = false;
//					break;
//				}
//			}
//			if( flagB && ( Bitr != CAB[pindx].LRU.end() ) )
//			{
//				Node = new CacheNode( Bitr->get_MemoryLocation() , Bitr->get_memsize() );
//				CA[pindx].LRU.push_back( *Node );
//				++Bitr;
//
//			}
//			flagB = true;
//			if( CA[pindx].LRU.size() >= CA[pindx].get_line() )
//			{
//				break;
//			}
//		}
//	}
//	return CA;
//}
//CacheAnalysis * WCET::Intersection( CacheAnalysis * A , CacheAnalysis * B )
//{
//	//errs() << "ERROR 100\n";
//	CacheAnalysis *CA = new CacheAnalysis[pHyperOpNumber];
//	for ( int pindx = 0 ; pindx < pHyperOpNumber ; pindx++ )
//	{
//		CacheAnalysis CAA;
//		CacheAnalysis CAB;
//
//		for ( auto Aitr = A[pindx].LRU.rbegin() ; Aitr != A[pindx].LRU.rend() ; ++Aitr )
//		{
//			for ( auto Bitr = B[pindx].LRU.rbegin() ; Bitr != B[pindx].LRU.rend() ; ++Bitr )
//			{
//				if( Aitr->get_MemoryLocation() == Bitr->get_MemoryLocation() )
//				{
//					CacheNode *Node = new CacheNode( Aitr->get_MemoryLocation() , Aitr->get_memsize() );
//					CAA.LRU.push_front( *Node );
//				}
//			}
//		}
//		CAA.dump();
//		//errs() << "ERROR 101\n";
//		for ( auto Bitr = B[pindx].LRU.rbegin() ; Bitr != B[pindx].LRU.rend() ; ++Bitr )
//		{
//			for ( auto Aitr = A[pindx].LRU.rbegin() ; Aitr != A[pindx].LRU.rend() ; ++Aitr )
//			{
//				if( Bitr->get_MemoryLocation() == Aitr->get_MemoryLocation() )
//				{
//					CacheNode *Node = new CacheNode( Bitr->get_MemoryLocation() , Bitr->get_memsize() );
//					CAB.LRU.push_front( *Node );
//				}
//			}
//		}
//		CAB.dump();
//		//errs() << "ERROR 102\n";
//		bool flagA = true;
//		bool flagB = true;
//		auto CAAitr = CAA.LRU.rbegin();
//		auto CABitr = CAB.LRU.rbegin();
//		if( CAA.LRU.size() > 0 )
//		{
//			while ( ( CAAitr != CAA.LRU.rend() ) && ( CABitr != CAB.LRU.rend() ) )
//			{
//				flagA = true;
//				for ( auto itr = CA[pindx].LRU.begin() ; itr != CA[pindx].LRU.end() ; ++itr )
//				{
//					if( itr->get_MemoryLocation() == CAAitr->get_MemoryLocation() )
//					{
//						flagA = false;
//						break;
//					}
//				}
//
//				//errs() << "ERROR 103\n";
//				if( flagA && ( CAAitr != CAA.LRU.rend() ) )
//				{
//					CacheNode *Node = new CacheNode( CAAitr->get_MemoryLocation() , CAAitr->get_memsize() );
//					CA[pindx].LRU.push_front( *Node );
//				}
//				++CAAitr;
//				flagB = true;
//				//errs() << "ERROR 104\n";
//				for ( auto itr = CA[pindx].LRU.begin() ; itr != CA[pindx].LRU.end() ; ++itr )
//				{
//					if( itr->get_MemoryLocation() == CABitr->get_MemoryLocation() )
//					{
//						flagB = false;
//						break;
//					}
//				}
//				//errs() << "ERROR 105\n";
//				if( flagB && ( CABitr != CAB.LRU.rend() ) )
//				{
//					CacheNode *Node = new CacheNode( CABitr->get_MemoryLocation() , CABitr->get_memsize() );
//					CA[pindx].LRU.push_front( *Node );
//				}
//				++CABitr;
//
//				errs() << "Size of CA: " << CA[pindx].LRU.size() << "\n";
//				CA[pindx].dump();
//			//	errs() << "ERROR 106\n";
//			}
//		//	errs() << "ERROR 107\n";
//		}
//	}
//	//errs() << "ERROR 108\n";
//	return CA;
//}
////CacheNode
//CacheNode::CacheNode( void * ptr , unsigned int _size )
//{
//	this->MemoryLocation = ptr;
//	this->size = _size;
//}
//void CacheNode::set_MemoryLocation( void * ptr )
//{
//	this->MemoryLocation = ptr;
//}
//void * CacheNode::get_MemoryLocation()
//{
//	return this->MemoryLocation;
//}
//void CacheNode::set_memsize( unsigned int i )
//{
//	this->size = i;
//}
//unsigned int CacheNode::get_memsize()
//{
//	return this->size;
//}
//
////Cache Analysis
//WCET::CacheAnalysis::CacheAnalysis()
//{
//	this->associativity = 2;
//	this->cachesize = ( 1024 * 8 * 2 );
//	this->linesize = 8;
//	this->NumberofLines = 2;
//}
//void WCET::CacheAnalysis::set_associativity( int a )
//{
//	this->associativity = a;
//}
//void WCET::CacheAnalysis::set_line( int l )
//{
//	this->linesize = l;
//}
//int WCET::CacheAnalysis::get_line()
//{
//	return this->linesize;
//}
//void WCET::CacheAnalysis::set_cachsize( int c )
//{
//	this->cachesize = c;
//	if( this->associativity == 0 )
//		this->associativity = 1;
//	if( this->linesize == 0 )
//		this->linesize = 1;
//	this->NumberofLines = c / ( this->associativity * this->linesize );
//
//}
//bool WCET::CacheAnalysis::Update( void * ptr , unsigned int _size )
//{
//	auto lsitr = this->LRU.begin();
//	auto nextptr = this->LRU.begin();
//	bool flag = false;
//	for ( ; lsitr != this->LRU.end() ; ++lsitr )
//	{
//		if( ptr == ( *lsitr ).get_MemoryLocation() )
//		{
//			nextptr = lsitr;
//			++nextptr;
//			flag = true;
//			break;
//		}
//	}
//	if( flag == true )
//	{
//		if( lsitr != this->LRU.begin() )
//			this->LRU.splice( this->LRU.begin() , this->LRU , lsitr , nextptr );
//		return true;
//	}
//	else
//	{
//		WCET::CacheNode *node = new CacheNode( ptr , _size );
//		if( this->LRU.size() >= this->NumberofLines )
//		{
//			this->LRU.push_front( *node );
//			this->LRU.pop_back();
//		}
//		else
//		{
//			this->LRU.push_front( *node );
//		}
//	}
//	return false;
//}
//bool WCET::CacheAnalysis::In_Cache( MachineInstr * MI )
//{
//
////	if( MI->mayLoad( llvm::MachineInstr::QueryType::AnyInBundle ) || MI->mayStore( llvm::MachineInstr::QueryType::AnyInBundle ) )
//	if( !MI->memoperands_empty() )
//	{
//		return this->Update( MA.at( MI ).first , MA.at( MI ).second );
//	}
//	return false;
//
//}
//void WCET::CacheAnalysis::dump()
//{
//	if( this->LRU.empty() )
//		errs() << "Cache is Empty\n";
//	else
//		for ( auto itr = this->LRU.begin() ; itr != this->LRU.end() ; ++itr )
//		{
//			errs() << "MEMORY ADDR: " << ( *itr ).get_MemoryLocation() << " , Size: " << ( *itr ).get_memsize() << "\n";
//		}
//}
////pHyperOpBasicBlock
//pHyperOpBasicBlock::pHyperOpBasicBlock( MachineBasicBlock * B , CacheAnalysis *CA )
//{
//	//errs() << "pBB error 01\n";
//	MachineFunction *MF = ( const_cast< MachineFunction* >( B->getParent() ) );
//	const TargetInstrInfo* TII;
//	const InstrItineraryData *InstrItins;
//	const TargetMachine &TM = MF->getTarget();
//	TII = MF->getTarget().getInstrInfo();
//	InstrItins = ( ( const REDEFINETargetMachine& ) TM ).getInstrItineraryData();
//
//	//Creating Function unit Graph
//	int pHyperOpIndex = -1;
//	int pHyperOpIndexold = -2;
//	MachineBasicBlock::instr_iterator instrItr = B->instr_begin();
//	MachineBasicBlock::instr_iterator instrItrold;
//	//errs() << "pBB error 02\n";
//	for ( ; instrItr != B->instr_end() ; ++instrItr )
//	{
//		FunctionUnit.Iinsert( &*instrItr );
//		if( !instrItr->isInsideBundle() )
//		{
//			pHyperOpIndex++;
//		}
//		//errs() << "pBB error 03\n";
//		if( InstrItins && !InstrItins->isEmpty() )
//		{
//			const InstrStage *IS = InstrItins->beginStage( instrItr->getDesc().getSchedClass() );
//			const InstrStage *E = InstrItins->endStage( instrItr->getDesc().getSchedClass() );
//			int unit = 0;
//			for ( ; IS != E ; ++IS )
//			{
//				unsigned StageDepth = IS->getCycles();
//				//errs() << "pBB error 03a\n";
//				if( unit != 3 )
//					{
//						//errs() << "pBB error 03aa\n";
//						( this->pHyperBB[pHyperOpIndex] ).add_Edge( FunctionUnit.get_FuncUnitNumber( &*instrItr , unit ) , FunctionUnit.get_FuncUnitNumber( &*instrItr , unit + 1 ) , StageDepth );
//						//errs() << "pBB error 03ab\n";
//					}
//				else
//				{
//					//errs() << "pBB error 03b\n";
//					if( StageDepth <= 1 )
//						( this->pHyperBB[pHyperOpIndex] ).add_Edge( FunctionUnit.get_FuncUnitNumber( &*instrItr , unit ) , FunctionUnit.get_FuncUnitNumber( &*instrItr , unit + 1 ) , StageDepth );
//					else
//					{
//						//errs() << "pBB error 03c\n";
//						if( !CA[pHyperOpIndex].In_Cache( instrItr ) )
//						{
//							//errs() << "pBB error 03d\n";
//							( this->pHyperBB[pHyperOpIndex] ).add_Edge( FunctionUnit.get_FuncUnitNumber( &*instrItr , unit ) , FunctionUnit.get_FuncUnitNumber( &*instrItr , unit + 1 ) , StageDepth );
//						}
//						else
//						{
//							//errs() << "pBB error 03e\n";
//							( this->pHyperBB[pHyperOpIndex] ).add_Edge( FunctionUnit.get_FuncUnitNumber( &*instrItr , unit ) , FunctionUnit.get_FuncUnitNumber( &*instrItr , unit + 1 ) , 1 );
//						}
//					}
//				}
//				unit++;
//			}
//		}
//		//errs() << "pBB error 04\n";
//		if( pHyperOpIndex == pHyperOpIndexold )
//		{
//
//			for ( int unit = 0 ; unit < PIPEDEPTH ; unit++ )
//			{
//				( this->pHyperBB[pHyperOpIndex] ).add_Edge( FunctionUnit.get_FuncUnitNumber( &*instrItrold , unit + 1 ) , FunctionUnit.get_FuncUnitNumber( &*instrItr , unit ) , 0 );
//			}
//		}
//		//errs() << "pBB error 05\n";
//		pHyperOpIndexold = pHyperOpIndex;
//		instrItrold = instrItr;
//	}
////Handling Instruction Function Unit Dependency
////TODO Include Parallelism by Floating and Integer Instruction
////errs()<<"WCET: pH0: "<<this->pHyperBB[0].get_Wcet()<<" WCET: pH1: "<<this->pHyperBB[1].get_Wcet()<<" WCET: pH2: "<<this->pHyperBB[2].get_Wcet()<<" WCET: pH3: "<<this->pHyperBB[3].get_Wcet()<<"\n";
//}
//
////pHyperOpControlFlowGraph
//pHyperOpControlFlowGraph::pHyperOpControlFlowGraph( llvm::MachineFunction &MF , llvm::MachineDominatorTree *DT )
//{
//	int numblock = 0;
//	this->DT = DT;
//	errs() << "Successor BEGIN\n";
//	//List of Successor Machine Basic Block
//	for ( auto bitr = MF.begin() ; bitr != MF.end() ; ++bitr )
//	{
//		numblock++;
//		if( bitr->succ_size() == 0 )
//		{
//			this->LeafMBB.push_back( bitr );
//			errs() << "Leaf MBB are: " << bitr->getNumber() << "\n";
//		}
//	}
//
//	errs() << "DFS BEGIN\n";
//	//Apply DFS on all Leaf MBB
//	for ( auto leafitr = this->LeafMBB.begin() ; leafitr != this->LeafMBB.end() ; ++leafitr )
//	{
//		this->pHyperOpMayDFS( *leafitr );
//		errs() << "May Analysis Complete\n";
//		this->pHyperOpMustDFS( *leafitr );
//
//		errs() << "Completed  Leaf\n";
//	}
//
//	errs() << "pCFG :ADD Edges\n";
//	errs() << "# of Basic Blocks " << numblock << "\n";
//	errs() << "# of Basic Blocks " << this->MayLookUp[0].size() << "\n";
//
//	//ADD EDGES TO CFG
//	for ( auto& B : MF )
//	{
//		for ( MachineBasicBlock::succ_iterator succItr = B.succ_begin() ; succItr != B.succ_end() ; succItr++ )
//		{
//			for ( int pHyperCFGindex = 0 ; pHyperCFGindex < pHyperOpNumber ; pHyperCFGindex++ )
//			{
//				if( !DT->dominates( ( *succItr ) , ( &B ) ) )
//				{
//					this->pHyperMustCFG[pHyperCFGindex].add_Edge( this->MustLookUp[pHyperCFGindex].at( B.getNumber() ) , this->MustLookUp[pHyperCFGindex].at( ( *succItr )->getNumber() ) , this->MustLookUp[pHyperCFGindex].at( B.getNumber() )->get_Wcet() );
//					this->pHyperMayCFG[pHyperCFGindex].add_Edge( this->MayLookUp[pHyperCFGindex].at( B.getNumber() ) , this->MayLookUp[pHyperCFGindex].at( ( *succItr )->getNumber() ) , this->MayLookUp[pHyperCFGindex].at( B.getNumber() )->get_Wcet() );
//				}
//			}
//		}
//	}
//	//this->pHyperMayCFG[0].xdot();
//	//this->pHyperMustCFG[0].xdot();
//}
//void pHyperOpControlFlowGraph::pHyperOpMayDFS( llvm::MachineBasicBlock * MBB )
//{
//	//Return Memorize Computed Results
//	auto memitr = this->MayCA.find( MBB->getNumber() );
//	if( memitr != this->MayCA.end() )
//	{
//		errs() << "MEMOIZATION USED BY DFS\n";
//		return;
//	}
//
//	if( MBB->pred_size() == 0 )
//	{
//		//errs() << "ERROR 01 MBB Number: " << MBB->getNumber() << "\n";
//		CacheAnalysis *MayCA = new CacheAnalysis[pHyperOpNumber];
//		//errs() << "ERROR 010 MBB Number: " << MBB->getNumber() << "\n";
//		pHyperOpBasicBlock *pbb = new pHyperOpBasicBlock( MBB , MayCA );
//		//errs() << "ERROR 011 MBB Number: " << MBB->getNumber() << "\n";
//		for ( int pindex = 0 ; pindex < pHyperOpNumber ; pindex++ )
//		{
//			pHyperOpBB *obj = new pHyperOpBB;
//			*obj = pbb->pHyperBB[pindex];
//			this->MayLookUp[pindex].insert( std::make_pair( MBB->getNumber() , obj ) );
//			//errs() << "ERROR 012 MBB Number: " << MBB->getNumber() << "\n";
//			this->pHyperMayCFG[pindex].add_Vertex( obj );
//			//errs() << "BB #: " << MBB->getNumber() << "\n";
//		}
//		//errs() << "ERROR 02\n";
//		this->MayCA.insert( std::make_pair( MBB->getNumber() , MayCA ) );
//		return;
//	}
//	else
//	{
//		//errs() << "ERROR 04\n";
//
//		//errs() << "ERROR 05\n";
//		//Get CA of All parent Nodes
//		auto isvisit = this->Ismayvisted.find( MBB );
//		for ( auto preditr = MBB->pred_begin() ; preditr != MBB->pred_end() ; ++preditr )
//		{
//			if( this->DT->dominates( ( *preditr ) , ( MBB ) ) || ( isvisit == this->Ismayvisted.end() ) )
//			{
//				this->pHyperOpMayDFS( ( *preditr ) );
//				this->Ismayvisted.insert( std::make_pair( MBB , true ) );
//			}
//		}
//
//		CacheAnalysis *MayCA = new CacheAnalysis[pHyperOpNumber];
//	//	errs() << "ERROR 06\n";
//		auto preditr = MBB->pred_begin();
//		auto Aitr = MBB->pred_begin();
//		*MayCA = *this->MayCA.at( ( *Aitr )->getNumber() );
//
//		++preditr;
//		for ( ; preditr != MBB->pred_end() ; ++preditr )
//		{
//			if( this->DT->dominates( ( *preditr ) , ( MBB ) ) )
//			{
//				auto ptr = WCET::Union( MayCA , this->MayCA.at( ( *preditr )->getNumber() ) );
//				*MayCA = *ptr;
//			}
//		}
//		//errs() << "ERROR 07\n";
//		pHyperOpBasicBlock *pbb = new pHyperOpBasicBlock( MBB , MayCA );
//		for ( int pindex = 0 ; pindex < pHyperOpNumber ; pindex++ )
//		{
//			pHyperOpBB *obj = new pHyperOpBB;
//			*obj = pbb->pHyperBB[pindex];
//			this->MayLookUp[pindex].insert( std::make_pair( MBB->getNumber() , obj ) );
//			this->pHyperMayCFG[pindex].add_Vertex( obj );
//			//errs() << "BB #: " << MBB->getNumber() << "\n";
//		}
//		//errs() << "ERROR 08\n";
//		this->MayCA.insert( std::make_pair( MBB->getNumber() , MayCA ) );
//	//	errs() << "ERROR 09\n";
//		return;
//	}
//
//}
//void pHyperOpControlFlowGraph::pHyperOpMustDFS( llvm::MachineBasicBlock * MBB )
//{
//	//Return Memorize Computed Results
//	auto memitr = this->MustCA.find( MBB->getNumber() );
//	if( memitr != this->MustCA.end() )
//	{
//		//errs() << "MEMOIZATION USED BY DFS\n";
//		return;
//	}
//
//	if( MBB->pred_size() == 0 )
//	{
//		//errs() << "ERROR 10\n";
//		CacheAnalysis *MustCA = new CacheAnalysis[pHyperOpNumber];
//
//		pHyperOpBasicBlock *pbb = new pHyperOpBasicBlock( MBB , MustCA );
//
//		for ( int pindex = 0 ; pindex < pHyperOpNumber ; pindex++ )
//		{
//			pHyperOpBB *obj = new pHyperOpBB;
//			*obj = pbb->pHyperBB[pindex];
//			this->MustLookUp[pindex].insert( std::make_pair( MBB->getNumber() , obj ) );
//			this->pHyperMustCFG[pindex].add_Vertex( obj );
//		}
//		//errs() << "ERROR 11\n";
//		this->MustCA.insert( std::make_pair( MBB->getNumber() , MustCA ) );
//		//errs() << "ERROR 12\n";
//		return;
//	}
//	else
//	{
//		//errs() << "ERROR 13\n";
//
//		//errs() << "ERROR 14\n";
//		//Get CA of All parent Nodes
//		auto isvisit = this->Ismustvisted.find( MBB );
//		for ( auto preditr = MBB->pred_begin() ; preditr != MBB->pred_end() ; ++preditr )
//		{
//
//			if( this->DT->dominates( ( *preditr ) , ( MBB ) ) || ( isvisit == this->Ismustvisted.end() ) )
//			{
//				this->pHyperOpMustDFS( ( *preditr ) );
//				this->Ismustvisted.insert( std::make_pair( MBB , true ) );
//			}
//		}
//		//errs() << "ERROR 15\n";
//		CacheAnalysis *MustCA = new CacheAnalysis[pHyperOpNumber];
//
//		auto preditr = MBB->pred_begin();
//		auto Aitr = MBB->pred_begin();
//		*MustCA = *this->MustCA.at( ( *Aitr )->getNumber() );
//
//		++preditr;
//		for ( ; preditr != MBB->pred_end() ; ++preditr )
//		{
//			if( this->DT->dominates( ( *preditr ) , ( MBB ) ) )
//			{
//				auto ptr = WCET::Intersection( MustCA , this->MustCA.at( ( *preditr )->getNumber() ) );
//				*MustCA = *ptr;
//			}
//		}
//		//errs() << "ERROR 16\n";
//		pHyperOpBasicBlock *pbb = new pHyperOpBasicBlock( MBB , MustCA );
//		for ( int pindex = 0 ; pindex < pHyperOpNumber ; pindex++ )
//		{
//			pHyperOpBB *obj = new pHyperOpBB;
//			*obj = pbb->pHyperBB[pindex];
//			this->MustLookUp[pindex].insert( std::make_pair( MBB->getNumber() , obj ) );
//			this->pHyperMustCFG[pindex].add_Vertex( obj );
//		}
//	//	errs() << "ERROR 17\n";
//		this->MustCA.insert( std::make_pair( MBB->getNumber() , MustCA ) );
//	//	errs() << "ERROR 18\n";
//		return;
//	}
//
//}
////IntraHyperOp
//SingleHyperOpWcet::SingleHyperOpWcet( llvm::MachineFunction *MFI , MachineLoopInfo *LI , MachineDominatorTree *DT )
//{
//	this->MF = MFI;
////Memory Alias;
//	WCETMemoryAliasing( MFI );
////PartialCFG test;
////Loop Annotations
//
////-CREATE pCFG
//	errs() << "Creating pHyperOp Control Flow Graph\n";
//	WCET::pHyperOpControlFlowGraph mycfg( *MFI , DT );
//	errs() << "Completed pHyperOp Control Flow  Graph\n";
//	for ( int pHopindex = 0 ; pHopindex < pHyperOpNumber ; pHopindex++ )
//	{
//		this->pHopMayCFG[pHopindex] = mycfg.pHyperMayCFG[pHopindex];
//		this->MayLookUp[pHopindex] = mycfg.MayLookUp[pHopindex];
//		this->pHopMustCFG[pHopindex] = mycfg.pHyperMustCFG[pHopindex];
//		this->MustLookUp[pHopindex] = mycfg.MustLookUp[pHopindex];
//	}
//	errs() << "Copying DATA COMPLETE\n";
////-MERGING pHop into HyerOp
////-- ADDING VERTEX
//	this->IntraHopWCET.add_Vertex( CFGSTART );
//	this->IntraHopBCET.add_Vertex( CFGSTART );
//	for ( int pHopindex = 0 ; pHopindex < pHyperOpNumber ; pHopindex++ )
//	{
//		auto ls = this->pHopMayCFG[pHopindex].get_VertexList();
//		for ( auto lsitr = ls.begin() ; lsitr != ls.end() ; ++lsitr )
//		{
//			this->IntraHopBCET.add_Vertex( *lsitr );
//			this->pBBMayExtraTime[pHopindex].insert( std::make_pair( *lsitr , std::make_pair( 0 , 0 ) ) );
//		}
//	}
//	errs() << "BCET VETREX COMPLETE\n";
//	for ( int pHopindex = 0 ; pHopindex < pHyperOpNumber ; pHopindex++ )
//	{
//		auto ls = this->pHopMustCFG[pHopindex].get_VertexList();
//		for ( auto lsitr = ls.begin() ; lsitr != ls.end() ; ++lsitr )
//		{
//			this->IntraHopWCET.add_Vertex( *lsitr );
//			this->pBBMustExtraTime[pHopindex].insert( std::make_pair( *lsitr , std::make_pair( 0 , 0 ) ) );
//		}
//	}
//
//	this->IntraHopWCET.add_Vertex( CFGEND );
//	errs() << "WCET VETREX COMPLETE\n";
////- INCLUDING LOOP ANALYSIS
//	this->LoopAnalysis( LI );
//	this->LS.dump();
//	errs() << "LOOP COMPLETE\n";
////--ADD EDGE CFG
//	for ( int pHopindex = 0 ; pHopindex < pHyperOpNumber ; pHopindex++ )
//	{
//		this->IntraHopWCET.add_Edge( CFGSTART , mycfg.MustLookUp[pHopindex].at( 0 ) , 4 + ( pHopindex * 3 ) );
//		this->IntraHopBCET.add_Edge( CFGSTART , mycfg.MayLookUp[pHopindex].at( 0 ) , 4 + ( pHopindex * 3 ) );
//
//		auto mayls = this->pHopMayCFG[pHopindex].get_VertexList();
//		auto maylsitr = mayls.begin();
//		for ( ; maylsitr != mayls.end() ; ++maylsitr )
//		{
//
//			auto adjls = this->pHopMayCFG[pHopindex].get_AdjList( *maylsitr );
//			for ( auto adjlsitr = adjls.begin() ; adjlsitr != adjls.end() ; ++adjlsitr )
//			{
//				this->IntraHopBCET.add_Edge( *maylsitr , *adjlsitr , ( *maylsitr )->get_Wcet() + this->pBBMayExtraTime[pHopindex].at( *maylsitr ).first );
//			}
//			if( adjls.size() == 0 )
//			{
//				this->IntraHopBCET.add_Edge( *maylsitr , BCETEND( pHopindex ) , ( *maylsitr )->get_Wcet() + this->pBBMayExtraTime[pHopindex].at( *maylsitr ).first );
//			}
//		}
//
//		auto mustls = this->pHopMustCFG[pHopindex].get_VertexList();
//		auto mustlsitr = mustls.begin();
//		for ( ; mustlsitr != mustls.end() ; ++mustlsitr )
//		{
//			auto adjls = this->pHopMustCFG[pHopindex].get_AdjList( *mustlsitr );
//			for ( auto adjlsitr = adjls.begin() ; adjlsitr != adjls.end() ; ++adjlsitr )
//			{
//				this->IntraHopWCET.add_Edge( *mustlsitr , *adjlsitr , ( *mustlsitr )->get_Wcet() + this->pBBMustExtraTime[pHopindex].at( *mustlsitr ).second );
//			}
//
//			if( adjls.size() == 0 )
//			{
//				this->IntraHopWCET.add_Edge( *mustlsitr , CFGEND , ( *mustlsitr )->get_Wcet() + this->pBBMustExtraTime[pHopindex].at( *mustlsitr ).second );
//			}
//		}
//
//	}
//	errs() << "EDGE COMPLETE\n";
////	this->IntraHopWCET.xdot_CriticalPath();
////	this->IntraHopBCET.xdot_ShortestPath();
////-ADDING MPI
////--CREATING A LOOKUP
//	const TargetInstrInfo* TII;
//	TII = this->MF->getTarget().getInstrInfo();
//	const TargetMachine &TM = this->MF->getTarget();
//	HyperOpInteractionGraph * HIG = ( ( REDEFINETargetMachine& ) TM ).HIG;
//	HyperOp* currentHyperOp = HIG->getHyperOp( const_cast< Function* >( this->MF->getFunction() ) );
//
//	PHyperOpInteractionGraph phopDependence = currentHyperOp->getpHyperOpDependenceMap();
//	for ( auto pHopDependenceItr = phopDependence.rbegin() ; pHopDependenceItr != phopDependence.rend() ; ++pHopDependenceItr )
//	{
//		MachineInstr* s = pHopDependenceItr->first;
//		MachineInstr* t = pHopDependenceItr->second;
//		pHyperOpBB* pstart;
//		pHyperOpBB* pend;
//
//		//Dummy Nodes for MPI
//		pHyperOpBB *ptrs = new pHyperOpBB;
//		pHyperOpBB *ptre = new pHyperOpBB;
//		this->MPILookup.insert( make_pair( s , ptrs ) );
//		this->MPILookup.insert( make_pair( t , ptre ) );
//	}
//
////--ADDING NODES
//	std::map< llvm::MachineBasicBlock * , bool > Q;
//	Q.clear();
//	for ( auto itr = MPILookup.begin() ; itr != MPILookup.end() ; ++itr )
//	{
//		auto Iptr = itr->first;
//		auto MBB = Iptr->getParent();
//		auto findItr = Q.find( MBB );
//
//		if( findItr == Q.end() )
//		{
//			for ( auto litr = MBB->instr_rbegin() ; litr != MBB->instr_rend() ; ++litr )
//			{
//				auto MessageInstrItr = MPILookup.find( &*litr );
//				if( MessageInstrItr != MPILookup.end() )
//				{
//					for ( int pLookindex = 0 ; pLookindex < pHyperOpNumber ; pLookindex++ )
//					{
//						pHyperOpBB* pbbs = this->MustLookUp[pLookindex].at( MBB->getNumber() );
//						if( pbbs->Is_Vertex( FunctionUnit.get_FuncUnitNumber( ( &*litr ) , 0 ) ) )
//						{
//							this->IntraHopWCET.split_Vertex( pbbs , MessageInstrItr->second , pbbs->get_NodeWcet( FunctionUnit.get_FuncUnitNumber( &*litr , 4 ) ) );
//							//this->IntraHopBCET.split_Vertex( pbbs , MessageInstrItr->second , pbbs->get_NodeWcet( FunctionUnit.get_FuncUnitNumber( &*litr , 4 ) ) );
//
//						}
//
//					}
//
//				}
//
//			}
//			Q.insert( make_pair( MBB , true ) );
//		}
//	}
//
////--ADD EDGES
//	for ( auto pHopDependenceItr = phopDependence.rbegin() ; pHopDependenceItr != phopDependence.rend() ; ++pHopDependenceItr )
//	{
//		MachineInstr* s = pHopDependenceItr->first;
//		MachineInstr* t = pHopDependenceItr->second;
//		this->IntraHopWCET.add_Edge( this->MPILookup.at( s ) , this->MPILookup.at( t ) , pHyperMessageWorstLatency );
//	}
////	this->IntraHopWCET.xdot_CriticalPath();
////	this->IntraHopBCET.xdot_ShortestPath();
//}
//int SingleHyperOpWcet::get_Wcet()
//{
//	long int i = 0;
//	for ( auto itr = this->IntraHopWCET.CrticalPath.begin() ; itr != this->IntraHopWCET.CrticalPath.end() ; ++itr )
//	{
//		i += itr->first->get_Cardinality();
//	}
//	i = i / 6;
//	i = ( i / 8 ) * 7 + 7;
//	return ( this->IntraHopWCET.get_Wcet() + ( int ) i );
//
//}
//int SingleHyperOpWcet::get_WcetNode( MachineInstr* I )
//{
////errs()<<"Reached Here 1\n";
////	I->dump();
//	WCET::pHyperOpBB* src;
//	for ( int pidx = 0 ; pidx < pHyperOpNumber ; pidx++ )
//	{
//		//errs()<<"Reached Here 2\n";
//		src = this->MustLookUp[pidx].at( I->getParent()->getNumber() );
//		//errs()<<"Reached Here 3\n";
//		if( src->Is_Vertex( FunctionUnit.get_FuncUnitNumber( I , 4 ) ) )
//		{
//			//errs()<<"Reached Here 4\n";
//			int g = 0;
//			int b = 0;
//			g = this->IntraHopWCET.get_NodeWcet( src );
//			//errs()<<"Reached Here 5\n";
//			b = src->get_NodeWcet( FunctionUnit.get_FuncUnitNumber( ( I ) , 4 ) );
//			//errs()<<"Reached Here 6\n";
//			return ( b + g );
//		}
//	}
//	return 0;
//}
//int SingleHyperOpWcet::get_Bcet()
//{
//
//	return this->IntraHopBCET.get_Bcet();
//}
//int SingleHyperOpWcet::get_BcetNode( MachineInstr* I )
//{
//
//	WCET::pHyperOpBB* src;
//	for ( int pidx = 0 ; pidx < pHyperOpNumber ; pidx++ )
//	{
//		src = this->MayLookUp[pidx].at( I->getParent()->getNumber() );
//		errs()<<"may aa gya 01\n";
//		if( src->Is_Vertex( FunctionUnit.get_FuncUnitNumber( I , 4 ) ) )
//		{
//			int g = 0;
//			int b = 0;
//			g = this->IntraHopWCET.get_NodeBcet( src );
//			b = src->get_NodeBcet( FunctionUnit.get_FuncUnitNumber( ( I ) , 4 ) );
//			return ( b + g );
//		}
//	}
//	return 0;
//}
//void SingleHyperOpWcet::LoopAnalysis( MachineLoopInfo *LI )
//{
//	for ( MachineLoopInfo::iterator LIT = LI->begin() ; LIT != LI->end() ; ++LIT )
//	{
//		MachineLoop* l = *LIT;
//		//errs() << "LOOP00\n";
//		LsDFS( l );
//		//errs() << "LOOP01\n";
//		auto ls = this->LS.get_VertexList();
//		for ( auto itr = ls.begin() ; itr != ls.end() ; ++itr )
//		{
//			this->pLoopBound.insert( std::make_pair( *itr , std::make_pair( 18 , 18 ) ) );
//		}
//		//errs() << "LOOP02\n";
//		LoopDFS( l );
//		//errs() << "LOOP03\n";
//	}
//	//errs() << "LOOP04\n";
//}
//void SingleHyperOpWcet::LsDFS( MachineLoop *L )
//{
//	this->LS.add_Vertex( L );
//	std::vector< MachineLoop* > subLoops = L->getSubLoops();
//	for ( MachineLoop::iterator subitr = subLoops.begin() ; subitr != subLoops.end() ; ++subitr )
//	{
//		this->LS.add_Edge( L , *subitr , L->getLoopDepth() );
//		LsDFS( *subitr );
//	}
//
//}
//void SingleHyperOpWcet::LoopDFS( MachineLoop *L )
//{
//	int min = 0;
//	int max = 0;
//	std::vector< MachineLoop* > subLoops = L->getSubLoops();
//	//errs() << "LOOP02a\n";
//	for ( MachineLoop::iterator subitr = subLoops.begin() ; subitr != subLoops.end() ; ++subitr )
//	{
//		LoopDFS( *subitr );
//	}
//	//errs() << "LOOP02b\n";
//// ADD VERTEX
//	PartialCFG mypartialBCETcfg[pHyperOpNumber];
//	PartialCFG mypartialWCETcfg[pHyperOpNumber];
//	for ( auto BBitr = ( L )->block_begin() ; BBitr != ( L )->block_end() ; ++BBitr )
//	{
//		for ( int pindex = 0 ; pindex < pHyperOpNumber ; pindex++ )
//		{
//			mypartialBCETcfg[pindex].add_Vertex( this->MayLookUp[pindex].at( ( *BBitr )->getNumber() ) );
//			mypartialWCETcfg[pindex].add_Vertex( this->MustLookUp[pindex].at( ( *BBitr )->getNumber() ) );
//		}
//	}
//	//errs() << "LOOP02c\n";
////ADD EDGE
//	for ( int pindex = 0 ; pindex < pHyperOpNumber ; pindex++ )
//	{
//		auto ls = mypartialWCETcfg[pindex].get_VertexList();
//		for ( auto itr = ls.begin() ; itr != ls.end() ; ++itr )
//		{
//
//			auto adjls = this->pHopMustCFG[pindex].get_AdjList( *itr );
//			for ( auto litr = adjls.begin() ; litr != adjls.end() ; ++litr )
//			{
//				if( mypartialWCETcfg[pindex].Is_Vertex( *litr ) )
//				{
//					mypartialWCETcfg[pindex].add_Edge( *itr , *litr , ( *itr )->get_Wcet() + this->pBBMustExtraTime[pindex].at( *itr ).second );
//				}
//
//			}
//
//		}
//		//errs() << "LOOP02d\n";
//		ls = mypartialBCETcfg[pindex].get_VertexList();
//		for ( auto itr = ls.begin() ; itr != ls.end() ; ++itr )
//		{
//
//			auto adjls = this->pHopMayCFG[pindex].get_AdjList( *itr );
//			for ( auto litr = adjls.begin() ; litr != adjls.end() ; ++litr )
//			{
//				if( mypartialBCETcfg[pindex].Is_Vertex( *litr ) )
//				{
//					mypartialBCETcfg[pindex].add_Edge( *itr , *litr , ( *itr )->get_Wcet() + this->pBBMayExtraTime[pindex].at( *itr ).first );
//				}
//
//			}
//
//		}
//		//errs() << "LOOP02e: " << ls.size() << "\n";
//		ls = mypartialBCETcfg[pindex].get_VertexList();
//		for ( auto itr = ls.begin() ; itr != ls.end() ; ++itr )
//		{
//			if( mypartialBCETcfg[pindex].get_VertexDegree( *itr ) == 0 )
//			{
//			//	errs() << "LOOP02ei\n";
//				mypartialBCETcfg[pindex].add_Edge( *itr , CFGEND , ( *itr )->get_Wcet() + this->pBBMayExtraTime[pindex].at( *itr ).first );
//			}
//		}
//
//		ls = mypartialWCETcfg[pindex].get_VertexList();
//		for ( auto itr = ls.begin() ; itr != ls.end() ; ++itr )
//		{
//			if( mypartialWCETcfg[pindex].get_VertexDegree( *itr ) == 0 )
//			{
//		//		errs() << "LOOP02ei\n";
//				mypartialWCETcfg[pindex].add_Edge( *itr , CFGEND , ( *itr )->get_Wcet() + this->pBBMustExtraTime[pindex].at( *itr ).second );
//			}
//		}
//
//		//errs() << "LOOP02f\n";
//		min = mypartialBCETcfg[pindex].get_Bcet();
//		min = min * this->pLoopBound.at( L ).first;
//		max = mypartialWCETcfg[pindex].get_Wcet();
//		max = max * this->pLoopBound.at( L ).second;
//	//	errs() << "LOOP02g\n";
//		this->pBBMayExtraTime[pindex].at( this->MayLookUp[pindex].at( ( L->getHeader() )->getNumber() ) ).first = min;
//		this->pBBMustExtraTime[pindex].at( this->MustLookUp[pindex].at( ( L->getHeader() )->getNumber() ) ).second = max;
//	//	errs() << "LOOP02h\n";
//		for ( MachineLoop::iterator subloopitr = subLoops.begin() ; subloopitr != subLoops.end() ; ++subloopitr )
//		{
//			this->pBBMayExtraTime[pindex].at( this->MayLookUp[pindex].at( ( ( *subloopitr )->getHeader() )->getNumber() ) ).first = 0;
//			this->pBBMustExtraTime[pindex].at( this->MustLookUp[pindex].at( ( ( *subloopitr )->getHeader() )->getNumber() ) ).second = 0;
//			this->pBBMayExtraTime[pindex].at( this->MayLookUp[pindex].at( ( ( *subloopitr )->getHeader() )->getNumber() ) ).second = 0;
//			this->pBBMustExtraTime[pindex].at( this->MustLookUp[pindex].at( ( ( *subloopitr )->getHeader() )->getNumber() ) ).first = 0;
//		}
//	//	errs() << "LOOP02i\n";
//	}
//
//}
//void SingleHyperOpWcet::set_LoopBound( MachineLoop * l , unsigned int min , unsigned max )
//{
//	auto itr = LoopBounds.find( l );
//	if( itr != LoopBounds.end() )
//	{
//		LoopBounds.at( l ).first = min;
//		LoopBounds.at( l ).second = max;
//	}
//	else
//		LoopBounds.insert( std::make_pair( l , std::make_pair( min , max ) ) );
//
//}
//void SingleHyperOpWcet::set_LoopUpperBound( MachineLoop * l , unsigned int max )
//{
//	auto itr = LoopBounds.find( l );
//	if( itr != LoopBounds.end() )
//		( LoopBounds.at( l ) ).second = max;
//	else
//		LoopBounds.insert( std::make_pair( l , std::make_pair( 0 , max ) ) );
//
//}
//void SingleHyperOpWcet::set_LoopLowerBound( MachineLoop * l , unsigned min )
//{
//	auto itr = LoopBounds.find( l );
//	if( itr != LoopBounds.end() )
//		( LoopBounds.at( l ) ).first = min;
//	else
//		LoopBounds.insert( std::make_pair( l , std::make_pair( min , MAXLOOPBOUND ) ) );
//
//}
//unsigned int SingleHyperOpWcet::get_LoopUpperBound( MachineLoop * l )
//{
//	auto itr = LoopBounds.find( l );
//	if( itr != LoopBounds.end() )
//		return LoopBounds.at( l ).second;
//	else
//		return MAXLOOPBOUND;
//}
//unsigned int SingleHyperOpWcet::get_LoopLowerBound( MachineLoop * l )
//{
//	auto itr = LoopBounds.find( l );
//	if( itr != LoopBounds.end() )
//		return LoopBounds.at( l ).first;
//	else
//		return 0;
//}
//void SingleHyperOpWcet::dumpLoop()
//{
//	auto ls = this->LS.get_VertexList();
//	for ( auto itr = ls.begin() ; itr != ls.end() ; ++itr )
//	{
//
//		for ( auto BBitr = ( *itr )->block_begin() ; BBitr != ( *itr )->block_end() ; ++BBitr )
//		{
//			errs() << ( *BBitr )->getNumber() << ", ";
//		}
//		errs() << "\n";
//
//	}
//}
//
////CLASS Functional Unit
//template<class T> unsigned long int FuncUnit< T >::ID;
//template<class T> FuncUnit< T >::FuncUnit()
//{
//	this->TLookup.clear();
//}
//template<class T> void FuncUnit< T >::Ireset()
//{
//	this->TLookup.clear();
//	this->ID = 0;
//}
//template<class T> void FuncUnit< T >::Iinsert( T _u )
//{
//	if( this->TLookup.insert( std::make_pair( _u , this->ID ) ).second )
//		this->ID = this->ID + 1;
//}
//template<class T> void FuncUnit< T >::Idelete( T _u )
//{
//	typename FuncMap::iterator itr = this->TLookup.find( _u );
//	if( itr != this->TLookup.end() )
//		this->TLookup.erase( itr );
//
//}
//template<class T> void FuncUnit< T >::dump()
//{
//	typename FuncMap::iterator itr;
//	for ( itr = this->TLookup.begin(); itr != this->TLookup.end() ; ++itr )
//		std::cout << "Key: " << itr->first << ", Value: " << itr->second << "\n";
//}
//template<class T> unsigned long int FuncUnit< T >::get_FuncUnitNumber( T _u , unsigned _stage )
//{
////_stage should be <= PIPEDEPTH
//	unsigned long int i;
//	typename FuncMap::iterator itr = this->TLookup.find( _u );
//	if( itr != this->TLookup.end() )
//	{
//		i = this->TLookup.at( _u ) * ( PIPEDEPTH + 1 ) + _stage;
//	}
//	else
//	{
//		i = -1;
//	}
//
////*
//	return i;
//}
//
////DONE CLASS AdjEdge
//template<class T> AdjEdge< T >::AdjEdge( T _v , int _w )
//{
//	Vertex = _v;
//	Weight = _w;
//}
//template<class T> int AdjEdge< T >::get_weight()
//{
//	return this->Weight;
//}
//template<class T> T AdjEdge< T >::get_vertex()
//{
//	return this->Vertex;
//}
//template<class T> void AdjEdge< T >::set_weight( int _w )
//{
//	this->Weight = _w;
//}
//
////DONE CLASS DWGraph
//template<class T> DWGraph< T >::DWGraph()
//{
//	this->G.clear();
//	this->n = 0;
//}
//template<class T> unsigned long int DWGraph< T >::get_SizeOfGraph()
//{
//	unsigned long int size = 0;
//	for ( typename DWG::iterator itr = this->G.begin() ; itr != this->G.end() ; ++itr )
//	{
//		size += get_VertexDegree( itr->first );
//	}
//	return size;
//
//}
//template<class T> bool DWGraph< T >::add_Vertex( T _u )
//{
//
//	if( this->G.insert( make_pair( _u , new AdjList ) ).second )
//	{
//		this->Lookup.insert( std::make_pair( _u , n ) );
//		this->n++;
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}
//template<class T> bool DWGraph< T >::split_Vertex( T _u , T _v , int delta )
//{
//	typename DWG::iterator itr = this->G.find( _u );
//	typename DWG::iterator jitr;
//	if( itr == this->G.end() )
//	{
//		return false;
//	}
//	else
//	{
//
//		if( this->G.insert( make_pair( _v , this->G.at( _u ) ) ).second )
//		{
//			this->Lookup.insert( std::make_pair( _v , n ) );
//			this->n++;
//
//			this->G.at( _u ) = new AdjList;
//			AdjEdge< T > *E = new AdjEdge< T >( _v , delta );
//			this->G.at( _u )->push_back( *E );
//
//			jitr = this->G.find( _v );
//			for ( typename AdjList::iterator litr = ( ( *jitr ).second )->begin() ; litr != ( ( *jitr ).second )->end() ; ++litr )
//			{
//				litr->set_weight( litr->get_weight() - delta );
//			}
//
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//}
//template<class T> bool DWGraph< T >::Is_Vertex( T _u )
//{
//	typename DWG::iterator itr = this->G.find( _u );
//	return ( itr != this->G.end() );
//
//}
//template<class T> bool DWGraph< T >::Is_Edge( T _u , T _v )
//{
//	bool ue , ve;
//	typename DWG::iterator itr = this->G.find( _u );
//	ue = ( itr == this->G.end() );
//	ve = ( this->G.find( _v ) == this->G.end() );
//
//	if( ue || ve )
//		return false;
//
//	for ( typename AdjList::iterator litr = ( ( *itr ).second )->begin() ; litr != ( ( *itr ).second )->end() ; ++litr )
//	{
//		if( litr->get_vertex() == _v )
//			return true;
//	}
//	return false;
//}
//template<class T> bool DWGraph< T >::set_EdgeWeight( T _u , T _v , int _w )
//{
//	bool ue , ve;
//	typename DWG::iterator itr = this->G.find( _u );
//	ue = ( itr != this->G.end() );
//	ve = ( this->G.find( _v ) != this->G.end() );
//	if( ue && ve )
//	{
//
//		for ( typename AdjList::iterator litr = ( ( *itr ).second )->begin() ; litr != ( ( *itr ).second )->end() ; ++litr )
//		{
//			if( litr->get_vertex() == _v )
//				litr->set_weight( _w );
//		}
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//
//}
//template<class T> bool DWGraph< T >::add_Edge( T _u , T _v , int _w )
//{
//	AdjEdge< T > *E = new AdjEdge< T >( _v , _w );
//	bool ue , ve;
//	ue = ( this->G.find( _u ) != this->G.end() );
//	ve = ( this->G.find( _v ) != this->G.end() );
//	if( ue && ve )
//		this->G.at( _u )->push_back( *E );
//	else
//	{
//		add_Vertex( _u );
//		add_Vertex( _v );
//		this->G.at( _u )->push_back( *E );
//	}
//	return true;
//}
//template<class T> bool DWGraph< T >::remove_Edge( T _u , T _v )
//{
//	bool ue , ve;
//	ue = ( this->G.find( _u ) != this->G.end() );
//	ve = ( this->G.find( _v ) != this->G.end() );
//	if( ue && ve )
//	{
//		typename DWG::iterator itr = this->G.find( _u );
//		for ( typename AdjList::iterator litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
//		{
//			if( ( this->Lookup.at( litr->get_vertex() ) ) == ( this->Lookup.at( _v ) ) )
//			{
//				( this->G.at( _u ) )->erase( litr-- );
//			}
//		}
//		return true;
//	}
//
//	return false;
//
//}
//template<class T> void DWGraph< T >::dump()
//{
//	for ( typename DWG::iterator itr = this->G.begin() ; itr != this->G.end() ; ++itr )
//	{
//		std::cout << this->Lookup.at( ( *itr ).first ) << ": ";
//		for ( typename AdjList::iterator litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
//		{
//			std::cout << " " << this->Lookup.at( litr->get_vertex() ) << "(" << litr->get_weight() << "),";
//		}
//		std::cout << std::endl;
//	}
//}
//template<class T> void DWGraph< T >::xdot()
//{
//	std::ofstream df;
//	std::string file( "graph" );
//	file.append( itostr((int64_t)WCET::get_gidx()) );
//	file.append( ".dot" );
//	df.open( file.c_str() );
//	df << "digraph{\n";
//	for ( typename DWG::iterator itr = this->G.begin() ; itr != this->G.end() ; ++itr )
//	{
//		df << this->Lookup.at( ( *itr ).first ) << "[label=\"" << this->Lookup.at( ( *itr ).first ) << "\"];\n";
//	}
//	for ( typename DWG::iterator itr = this->G.begin() ; itr != this->G.end() ; ++itr )
//	{
//		for ( typename AdjList::iterator litr = ( ( *itr ).second )->begin() ; litr != ( ( *itr ).second )->end() ; ++litr )
//		{
//			df << " " << this->Lookup.at( ( *itr ).first ) << "->" << this->Lookup.at( litr->get_vertex() ) << "[label=" << litr->get_weight() << "];\n";
//		}
//	}
//	df << "}\n";
//	df.close();
//}
//template<class T> unsigned long int DWGraph< T >::get_Cardinality()
//{
//	return this->n;
//}
//template<class T> unsigned long int DWGraph< T >::get_VertexDegree( T _u )
//{
//	return this->G.at( _u )->size();
//}
//template<class T> int DWGraph< T >::get_EdgeWeight( T _u , T _v )
//{
//	bool ue , ve;
//	typename DWG::iterator itr = this->G.find( _u );
//	ue = ( itr == this->G.end() );
//	ve = ( this->G.find( _v ) == this->G.end() );
//
//	if( ue || ve )
//		return INT_MIN;
//
//	for ( typename AdjList::iterator litr = ( ( *itr ).second )->begin() ; litr != ( ( *itr ).second )->end() ; ++litr )
//	{
//		if( litr->get_vertex() == _v )
//			return litr->get_weight();
//	}
//
//	return INT_MIN;
//}
//template<class T> list< T > DWGraph< T >::get_VertexList()
//{
//	list< T > l;
//	l.clear();
//	for ( typename DWG::iterator itr = this->G.begin() ; itr != this->G.end() ; ++itr )
//	{
//		l.push_back( itr->first );
//	}
//	return l;
//}
//template<class T> list< T > DWGraph< T >::get_AdjList( T _u )
//{
//	list< T > l;
//	l.clear();
//	typename DWG::iterator itr = this->G.find( _u );
//	if( itr != this->G.end() )
//	{
//		for ( typename AdjList::iterator litr = ( ( *itr ).second )->begin() ; litr != ( ( *itr ).second )->end() ; ++litr )
//		{
//			l.push_back( litr->get_vertex() );
//		}
//	}
//	return l;
//}
//
////CLASS DWAGraph
//template<class T> DWAGraph< T >::DWAGraph()
//{
//	this->G.clear();
//	this->n = 0;
//	this->TSL.clear();
//}
//template<class T> void DWAGraph< T >::TopologicalSorting()
//{
//	std::queue< T > Q;
//	std::map< T , int > indegree;
//	T _u;
//	typename DWGraph< T >::DWG::iterator _uitr;
//	typename DWGraph< T >::DWG tsg( this->G );
//
//	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
//	{
//		indegree.insert( std::make_pair( ( itr->first ) , 0 ) );
//	}
//
//	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
//	{
//		for ( typename DWGraph< T >::AdjList::iterator litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
//		{
//			indegree.at( litr->get_vertex() ) += 1;
//		}
//	}
//
//	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
//	{
//		if( indegree.at( itr->first ) == 0 )
//		{
//			Q.push( itr->first );
//			indegree.at( itr->first ) -= 1;
//		}
//	}
//	while ( !Q.empty() )
//	{
//		_u = Q.front();
//		Q.pop();
//		this->TSL.push_back( _u );
//		_uitr = tsg.find( _u );
//
//		for ( typename DWGraph< T >::AdjList::iterator litr = _uitr->second->begin() ; litr != _uitr->second->end() ; ++litr )
//		{
//			indegree.at( litr->get_vertex() ) -= 1;
//			if( indegree.at( litr->get_vertex() ) == 0 )
//			{
//				Q.push( litr->get_vertex() );
//			}
//		}
//		tsg.erase( _u );
//	}
//
//}
//template<class T> void DWAGraph< T >::dump_TSL()
//{
//	if( this->TSL.empty() )
//		this->TopologicalSorting();
//
//	for ( typename std::list< T >::iterator itr = this->TSL.begin() ; itr != this->TSL.end() ; ++itr )
//	{
//		std::cout << this->Lookup.at( *itr ) << ",";
//	}
//	std::cout << std::endl;
//
//}
//template<class T> void DWAGraph< T >::CriticalPath()
//{
//	std::queue< T > Q;
////Vertex , Degree , Weight
//	std::map< T , std::pair< int , int > > indegree;
//	T _u;
//	typename DWGraph< T >::DWG::iterator _uitr;
//	typename DWGraph< T >::DWG tsg( this->G );
//	std::multimap< int , T > CTVM;
//	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
//	{
//		indegree.insert( std::make_pair( ( itr->first ) , std::make_pair( 0 , 0 ) ) );
//	}
//
//	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
//	{
//		for ( typename DWGraph< T >::AdjList::iterator litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
//		{
//			indegree.at( litr->get_vertex() ).first += 1;
//		}
//	}
//
//	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
//	{
//		if( indegree.at( itr->first ).first == 0 )
//		{
//			Q.push( itr->first );
//			indegree.at( itr->first ).first -= 1;
//		}
//	}
//
//	while ( !Q.empty() )
//	{
//		_u = Q.front();
//		Q.pop();
//		CTVM.insert( std::make_pair( indegree.at( _u ).second , _u ) );
//		_uitr = tsg.find( _u );
//
//		for ( typename DWGraph< T >::AdjList::iterator litr = _uitr->second->begin() ; litr != _uitr->second->end() ; ++litr )
//		{
//			indegree.at( litr->get_vertex() ).first -= 1;
//			indegree.at( litr->get_vertex() ).second = std::max( indegree.at( litr->get_vertex() ).second , indegree.at( _u ).second + litr->get_weight() );
//			if( indegree.at( litr->get_vertex() ).first == 0 )
//			{
//				Q.push( litr->get_vertex() );
//			}
//		}
//		tsg.erase( _u );
//	}
//
////Critical Path
//
//	typename std::multimap< int , T >::reverse_iterator iitr = CTVM.rbegin();
//	typename std::multimap< int , T >::reverse_iterator jitr = CTVM.rbegin();
//	this->CrticalPath.insert( std::make_pair( ( iitr->second ) , ( iitr->first ) ) );
//	while ( jitr != CTVM.rend() )
//	{
//		if( iitr->first == jitr->first + this->get_EdgeWeight( jitr->second , iitr->second ) )
//		{
//			this->CrticalPath.insert( std::make_pair( ( jitr->second ) , ( jitr->first ) ) );
//			iitr = jitr;
//		}
//		++jitr;
//	}
//
//}
//template<class T> int DWAGraph< T >::get_Wcet()
//{
//	if( this->G.empty() )
//		return 0;
//
//	std::queue< T > Q;
////Vertex , Degree , Weight
//	std::map< T , std::pair< int , int > > indegree;
//	T _u;
//	typename DWGraph< T >::DWG::iterator _uitr;
//	typename DWGraph< T >::DWG tsg( this->G );
//	this->CTVM.clear();
//	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
//	{
//		indegree.insert( std::make_pair( ( itr->first ) , std::make_pair( 0 , 0 ) ) );
//	}
//
//	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
//	{
//		for ( typename DWGraph< T >::AdjList::iterator litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
//		{
//			indegree.at( litr->get_vertex() ).first += 1;
//		}
//	}
//
//	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
//	{
//		if( indegree.at( itr->first ).first == 0 )
//		{
//			Q.push( itr->first );
//			indegree.at( itr->first ).first -= 1;
//		}
//	}
//
//	while ( !Q.empty() )
//	{
//		_u = Q.front();
//		Q.pop();
//		this->CTVM.insert( std::make_pair( indegree.at( _u ).second , _u ) );
//		_uitr = tsg.find( _u );
//
//		for ( typename DWGraph< T >::AdjList::iterator litr = _uitr->second->begin() ; litr != _uitr->second->end() ; ++litr )
//		{
//			indegree.at( litr->get_vertex() ).first -= 1;
//			indegree.at( litr->get_vertex() ).second = std::max( indegree.at( litr->get_vertex() ).second , indegree.at( _u ).second + litr->get_weight() );
//			if( indegree.at( litr->get_vertex() ).first == 0 )
//			{
//				Q.push( litr->get_vertex() );
//			}
//		}
//		tsg.erase( _u );
//	}
//
//	return this->CTVM.rbegin()->first;
//
//}
//template<class T> int DWAGraph< T >::get_NodeWcet( T Node )
//{
//	if( this->G.empty() )
//		return 0;
//
//	if( this->CTVM.empty() )
//	{
//		std::queue< T > Q;
//		//Vertex , Degree , Weight
//		std::map< T , std::pair< int , int > > indegree;
//		T _u;
//		typename DWGraph< T >::DWG::iterator _uitr;
//		typename DWGraph< T >::DWG tsg( this->G );
//		for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
//		{
//			indegree.insert( std::make_pair( ( itr->first ) , std::make_pair( 0 , 0 ) ) );
//		}
//
//		for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
//		{
//			for ( typename DWGraph< T >::AdjList::iterator litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
//			{
//				indegree.at( litr->get_vertex() ).first += 1;
//			}
//		}
//
//		for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
//		{
//			if( indegree.at( itr->first ).first == 0 )
//			{
//				Q.push( itr->first );
//				indegree.at( itr->first ).first -= 1;
//			}
//		}
//
//		while ( !Q.empty() )
//		{
//			_u = Q.front();
//			Q.pop();
//			CTVM.insert( std::make_pair( indegree.at( _u ).second , _u ) );
//			_uitr = tsg.find( _u );
//
//			for ( typename DWGraph< T >::AdjList::iterator litr = _uitr->second->begin() ; litr != _uitr->second->end() ; ++litr )
//			{
//				indegree.at( litr->get_vertex() ).first -= 1;
//				indegree.at( litr->get_vertex() ).second = std::max( indegree.at( litr->get_vertex() ).second , indegree.at( _u ).second + litr->get_weight() );
//				if( indegree.at( litr->get_vertex() ).first == 0 )
//				{
//					Q.push( litr->get_vertex() );
//				}
//			}
//			tsg.erase( _u );
//		}
//	}
//	for ( typename std::multimap< int , T >::iterator mitr = this->CTVM.begin() ; mitr != this->CTVM.end() ; ++mitr )
//	{
//		if( ( *mitr ).second == Node )
//			return ( *mitr ).first;
//	}
//	return 0;
//
//}
//template<class T> int DWAGraph< T >::get_Bcet()
//{
//	if( this->G.empty() )
//	{
//		return 0;
//	}
//	this->Distance.clear();
//	int MIN = 0;
//	std::map< T , bool > IsVisited;
//	std::map< T , int > indegree;
//	T StartNode;
//	T Node;
//
////Initialization
//	for ( auto itr = this->G.begin() ; itr != this->G.end() ; ++itr )
//	{
//		indegree.insert( std::make_pair( itr->first , 0 ) );
//		IsVisited.insert( std::make_pair( itr->first , false ) );
//		this->Distance.insert( std::make_pair( itr->first , INT_MAX ) );
//	}
////Find Source
//	for ( auto itr = this->G.begin() ; itr != this->G.end() ; ++itr )
//	{
//		for ( auto litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
//		{
//			indegree.at( litr->get_vertex() ) += 1;
//		}
//	}
//	for ( auto itr = this->G.begin() ; itr != this->G.end() ; ++itr )
//	{
//		if( indegree.at( itr->first ) == 0 )
//		{
//			StartNode = itr->first;
//			break;
//		}
//	}
//	Distance.at( StartNode ) = 0;
////Find Shortest Paths
//	for ( auto itr = this->G.begin() ; itr != this->G.end() ; ++itr )
//	{
//		int min = INT_MAX;
//		for ( auto ditr = Distance.begin() ; ditr != Distance.end() ; ++ditr )
//		{
//			if( ( min > ( ditr->second ) ) && !IsVisited.at( ditr->first ) )
//			{
//				Node = ditr->first;
//				min = ditr->second;
//			}
//		}
//
//		IsVisited.at( Node ) = true;
//
//		for ( auto litr = this->G.at( Node )->begin() ; litr != this->G.at( Node )->end() ; ++litr )
//		{
//			auto V = litr->get_vertex();
//			if( !IsVisited.at( V ) )
//			{
//				if( Distance.at( V ) > ( Distance.at( Node ) + litr->get_weight() ) )
//				{
//					Distance.at( V ) = ( Distance.at( Node ) + litr->get_weight() );
//				}
//			}
//		}
//
//	}
//
////Report Max as BCET
//	for ( auto MaxItr = this->G.begin() ; MaxItr != this->G.end() ; ++MaxItr )
//	{
//		if( this->get_VertexDegree( MaxItr->first ) == 0 )
//		{
//			if( MIN < Distance.at( MaxItr->first ) )
//			{
//				MIN = Distance.at( MaxItr->first );
//			}
//		}
//	}
//	return MIN;
//
//}
//template<class T> int DWAGraph< T >::get_NodeBcet( T Node )
//{
//	if( Distance.empty() )
//	{
//		this->get_Bcet();
//	}
//	errs()<<"may aa gya 03a\n";
//	return Distance.at( Node );
//}
//template<class T> void DWAGraph< T >::dump_CriticalPath()
//{
//	if( this->CrticalPath.empty() )
//		this->CriticalPath();
//
//	for ( typename std::map< T , int >::iterator itr = this->CrticalPath.begin() ; itr != this->CrticalPath.end() ; ++itr )
//	{
//		std::cout << this->Lookup.at( itr->first ) << "(" << itr->second << "), ";
//	}
//	std::cout << std::endl;
//}
//template<class T> void DWAGraph< T >::xdot_CriticalPath()
//{
//	std::queue< T > Q;
////Vertex , Degree , Weight
//	std::map< T , std::pair< int , int > > indegree;
//	T _u;
//	typename DWGraph< T >::DWG::iterator _uitr;
//	typename DWGraph< T >::DWG tsg( this->G );
//	std::map< T , int > VCTM;
//	std::multimap< int , T > CTVM;
//	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
//	{
//		indegree.insert( std::make_pair( ( itr->first ) , std::make_pair( 0 , 0 ) ) );
//	}
//
//	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
//	{
//		for ( typename DWGraph< T >::AdjList::iterator litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
//		{
//			indegree.at( litr->get_vertex() ).first += 1;
//		}
//	}
//
//	for ( typename DWGraph< T >::DWG::iterator itr = tsg.begin() ; itr != tsg.end() ; ++itr )
//	{
//		if( indegree.at( itr->first ).first == 0 )
//		{
//			Q.push( itr->first );
//			indegree.at( itr->first ).first -= 1;
//		}
//	}
//
//	while ( !Q.empty() )
//	{
//		_u = Q.front();
//		Q.pop();
//		VCTM.insert( std::make_pair( _u , indegree.at( _u ).second ) );
//		CTVM.insert( std::make_pair( indegree.at( _u ).second , _u ) );
//		_uitr = tsg.find( _u );
//
//		for ( typename DWGraph< T >::AdjList::iterator litr = _uitr->second->begin() ; litr != _uitr->second->end() ; ++litr )
//		{
//			indegree.at( litr->get_vertex() ).first -= 1;
//			indegree.at( litr->get_vertex() ).second = std::max( indegree.at( litr->get_vertex() ).second , indegree.at( _u ).second + litr->get_weight() );
//			if( indegree.at( litr->get_vertex() ).first == 0 )
//			{
//				Q.push( litr->get_vertex() );
//			}
//		}
//		tsg.erase( _u );
//	}
//
//	std::ofstream df;
//	std::string file( "graph" );
//	file.append( itostr( WCET::get_gidx() ) );
//	file.append( ".dot" );
//	df.open( file.c_str() );
//	df << "digraph{\n";
//	if( XDOTRANK )
//		df << "\trankdir=LR;\n";
//
//	for ( typename DWGraph< T >::DWG::iterator itr = this->G.begin() ; itr != this->G.end() ; ++itr )
//	{
//		df << this->Lookup.at( ( *itr ).first ) << "[label=\"" << this->Lookup.at( ( *itr ).first ) << " (@" << VCTM.at( itr->first ) << ")" << "\"];\n";
//	}
//	for ( typename DWGraph< T >::DWG::iterator itr = this->G.begin() ; itr != this->G.end() ; ++itr )
//	{
//		for ( typename DWGraph< T >::AdjList::iterator litr = ( ( *itr ).second )->begin() ; litr != ( ( *itr ).second )->end() ; ++litr )
//		{
//			df << " " << this->Lookup.at( ( *itr ).first ) << "->" << this->Lookup.at( litr->get_vertex() ) << "[label=" << litr->get_weight() << "];\n";
//		}
//	}
//
////Critical Path
//
//	typename std::multimap< int , T >::reverse_iterator iitr = CTVM.rbegin();
//	typename std::multimap< int , T >::reverse_iterator jitr = CTVM.rbegin();
//	while ( jitr != CTVM.rend() )
//	{
//		if( iitr->first == jitr->first + this->get_EdgeWeight( jitr->second , iitr->second ) )
//		{
//			df << this->Lookup.at( iitr->second ) << "[fillcolor=red, style=filled];\n";
//			df << this->Lookup.at( jitr->second ) << "[fillcolor=red, style=filled];\n";
//			iitr = jitr;
//		}
//		++jitr;
//	}
//	if( XDOTRANK )
//	{
//		df << "{rank=same; ";
//		iitr = CTVM.rbegin();
//		jitr = CTVM.rbegin();
//		while ( jitr != CTVM.rend() )
//		{
//			if( iitr->first == jitr->first + this->get_EdgeWeight( jitr->second , iitr->second ) )
//			{
//				df << this->Lookup.at( iitr->second ) << " " << this->Lookup.at( jitr->second ) << " ";
//				iitr = jitr;
//			}
//			++jitr;
//		}
//
//		df << "}\n";
//	}
//	df << "}\n";
//	df.close();
//}
//template<class T> void DWAGraph< T >::dump_ShortestPath()
//{
//	if( this->Distance.empty() )
//		this->get_Bcet();
//
//	for ( auto itr = this->Distance.begin() ; itr != this->Distance.end() ; ++itr )
//	{
//		std::cout << this->Lookup.at( itr->first ) << "(" << itr->second << "), ";
//	}
//	std::cout << std::endl;
//}
//template<class T> void DWAGraph< T >::xdot_ShortestPath()
//{
//
//	this->get_Bcet();
//
//	std::ofstream df;
//	std::string file( "graph" );
//	file.append( itostr( WCET::get_gidx() ) );
//	file.append( ".dot" );
//	df.open( file.c_str() );
//	df << "digraph{\n";
//	if( XDOTRANK )
//		df << "\trankdir=LR;\n";
//
//	for ( auto itr = this->G.begin() ; itr != this->G.end() ; ++itr )
//	{
//		df << this->Lookup.at( itr->first ) << "[label=\"" << this->Lookup.at( itr->first ) << " (@" << Distance.at( itr->first ) << ")" << "\"];\n";
//	}
//	for ( auto itr = this->G.begin() ; itr != this->G.end() ; ++itr )
//	{
//		for ( auto litr = ( itr->second )->begin() ; litr != ( itr->second )->end() ; ++litr )
//		{
//			df << " " << this->Lookup.at( itr->first ) << "->" << this->Lookup.at( litr->get_vertex() ) << "[label=" << litr->get_weight() << "];\n";
//		}
//	}
//
////Shortest Path
//	TimeVextexMap InverseMap;
//	auto iitr = InverseMap.rbegin();
//	auto jitr = InverseMap.rbegin();
//	for ( auto itr = this->Distance.begin() ; itr != this->Distance.end() ; ++itr )
//	{
//		InverseMap.insert( std::make_pair( itr->second , itr->first ) );
//	}
//
//	int MIN = 0;
//
//	for ( auto MaxItr = InverseMap.rbegin() ; MaxItr != InverseMap.rend() ; ++MaxItr )
//	{
//		if( this->get_VertexDegree( MaxItr->second ) == 0 )
//		{
//			if( MIN < Distance.at( MaxItr->second ) )
//			{
//				MIN = Distance.at( MaxItr->second );
//				iitr = MaxItr;
//				jitr = MaxItr;
//			}
//		}
//	}
//
//	while ( jitr != InverseMap.rend() )
//	{
//		if( iitr->first == jitr->first + this->get_EdgeWeight( jitr->second , iitr->second ) )
//		{
//			df << this->Lookup.at( iitr->second ) << "[fillcolor=blue, style=filled];\n";
//			df << this->Lookup.at( jitr->second ) << "[fillcolor=blue, style=filled];\n";
//			iitr = jitr;
//		}
//		++jitr;
//	}
//
//	if( XDOTRANK )
//	{
//		df << "{rank=same; ";
//
//		int MIN = 0;
//		for ( auto MaxItr = InverseMap.rbegin() ; MaxItr != InverseMap.rend() ; ++MaxItr )
//		{
//			if( this->get_VertexDegree( MaxItr->second ) == 0 )
//			{
//				if( MIN < Distance.at( MaxItr->second ) )
//				{
//					MIN = Distance.at( MaxItr->second );
//					iitr = MaxItr;
//					jitr = MaxItr;
//				}
//			}
//		}
//		while ( jitr != InverseMap.rend() )
//		{
//			if( iitr->first == jitr->first + this->get_EdgeWeight( jitr->second , iitr->second ) )
//			{
//				df << this->Lookup.at( iitr->second ) << " " << this->Lookup.at( jitr->second ) << " ";
//				iitr = jitr;
//			}
//			++jitr;
//		}
//
//		df << "}\n";
//	}
//	df << "}\n";
//	df.close();
//}
