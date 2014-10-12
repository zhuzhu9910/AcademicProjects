/////////////////////////////////////////////////////////////////////
//  Graph.cpp - graph structure and relative operations to surport //
//            various graph algorithms and application             //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project2 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////


#include "Graph.h"
using namespace zqyG;
////----<test stub>------------------------------------------------------------------------------------------
#ifdef TEST_GRAPH
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

	zqyG::Graph<std::string, std::string, IFunctor<Vertex<std::string, std::string>>::FunctionType> graph;
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

	zqyG::Graph<std::string, std::string, IFunctor<Vertex<std::string, std::string>>::FunctionType> g2 = graph;
	g2.clearOperations();
	g2.setPreOpt(FunEnunciator);
	g2.DFS();
	std::cout<<"\n";
	for(size_t i = 0; i < g2.size(); ++i)
		std::cout<<g2[i].Value().c_str()<<"  num: "<<g2[i].num()<<"  lowlink: "<<g2[i].lowlink()<<std::endl;
	std::cout<<"\n";
	
	std::vector<size_t> v = FindVertexes<std::string, std::string>("NO.4 Vertex",g2);
	std::vector<std::pair<size_t,std::string>> e = FindEdges<std::string, std::string>("e10",g2);
	std::vector<std::vector<size_t>> rst = g2.findStrong();
	g2.removeVertex(2);
	g2.DFS();
	std::cout<<"\n";
	::system("pause");
	
}

#endif