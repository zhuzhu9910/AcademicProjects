#ifndef CONDENSEDGRAPH_H
#define CONDENSEDGRAPH_H
/////////////////////////////////////////////////////////////////////
//  CondensedGraph.h - generate a condensed graph from             //
//                    a specified graph                            //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project2 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations:
  ==================
  Provides a class to create and manage the condensed graph from a specified
  graph with some algorithm applied on the condensed graph. It also keeps a 
  referrence to the specified graph but is not responsible for it.
  
  Public Interface:
  =================
  Graph<std::string, std::string> cg;                                              //create a graph object
  CondensedGraph<std::string, std::string> cg(&graph)                              //create a condensed graph creator object
  cg.CreateCondensed();                                                            //create condensed graph
  std::vector<Vertex<std::vector<size_t>,size_t>> tpsort = cg.TopSortByIndgree();  //find a topological sort of the condensed graph

  Build Process:
  ==============
  cl /D:TEST_CONDENSEDGRAPH Vertex.cpp Vertex.cpp Graph.h Graph.cpp CondensedGraph.h CondensedGraph.cpp
  
  Maintenance History:
  ====================
  ver 1.0 : 14 Mar 10
  - first release
 */

#include <vector>
#include <string>
#include <iostream>
#include <stack>
#include <algorithm>
#include "Graph.h"


namespace zqyG
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////class of CondensedGraph for creating and managing of a condensed graph
template <typename VT, typename ET>
class CondensedGraph
{
public:
	CondensedGraph(Graph<VT,ET>* graph);
	CondensedGraph(const CondensedGraph<VT,ET>& cgp);
	~CondensedGraph();
	void CreateCondensed();
	std::vector<Vertex<std::vector<size_t>,size_t>> TopSortByIndgree();
	std::vector<Vertex<std::vector<size_t>,size_t>> TopSortByOutdgree();
	Graph<std::vector<size_t>,size_t>& GetCondensedGraph();
	Graph<VT,ET>& GetAttachedGraph();
	CondensedGraph<VT,ET>& operator = (const CondensedGraph<VT,ET>& cgp);
private:
	Graph<VT,ET>* _graph;
	Graph<std::vector<size_t>,size_t>* cndgraph;
	bool isInStrong(size_t vtx, const std::vector<size_t>& strong);
	bool hasEdgetoStrong(size_t evtx, const std::vector<size_t>& strong);

};

////----<constructor of graph class with an attached graph>---------------------------------------------------------------------------------
template <typename VT, typename ET>
CondensedGraph<VT,ET>::CondensedGraph(Graph<VT,ET> *graph) :_graph(graph),cndgraph(0)
{
	cndgraph = new Graph<std::vector<size_t>,size_t>();
}

////----<deconstruct the created condensed graph >------------------------------------------------------------------------------------------
template <typename VT, typename ET>
CondensedGraph<VT,ET>::~CondensedGraph()
{
	delete cndgraph;
}

////----<copy constructor of condensed graph class>------------------------------------------------------------------------------------------
template <typename VT, typename ET>
CondensedGraph<VT,ET>::CondensedGraph(const zqyG::CondensedGraph<VT,ET> &cgp) :cndgraph(0)
{
	*this = cgp;
}

////----<assignment operator of condensed graph class>---------------------------------------------------------------------------------------
template <typename VT, typename ET>
CondensedGraph<VT,ET>& CondensedGraph<VT,ET>::operator = (const CondensedGraph<VT,ET>& cgp)
{
	this->_graph = cgp._graph;
	if(cgp.cndgraph != 0)
	{
		delete this->cndgraph;
		this->cndgraph = new Graph<std::vector<size_t>,size_t>;
		*(this->cndgraph) = *(cgp.cndgraph);
	}
	else
		this->cndgraph = 0;
	return *this;
}

////----<get the referrence of the created condensed graph>-----------------------------------------------------------------------------------
template <typename VT, typename ET>
inline Graph<std::vector<size_t>,size_t>& CondensedGraph<VT,ET>::GetCondensedGraph()
{
	return *cndgraph;
}

////----<get the referrence of the attached graph>--------------------------------------------------------------------------------------------
template <typename VT, typename ET>
inline Graph<VT,ET>& CondensedGraph<VT,ET>::GetAttachedGraph()
{
	return *_graph;
}

////----<find topolofical sort of the condensed graph by indgree>------------------------------------------------------------------------------------------
template <typename VT, typename ET>
inline std::vector<Vertex<std::vector<size_t>,size_t>> CondensedGraph<VT,ET>::TopSortByIndgree()
{
	return cndgraph->TopSortByIndgree();
}

////----<find topological sort of the condensed graph by outdgree>------------------------------------------------------------------------------------------
template <typename VT, typename ET>
inline std::vector<Vertex<std::vector<size_t>,size_t>> CondensedGraph<VT,ET>::TopSortByOutdgree()
{
	return cndgraph->TopSortByOutdgree();
}

////----<with a specified adjacent table referrence, find this vertex is in a specified strong component>---------------------------------------------------
template <typename VT, typename ET>
inline bool CondensedGraph<VT,ET>::isInStrong(size_t vtx, const std::vector<size_t> &strong) 
{
	for(size_t i = 0; i < strong.size(); ++i)
		if(vtx == strong[i])
			return true;
	return false;
}

////----<find out if there is an edge from a specified vertex to a specified strong component>----------------------------------------------------------------
template <typename VT, typename ET>
inline bool CondensedGraph<VT,ET>::hasEdgetoStrong(size_t evtx, const std::vector<size_t> &strong) 
{
	for(size_t i = 0; i < (*_graph)[evtx].size(); ++i)
		if(isInStrong((*_graph)[evtx][i].first, strong))
			return true;
	return false;

}

////----<create a condensed graph from the attached graph>------------------------------------------------------------------------------------------
template <typename VT, typename ET>
inline void CondensedGraph<VT,ET>::CreateCondensed()
{
	//get the strong components
	std::vector<std::vector<size_t>> stg = _graph->findStrong();
	std::vector<std::pair<size_t,size_t>> sedges;
	//find edges amoung the strong components
	cndgraph->clear();
	for(size_t i = 0; i < stg.size(); ++i)
	{
		cndgraph->addNewVertex(stg[i]);
		for(size_t j = 0; j < _graph->size(); ++j)
			if(isInStrong(j,stg[i]))
				continue;
			else if(hasEdgetoStrong(j,stg[i]))
			{
				std::pair<size_t,size_t> temp;
				temp.first = j;
				temp.second = i;
				sedges.push_back(temp);
			}
	}
	//add edges to the condensed graph
	for(size_t i = 0; i < sedges.size(); ++i)
		for(size_t j = 0; j < stg.size(); ++j)
			if (isInStrong(sedges[i].first,stg[j]))
				(*cndgraph)[j].addEdge(sedges[i].second,69);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////a interface for classes have the capacity of generating condensed graphs
template <typename VT, typename ET>
struct IGraphBuilder
{
	virtual ~IGraphBuilder(){};
	virtual CondensedGraph<VT,ET>* GetCondensedGraph(){return 0;}
};


}

#endif