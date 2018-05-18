/*
 * wcet.h
 *
 *  Created on: 06-Jan-2017
 *      Author: tarun
 */

#ifndef WCET_H_
#define WCET_H_
#include<iostream>
#include <fstream>
#include<map>
#include<list>
#include<vector>
#include<queue>
#include<algorithm>
#include<climits>
#include<cstring>

#define DEBUG_TYPE "analyzer"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineMemOperand.h"
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
#include "llvm/IR/HyperOpInteractionGraph.h"
#include "REDEFINETargetMachine.h"
#include "llvm/IR/REDEFINEUtils.h"

#define s(x) errs()<< #x <<" = "<< x <<std::endl
#define pb errs()<<"------------------------------------------------"<<std::endl;
#define XDOTRANK 0 //True of False Formatting in XDOT Critical Path.
#define GND (void *)0//Void Pointer
#define PIPEDEPTH 5//Pipeline Depth 5 Stage
#define CFGSTART (pHyperOpBB *)0
#define CFGEND (pHyperOpBB *)1
#define BCETEND(x) (pHyperOpBB *) (x+2)
#define HyperGND (HyperOp *)0

#define MAXLOOPBOUND 1000
#define pHyperMessageWorstLatency 3
#define pHyperMessageBestLatency 1
#define pHyperOpNumber 4

namespace WCET
{
	unsigned int get_gidx();

	template<class T> class FuncUnit
	{
		protected:
			//UNIQUE ID FunctionUnit
			static unsigned long int ID;
			typedef std::map< T , unsigned long int > FuncMap;
			FuncMap TLookup;
		public:
			FuncUnit();
			void Ireset();
			void Iinsert( T );
			void Idelete( T );
			void dump();
			unsigned long int get_FuncUnitNumber( T , unsigned );
	};

	template<class T> class AdjEdge
	{
		protected:
			int Weight;
			T Vertex;
		public:
			AdjEdge( T , int );
			int get_weight();
			T get_vertex();
			void set_weight( int );
	};

	template<class T> class DWGraph
	{
		protected:
			typedef std::list< AdjEdge< T > >* AdjListPtr;
			typedef std::list< AdjEdge< T > > AdjList;
			typedef std::map< T , AdjListPtr > DWG;
			DWG G;
			//TO Create LOOKUP Table
			std::map< T , unsigned long int > Lookup;
			//Cardinality |G| of Graph
			unsigned long int n;
		public:
			DWGraph();
			//To get Number of Vertices in Graph |G|
			unsigned long int get_Cardinality();
			//To get Cardinality of Edges |E|
			unsigned long int get_SizeOfGraph();
			//To add vertex to graph
			bool add_Vertex( T );
			//To split first vertex into 2 Vertex given by first and second argument graph
			bool split_Vertex( T , T , int );
			//To add edge u to v of weight w
			bool add_Edge( T , T , int );
			//To remove edge u to v
			bool remove_Edge( T , T );
			//To get Weight of already added edge
			int get_EdgeWeight( T , T );
			//To Change Weight of already added edge
			bool set_EdgeWeight( T , T , int );
			//Vertex degree
			unsigned long int get_VertexDegree( T );
			//true if vertex is present
			bool Is_Vertex( T );
			//true if edge is present
			bool Is_Edge( T , T );
			//Adjacency Matrix form
			void dump();
			//get graph in xdot format in source folder
			void xdot();
			//get vertex list
			list< T > get_VertexList();
			//get adj. list
			list< T > get_AdjList( T );
	};

	template<class T> class DWAGraph : public DWGraph< T >
	{
		protected:
			typedef std::list< T > TSList;
			typedef std::map< T , unsigned long int > CPMap;
			typedef std::multimap< int , T > TimeVextexMap;
			TimeVextexMap CTVM;
			TSList TSL;
			std::map< T , int > Distance;
		public:
			CPMap CrticalPath;
			DWAGraph();
			void TopologicalSorting();
			void CriticalPath();
			int get_Wcet();
			int get_NodeWcet( T );
			int get_Bcet();
			int get_NodeBcet( T );
			void dump_TSL();
			void dump_CriticalPath();
			void xdot_CriticalPath();
			void dump_ShortestPath();
			void xdot_ShortestPath();
	};

	class CacheNode
	{
		protected:
			void * MemoryLocation;
			unsigned size;
		public:
			CacheNode( void * , unsigned int );
			void set_MemoryLocation( void * );
			void * get_MemoryLocation();
			void set_memsize( unsigned int );
			unsigned int get_memsize();
	};

