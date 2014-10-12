#ifndef GRAPHFROMXML_H
#define GRAPHFROMXML_H
/////////////////////////////////////////////////////////////////////
//  GraphFromXML.h - read information for creating graph from xml  //
//                and generate graphs according to it              //
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
  Provides a graph class for graphs from xml which is inherited from graph
  and has the ability to create a relative condensed graph.Provides functors
  to create graphs according to the parse tree from the xml document. It also
  provide a class to create the four-form graphs from xml document and keep them.
  
  Public Interface:
  =================
  XMLGraphs xmlgraphs;                                                            //create a class for creating graphs from xml
  xmlgraphs.AttachXMLFile("..\\graphs.xml");                                      //attach a xml file
  xmlgraphs.BuildGraphs();                                                        //parse the xml file and build graphs according to it
  GraphFromXML<std::string,std::string> gfx = xmlgraphs.getSringSGraphs()[0];     //get a <string string> type graphs from the created graphs
  zqyG::CondensedGraph<std::string,std::string>* cdsg = gfx.GetCondensedGraph();  //get condensed graph from the graph

  Build Process:
  ==============
  cl /D:TEST_GRAPHFROMXML Vertex.cpp Vertex.cpp Graph.h Graph.cpp CondensedGraph.h CondensedGraph.cpp 
  \GraphFromXML.h GraphFromXML.cpp /link Execute.lib
  
  Maintenance History:
  ====================
  ver 1.0 : 14 Mar 10
  - first release
 */


