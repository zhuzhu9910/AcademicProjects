/////////////////////////////////////////////////////////////////////
//  FileDependencyGraph.cpp - generate graphs to demonstrate file  //
//                     dependency relationships                    //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project2 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////

#include "FileDependencyGraph.h"
using namespace zqyG;

////----<void constructor of DependencyGraph class>-----------------------------------------------------------------------------------
DependencyGraph::DependencyGraph() :pgraph(0),pcg(0)
{
	try
	{
		pgraph = new zqyG::Graph<std::string, std::string>;
		pcg = new zqyG::CondensedGraph<std::string, std::string>(pgraph);
	}
	catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
	  //if constructing failed, delete initialized objects
	  if(!pgraph)
		  delete pgraph;
	  if(!pcg)
		  delete pcg;
    }
}

////----<deconstruct the class and delete the generated graph>------------------------------------------------------------------------
DependencyGraph::~DependencyGraph()
{
	delete this->pcg;
	delete this->pgraph;
	for(size_t i = 0; i < dependences.size(); ++i)
		dependences[i];
}

////----<generate graph from dependency interface objects>-----------------------------------------------------------------------------
void DependencyGraph::GenerateGraph()
{
	for(size_t i = 0; i < dependences.size(); ++i)
	{
		//create vertex for a dependency object
		zqyG::Vertex<std::string,std::string> dv;
		dv.Value() = dependences[i]->name();

		//find edges to its depended objects
		for(size_t j = 0; j < dependences.size(); ++j){
			if(i != j)
			{
				if(dependences[i]->hasDependencyWith(dependences[j]->DependencyName()))
				{
					dv.addEdge(j,dependences[i]->GetEdgeValue(dependences[j]->DependencyName()));
				}
			}
		}
		pgraph->addVertex(dv);
	}
}

////----<add a dependency object to the dependency relationship collection>--------------------------------------------------------------
void DependencyGraph::AddDependence(IDependency<std::string>* dItem)
{
	dependences.push_back(dItem);
}

////----<create a FileDependency object from a specified file and analyzing the dependencies then add to the dependencies collection>---
void FileDependencyGraph::AddFile(const std::string& filepath)
{
	if(filepath.empty())
		return;
	FileDependency* pfile = new FileDependency(filepath);
	pfile->AnalyzeDependencies();
	AddDependence(pfile);
}

////----<return the condensed graph object of the dependency graph>------------------------------------------------------------------------
zqyG::CondensedGraph<std::string,std::string>* FileDependencyGraph::GetCondensedGraph()
{
	pcg->CreateCondensed();
	return pcg;
}

////----<deconstruct the class and delete all FileDependency objects>-----------------------------------------------------------------------
PackageDependencyGraph::~PackageDependencyGraph()
{
	for(size_t i = 0; i < dependences.size(); ++i)
	{
		delete dynamic_cast<PackageDependency*>(dependences[i])->getheadfile();
		delete dynamic_cast<PackageDependency*>(dependences[i])->getsourcefile();
	}
}

////----<add package files and create package dependency objects and then add it to dependency collection>-----------------------------------
void PackageDependencyGraph::AddFile(const std::string &headerfilepath, const std::string &sourcefilepath)
{
	if(headerfilepath.empty())
		return;
	FileDependency* hf = new FileDependency(headerfilepath);
	FileDependency* sf = 0;
	hf->AnalyzeDependencies();
	if(sourcefilepath != "")
	{
		sf = new FileDependency(sourcefilepath);
		sf->AnalyzeDependencies();
	}
	AddDependence(new PackageDependency(hf,sf));
}


////----<return the condensed graph object of the dependency graph>----------------------------------------------------------------------------
zqyG::CondensedGraph<std::string,std::string>* PackageDependencyGraph::GetCondensedGraph()
{
	pcg->CreateCondensed();
	return pcg;
}

////----<test stub>------------------------------------------------------------------------------------------
#ifdef TEST_FILEDEPENDENCYGRAPH
void showGraph(zqyG::Graph<std::string, std::string>& fgraph)
{
	for(size_t i = 0; i < fgraph.size(); ++i)
	{
		std::cout<<"\nVertex"<<i<<": "<<fgraph[i].Value();
		std::cout<<"\n Children: ";
		for(size_t j = 0; j < fgraph[i].size(); ++j)
			std::cout<<"( "<<fgraph[i][j].first<<", "<<fgraph[i][j].second<<" ) ";
		std::cout<<"\n--------------";
	}
}

void main()
{
	FileDependencyGraph fdg;
	fdg.AddFile("..\\Graphs\\GraphFromXML.h");
	fdg.AddFile("..\\Graphs\\GraphFromXML.cpp");
	fdg.AddFile("..\\Graphs\\CondensedGraph.h");
	fdg.AddFile("..\\Graphs\\CondensedGraph.cpp");
	fdg.AddFile("..\\Graphs\\Vertex.h");
	fdg.AddFile("..\\Graphs\\Vertex.cpp");
	fdg.AddFile("..\\Graphs\\Graph.h");
	fdg.AddFile("..\\Graphs\\Graph.cpp");
	fdg.AddFile("..\\Graphs\\FileDependency.h");
	fdg.AddFile("..\\Graphs\\FileDependency.cpp");
	fdg.AddFile("..\\Graphs\\FileDependencyGraph.h");
	fdg.AddFile("..\\Graphs\\FileDependencyGraph.cpp");
	
	fdg.GenerateGraph();
	zqyG::CondensedGraph<std::string,std::string>* cdg;
	cdg = fdg.GetCondensedGraph();
	Graph<std::string,std::string>& fgraph = cdg->GetAttachedGraph();
	showGraph(fgraph);

	PackageDependencyGraph pdg;
	pdg.AddFile("..\\Graphs\\GraphFromXML.h","..\\Graphs\\GraphFromXML.cpp");
	pdg.AddFile("..\\Graphs\\CondensedGraph.h","..\\Graphs\\CondensedGraph.cpp");
	pdg.AddFile("..\\Graphs\\Vertex.h","..\\Graphs\\Vertex.cpp");
	pdg.AddFile("..\\Graphs\\Graph.h","..\\Graphs\\Graph.cpp");
	pdg.AddFile("..\\Graphs\\FileDependency.h","..\\Graphs\\FileDependency.cpp");
	pdg.AddFile("..\\Graphs\\FileDependencyGraph.h","..\\Graphs\\FileDependencyGraph.cpp");

	pdg.GenerateGraph();
	cdg = pdg.GetCondensedGraph();
	Graph<std::string,std::string>& fpg = cdg->GetAttachedGraph();
	std::cout <<"\n\n\n";
	showGraph(fpg);
	::system("pause");
}

#endif