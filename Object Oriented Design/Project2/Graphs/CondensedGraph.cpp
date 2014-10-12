/////////////////////////////////////////////////////////////////////
//  CondensedGraph.cpp - generate a condensed graph from           //
//                    a specified graph                            //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project2 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////

#include "CondensedGraph.h"

using namespace zqyG;

////----<test stub>------------------------------------------------------------------------------------------
#ifdef TEST_CONDENSEDGRAPH
void main()
{
	const int VexSize = 6;
	zqyG::Vertex<std::string, std::string> vex[VexSize] = 
	   {zqyG::Vertex<std::string, std::string>("NO.1 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.2 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.3 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.4 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.5 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.6 Vertex")};

	zqyG::Graph<std::string, std::string> graph;
	for(size_t i = 0; i < VexSize; ++i)
		graph.addVertex(vex[i]);
	graph[0].addEdge(1,"e1"); 
	graph[0].addEdge(2,"e2");  
	graph[0].addEdge(3,"e3");
	graph[1].addEdge(2,"e4");
	graph[1].addEdge(4,"e6");
	graph[1].addEdge(5,"e7");
	graph[2].addEdge(5,"e8");
	graph[2].addEdge(0,"e9");
	graph[5].addEdge(1,"e10");

	CondensedGraph<std::string, std::string> cg(&graph);
	cg.CreateCondensed();
	std::vector<Vertex<std::vector<size_t>,size_t>> tpsort = cg.TopSortByIndgree();
	std::vector<Vertex<std::vector<size_t>,size_t>> tpsort1 = cg.TopSortByOutdgree();
	CondensedGraph<std::string, std::string> cg1 =cg;

	::system("pause");
	
}

#endif