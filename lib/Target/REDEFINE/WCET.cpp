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
unsigned int WCET::get_gidx() {
	unsigned int id = gidx;
	gidx = gidx + 1;
	return id;
}
//LoopAnalysis
LoopAnalysis::LoopAnalysis(MachineLoopInfo *LI) {
	this->add_Vertex(LoopScopeRoot);

	for (MachineLoopInfo::iterator LIT = LI->begin(); LIT != LI->end(); ++LIT)
	{
		MachineLoop* l = *LIT;
		this->add_Edge(LoopScopeRoot, l, l->getLoopDepth());
		LoopDFS(l);
	}
}
void LoopAnalysis::LoopDFS(MachineLoop *L) {
	this->add_Vertex(L);
	errs() << "getLD entry " << L->getLoopDepth() << "\n";
	for (auto BBitr = L->block_begin(); BBitr != L->block_end(); ++BBitr)
	{
		errs() << (*BBitr)->getNumber() << ", ";
	}
	errs() << "\n";
	std::vector<MachineLoop*> subLoops = L->getSubLoops();
	for (MachineLoop::iterator subitr = subLoops.begin(); subitr != subLoops.end(); ++subitr)
	{
		this->add_Edge(L, *subitr, L->getLoopDepth());
		LoopDFS(*subitr);
	}

}
void LoopAnalysis::dump() {
	list<MachineLoop *> l=this->get_AdjList(LoopScopeRoot);
	for (auto itr = l.begin(); itr != l.end(); ++itr)
	{
		(*itr)->dump();
	}
}
//CLASS Functional Unit
template<class T> unsigned long int FuncUnit<T>::ID;
template<class T> FuncUnit<T>::FuncUnit() {
	this->TLookup.clear();
}
template<class T> void FuncUnit<T>::Ireset() {
	this->TLookup.clear();
	this->ID = 0;
}
template<class T> void FuncUnit<T>::Iinsert(T _u) {
	if (this->TLookup.insert(std::make_pair(_u, this->ID)).second)
		this->ID = this->ID + 1;
}
template<class T> void FuncUnit<T>::Idelete(T _u) {
	typename FuncMap::iterator itr = this->TLookup.find(_u);
	if (itr != this->TLookup.end())
		this->TLookup.erase(itr);

}
template<class T> void FuncUnit<T>::dump() {
	typename FuncMap::iterator itr;
	for (itr = this->TLookup.begin(); itr != this->TLookup.end(); ++itr)
		std::cout << "Key: " << itr->first << ", Value: " << itr->second << "\n";
}
template<class T> unsigned long int FuncUnit<T>::get_FuncUnitNumber(T _u, unsigned _stage) {
//_stage should be <= PIPEDEPTH
	unsigned long int i;
	typename FuncMap::iterator itr = this->TLookup.find(_u);
	if (itr != this->TLookup.end())
	{
		i = this->TLookup.at(_u) * PIPEDEPTH + _stage;
	} else
	{
		i = -1;
	}

//*
	return i;
}

