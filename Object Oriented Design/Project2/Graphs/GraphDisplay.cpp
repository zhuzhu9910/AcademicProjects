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

#include "GraphDisplay.h"
#include "Graph.h"
using namespace zqyG;

////----<build the graphs and get the objects composed them>------------------------------------------------------------------------------------------
void DisplayGraphs::Build()
{
	fdgc = ge->getCgraphofFileDependence();
	pdgc = ge->getCgraphofPackDependence();
	xmlg = ge->GetXMLGraphs();
}

////----<display all 4 kinds of graphs generated from xml>------------------------------------------------------------------------------------------
void DisplayGraphs::DisplayXmlGraphs()
{
	if(this->xmlg == 0)
		return;
	std::vector<GraphFromXML<std::string, std::string>> ss = this->xmlg->getSringSGraphs();
	std::vector<GraphFromXML<std::string,double>> sd = this->xmlg->getStringDGraphs();
	std::vector<GraphFromXML<double, std::string>> ds = this->xmlg->getDoulbeSGraphs();
	std::vector<GraphFromXML<double, double>> dd = this->xmlg->getDoubleDGraphs();
	std::cout<<"\n====================================================================";
	std::cout<<"\n Display Graphs From XML\n";
	std::cout<<"\n -----------------------------------------------------------------------";
	std::cout<<"\n Graphs with string vertex value and string edge value";
	std::cout<<"\n -----------------------------------------------------------------------";
	ShowXmlGraph<std::string,std::string>(ss);
	std::cout<<"\n -----------------------------------------------------------------------";
	std::cout<<"\n Graphs with string vertex value and double edge value";
	std::cout<<"\n -----------------------------------------------------------------------";
	ShowXmlGraph<std::string,double>(sd);
	std::cout<<"\n -----------------------------------------------------------------------";
	std::cout<<"\n Graphs with double vertex value and string edge value";
	std::cout<<"\n -----------------------------------------------------------------------";
	ShowXmlGraph<double, std::string>(ds);
	std::cout<<"\n -----------------------------------------------------------------------";
	std::cout<<"\n Graphs with double vertex value and double edge value";
	std::cout<<"\n -----------------------------------------------------------------------";
	ShowXmlGraph<double,double>(dd);
}

////----<display file dependency graphs and the package dependency graphs>------------------------------------------------------------------
void DisplayGraphs::DisplayFileDependency()
{
	std::cout<<"\n====================================================================";
	std::cout<<"\n Display Packages Dependency Relationship with toplogically sorted vertexes";
	std::cout<<"\n -----------------------------------------------------------------------";
	zqyG::CondensedGraph<std::string,std::string>* cdgp = ge->getCgraphofPackDependence();
	if(cdgp)
	{
		zqyG::Graph<std::string,std::string>& graphpk = cdgp->GetAttachedGraph();
		std::vector<Vertex<std::string, std::string>> vtxs = graphpk.TopSortByOutdgree();
		for(size_t i = 0; i < vtxs.size(); ++i)
			DisplayVertexes<std::string, std::string>(&vtxs[i],&graphpk);
	}

	std::cout<<"\n -----------------------------------------------------------------------";
	std::cout<<"\n Display Files Dependency Relationship with toplogically sorted vertexes";
	std::cout<<"\n -----------------------------------------------------------------------";
	zqyG::CondensedGraph<std::string,std::string>* cdgf = ge->getCgraphofFileDependence();
	if(cdgf)
	{
		zqyG::Graph<std::string,std::string>& graphpk = cdgf->GetAttachedGraph();
		std::vector<Vertex<std::string, std::string>> vtxs = graphpk.TopSortByOutdgree();
		for(size_t i = 0; i < vtxs.size(); ++i)
			DisplayVertexes<std::string, std::string>(&vtxs[i],&graphpk);
	}
	std::cout<<"\n====================================================================\n";
}

