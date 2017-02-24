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

//#include "llvm/Support/raw_ostream.h"
#define s(x) cout<< #x <<" = "<< x <<std::endl
#define pb cout<<"------------------------------------------------"<<std::endl;
#define XDOTRANK 0 //True of False
#define GND (void *)0
#define PIPEDEPTH 5
namespace WCET
{
	template <class T> class FuncUnit
	{
	protected:
		//UNIQUE ID FunctionUnit
		static unsigned long int ID;
		typedef std::map< T , unsigned long int > FuncMap;
		FuncMap TLookup;
	public:
		FuncUnit();
		void Ireset();
		void Iinsert(T);
		void Idelete(T);
		void dump();
		unsigned long int get_FuncUnitNumber(T,unsigned);
	};

	template <class T> class AdjEdge
	{
	protected:
		int Weight;
		T Vertex;
	public:
		AdjEdge(T,int);
		int get_weight();
		T get_vertex();
		void set_weight(int);
	};
	template <class T> class DFSNode
	{
	protected:
		unsigned long int  Pre;
		unsigned long int  Post;
		T    Parent;
		bool Visited;

	public:
		DFSNode(T);
		unsigned long int get_Pre();
		unsigned long int get_Post();
		T get_Parent();
		void set_Pre(unsigned long int);
		void set_Post(unsigned long int);
		void set_Parent(T);
		bool IsVisited();
		void set_Visited();
	};


	template <class T> class DWGraph
	{
	protected:
		typedef std::list< AdjEdge<T> >* AdjListPtr;
		typedef std::list< AdjEdge<T> > AdjList;
		typedef std::map< T , AdjListPtr > DWG;
		DWG G;
		//TO Create LOOKUP Table
		std::map< T , unsigned long int > Lookup;
		//Cardinality |G| of Graph
		unsigned long int n;
		//UNIQUE ID OF EVERY DWGraph created
		static unsigned int gidx;
	public:
		 DWGraph();
		//To get Number of Vertices in Graph |G|
		 unsigned long int get_Cardinality();
		//To get Cardinality of Edges |E|
		 unsigned long int get_SizeOfGraph();
		//To add vertex to graph
		bool add_Vertex(T);
		//To add edge u to v of weight w
		bool add_Edge(T,T,int);
		//To remove edge u to v
		bool remove_Edge(T,T);
		//To get Weight of already added edge
		int get_EdgeWeight(T,T);
		//To Change Weight of already added edge
		bool set_EdgeWeight(T,T,int);
		//Vertex degree
		unsigned long int get_VertexDegree(T);
		//true if vertex is present
		bool Is_Vertex(T);
		//true if edge is present
		bool Is_Edge(T,T);
		//Adjacency Matrix form
		void dump();
		//get graph in xdot format in source folder
	    void xdot();
	};


	template <class T> class DWAGraph : public DWGraph<T>
	{
	protected:
		typedef std::list< T > TSList;
		typedef std::map<T,unsigned long int> CPMap;
		TSList TSL;
		CPMap CrticalPath;
	public :
		DWAGraph();
		 void TopologicalSorting();
		 void CriticalPath();
		 int get_Wcet();
		 int get_NodeWcet(T);
		 void dump_TSL();
		 void dump_CriticalPath();
		 void xdot_CriticalPath();
	};

	template <class T> class DWXGraph : public DWAGraph<T>
	{
	protected:
		unsigned int Count;
		typedef DFSNode<T> DNode;
		typedef DFSNode<T>* DNodePtr;
		typedef std::map< T , DNodePtr > GraphProfileMap;
		GraphProfileMap GPM;
		//std::make_pair<int,int> CHK;
	public :
		 DWXGraph();
		void DFS(T);
		void GraphProfile();
		void LoopScope();
		void dump_DFS();
		void xdot_DFS();
	};

}


#endif /* WCET_H_ */