	class CacheAnalysis
	{
		protected:
			int linesize;
			int cachesize;
			int associativity;
			int NumberofLines;
		public:
			std::list< CacheNode > LRU;
			CacheAnalysis();
			void set_line( int );
			int get_line( );
			void set_cachsize( int );
			void set_associativity( int );
			bool Update(void *,unsigned int);
			bool In_Cache( MachineInstr* );
			void dump();

	};

	typedef DWAGraph< unsigned long int > pHyperOpBB;
	typedef std::map< MachineInstr* , pHyperOpBB* > MachinepHyperOpBBLookup;
	typedef DWAGraph< pHyperOpBB* > pHyperOpCFG;
	typedef DWAGraph< pHyperOpBB* > PartialCFG;
	typedef DWAGraph< pHyperOpBB* > IntraHyperOp;
	typedef std::map< pHyperOpBB* , std::pair< int , int > > pHyperOpBBExtraTime;
	typedef DWGraph< MachineLoop* > LoopScope;
	typedef std::map< MachineLoop * , std::pair< unsigned int , unsigned int > > LoopBound;
	typedef WCET::DWAGraph< HyperOp * > HyperOpGraph;
	typedef std::map< MachineInstr * , std::pair<void *,unsigned int> > MemoryAliasMap;
	typedef std::map< int,pHyperOpBB* > pBBLookUp;
	typedef std::map<int,CacheAnalysis *> CacheLookUp;


	void WCETMemoryAliasing( MachineFunction *MF );
	CacheAnalysis * Union(CacheAnalysis *,CacheAnalysis *);
	CacheAnalysis * Intersection(CacheAnalysis *,CacheAnalysis *);

	class pHyperOpBasicBlock
	{
		public:
			pHyperOpBB pHyperBB[pHyperOpNumber];
			pHyperOpBasicBlock( llvm::MachineBasicBlock * ,CacheAnalysis *CA);
	};

	class pHyperOpControlFlowGraph
	{
		public:
			llvm::MachineDominatorTree *DT;

			std::vector<llvm::MachineBasicBlock *> LeafMBB;
			std::map<llvm::MachineBasicBlock *,bool> Ismayvisted;
			std::map<llvm::MachineBasicBlock *,bool> Ismustvisted;

			CacheLookUp MayCA;
			CacheLookUp MustCA;

			pBBLookUp MayLookUp[pHyperOpNumber];
			pBBLookUp MustLookUp[pHyperOpNumber];

			pHyperOpCFG pHyperMayCFG[pHyperOpNumber];
			pHyperOpCFG pHyperMustCFG[pHyperOpNumber];

			pHyperOpControlFlowGraph( llvm::MachineFunction &MF , llvm::MachineDominatorTree *DT );
			void pHyperOpMayDFS(llvm::MachineBasicBlock *);
			void pHyperOpMustDFS(llvm::MachineBasicBlock *);

	};

	class SingleHyperOpWcet
	{
		private:
			llvm::MachineFunction* MF;
			pHyperOpCFG pHopMustCFG[pHyperOpNumber];
			pHyperOpCFG pHopMayCFG[pHyperOpNumber];
			MachinepHyperOpBBLookup MPILookup;
			pBBLookUp MustLookUp[pHyperOpNumber];
			pBBLookUp MayLookUp[pHyperOpNumber];
			LoopScope LS;
			LoopBound pLoopBound;
			pHyperOpBBExtraTime pBBMayExtraTime[pHyperOpNumber];
			pHyperOpBBExtraTime pBBMustExtraTime[pHyperOpNumber];
			IntraHyperOp IntraHopWCET;
			IntraHyperOp IntraHopBCET;

		public:
			SingleHyperOpWcet( llvm::MachineFunction* MF , MachineLoopInfo *LI , MachineDominatorTree *DT );
			void LoopAnalysis( MachineLoopInfo *LI );
			void LsDFS( MachineLoop * );
			void LoopDFS( MachineLoop * );

			void set_LoopBound( MachineLoop * , unsigned int , unsigned int );
			void set_LoopUpperBound( MachineLoop * , unsigned int );
			void set_LoopLowerBound( MachineLoop * , unsigned int );
			unsigned int get_LoopUpperBound( MachineLoop * L );
			unsigned int get_LoopLowerBound( MachineLoop * L );
			int get_Wcet();
			int get_WcetNode( MachineInstr* );
			int get_Bcet();
			int get_BcetNode( MachineInstr* );
			void dumpLoop();
	};

}

#endif /* WCET_H_ */