////----<static function to display strong components in condensed graphs>---------------------------------------------------------------------
////input the collections strong components, the oriningal graph and the composed graph
void DisplayGraphs::ShowCondensedGraph(std::vector<Vertex<std::vector<size_t>,size_t>>& tpsort,
									   zqyG::Graph<std::string,std::string>& graph
									   ,zqyG::Graph<std::vector<size_t>,size_t>& cgraph)
{
	for(size_t i = 0; i < tpsort.size(); ++i)
	{
		std::cout<<"\nStrong Component "<<cgraph.findVertexes(tpsort[i].Value()).at(0)<<": ";
		for(size_t j = 0; j < tpsort[i].Value().size(); ++j)
			std::cout<<"\n Vertex: "<<graph[tpsort[i].Value()[j]].Value();
		std::cout<<"\nEges to Strong Components: ";
		for(size_t j = 0; j < tpsort[i].size(); ++j)
			std::cout<<tpsort[i][j].first  <<" ";
		std::cout<<std::endl;
	}
}


////----<test 1, show meeting requirement of 1, 2, 3, 6>----------------------------------------------------------------------------
void test1()
{
	std::cout<<"\n====================================================================";
	std::cout<<"\nRequirement 1 and 2:\n Please check my codes to see I have met them.";
	std::cout<<"\n====================================================================\n\n";
	std::cout<<"\n====================================================================";
	std::cout<<"\nRequirement 3 and 6:\n Please check the Vertex.h, Vertex.cpp, Graph.h, Graph.cpp files to see that I have met it.";
	std::cout<<"\n====================================================================\n\n";
	::system("pause");
}

////----<function to adding edges of the graph in test2>-----------------------------------------------------------------------------
template<typename T>
void test2_edgeAdding(T& graph)
{
	graph[0].addEdge(3,"e1"); 
	graph[0].addEdge(6,"e2");  
	graph[1].addEdge(4,"e3");
	graph[1].addEdge(6,"e4");
	graph[2].addEdge(0,"e6");
	graph[1].addEdge(5,"e7");
	graph[4].addEdge(5,"e8");
	graph[4].addEdge(6,"e9");
	graph[5].addEdge(1,"e10");
	graph[6].addEdge(2,"e10");
}

////----<test case to show meeting requirement 4>------------------------------------------------------------------------------------------
void testR4(zqyG::Graph<std::string, std::string, IFunctor<Vertex<std::string, std::string>>::FunctionType>& gFct,
			zqyG::Graph<std::string,std::string>& gFctor)
{
	std::cout<<"\n====================================================================";
	std::cout<<"\nRequirement 4:\n Using Function and Functor to DFS the Graph enunicate the content of the graph";
	std::cout<<"\n====================================================================";
	std::cout<<"\nUsing Functor to DFS:\n";
	Enunciator<std::string,std::string> efunctor(gFctor);
	gFctor.setPreOpt(&efunctor);
	gFctor.DFS();
	std::cout<<"\n\nUsing Function to DFS:\n";
	gFct.setPreOpt(FunEnunciator);
	gFct.DFS();
	std::cout<<"\n====================================================================";
	std::cout<<"\nRequirement 4:\n copy constructor and assignment.\n apply copy constructor and assignment operation to the above object and then display them again";
	std::cout<<"\n====================================================================";
	zqyG::Graph<std::string, std::string, IFunctor<Vertex<std::string, std::string>>::FunctionType> gft = gFct;
	zqyG::Graph<std::string,std::string> gfctor;
	gfctor = gFctor;
	Enunciator<std::string,std::string> ecfunctr(gfctor); 
	gfctor.setPreOpt(&ecfunctr);
	std::cout<<"\nThe object applied assignment operation:\n";
	gfctor.DFS();
	std::cout<<"\n\nThe object applied copy constructor:\n";
	gft.DFS();
	::system("pause");
}

