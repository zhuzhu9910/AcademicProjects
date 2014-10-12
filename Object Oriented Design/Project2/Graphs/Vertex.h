#ifndef VERTEX_H
#define VERTEX_H
/////////////////////////////////////////////////////////////////////
//  Vertex.h - store information of vertexes for a graph           //
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
  Provides a template vertex class with template parameters of vertex
  value type and edge and necessary member function utilities to access
  the value of vertex and edges.
  
  Public Interface:
  =================
  Vertex<std::string, std::string> vex("NO.1 vertex");            //initialize a vertex with value "NO.1 vertex"
  vex.addEdge(1,"e1");                                            //add an edge (1,"e1") to the vertex        
  vexremoveEdge(0);                                               //remove an edge with reference number 0 from the vertex
  size_t vexSize = vex.size();                                    //get the amount of edges of the vertex
  std::pair<size_t,ET>> edge1 = vex[0];                           //get an edge of the vertex

  Build Process:
  ==============
  cl /D:TEST_VERTEX Vertex.cpp Vertex.cpp
  
  Maintenance History:
  ====================
  ver 1.0 : 14 Mar 10
  - first release
 */


#include <vector>
#include <string>
namespace zqyG
{

//////////////////////////////////////////////////////////////////////////////
/////class of vertex for a directed graph
template <typename VT, typename ET>
class Vertex
{
public:
	Vertex();
	Vertex(const VT& value);
	Vertex(const Vertex<VT,ET>& vertex);
	Vertex<VT,ET>& operator = (const Vertex<VT,ET>& vertex);
	std::pair<size_t,ET>& operator [] (size_t pos);
	size_t size() const;
	void addEdge(size_t child,const ET& edgeV);
	bool removeEdge(size_t ref,bool isvxRm = false);
	VT& Value();
	bool& isMarked();
	int& num();
	int& lowlink();
private:
	VT _v;
	bool _mark;
	int _num;
	int _lowlink;
	std::vector<std::pair<size_t,ET>> edges;
};

////----<void constructor of vertex class>------------------------------------------------------------------------------------------
template <typename VT, typename ET>
inline Vertex<VT,ET>::Vertex() : _mark(false),_num(0),_lowlink(0){};

////----<promotion constructor of vertex class with initializing vertex value>-----------------------------------------------------
template <typename VT, typename ET>
inline Vertex<VT,ET>::Vertex(const VT& value) :_v(value), _mark(false),_num(0),_lowlink(0){};

////----<copy constructor of vertex class>-----------------------------------------------------------------------------------------
template <typename VT, typename ET>
inline Vertex<VT,ET>::Vertex(const zqyG::Vertex<VT,ET> &vertex)
{
	(*this) = vertex;
}

////----<assignment operator of vertex class>--------------------------------------------------------------------------------------
template <typename VT, typename ET>
inline Vertex<VT,ET>& Vertex<VT,ET>::operator =(const zqyG::Vertex<VT,ET> &vertex)
{
	this->_v = vertex._v;
	this->_mark = vertex._mark;
	this->edges = vertex.edges;
	this->_num = vertex._num;
	this->_lowlink = vertex._lowlink;
	return *this;
}

////----<get the amount of edges this vertex direct to>---------------------------------------------------------------------------
template <typename VT, typename ET>
inline size_t Vertex<VT,ET>::size() const
{
	return edges.size();
}

////----<index operator of vertex>-------------------------------------------------------------------------------------------------
template <typename VT, typename ET>
inline std::pair<size_t,ET>& Vertex<VT,ET>::operator [](size_t pos)
{
	return edges.at(pos);
}

////----<add a edge to vertex with reference number in adjacent table and edge value>----------------------------------------------
template <typename VT, typename ET>
inline void Vertex<VT,ET>::addEdge(size_t child,const ET& edgeV)
{
	std::pair<size_t,ET> tpair(child,edgeV);
	for(size_t i = 0; i < edges.size(); ++i)
	{
		if(edges[i].first == child && edges[i].second == edgeV)
			return;
	}
	edges.push_back(tpair);
}

////----<void remove an edge from this vertex wiht specified reference number>-----------------------------------------------------
////the argument isvxRm notes that if this vertex will be remobed from the adjacent table
template <typename VT, typename ET>
inline bool Vertex<VT,ET>::removeEdge(size_t ref,bool isvxRm)
{
	std::vector<std::pair<size_t,ET>>::iterator iter;
	bool succ = false;
	for(size_t index = 0; index < edges.size(); ++index)
	{
		//if the vertex related to the edge will be removed, adjusts the relative vertex reference number
		if(isvxRm && edges[index].first > ref)
			edges[index].first--;
		if(edges[index].first == ref){
			edges.erase(edges.begin() + index);
			succ = true;
		}
	}
	return succ;
}


////----<access the reference of the mark of this vertex>-------------------------------------------------------------------------
template <typename VT, typename ET>
inline bool& Vertex<VT,ET>::isMarked()
{
	return _mark;
}

////----<access the value of this vertex>-----------------------------------------------------------------------------------------
template <typename VT, typename ET>
inline VT& Vertex<VT,ET>::Value() 
{
	return _v;
}
////----<access the number mark of this vertex>-----------------------------------------------------------------------------------
template <typename VT, typename ET>
inline int& Vertex<VT,ET>::num()
{
	return _num;
}

////----<access lowlink mark of this vertex>-----------------------------------------------------------------------------------------
template <typename VT, typename ET>
inline int& Vertex<VT,ET>::lowlink()
{
	return _lowlink;
}

}

#endif