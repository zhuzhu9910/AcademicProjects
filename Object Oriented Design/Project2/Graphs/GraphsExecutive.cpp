/////////////////////////////////////////////////////////////////////
//  GraphsExecutive.cpp - composed all function class and input xml//
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

#include "GraphsExecutive.h"
#include "../FileSystemDemo/FileSystem.h"
#include <stack>

////----<deconstruct the class>----------------------------------------------------------------------------------------------------
////delete all objects if it was built
GraphsExecutive::~GraphsExecutive()
{
	if(this->pfdepgraph)
		delete this->pfdepgraph;
	if(this->ppdepgraph)
		delete this->ppdepgraph;
	if(this->pxmlgraphs)
		delete this->pxmlgraphs;
}

////----<attach a xml file to generate graphs>------------------------------------------------------------------------------------------
bool GraphsExecutive::InputFromXML(const std::string &xmlfile)
{
	if(!pxmlgraphs)
		pxmlgraphs = new XMLGraphs;
	if(!pxmlgraphs->AttachXMLFile(xmlfile))
	{
		std::cout<<"\nCan not Open XML File!!!\n";
		delete pxmlgraphs;
		pxmlgraphs = 0;
		return false;
	}
	return true;
}

////----<return the class stored all generated graphs from xml>------------------------------------------------------------------------
XMLGraphs* GraphsExecutive::GetXMLGraphs()
{
	if(!this->pxmlgraphs)
		return 0;
	return this->pxmlgraphs;
}

////----<return condensed graph of the file dependency graph>--------------------------------------------------------------------------
zqyG::CondensedGraph<std::string , std::string>* GraphsExecutive::getCgraphofFileDependence()
{
	if(!this->pfdepgraph)
		return 0;
	return this->pfdepgraph->GetCondensedGraph();
}

////----<return condensed graph of the package dependency graph>------------------------------------------------------------------------
zqyG::CondensedGraph<std::string , std::string>* GraphsExecutive::getCgraphofPackDependence()
{
	if(!this->ppdepgraph)
		return 0;
	return this->ppdepgraph->GetCondensedGraph();
}

////----<any initialized graph generator create graphs>---------------------------------------------------------------------------------
void GraphsExecutive::GenerateGraphs()
{
	if(this->pfdepgraph)
		this->pfdepgraph->GenerateGraph();
	if(this->ppdepgraph)
		this->ppdepgraph->GenerateGraph();
	if(this->pxmlgraphs)
		this->pxmlgraphs->BuildGraphs();
}

////----<create relating objects and add a file package to generate package and file dependency graph>-----------------------------------
void GraphsExecutive::AddFilePackage(const std::string& head,const std::string& source)
{
	if(!pfdepgraph)
		pfdepgraph = new FileDependencyGraph;
	if(!ppdepgraph)
		ppdepgraph = new PackageDependencyGraph;
	pfdepgraph->AddFile(head);
	pfdepgraph->AddFile(source);
	ppdepgraph->AddFile(head,source);
}

//----< merge the contents of two file vectors >-------------------------------------------------------------------------------------------
std::vector<std::string> GraphsExecutive::mergeFiles(const std::vector<std::string>& f1, const std::vector<std::string>& f2)
{
	std::vector<std::string>  sum(f1.begin(),f1.end());
  for(size_t i=0; i<f2.size(); ++i)
    sum.push_back(f2[i]);
  return sum;
}

////----<extract file packages from command line arguments>---------------------------------------------------------------------------------
bool GraphsExecutive::AyaCmldForFileDep(int argc, char *argv[])
{
	if(argc < 3)
		return false;

	//get head files and source files seperately from specified directories
	WinTools_Extracts::FileHandler fh;
	std::vector<std::string> hdrs;
	std::vector<std::string> imps;
	for(int i = 2; i < argc; ++i)
	{
		std::string dir = argv[i];
		std::stack<std::string> sdirs;
		std::vector<std::string> dirs = fh.getSubDirectorySpecs(argv[i]);
		sdirs.push(argv[i]);
		while(!sdirs.empty()){
			dir = sdirs.top();
			sdirs.pop();
			dirs = fh.getSubDirectorySpecs(dir);
		    for(size_t j = 2; j < dirs.size(); ++j)
				sdirs.push(dirs[j]);
			hdrs = mergeFiles(hdrs,fh.getFileSpecs(dir, "*.h"));
			imps = mergeFiles(imps,fh.getFileSpecs(dir, "*.cpp"));
		}
	}

	//recognize file packages and add them into file dependency objects
	for (size_t i = 0; i < hdrs.size(); ++i)
	{
		std::string header = hdrs[i];
		std::string source;
		for(std::vector<std::string>::iterator it = imps.begin(); it < imps.end(); ++it)
		{
			std::string hname = FileDependency::getNameWithoutExp(FileDependency::getNameFromPath(header));
			std::string cppname = FileDependency::getNameWithoutExp(FileDependency::getNameFromPath(*it));
			if(hname == cppname)
			{
				source = *it;
				imps.erase(it);
				break;
			}
		}
		AddFilePackage(header,source);
	}

	return true;
}

////----<extract information from command line arguments>---------------------------------------------------------------------------------
bool GraphsExecutive::InputFromCmdL(int argc, char *argv[])
{
	if(argc < 3)
		return false;
	std::string argv1 = argv[1];
	//"-xml" command means generate graphs according to the following xml file
	if(argv1 == "-xml")
		return InputFromXML(argv[2]);
	//"-file" command means generate dependency graphs for files in following directories
	if(argv1 == "-file")
		return AyaCmldForFileDep(argc,argv);
	return false;
}


////----<test stub>------------------------------------------------------------------------------------------------------------------------
#ifdef TEST_GRAPHSEXECUTIVE

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

int main(int argc, char* argv[])
{
	GraphsExecutive ge;
	if(!ge.InputFromCmdL(argc,argv))
	{
		std::cout<<"input format error!"<<std::endl;
		return 1;
	}
	ge.GenerateGraphs();
	zqyG::CondensedGraph<std::string,std::string>* cdgp = ge.getCgraphofPackDependence();
	if(cdgp)
		showGraph(cdgp->GetAttachedGraph());

	zqyG::CondensedGraph<std::string,std::string>* cdgf = ge.getCgraphofFileDependence();
	if(cdgf)
		showGraph(cdgp->GetAttachedGraph());

	XMLGraphs* cdgxml = ge.GetXMLGraphs();
	GraphsExecutive ge1;
	ge1.InputFromXML("..\\graphs.xml");
	ge1.GenerateGraphs();
	GraphsExecutive ge2;
	ge2.AddFilePackage("..\\Graphs\\GraphFromXML.h","..\\Graphs\\GraphFromXML.h");
	ge2.GenerateGraphs();


	return 1;
}

#endif