//DONE CLASS AdjEdge
template<class T> AdjEdge<T>::AdjEdge(T _v, int _w) {
	Vertex = _v;
	Weight = _w;
}
template<class T> int AdjEdge<T>::get_weight() {
	return this->Weight;
}
template<class T> T AdjEdge<T>::get_vertex() {
	return this->Vertex;
}
template<class T> void AdjEdge<T>::set_weight(int _w) {
	this->Weight = _w;
}
//CLASS DFSNode
template<class T> DFSNode<T>::DFSNode(T _u) {
	this->Parent = _u;
	this->Post = -1;
	this->Pre = -1;
	this->Visited = 0;
}
template<class T> unsigned long int DFSNode<T>::get_Pre() {
	return this->Pre;
}
template<class T> unsigned long int DFSNode<T>::get_Post() {
	return this->Post;
}
template<class T> T DFSNode<T>::get_Parent() {
	return this->Parent;
}
template<class T> void DFSNode<T>::set_Pre(unsigned long int _Pre) {
	this->Pre = _Pre;
}
template<class T> void DFSNode<T>::set_Post(unsigned long int _Post) {
	this->Post = _Post;
}
template<class T> void DFSNode<T>::set_Parent(T _Parent) {
	this->Parent = _Parent;
}
template<class T> void DFSNode<T>::set_Visited() {
	this->Visited = true;
}
template<class T> bool DFSNode<T>::IsVisited() {
	return this->Visited;
}
//DONE CLASS DWGraph
template<class T> DWGraph<T>::DWGraph() {
	this->G.clear();
	this->n = 0;
}
template<class T> unsigned long int DWGraph<T>::get_SizeOfGraph() {
	unsigned long int size = 0;
	for (typename DWG::iterator itr = this->G.begin(); itr != this->G.end(); ++itr)
	{
		size += get_VertexDegree(itr->first);
	}
	return size;

}
template<class T> bool DWGraph<T>::add_Vertex(T _u) {

	if (this->G.insert(make_pair(_u, new AdjList)).second)
	{
		this->Lookup.insert(std::make_pair(_u, n));
		this->n++;
		return true;
	} else
	{
		return false;
	}
}
template<class T> bool DWGraph<T>::Is_Vertex(T _u) {
	typename DWG::iterator itr = this->G.find(_u);
	return (itr != this->G.end());

}
template<class T> bool DWGraph<T>::Is_Edge(T _u, T _v) {
	bool ue, ve;
	typename DWG::iterator itr = this->G.find(_u);
	ue = (itr == this->G.end());
	ve = (this->G.find(_v) == this->G.end());

	if (ue || ve)
		return false;

	for (typename AdjList::iterator litr = ((*itr).second)->begin(); litr != ((*itr).second)->end(); ++litr)
	{
		if (litr->get_vertex() == _v)
			return true;
	}
	return false;
}
template<class T> bool DWGraph<T>::set_EdgeWeight(T _u, T _v, int _w) {
	bool ue, ve;
	typename DWG::iterator itr = this->G.find(_u);
	ue = (itr != this->G.end());
	ve = (this->G.find(_v) != this->G.end());
	if (ue && ve)
	{

		for (typename AdjList::iterator litr = ((*itr).second)->begin(); litr != ((*itr).second)->end(); ++litr)
		{
			if (litr->get_vertex() == _v)
				litr->set_weight(_w);
		}
		return true;
	} else
	{
		return false;
	}

}
template<class T> bool DWGraph<T>::add_Edge(T _u, T _v, int _w) {
	AdjEdge<T> *E = new AdjEdge<T>(_v, _w);
	bool ue, ve;
	ue = (this->G.find(_u) != this->G.end());
	ve = (this->G.find(_v) != this->G.end());
	if (ue && ve)
		this->G.at(_u)->push_back(*E);
	else
	{
		add_Vertex(_u);
		add_Vertex(_v);
		this->G.at(_u)->push_back(*E);
	}
	return true;
}
template<class T> bool DWGraph<T>::remove_Edge(T _u, T _v) {
	bool ue, ve;
	ue = (this->G.find(_u) != this->G.end());
	ve = (this->G.find(_v) != this->G.end());
	if (ue && ve)
	{
		typename DWG::iterator itr = this->G.find(_u);
		for (typename AdjList::iterator litr = (itr->second)->begin(); litr != (itr->second)->end(); ++litr)
		{
			if ((this->Lookup.at(litr->get_vertex())) == (this->Lookup.at(_v)))
			{
				(this->G.at(_u))->erase(litr--);
			}
		}
		return true;
	}

	return false;

}
template<class T> void DWGraph<T>::dump() {
	for (typename DWG::iterator itr = this->G.begin(); itr != this->G.end(); ++itr)
	{
		std::cout << this->Lookup.at((*itr).first) << ": ";
		for (typename AdjList::iterator litr = (itr->second)->begin(); litr != (itr->second)->end(); ++litr)
		{
			std::cout << " " << this->Lookup.at(litr->get_vertex()) << "(" << litr->get_weight() << "),";
		}
		std::cout << std::endl;
	}
}
template<class T> void DWGraph<T>::xdot() {
	std::ofstream df;
	std::string file("graph");
	file.append(std::to_string(WCET::get_gidx()));
	file.append(".dot");
	df.open(file);
	df << "digraph{\n";
	for (typename DWG::iterator itr = this->G.begin(); itr != this->G.end(); ++itr)
	{
		df << this->Lookup.at((*itr).first) << "[label=\"" << this->Lookup.at((*itr).first) << "\"];\n";
	}
	for (typename DWG::iterator itr = this->G.begin(); itr != this->G.end(); ++itr)
	{
		for (typename AdjList::iterator litr = ((*itr).second)->begin(); litr != ((*itr).second)->end(); ++litr)
		{
			df << " " << this->Lookup.at((*itr).first) << "->" << this->Lookup.at(litr->get_vertex()) << "[label=" << litr->get_weight() << "];\n";
		}
	}
	df << "}\n";
	df.close();
}
template<class T> unsigned long int DWGraph<T>::get_Cardinality() {
	return this->n;
}
template<class T> unsigned long int DWGraph<T>::get_VertexDegree(T _u) {
	return this->G.at(_u)->size();
}
template<class T> int DWGraph<T>::get_EdgeWeight(T _u, T _v) {
	bool ue, ve;
	typename DWG::iterator itr = this->G.find(_u);
	ue = (itr == this->G.end());
	ve = (this->G.find(_v) == this->G.end());

	if (ue || ve)
		return INT_MIN;

	for (typename AdjList::iterator litr = ((*itr).second)->begin(); litr != ((*itr).second)->end(); ++litr)
	{
		if (litr->get_vertex() == _v)
			return litr->get_weight();
	}

	return INT_MIN;
}
template<class T> list<T> DWGraph<T>::get_VertexList() {
	list<T> l;
	l.clear();
	for (typename DWG::iterator itr = this->G.begin(); itr != this->G.end(); ++itr)
	{
		l.push_back(itr->first);
	}
	return l;
}
template<class T> list<T> DWGraph<T>::get_AdjList(T _u) {
	list<T> l;
	l.clear();
	typename DWG::iterator itr = this->G.find(_u);
	if (itr != this->G.end())
	{
		for (typename AdjList::iterator litr = ((*itr).second)->begin(); litr != ((*itr).second)->end(); ++litr)
		{
			l.push_back(litr->get_vertex());
		}
	}
	return l;
}
//CLASS DWAGraph
template<class T> DWAGraph<T>::DWAGraph() {
	this->G.clear();
	this->n = 0;
	this->TSL.clear();
}
template<class T> void DWAGraph<T>::TopologicalSorting() {
	std::queue<T> Q;
	std::map<T, int> indegree;
	T _u;
	typename DWGraph<T>::DWG::iterator _uitr;
	typename DWGraph<T>::DWG tsg(this->G);

	for (typename DWGraph<T>::DWG::iterator itr = tsg.begin(); itr != tsg.end(); ++itr)
	{
		indegree.insert(std::make_pair((itr->first), 0));
	}

	for (typename DWGraph<T>::DWG::iterator itr = tsg.begin(); itr != tsg.end(); ++itr)
	{
		for (typename DWGraph<T>::AdjList::iterator litr = (itr->second)->begin(); litr != (itr->second)->end(); ++litr)
		{
			indegree.at(litr->get_vertex()) += 1;
		}
	}

	for (typename DWGraph<T>::DWG::iterator itr = tsg.begin(); itr != tsg.end(); ++itr)
	{
		if (indegree.at(itr->first) == 0)
		{
			Q.push(itr->first);
			indegree.at(itr->first) -= 1;
		}
	}
	while (!Q.empty())
	{
		_u = Q.front();
		Q.pop();
		this->TSL.push_back(_u);
		_uitr = tsg.find(_u);

		for (typename DWGraph<T>::AdjList::iterator litr = _uitr->second->begin(); litr != _uitr->second->end(); ++litr)
		{
			indegree.at(litr->get_vertex()) -= 1;
			if (indegree.at(litr->get_vertex()) == 0)
			{
				Q.push(litr->get_vertex());
			}
		}
		tsg.erase(_u);
	}

}
template<class T> void DWAGraph<T>::dump_TSL() {
	if (this->TSL.empty())
		this->TopologicalSorting();

	for (typename std::list<T>::iterator itr = this->TSL.begin(); itr != this->TSL.end(); ++itr)
	{
		std::cout << this->Lookup.at(*itr) << ",";
	}
	std::cout << std::endl;

}
template<class T> void DWAGraph<T>::CriticalPath() {
	std::queue<T> Q;
//Vertex , Degree , Weight
	std::map<T, std::pair<int, int> > indegree;
	T _u;
	typename DWGraph<T>::DWG::iterator _uitr;
	typename DWGraph<T>::DWG tsg(this->G);
	std::multimap<int, T> CTVM;
	for (typename DWGraph<T>::DWG::iterator itr = tsg.begin(); itr != tsg.end(); ++itr)
	{
		indegree.insert(std::make_pair((itr->first), std::make_pair(0, 0)));
	}

	for (typename DWGraph<T>::DWG::iterator itr = tsg.begin(); itr != tsg.end(); ++itr)
	{
		for (typename DWGraph<T>::AdjList::iterator litr = (itr->second)->begin(); litr != (itr->second)->end(); ++litr)
		{
			indegree.at(litr->get_vertex()).first += 1;
		}
	}

	for (typename DWGraph<T>::DWG::iterator itr = tsg.begin(); itr != tsg.end(); ++itr)
	{
		if (indegree.at(itr->first).first == 0)
		{
			Q.push(itr->first);
			indegree.at(itr->first).first -= 1;
		}
	}

	while (!Q.empty())
	{
		_u = Q.front();
		Q.pop();
		CTVM.insert(std::make_pair(indegree.at(_u).second, _u));
		_uitr = tsg.find(_u);

		for (typename DWGraph<T>::AdjList::iterator litr = _uitr->second->begin(); litr != _uitr->second->end(); ++litr)
		{
			indegree.at(litr->get_vertex()).first -= 1;
			indegree.at(litr->get_vertex()).second = std::max(indegree.at(litr->get_vertex()).second, indegree.at(_u).second + litr->get_weight());
			if (indegree.at(litr->get_vertex()).first == 0)
			{
				Q.push(litr->get_vertex());
			}
		}
		tsg.erase(_u);
	}

//Critical Path

	typename std::multimap<int, T>::reverse_iterator iitr = CTVM.rbegin();
	typename std::multimap<int, T>::reverse_iterator jitr = CTVM.rbegin();
	this->CrticalPath.insert(std::make_pair((iitr->second), (iitr->first)));
	while (jitr != CTVM.rend())
	{
		if (iitr->first == jitr->first + this->get_EdgeWeight(jitr->second, iitr->second))
		{
			this->CrticalPath.insert(std::make_pair((jitr->second), (jitr->first)));
			iitr = jitr;
		}
		++jitr;
	}

}
template<class T> int DWAGraph<T>::get_Wcet() {
	if (this->G.empty())
		return 0;

	std::queue<T> Q;
//Vertex , Degree , Weight
	std::map<T, std::pair<int, int> > indegree;
	T _u;
	typename DWGraph<T>::DWG::iterator _uitr;
	typename DWGraph<T>::DWG tsg(this->G);
	this->CTVM.clear();
	for (typename DWGraph<T>::DWG::iterator itr = tsg.begin(); itr != tsg.end(); ++itr)
	{
		indegree.insert(std::make_pair((itr->first), std::make_pair(0, 0)));
	}

	for (typename DWGraph<T>::DWG::iterator itr = tsg.begin(); itr != tsg.end(); ++itr)
	{
		for (typename DWGraph<T>::AdjList::iterator litr = (itr->second)->begin(); litr != (itr->second)->end(); ++litr)
		{
			indegree.at(litr->get_vertex()).first += 1;
		}
	}

	for (typename DWGraph<T>::DWG::iterator itr = tsg.begin(); itr != tsg.end(); ++itr)
	{
		if (indegree.at(itr->first).first == 0)
		{
			Q.push(itr->first);
			indegree.at(itr->first).first -= 1;
		}
	}

	while (!Q.empty())
	{
		_u = Q.front();
		Q.pop();
		this->CTVM.insert(std::make_pair(indegree.at(_u).second, _u));
		_uitr = tsg.find(_u);

		for (typename DWGraph<T>::AdjList::iterator litr = _uitr->second->begin(); litr != _uitr->second->end(); ++litr)
		{
			indegree.at(litr->get_vertex()).first -= 1;
			indegree.at(litr->get_vertex()).second = std::max(indegree.at(litr->get_vertex()).second, indegree.at(_u).second + litr->get_weight());
			if (indegree.at(litr->get_vertex()).first == 0)
			{
				Q.push(litr->get_vertex());
			}
		}
		tsg.erase(_u);
	}

	return this->CTVM.rbegin()->first;

}
template<class T> int DWAGraph<T>::get_NodeWcet(T Node) {
	if (this->G.empty())
		return 0;

	if (this->CTVM.empty())
	{
		std::queue<T> Q;
		//Vertex , Degree , Weight
		std::map<T, std::pair<int, int> > indegree;
		T _u;
		typename DWGraph<T>::DWG::iterator _uitr;
		typename DWGraph<T>::DWG tsg(this->G);
		for (typename DWGraph<T>::DWG::iterator itr = tsg.begin(); itr != tsg.end(); ++itr)
		{
			indegree.insert(std::make_pair((itr->first), std::make_pair(0, 0)));
		}

		for (typename DWGraph<T>::DWG::iterator itr = tsg.begin(); itr != tsg.end(); ++itr)
		{
			for (typename DWGraph<T>::AdjList::iterator litr = (itr->second)->begin(); litr != (itr->second)->end(); ++litr)
			{
				indegree.at(litr->get_vertex()).first += 1;
			}
		}

		for (typename DWGraph<T>::DWG::iterator itr = tsg.begin(); itr != tsg.end(); ++itr)
		{
			if (indegree.at(itr->first).first == 0)
			{
				Q.push(itr->first);
				indegree.at(itr->first).first -= 1;
			}
		}

		while (!Q.empty())
		{
			_u = Q.front();
			Q.pop();
			CTVM.insert(std::make_pair(indegree.at(_u).second, _u));
			_uitr = tsg.find(_u);

			for (typename DWGraph<T>::AdjList::iterator litr = _uitr->second->begin(); litr != _uitr->second->end(); ++litr)
			{
				indegree.at(litr->get_vertex()).first -= 1;
				indegree.at(litr->get_vertex()).second = std::max(indegree.at(litr->get_vertex()).second, indegree.at(_u).second + litr->get_weight());
				if (indegree.at(litr->get_vertex()).first == 0)
				{
					Q.push(litr->get_vertex());
				}
			}
			tsg.erase(_u);
		}
	}
	for (typename std::multimap<int, T>::iterator mitr = this->CTVM.begin(); mitr != this->CTVM.end(); ++mitr)
	{
		if ((*mitr).second == Node)
			return (*mitr).first;
	}
	return 0;

}
template<class T> void DWAGraph<T>::dump_CriticalPath() {
	if (this->CrticalPath.empty())
		this->CriticalPath();

	for (typename std::map<T, int>::iterator itr = this->CrticalPath.begin(); itr != this->CrticalPath.end(); ++itr)
	{
		std::cout << this->Lookup.at(itr->first) << "(" << itr->second << "), ";
	}
	std::cout << std::endl;
}
template<class T> void DWAGraph<T>::xdot_CriticalPath() {
	std::queue<T> Q;
//Vertex , Degree , Weight
	std::map<T, std::pair<int, int> > indegree;
	T _u;
	typename DWGraph<T>::DWG::iterator _uitr;
	typename DWGraph<T>::DWG tsg(this->G);
	std::map<T, int> VCTM;
	std::multimap<int, T> CTVM;
	for (typename DWGraph<T>::DWG::iterator itr = tsg.begin(); itr != tsg.end(); ++itr)
	{
		indegree.insert(std::make_pair((itr->first), std::make_pair(0, 0)));
	}

	for (typename DWGraph<T>::DWG::iterator itr = tsg.begin(); itr != tsg.end(); ++itr)
	{
		for (typename DWGraph<T>::AdjList::iterator litr = (itr->second)->begin(); litr != (itr->second)->end(); ++litr)
		{
			indegree.at(litr->get_vertex()).first += 1;
		}
	}

	for (typename DWGraph<T>::DWG::iterator itr = tsg.begin(); itr != tsg.end(); ++itr)
	{
		if (indegree.at(itr->first).first == 0)
		{
			Q.push(itr->first);
			indegree.at(itr->first).first -= 1;
		}
	}

	while (!Q.empty())
	{
		_u = Q.front();
		Q.pop();
		VCTM.insert(std::make_pair(_u, indegree.at(_u).second));
		CTVM.insert(std::make_pair(indegree.at(_u).second, _u));
		_uitr = tsg.find(_u);

		for (typename DWGraph<T>::AdjList::iterator litr = _uitr->second->begin(); litr != _uitr->second->end(); ++litr)
		{
			indegree.at(litr->get_vertex()).first -= 1;
			indegree.at(litr->get_vertex()).second = std::max(indegree.at(litr->get_vertex()).second, indegree.at(_u).second + litr->get_weight());
			if (indegree.at(litr->get_vertex()).first == 0)
			{
				Q.push(litr->get_vertex());
			}
		}
		tsg.erase(_u);
	}

	std::ofstream df;
	std::string file("graph");
	file.append(std::to_string(WCET::get_gidx()));
	file.append(".dot");
	df.open(file);
	df << "digraph{\n";
	if (XDOTRANK)
		df << "\trankdir=LR;\n";

	for (typename DWGraph<T>::DWG::iterator itr = this->G.begin(); itr != this->G.end(); ++itr)
	{
		df << this->Lookup.at((*itr).first) << "[label=\"" << this->Lookup.at((*itr).first) << " (@" << VCTM.at(itr->first) << ")" << "\"];\n";
	}
	for (typename DWGraph<T>::DWG::iterator itr = this->G.begin(); itr != this->G.end(); ++itr)
	{
		for (typename DWGraph<T>::AdjList::iterator litr = ((*itr).second)->begin(); litr != ((*itr).second)->end(); ++litr)
		{
			df << " " << this->Lookup.at((*itr).first) << "->" << this->Lookup.at(litr->get_vertex()) << "[label=" << litr->get_weight() << "];\n";
		}
	}

//Critical Path

	typename std::multimap<int, T>::reverse_iterator iitr = CTVM.rbegin();
	typename std::multimap<int, T>::reverse_iterator jitr = CTVM.rbegin();
	while (jitr != CTVM.rend())
	{
		if (iitr->first == jitr->first + this->get_EdgeWeight(jitr->second, iitr->second))
		{
			df << this->Lookup.at(iitr->second) << "[fillcolor=red, style=filled];\n";
			df << this->Lookup.at(jitr->second) << "[fillcolor=red, style=filled];\n";
			iitr = jitr;
		}
		++jitr;
	}
	if (XDOTRANK)
	{
		df << "{rank=same; ";
		iitr = CTVM.rbegin();
		jitr = CTVM.rbegin();
		while (jitr != CTVM.rend())
		{
			if (iitr->first == jitr->first + this->get_EdgeWeight(jitr->second, iitr->second))
			{
				df << this->Lookup.at(iitr->second) << " " << this->Lookup.at(jitr->second) << " ";
				iitr = jitr;
			}
			++jitr;
		}

		df << "}\n";
	}
	df << "}\n";
	df.close();
}