#include "CondensedGraph.h"
#include "XMLDocExecute.h"
#include <sstream>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
////class of graph building from xml
template<typename VT,typename ET>
class GraphFromXML :public zqyG::IGraphBuilder<VT,ET>, public zqyG::Graph<VT,ET>
{
public:
////----<void constructor of the class with creating a condensed graph creator in this class>--------------------------------------
	GraphFromXML()
	{
		cgph = new zqyG::CondensedGraph<VT,ET>(this);
	}
////----<copy constructor of this class>------------------------------------------------------------------------------------------
	GraphFromXML(const GraphFromXML& gfx): zqyG::Graph<VT,ET>(gfx)
	{
		if (gfx.cgph != 0)
		{
			cgph = new zqyG::CondensedGraph<VT,ET>(this);
		}
		else
			cgph = 0;
	}
////----<assignment operator of this class>------------------------------------------------------------------------------------------
	GraphFromXML<VT,ET>& operator = (const GraphFromXML<VT,ET>& gfx)
	{
		if(this == &gfx)
			return *this;
		zqyG::Graph<VT,ET>::operator =(gfx);
		if (gfx.cgph != 0)
		{
			cgph = new zqyG::CondensedGraph<VT,ET>(this);
		}
		else
			cgph = 0;
		return *this;

	}
////----<implement get a condensed graph creator function of the interface for this class>---------------------------------------------
	zqyG::CondensedGraph<VT,ET>* GetCondensedGraph()
	{
		cgph->CreateCondensed();
		return cgph;
	}
	~GraphFromXML()
	{
		delete cgph;
	}
private:
	zqyG::CondensedGraph<VT,ET>* cgph;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////class for keeping data of graph generator from xml
class GraphGeneratorData
{
public:
	enum GraphType {SS,DD,SD,DS};
////----<constructor to input the data utility referrence and pointers>--------------------------------------------------------------------
	GraphGeneratorData(std::vector<GraphFromXML<std::string, std::string>>* ssg,
		std::vector<GraphFromXML<std::string, double>>* sdg,
		std::vector<GraphFromXML<double, std::string>>* dsg,
		std::vector<GraphFromXML<double,double>>* ddg,
		zqyG::Vertex<std::string, std::string>& cvss,
		zqyG::Vertex<std::string, double>& cvsd,
		zqyG::Vertex<double, std::string>& cvds,
		zqyG::Vertex<double, double>& cvdd,
		GraphType& graphtype)
		: SSg(ssg),SDg(sdg),DSg(dsg),DDg(ddg),
		curVSS(cvss), curVSD(cvsd), curVDS(cvds), curVDD(cvdd),gtype(graphtype){}
protected:
	//vectors of the generated graphs
	std::vector<GraphFromXML<std::string, std::string>>* SSg;
	std::vector<GraphFromXML<std::string, double>>* SDg;
	std::vector<GraphFromXML<double, std::string>>* DSg;
	std::vector<GraphFromXML<double,double>>* DDg;
	//current analyzing and generating vertex
	zqyG::Vertex<std::string, std::string> &curVSS;
	zqyG::Vertex<std::string, double> &curVSD;
	zqyG::Vertex<double, std::string> &curVDS;
	zqyG::Vertex<double, double> &curVDD;
	//template type of current generating graph 
	GraphType& gtype;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////pre-operation when DFS the parse tree to generate the graph
class PreGraphGenerator : public Operation,public GraphGeneratorData
{
public:
////----<constructor to input the data>------------------------------------------------------------------------------------------
	PreGraphGenerator(std::vector<GraphFromXML<std::string, std::string>>* _ssg,
		std::vector<GraphFromXML<std::string, double>>* _sdg,
		std::vector<GraphFromXML<double, std::string>>* _dsg,
		std::vector<GraphFromXML<double,double>>* _ddg,
		zqyG::Vertex<std::string, std::string>& _cvss,
		zqyG::Vertex<std::string, double>& _cvsd,
		zqyG::Vertex<double, std::string>& _cvds,
		zqyG::Vertex<double, double>& _cvdd,
		GraphType& _graphtype)
		: GraphGeneratorData(_ssg,_sdg,_dsg,_ddg,_cvss,_cvsd,_cvds,_cvdd,_graphtype){}
	void operator()(PNode<XMLValue>* pNode);
private:
	std::string getAttributeValue(const std::string name, XMLValue& value);
	void BuildGraph(XMLValue& value);
	void BuildVertex(XMLValue& value);
	void BuildEge(PNode<XMLValue>* pNode);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////post-operation when DFS the parse tree to generate the graph
class PostGraphGenerator : public Operation,public GraphGeneratorData
{
public:
////----<constructor to input the data>------------------------------------------------------------------------------------------
	PostGraphGenerator(std::vector<GraphFromXML<std::string, std::string>>* _ssg,
		std::vector<GraphFromXML<std::string, double>>* _sdg,
		std::vector<GraphFromXML<double, std::string>>* _dsg,
		std::vector<GraphFromXML<double,double>>* _ddg,
		zqyG::Vertex<std::string, std::string>& _cvss,
		zqyG::Vertex<std::string, double>& _cvsd,
		zqyG::Vertex<double, std::string>& _cvds,
		zqyG::Vertex<double, double>& _cvdd,
		GraphType& _graphtype)
		: GraphGeneratorData(_ssg,_sdg,_dsg,_ddg,_cvss,_cvsd,_cvds,_cvdd,_graphtype){}
	void operator()(PNode<XMLValue>* pNode);
private:
	std::string getAttributeValue(const std::string name, XMLValue& value);
	void BuildVertex();
};

////----<post-operation to find if it is leaving a "v" tag xml element and build the vertex for graph>-------------------------------------
////when we leave the v nodes it means it finished the vertex creating and need to put the vertex into the graph
inline void PostGraphGenerator::operator()(PNode<XMLValue>* pNode)
{
	if(pNode->value().Type() == XMLValue::TAG && "v" == pNode->value().TagName())
	{

		BuildVertex();
	}
}

////----<according to current building graph type to put created vertex to current building graph>------------------------------------------
inline void PostGraphGenerator::BuildVertex()
{
	switch(this->gtype)
	{
	case SS:
		SSg->back().addVertex(curVSS);
		break;
	case SD:
		SDg->back().addVertex(curVSD);
		break;
	case DS:
		DSg->back().addVertex(curVDS);
		break;
	case DD:
		DDg->back().addVertex(curVDD);
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////class for creating graphs from xml file
class XMLGraphs
{
public:
	XMLGraphs();
	bool AttachXMLFile(std::string filename);
	void BuildGraphs();
	std::vector<GraphFromXML<std::string, std::string>>& getSringSGraphs();
	std::vector<GraphFromXML<std::string, double>>& getStringDGraphs();
	std::vector<GraphFromXML<double, std::string>>& getDoulbeSGraphs();
	std::vector<GraphFromXML<double,double>>& getDoubleDGraphs();
private:
	std::vector<GraphFromXML<std::string, std::string>> SSGraphs;
	std::vector<GraphFromXML<std::string, double>> SDGraphs;
	std::vector<GraphFromXML<double, std::string>> DSGraphs;
	std::vector<GraphFromXML<double,double>> DDGraphs;
	XMLDocExecute xmldocexe;
};


#endif