/////////////////////////////////////////////////////////////////////
//  Graph.h - graph structure and relative operations to surport   //
//            various graph algorithms and application             //
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
  Provides a templated graph class with adjacent vertex table to store the
  structure of the graph. Using public functor and function interface to
  provide vertex and edge operation during DFS. The graph class support topological
  sort and find strong components of the graph. Provides global algorithm for
  find specified value vertex collections and edge collections.
  
  Public Interface:
  =================
  Vertex<std::string, std::string> ve("NO.1");                                                     //create a new vertex
  Graph<std::string, std::string ,IFunctor<Vertex<std::string, std::string>>::FunctionType> graph; //create a new graph class with function operator for DFS
  graph.addVertex(ve);                                                                             //add a vertex to the graph 
  graph.setPreOpt(FunEnunciator);                                                                  //set the pre-operation of the DFS
  graph.DFS();                                                                                     //DFS the graph
  std::vector<std::vector<size_t>> strong = graph.findStrong();                                    //find strong components of the graph
  std::vector<Vertex<VT,ET>> tpsort = graph.TopSortByOutdgree();                                   //find a topological sort of the graph
  Build Process:
  ==============
  cl /D:TEST_GRAPH Vertex.cpp Vertex.cpp Graph.h Graph.cpp
  
  Maintenance History:
  ====================
  ver 1.0 : 14 Mar 10
  - first release
 */

#ifndef GRAPH_H
#define GRAPH_H
#include <vector>
#include <string>
#include <iostream>
#include <stack>
#include <algorithm>
#include "Vertex.h"
#define MIN(num1, num2)(num1)<(num2)?(num1):(num2)
namespace zqyG
{


////////////////////////////////////////////////////////////////////////////////////////
////Class of outside defined functor interface applying during walking through the graph
template<typename T>
class IFunctor
{
public:
	typedef void (FunctionType) (T*, T*);
	virtual ~IFunctor(){}
	virtual void operator() (T* from){}
	virtual void operator() (T* from,T* to){}
};

////////////////////////////////////////////////////////////////////////////////////////
////class of graph with a adjacent vertex table
template <typename VT, typename ET, typename FUN = IFunctor<Vertex<VT,ET>>>
class Graph
{
public:
	
	Graph();
	Graph(const Graph<VT, ET, FUN>& graph);
	Graph<VT, ET, FUN>& operator = (const Graph<VT, ET, FUN>& graph);
	Vertex<VT,ET>& operator [] (size_t pos);
	void addVertex(const Vertex<VT,ET>& vertex);
	void addNewVertex(const VT& Vvalue);
	bool removeVertex(size_t pos);
	std::vector<size_t> findVertexes(VT value);
	size_t findEntry(const Vertex<VT,ET>& vertex) const;
	std::vector<std::vector<size_t>> findStrong();

	FUN* setPreOpt(FUN* popt);
	FUN* setPostOpt(FUN* popt);
	void DFS();
	void DFS(Vertex<VT,ET>* pStart);
	size_t nextUnmarkedChild(Vertex<VT,ET>* pvertex);

	std::vector<Vertex<VT,ET>> TopSortByOutdgree();
	std::vector<Vertex<VT,ET>> TopSortByIndgree();
	
	void clearMarks();
	void clearOperations();
	size_t size();
	void clear();
private:
	void Strong_DFS(Vertex<VT,ET>* pStart);
	std::vector<Vertex<VT,ET>> vertexes;
	FUN* pPreOpt;
	FUN* pPostOpt;