//TODO CLASS DWXGraph
template<class T> DWXGraph<T>::DWXGraph() {
	this->Count = 0;
	this->G.clear();
	this->GPM.clear();
	this->n = 0;
}
template<class T> void DWXGraph<T>::GraphProfile() {
	this->GPM.clear();
	for (typename DWGraph<T>::DWG::iterator itr = this->G.begin(); itr != this->G.end(); ++itr)
	{
		this->GPM.insert(std::make_pair(itr->first, new DNode(itr->first)));
	}

	std::map<T, int> indegree;

	for (typename DWGraph<T>::DWG::iterator itr = this->G.begin(); itr != this->G.end(); ++itr)
	{
		indegree.insert(std::make_pair((itr->first), 0));
	}

	for (typename DWGraph<T>::DWG::iterator itr = this->G.begin(); itr != this->G.end(); ++itr)
	{
		for (typename DWGraph<T>::AdjList::iterator litr = (itr->second)->begin(); litr != (itr->second)->end(); ++litr)
		{
			indegree.at(litr->get_vertex()) += 1;
		}
	}

	for (typename DWGraph<T>::DWG::iterator itr = this->G.begin(); itr != this->G.end(); ++itr)
	{
		if (indegree.at(itr->first) == 0)
		{
			std::cout << "ROOT: " << this->Lookup.at(itr->first) << std::endl;
			this->DFS(itr->first);
			return;
		}
	}

}
template<class T> void DWXGraph<T>::DFS(T _v) {
	this->GPM.at(_v)->set_Visited();
	this->GPM.at(_v)->set_Pre(this->Count);
	this->Count++;
	for (typename DWGraph<T>::AdjList::iterator litr = (this->G.at(_v))->begin(); litr != (this->G.at(_v))->end(); ++litr)
	{
		if (!this->GPM.at(litr->get_vertex())->IsVisited())
		{
			this->GPM.at(litr->get_vertex())->set_Parent(_v);
			DFS(litr->get_vertex());
			this->GPM.at(_v)->set_Post(this->Count);
			this->Count++;
		}
	}
	this->GPM.at(_v)->set_Post(this->Count);
}
template<class T> void DWXGraph<T>::dump_DFS() {
	for (typename DWXGraph<T>::GraphProfileMap::iterator itr = this->GPM.begin(); itr != this->GPM.end(); ++itr)
	{
		std::cout << this->Lookup.at(itr->first);
		if (this->Lookup.find(this->GPM.at(itr->first)->get_Parent()) != this->Lookup.end())
			std::cout << " : " << this->Lookup.at(itr->second->get_Parent());
		else
			std::cout << " : " << "GND";
		std::cout << " (" << itr->second->get_Pre() << " , ";
		std::cout << itr->second->get_Post() << ")" << std::endl;
	}
}
template<class T> void DWXGraph<T>::xdot_DFS() {

	std::ofstream df;
	std::string file("graph");
	file.append(std::to_string(WCET::get_gidx()));
	file.append(".dot");
	df.open(file);
	df << "digraph{\n";
	for (typename DWGraph<T>::DWG::iterator itr = this->G.begin(); itr != this->G.end(); ++itr)
	{
		if (this->Lookup.find(this->GPM.at(itr->first)->get_Parent()) != this->Lookup.end())
			df << this->Lookup.at((*itr).first) << "[label=\"" << this->Lookup.at((*itr).first) << "<=" << this->Lookup.at(this->GPM.at(itr->first)->get_Parent()) << " (" << this->GPM.at(itr->first)->get_Pre() << " , " << this->GPM.at(itr->first)->get_Post() << ")" << "\"];\n";
		else
			df << this->Lookup.at((*itr).first) << "[label=\"" << this->Lookup.at((*itr).first) << "<=" << "GND" << " (" << this->GPM.at(itr->first)->get_Pre() << " , " << this->GPM.at(itr->first)->get_Post() << ")" << "\"];\n";
	}

	for (typename DWGraph<T>::DWG::iterator itr = this->G.begin(); itr != this->G.end(); ++itr)
	{
		for (typename DWGraph<T>::AdjList::iterator litr = ((*itr).second)->begin(); litr != ((*itr).second)->end(); ++litr)
		{
			if (((this->GPM.at(itr->first)->get_Pre()) < (this->GPM.at(litr->get_vertex())->get_Pre())) && ((this->GPM.at(itr->first)->get_Post()) > (this->GPM.at(litr->get_vertex())->get_Post())))
			{
				if ((this->GPM.at(itr->first)->get_Pre()) == (this->GPM.at(litr->get_vertex())->get_Pre() - 1))
				{
					//TREE EDGES
					df << " " << this->Lookup.at((*itr).first) << "->" << this->Lookup.at(litr->get_vertex()) << "[label=" << litr->get_weight() << "];\n";
				} else
				{
					//FORWARD EDGES
					df << " " << this->Lookup.at((*itr).first) << "->" << this->Lookup.at(litr->get_vertex()) << "[label=" << litr->get_weight() << " color=darkorange" << "];\n";
				}
			} else
			{
				if (((this->GPM.at(itr->first)->get_Pre()) > (this->GPM.at(litr->get_vertex())->get_Pre())) && ((this->GPM.at(itr->first)->get_Post()) < (this->GPM.at(litr->get_vertex())->get_Post())))
				{
					//BACK EDGES
					df << " " << this->Lookup.at((*itr).first) << "->" << this->Lookup.at(litr->get_vertex()) << "[label=" << litr->get_weight() << " color=red" << "];\n";
				} else
				{
					//CROSS EDGES
					df << " " << this->Lookup.at((*itr).first) << "->" << this->Lookup.at(litr->get_vertex()) << "[label=" << litr->get_weight() << " color=darkorchid3 style=dotted" << "];\n";
				}
			}
		}
	}
	df << "}\n";
	df.close();
}
template<class T> void DWXGraph<T>::LoopScope() {

	for (typename DWGraph<T>::DWG::iterator itr = this->G.begin(); itr != this->G.end(); ++itr)
	{
		for (typename DWGraph<T>::AdjList::iterator litr = ((*itr).second)->begin(); litr != ((*itr).second)->end(); ++litr)
		{

			if (((this->GPM.at(itr->first)->get_Pre()) > (this->GPM.at(litr->get_vertex())->get_Pre())) && ((this->GPM.at(itr->first)->get_Post()) < (this->GPM.at(litr->get_vertex())->get_Post())))
			{
				//BACK EDGES
				std::cout << " " << this->Lookup.at(itr->first) << " ( " << (this->GPM.at(itr->first)->get_Pre()) << " , " << (this->GPM.at(itr->first)->get_Post()) << " )" << "->" << this->Lookup.at(litr->get_vertex())\
 << " ( " << (this->GPM.at(litr->get_vertex())->get_Pre()) << " , "
						<< (this->GPM.at(litr->get_vertex())->get_Post()) << " )" << std::endl;
			}
		}
	}

}
