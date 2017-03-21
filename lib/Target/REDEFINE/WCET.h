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
#include "llvm/Analysis/LoopInfo.h"
#include "HyperOpInteractionGraph.h"
#include "REDEFINETargetMachine.h"

#define s(x) errs()<< #x <<" = "<< x <<std::endl
#define pb errs()<<"------------------------------------------------"<<std::endl;
#define XDOTRANK 0 //True of False Formatting in XDOT Critical Path.
#define GND (void *)0//Void Pointer
#define PIPEDEPTH 5//Pipeline Depth 5 Stage
#define LoopScopeRoot (MachineLoop*) 0
#define CFGSTART (pHyperOpBB *)0
#define CFGEND (pHyperOpBB *)1
#define MAXLOOPBOUND 100
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

	template<class T> class DFSNode
	{
		protected:
			unsigned long int Pre;
			unsigned long int Post;
			T Parent;
			bool Visited;

		public:
			DFSNode( T );
			unsigned long int get_Pre();
			unsigned long int get_Post();
			T get_Parent();
			void set_Pre( unsigned long int );
			void set_Post( unsigned long int );
			void set_Parent( T );
			bool IsVisited();
			void set_Visited();
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
			bool split_Vertex( T , T );
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
			CPMap CrticalPath;

		public:
			DWAGraph();
			void TopologicalSorting();
			void CriticalPath();
			int get_Wcet();
			int get_NodeWcet( T );
			void dump_TSL();
			void dump_CriticalPath();
			void xdot_CriticalPath();
	};

	typedef DWAGraph< unsigned long int > pHyperOpBB;
	typedef std::map< MachineInstr* , pHyperOpBB* > MachinepHyperOpBBLookup;
	typedef DWGraph< pHyperOpBB* > pHyperOpCFG;
	typedef DWAGraph< pHyperOpBB* > IntraHyperOp;
	typedef DWGraph< MachineLoop* > LoopScope;
	typedef std::map< MachineLoop * , unsigned int > LoopBound;
	class pHyperOpBasicBlock
	{
		protected:
			pHyperOpBB pHyperBB[4];
		public:
			pHyperOpBasicBlock( llvm::MachineBasicBlock * );
			pHyperOpBB get_pHyperOpBB( int );
	};

	class pHyperOpControlFlowGraph
	{
		public:
			std::vector< pHyperOpBB* > LookUp[4];
			pHyperOpCFG pHyperCFG[4];
			pHyperOpControlFlowGraph( llvm::MachineFunction &MF , llvm::MachineDominatorTree *DT );
	};

	class LoopAnalysis
	{
		public:
			LoopScope LS;
			LoopAnalysis( MachineLoopInfo *LI );
			void set_LoopBounds( MachineLoop * , unsigned int );
			unsigned int get_LoopBounds( MachineLoop * L );
			void LoopDFS( MachineLoop * );
			int Loop_Wcet( MachineLoop * , int );
			void dumpDFS( MachineLoop * );
			void dump();
	};

	class LoopSingleIterationWcet : public DWAGraph< MachineLoop* >
	{
		protected:
			typedef std::queue< llvm::MachineBasicBlock * > BBQ;
			BBQ q;
			BBQ qt;

			int LoopSIWcet[4];
			MachineLoop* L;
		public:
			LoopSingleIterationWcet( MachineLoop* );
			int get_LoopSingleIterationWcet( int );
	};

	class SingleHyperOpWcet
	{
		private:
			llvm::MachineFunction* MF;
			pHyperOpCFG pHopCFG[4];
			MachinepHyperOpBBLookup MPILookup;
			pHyperOpCFG pHopMPI;
			std::vector< pHyperOpBB* > LookUp[4];
			LoopScope LScope[4];
			IntraHyperOp IntraHop;
		public:
			SingleHyperOpWcet( llvm::MachineFunction* MF , MachineLoopInfo *LI , MachineDominatorTree *DT );
			int get_Wcet();
			int get_WcetNode( int Node );
	};

}

#endif /* WCET_H_ */