////----<test case to show meeting requirement 5>------------------------------------------------------------------------------------------
void testR5(zqyG::Graph<std::string,std::string>& graph)
{
	std::cout<<"\n====================================================================";
	std::cout<<"\nRequirement 5:\n Creation of the condensed graph and toplogically sort the strong components then display them";
	std::cout<<"\n====================================================================";
	CondensedGraph<std::string, std::string> cg(&graph);
	cg.CreateCondensed();
	std::vector<Vertex<std::vector<size_t>,size_t>> tpsort1 = cg.TopSortByIndgree();
	std::vector<Vertex<std::vector<size_t>,size_t>> tpsort2 = cg.TopSortByOutdgree();
	std::cout<<"\n -----------------------------------------------------------------------";
	std::cout<<"\nTopological sort by indgree: ";
	std::cout<<"\n -----------------------------------------------------------------------";
	DisplayGraphs::ShowCondensedGraph(tpsort1,graph,cg.GetCondensedGraph());
	std::cout<<"\n -----------------------------------------------------------------------";
	std::cout<<"\nTopological sort by outdgree: ";
	std::cout<<"\n -----------------------------------------------------------------------";
	DisplayGraphs::ShowCondensedGraph(tpsort2,graph,cg.GetCondensedGraph());
	std::cout<<"\n\n";
	::system("pause");
}

////----<test case to show meeting requirement 7>------------------------------------------------------------------------------------------
void testR7(zqyG::Graph<std::string,std::string>& graph)
{
	std::cout<<"\n====================================================================";
	std::cout<<"\nRequirement 7: Global functons to find collections of specified vertices and edges";
	std::cout<<"\n====================================================================";
	std::cout<<"\nSearch for vertexes has value \'NO.1 Vertex\': ";
	std::vector<size_t> vs = FindVertexes<std::string,std::string>("NO.1 Vertex",graph);
	for(size_t i = 0; i < vs.size(); ++i)
		DisplayGraphs::DisplayVertexes<std::string,std::string>(&graph[vs[i]]);
	std::cout<<"\nSearch for edges has value \'e10\':\n ";
	std::vector<std::pair<size_t,std::string>> es = FindEdges<std::string,std::string>("e10",graph);
	for(size_t i = 0; i < es.size(); ++i)
		std::cout<<"("<<es[i].first<<" , "<<es[i].second<<") ";
	std::cout<<"\n\n";
	::system("pause");

}

////----<test case to show meeting requirements>------------------------------------------------------------------------------------------
void test2()
{
	const int VexSize = 7;
	zqyG::Vertex<std::string, std::string> vex[VexSize] = 
	   {zqyG::Vertex<std::string, std::string>("NO.1 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.2 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.3 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.4 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.5 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.6 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.7 Vertex")};

	zqyG::Graph<std::string, std::string, IFunctor<Vertex<std::string, std::string>>::FunctionType> graphFct;
	zqyG::Graph<std::string,std::string> graphFctor;
	for(size_t i = 0; i < VexSize; ++i)
	{
		graphFct.addVertex(vex[i]);
		graphFctor.addVertex(vex[i]);
	}
	test2_edgeAdding<zqyG::Graph<std::string, std::string, IFunctor<Vertex<std::string, std::string>>::FunctionType>>(graphFct);
	test2_edgeAdding<zqyG::Graph<std::string,std::string>>(graphFctor);
	testR4(graphFct,graphFctor);
	testR5(graphFctor);
	testR7(graphFctor);
	

}

////----<executive test cases for project 2>------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	GraphsExecutive ge;
	if(!ge.InputFromCmdL(argc,argv))
	{
		std::cout<<"input format error!"<<std::endl;
		return 1;
	}
	ge.GenerateGraphs();
	DisplayGraphs dpg(&ge);
	dpg.Build();

	std::string cm = "-xml";
	if (cm == argv[1])
	{
		test1();
		test2();
		std::cout<<"\n====================================================================";
		std::cout<<"\nRequirement 6, 8, 9: Read graphs information from XML to build and display graphs";
		std::cout<<"\n====================================================================\n";
		dpg.DisplayXmlGraphs();
		std::cout<<"\n\n";
	}
	cm = "-file";
	if(cm == argv[1])
	{
		std::cout<<"\n====================================================================";
		std::cout<<"\nRequirement 10: Analysize files and packages dependencies";
		std::cout<<"\n====================================================================\n";
		dpg.DisplayFileDependency();
	}
	::system("pause");
	return 1;
}