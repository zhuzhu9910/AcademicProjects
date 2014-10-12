#ifndef FILEDEPENDENCYGRAPH_H
#define FILEDEPENDENCYGRAPH_H
/////////////////////////////////////////////////////////////////////
//  FileDependencyGraph.h - generate graphs to demonstrate file    //
//                     dependency relationships                    //
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
  Provides a DenpendencyGraph class as a base class to provide functions
  of create dependency graph from the objects using interface IDependency.
  FileDependencyGraph class provides operation to accept files and
  create FileDependency objects for analyzing and create dependency graphs. 
  PackageDependencyGraph class provides operations to accept files to 
  analyze package dependencies and create dependency graphs.
  
  Public Interface:
  =================
  FileDependencyGraph fdg;                                                //create a FileDependencyGraph object
  fdg.AddFile(some file);                                                 //add a file to generate dependency graph
  fdg.GenerateGraph();                                                    //generate dependency graph
  CondensedGraph<std::string,std::string>* cdg = fdg.GetCondensedGraph(); //get the condensed graph object of the denpendency graph
  Graph<std::string,std::string>& fdgraph = cdg->GetAttachedGraph();      //get the graph of the dependency graph

  PackageDependencyGraph pdg;                                             //create a PackageDependencyGraph object
  pdg.AddFile(head file,source file);                                     //add package files to analyze
  pdg.GenerateGraph();                                                    //generate dependency graph
  cdg = pdg.GetCondensedGraph();                                          //get the condensed graph object of the denpendency graph
  Graph<std::string,std::string>& fpg = cdg->GetAttachedGraph();          //get the graph of the dependency graph


  Build Process:
  ==============
  cl /D:TEST_FILEDEPENDENCYGRAPH FileDependency.h FileDependency.cpp FileDependencyGraph.h FileDependencyGraph.cpp 
  
  Maintenance History:
  ====================
  ver 1.0 : 14 Mar 10
  - first release
  ver 1.1 : 16 Mar 10
  - modified to meet the last requirement
 */

#include "FileDependency.h"
#include "CondensedGraph.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////base class for generating dependency graphs
class DependencyGraph
{
public:
	DependencyGraph();
	~DependencyGraph();
	void AddDependence(IDependency<std::string>* dItem);
	void GenerateGraph();
protected:
	zqyG::Graph<std::string,std::string>* pgraph;
	zqyG::CondensedGraph<std::string,std::string>* pcg;
	std::vector<IDependency<std::string>*> dependences;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////class for generating file dependency graph
class FileDependencyGraph : public zqyG::IGraphBuilder<std::string, std::string>, public DependencyGraph
{
public:
	void AddFile(const std::string& filepath);
	FileDependencyGraph();
	zqyG::CondensedGraph<std::string,std::string>* GetCondensedGraph();
	
};

////----<void constructor of FileDpendencyGraph class>------------------------------------------------------------------------------
inline FileDependencyGraph::FileDependencyGraph() :DependencyGraph() {}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////class for generating package dependency graph
class PackageDependencyGraph : public zqyG::IGraphBuilder<std::string, std::string>, public DependencyGraph
{
public:
	void AddFile(const std::string& headerfilepath, const std::string& sourcefilepath = "");
	PackageDependencyGraph();
	~PackageDependencyGraph();
	zqyG::CondensedGraph<std::string,std::string>* GetCondensedGraph();
};

////----<void constructor of PackageDependencyGraph class>---------------------------------------------------------------------------
inline PackageDependencyGraph::PackageDependencyGraph() :DependencyGraph(){}


#endif