	std::stack<Vertex<VT,ET>*> stk_strong;
	std::vector<std::vector<size_t>> strong_components;
	int numcnt;
};

////----<void constructor of graph class>------------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline Graph<VT, ET, FUN>::Graph() :pPreOpt(0),pPostOpt(0), numcnt(0){}

////----<copy constructor of vertex class>-----------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline Graph<VT, ET, FUN>::Graph(const zqyG::Graph<VT, ET, FUN> &graph)
{
	(*this) = graph;
}

////----<add a vertex to the graph with vertex object>-----------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline void Graph<VT, ET, FUN>::addVertex(const zqyG::Vertex<VT,ET> &vertex)
{
	vertexes.push_back(vertex);
}

////----<create a vertex with specified value and add it into the graph>-----------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline void Graph<VT, ET, FUN>::addNewVertex(const VT& Vvalue)
{
	vertexes.push_back(Vertex<VT,ET>(Vvalue));
}

////----<remove a vertex from the graph>-------------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline bool Graph<VT, ET, FUN>::removeVertex(size_t pos)
{
	if(pos < vertexes.size())
	{
		//remove the relative edges of the vertex from the graph and
		//adjust the referrence number of the edges to the adjacent table
		for(size_t i = 0; i < vertexes.size(); ++i)
			vertexes[i].removeEdge(pos,true);
		//remove the vertex from adjacent table
		vertexes.erase(vertexes.begin() + pos);
		return true;
	}
	return false;
}

////----<clear marks of the vertexes on the graph>---------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline void Graph<VT, ET, FUN>::clearMarks()
{
	numcnt = 0;
	for(size_t i = 0; i < vertexes.size(); ++i)
	{
		vertexes[i].isMarked() = false;
		vertexes[i].num() = 0;
		vertexes[i].lowlink() = 0;
	}
}

////----<clear the functors for DFS to empty status>---------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline void Graph<VT, ET, FUN>::clearOperations()
{
	pPreOpt = 0;
	pPostOpt = 0;
}

////----<find vertexes using specified vertex value>------------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline std::vector<size_t> Graph<VT, ET, FUN>::findVertexes(VT value)
{
	std::vector<size_t> rst;
	for (size_t i = 0; i < vertexes.size(); ++i)
		if (value == vertexes[i].Value())
			rst.push_back(i);
	return rst;
}

////----<find the next unmarked edge for specified vertex>------------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline size_t Graph<VT, ET, FUN>::nextUnmarkedChild(Vertex<VT,ET>* pvertex)
{
	for(size_t i = 0; i < pvertex->size(); ++i)
	{
		size_t ref = (*pvertex)[i].first;
		if(!this->vertexes[ref].isMarked())
		{
			return ref;
		}
	}
	return -1;
}

////----<find the entry referrence number of a vertex>------------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline size_t Graph<VT, ET, FUN>::findEntry(const Vertex<VT,ET>& vertex) const
{
	for (size_t i = 0; i < vertexes.size(); ++i)
	{
		const Vertex<VT,ET>* v = &vertexes[i];
		if (&(vertexes[i]) == &vertex)
			return i;
	}
	return -1;
}

////----<index operator of graph class>----------------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline Vertex<VT,ET>& Graph<VT, ET, FUN>::operator [] (size_t pos)
{
	return vertexes.at(pos);
}

////----<set the post-operation of DFS>----------------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline FUN* Graph<VT, ET, FUN>::setPostOpt(FUN *popt) 
{
	FUN* ptemp = this->pPostOpt;
	this->pPostOpt = popt;
	return ptemp;
}

////----<set the pre-operation of DFS>-----------------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline FUN* Graph<VT, ET, FUN>::setPreOpt(FUN *popt) 
{
	FUN* ptemp = this->pPreOpt;
	this->pPreOpt = popt;
	return ptemp;
}

////----<clear the graph, set the graph to an empty graph>----------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
void Graph<VT, ET, FUN>::clear()
{
	this->vertexes.clear();
	this->strong_components.clear();
	while(!this->stk_strong.empty())
		this->stk_strong.pop();
}

////----<get the size of the adjacent table>------------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline size_t Graph<VT, ET, FUN>::size()
{
	return vertexes.size();
}

////----<assignment operator of graph class>------------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline Graph<VT, ET, FUN>& Graph<VT, ET, FUN>::operator = (const Graph<VT, ET, FUN>& graph)
{
	this->vertexes = graph.vertexes;
	this->pPreOpt = graph.pPreOpt;
	this->pPostOpt = graph.pPostOpt;
	return *this;

}

////----<find a topological sort by indgree of the vertexes>------------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline std::vector<Vertex<VT,ET>> Graph<VT, ET, FUN>::TopSortByIndgree()
{
	std::vector<Vertex<VT,ET>> sorted;
	const size_t mysize = this->size();
	std::vector<int> indgrees( mysize,0);
	//build an indgree table
	for(size_t i = 0; i < mysize; ++i)
		for(size_t j = 0; j < (*this)[i].size(); ++j)
			indgrees[(*this)[i][j].first] = indgrees[(*this)[i][j].first] + 1;
	//find topological sort by indgree
	size_t count = 0;
	while(mysize != sorted.size())
	{
		if(indgrees[count] <= 0 && !(*this)[count].isMarked())
		{
			sorted.push_back((*this)[count]);
			for(size_t i = 0; i < (*this)[count].size(); ++i)
				indgrees[(*this)[count][i].first] = indgrees[(*this)[count][i].first] - 1;
			(*this)[count].isMarked() = true;
		}
		count = (count + 1) % mysize;
	}
	return sorted;
}

////----<find a topological sort by outdgree of the vertexes>------------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline std::vector<Vertex<VT,ET>> Graph<VT, ET, FUN>::TopSortByOutdgree()
{
	std::vector<Vertex<VT,ET>> sorted;
	const size_t mysize = this->size();
	size_t count = 0;
	this->clearMarks();
	//find topological sort by gradually reduce the out dgree of the vertexes
	while(mysize != sorted.size())
	{
		Vertex<VT,ET>& temp = (*this)[count];
		if(-1 == this->nextUnmarkedChild(&temp) && !temp.isMarked())
		{
			sorted.push_back(temp);
			temp.isMarked() = true;
		}
		count = (count + 1) % mysize;
	}

	return sorted;

}

////----<DFS search for strong components>------------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
void Graph<VT, ET, FUN>::Strong_DFS(Vertex<VT,ET> *pStart)
{
	//initialize the num and lowlink value when enter a vertex
	numcnt++;
	pStart->num() = numcnt;
	pStart->lowlink() = pStart->num();
	stk_strong.push(pStart);
    Vertex<VT,ET>* pTemp;
    for(size_t i=0; i<pStart->size(); ++i)
    {
	//Depth first search and find the lowlink value of every vertex
	  pTemp = &vertexes[(*pStart)[i].first];
	  if (!pTemp->isMarked())
	  {
		pTemp->isMarked() = true;
		Strong_DFS(pTemp);
		pStart->lowlink() = MIN(pStart->lowlink(),pTemp->lowlink());
	  }
	  else if (pTemp->num() < pStart->num())
	  {
		if(std::find(stk_strong._Get_container().begin(),stk_strong._Get_container().end(),pTemp)
			!= stk_strong._Get_container().end())
			pStart->lowlink() = MIN(pStart->lowlink(),pTemp->lowlink());
	  }
    }
	//if find strong compenents, pop them out of the stack and sorted into the result set
	if(pStart->lowlink() == pStart->num()){
		std::vector<size_t> stg_cmpt;
		while(!stk_strong.empty() && (stk_strong.top()->num() >= pStart->num()))
		{
			stg_cmpt.push_back(findEntry(*(stk_strong.top())));
			stk_strong.pop();
		}
		strong_components.push_back(stg_cmpt);
	}
}

////----<general DFS using user specified functor>------------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline void Graph<VT, ET, FUN>::DFS()
{
	clearMarks();
	for(size_t i = 0; i < vertexes.size(); ++i)
		if(!vertexes[i].isMarked())
		{
			vertexes[i].isMarked() = true;
			if (pPreOpt != 0)
				(*pPreOpt)(0, &(vertexes[i]));
			DFS(&(vertexes[i]));
			if (pPostOpt != 0)
				(*pPostOpt)(0, &(vertexes[i]));
		}
}

////----<general DFS execution body>-------------------------------------------------------------------------------------------------
////go into every vertex and execute the pre-operation before enter a vertex and execute the post-operation before leave a vertex
template <typename VT, typename ET, typename FUN>
void Graph<VT, ET, FUN>::DFS(Vertex<VT,ET> *pStart)
{
	numcnt++;
	pStart->num() = numcnt;
    Vertex<VT,ET>* pTemp;
    for(size_t i=0; i<pStart->size(); ++i)
    {
	  pTemp = &vertexes[(*pStart)[i].first];
	  if (!pTemp->isMarked())
	  {
		pTemp->isMarked() = true;
		if (pPreOpt != 0)
			(*pPreOpt)(pStart, pTemp);
		DFS(pTemp);
		if (pPostOpt != 0)
			(*pPostOpt)(pStart, pTemp);
	  }
    }
}

////----<find strong compoenents of the graph using DFS>------------------------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
std::vector<std::vector<size_t>> Graph<VT, ET, FUN>::findStrong()
{
	clearMarks();
	this->stk_strong = std::stack<Vertex<VT,ET>*>();
	this->strong_components.clear();

	for(size_t i = 0; i < vertexes.size(); ++i)
		if(!vertexes[i].isMarked())
		{
			vertexes[i].isMarked() = true;
			Strong_DFS(&(vertexes[i]));
		}
	return strong_components;
}

////----<Global Algorithm to find a collection of vertices with specified vertex value in a graph>--------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline std::vector<size_t> FindVertexes(const VT& Vvalue, Graph<VT, ET, FUN>& graph)
{
	std::vector<size_t> rst;
	for (size_t i = 0; i < graph.size(); ++i)
		if (Vvalue == graph[i].Value())
			rst.push_back(i);
	return rst;
}

////----<Global Algorithm to find a collection of vertices with specified vertex value in a graph with default graph template parameter>------------------------------
template <typename VT, typename ET>
inline std::vector<size_t> FindVertexes(const VT& Vvalue, Graph<VT, ET>& graph)
{
	std::vector<size_t> rst;
	for (size_t i = 0; i < graph.size(); ++i)
		if (Vvalue == graph[i].Value())
			rst.push_back(i);
	return rst;
}

////----<Global Algorithm to find a collection of edges with specified edge value in a graph>------------------------------------------------------------------------
template <typename VT, typename ET, typename FUN>
inline std::vector<std::pair<size_t,ET>> FindEdges(const ET& Evalue, Graph<VT, ET, FUN>& graph)
{
	std::vector<std::pair<size_t,ET>> rst;
	for (size_t i = 0; i < graph.size(); ++i)
		for (size_t j = 0; j < graph[i].size(); ++j)
		if (Evalue == graph[i][j].second)
			rst.push_back(graph[i][j]);
	return rst;
}

////----<Global Algorithm to find a collection of edges with specified edge value in a graph with default graph template parameter>----------------------------------
template <typename VT, typename ET>
inline std::vector<std::pair<size_t,ET>> FindEdges(const ET& Evalue, Graph<VT, ET>& graph)
{
	std::vector<std::pair<size_t,ET>> rst;
	for (size_t i = 0; i < graph.size(); ++i)
		for (size_t j = 0; j < graph[i].size(); ++j)
		if (Evalue == graph[i][j].second)
			rst.push_back(graph[i][j]);
	return rst;
}



////----<Enunciating function to display the vertex the DFS visted and the edges it traversed>-----------------------------------------------------------------------
template <typename VT, typename ET>
void FunEnunciator(Vertex<VT,ET>* from,Vertex<VT,ET>* to)
{
		if(from == 0)
		{
			std::cout<<"\n"<<"Visit Vertex: "<<to->Value();
			return;
		}

		std::cout<<"\n"<<"Visit Edge: <From: "<<from->Value()<<" To: "<<to->Value()<<">";
		std::cout<<"\n"<<"Visit Vertex: "<<to->Value();
}

//////////////////////////////////////////////////////////////////////////////////////////
////Enunciator functor for DFS of the graph
template <typename VT, typename ET>
class Enunciator : public IFunctor<Vertex<VT,ET>>
{
public:
////----<constructor of the enunciator with an argument to get the referrence of the graph>----------------------------------------------------------
	Enunciator (const Graph<VT, ET>& graph) :_graph(graph){};

////----<() operator to provide the function of enuniating the vertex and edges when visit them>-----------------------------------------------------
	void operator() (Vertex<VT,ET>* from,Vertex<VT,ET>* to)
	{
		if(from == 0)
		{
			std::cout<<"\n"<<"Visit Vertex: "<<to->Value();
			return;
		}
		size_t inObj2 = _graph.findEntry(*to);
		ET edge;
		for(size_t i = 0; i < from->size(); ++i)
			if((*from)[i].first == inObj2){
				edge = (*from)[i].second;
				break;
			}

		std::cout<<"\n"<<"Visit Edge: <From: "<<from->Value()<<" To: "<<to->Value()<<" Edge Value: "<<edge;
		std::cout<<"\n"<<"Visit Vertex: "<<to->Value();
	}
private:
	const Graph<VT, ET>& _graph;
	
};


}












#endif