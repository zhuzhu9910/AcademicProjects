#ifndef GRAPHEXECUTIVE_H
#define GRAPHEXECUTIVE_H
/////////////////////////////////////////////////////////////////////
//  GraphsExecutive.h - composed all function class and input xml  //
//  file to generate graphs according to it, input file packages to//
//  generate dependency graphs and input command line arguments to // 
//  generate relating graphs according to the command.             //
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
  Provides a class to build graphs from xml files or analyzing file
  dependencies and generate dependency graphs. It provides the ability
  of inputing arguments from command line and build specified graphs
  from the command argruments.For exmaple, "-xml" and following the file
  path for building graphs from xml or "-file" and following directory paths.
  
  Public Interface:
  =================
  GraphsExecutive ge;                                                                    //create a GraphsExecutive object
  bool succ = ge.InputFromCmdL(argc,argv);                                               //get command from command line arguments
  ge.GenerateGraphs();                                                                   //generate graphs
  zqyG::CondensedGraph<std::string,std::string>* cdgp = ge.getCgraphofPackDependence();  //get condensed graph of package dependency graph
  zqyG::CondensedGraph<std::string,std::string>* cdgf = ge.getCgraphofFileDependence();  //get condensed graph of file dependency graph

  XMLGraphs* cdgxml = ge.GetXMLGraphs();                                                 //get graphs generated according to xml
  ge.InputFromXML(some xml file);                                                        //input xml file to generate graphs
  ge.AddFilePackage(head file,source file);                                              //input file packages to generate dependency graphs
  ge.GenerateGraphs();                                                                   //generate graphs

  Build Process:
  ==============
  Required files
  - Vertex.cpp Vertex.cpp Graph.h Graph.cpp CondensedGraph.h CondensedGraph.cpp
  FileDependency.h FileDependency.cpp FileDependencyGraph.h FileDependencyGraph.cpp 
  GraphFromXML.h GraphFromXML.cpp GraphsExecutive.h GraphsExecutive.cpp
  Build commands
    - devenv Project2.sln
  
  Maintenance History:
  ====================
  ver 1.0 : 14 Mar 10
  - first release
  ver 1.1 : 16 Mar 10
  - modified to meet the last requirement
 */


#include "FileDependencyGraph.h"
#include "CondensedGraph.h"
#include "GraphFromXML.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////class of generating graphs according xml and dependency graphs
class GraphsExecutive
{
public:
	GraphsExecutive();
	~GraphsExecutive();
	bool InputFromXML(const std::string& xmlfile);
	void GenerateGraphs();
	void AddFilePackage(const std::string& head,const std::string& source = "");
	XMLGraphs* GetXMLGraphs();
	zqyG::CondensedGraph<std::string , std::string>* getCgraphofFileDependence();
	zqyG::CondensedGraph<std::string , std::string>* getCgraphofPackDependence();
	bool InputFromCmdL(int argc, char* argv[]);
private:
	GraphsExecutive(const GraphsExecutive& ge);
	GraphsExecutive& operator = (const GraphsExecutive& ge);
	bool AyaCmldForFileDep(int argc, char* argv[]);
	std::vector<std::string> mergeFiles(const std::vector<std::string>& f1, const std::vector<std::string>& f2);
	XMLGraphs* pxmlgraphs;
	FileDependencyGraph* pfdepgraph;
	PackageDependencyGraph* ppdepgraph;
};

////----<void constructor of GraphsExecutive class>---------------------------------------------------------------------------------------
inline GraphsExecutive::GraphsExecutive() :pxmlgraphs(0),pfdepgraph(0),ppdepgraph(0){}





#endif