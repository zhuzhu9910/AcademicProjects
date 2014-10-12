#ifndef GRAPHDISPLAY_H
#define GRAPHDISPLAY_H
/////////////////////////////////////////////////////////////////////
//  GraphDisplay.h - display the content of a graph, vertex or a   // 
//               collection of vertexes                            //
//                                                                 //
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
  Provides a class using the GraphsExecutive class to display the generated class
  and vertexes. This module also provides some test functions to demonstrate the 
  meeting of the requirement of the project 2.
  
  Public Interface:
  =================
  GraphsExecutive ge;                                                                    //create a GraphsExecutive object
  bool succ = ge.InputFromCmdL(argc,argv);                                               //get command from command line arguments
  ge.GenerateGraphs();                                                                   //generate graphs
  DisplayGraphs dpg(&ge);                                                                //create a DisplayGraphs with a GraphsExecutive object
  dpg.Build();                                                                           //build and get the objects composed the graphs
  dpg.DisplayXmlGraphs();                                                                //display graphs building from graphs
  dpg.DisplayFileDependency();                                                           //display file and package dependency graphs

  Build Process:
  ==============
  Required files
  - Vertex.cpp Vertex.cpp Graph.h Graph.cpp CondensedGraph.h CondensedGraph.cpp
  FileDependency.h FileDependency.cpp FileDependencyGraph.h FileDependencyGraph.cpp 
  GraphFromXML.h GraphFromXML.cpp GraphsExecutive.h GraphsExecutive.cpp 
  GraphDisplay.h GraphDisplay.cpp

  Build commands 
    - devenv Project2.sln
  
  Maintenance History:
  ====================
  ver 1.0 : 15 Mar 10
  - first release
 */
#include "FileDependencyGraph.h"
#include "CondensedGraph.h"
#include "Graph.h"
#include "GraphFromXML.h"
#include "GraphsExecutive.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////class for displaying graphs
class DisplayGraphs
{
public:
	DisplayGraphs(GraphsExecutive* gexe);
	
	template<typename VT,typename ET>
	static void DisplayGraph(zqyG::Graph<VT,ET>* graph);

	template<typename VT,typename ET>
	static void DisplayVertexes(zqyG::Vertex<VT,ET>* vt);

	template<typename VT,typename ET>
	static void DisplayVertexes(zqyG::Vertex<VT,ET>* vt, zqyG::Graph<VT,ET>* gh);

	static void ShowCondensedGraph(std::vector<zqyG::Vertex<std::vector<size_t>,size_t>>& tpsort
		,zqyG::Graph<std::string,std::string>& graph
		,zqyG::Graph<std::vector<size_t>,size_t>& cgraph);
	
	void Build();
	void DisplayXmlGraphs();
	void DisplayFileDependency();
private:
	GraphsExecutive *ge;
	XMLGraphs* xmlg;
	template<typename VT,typename ET>
	void ShowXmlGraph(std::vector<GraphFromXML<VT,ET>>& xmlg);
	zqyG::CondensedGraph<std::string , std::string>* fdgc;
	zqyG::CondensedGraph<std::string , std::string>* pdgc;
};

////----<constructor of the class inputing a GraphsExecutive object>----------------------------------------------------------------
inline DisplayGraphs::DisplayGraphs(GraphsExecutive* gexe) :ge(gexe),fdgc(0),pdgc(0),xmlg(0){}

////----<static function to display a graph>------------------------------------------------------------------------------------------
template<typename VT,typename ET>
void DisplayGraphs::DisplayGraph(zqyG::Graph<VT,ET> *graph)
{
	for(size_t i = 0; i < graph->size(); ++i)
	{
		std::cout<<"\nVertex "<<i<<": "<<(*graph)[i].Value();
		std::cout<<"\n Edges: ";
		for(size_t j = 0; j < (*graph)[i].size(); ++j)
			std::cout<<"( "<<(*graph)[i][j].first<<", "<<(*graph)[i][j].second<<" ) ";
		std::cout<<"\n--------------";
	}
}

////----<static function to display a vertex>------------------------------------------------------------------------------------------
template<typename VT,typename ET>
void DisplayGraphs::DisplayVertexes(zqyG::Vertex<VT,ET>* vt)
{
	std::cout<<"\nVertex "<<": "<<vt->Value();
	std::cout<<"\n Edges: ";
	for(size_t j = 0; j < vt->size(); ++j)
		std::cout<<"( "<<(*vt)[j].first<<", "<<(*vt)[j].second<<" ) ";
	std::cout<<"\n--------------";
}

////----<static function to display a vertex>------------------------------------------------------------------------------------------
template<typename VT,typename ET>
void DisplayGraphs::DisplayVertexes(zqyG::Vertex<VT,ET>* vt, zqyG::Graph<VT,ET>* gh)
{
	std::cout<<"\nVertex "<<": "<<vt->Value();
	std::cout<<"\n Edges: ";
	for(size_t j = 0; j < vt->size(); ++j)
		std::cout<<"( "<<(*gh)[(*vt)[j].first].Value()<<", "<<(*vt)[j].second<<" ) ";
	std::cout<<"\n--------------";
}

////----<display a collection of graphs generated from xml>----------------------------------------------------------------------------
template<typename VT,typename ET>
void DisplayGraphs::ShowXmlGraph(std::vector<GraphFromXML<VT,ET>>& xmlg)
{
	for(size_t i = 0; i < xmlg.size(); ++i)
	{
		std::cout<<"\nGraph"<<i<<": ";
		DisplayGraph<VT,ET>(&xmlg[i]);
		std::cout<<"\n";
	}
}
#endif