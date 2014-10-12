/////////////////////////////////////////////////////////////////////
//  GraphFromXML.cpp - read information for creating graph from xml//
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

#include "GraphFromXML.h"

using namespace zqyG;

////----<void constructor, building the xmldocument and some basic utilities>------------------------------------------------------
XMLGraphs::XMLGraphs()
{
	xmldocexe.Build();
}

////----<read and parse the xml file and create graphs according to it>------------------------------------------------------------
void XMLGraphs::BuildGraphs()
{
	//create variables which need to be shared between the to functor
	zqyG::Vertex<std::string, std::string> vSS;
	zqyG::Vertex<std::string, double> vSD;
	zqyG::Vertex<double, std::string> vDS;
	zqyG::Vertex<double, double> vDD;
	GraphGeneratorData::GraphType graphtype;
	//create graph building functor
	PreGraphGenerator preopt(&SSGraphs,&SDGraphs,&DSGraphs,&DDGraphs,vSS,vSD,vDS,vDD,graphtype);
	PostGraphGenerator postopt(&SSGraphs,&SDGraphs,&DSGraphs,&DDGraphs,vSS,vSD,vDS,vDD,graphtype);
	//parse the xml file and get the parse tree
	xmldocexe.Parse();
	XMLTree* ptree = xmldocexe.GetDocument().GetTree();
	//DFS the parse tree using the functors to build the graphs
	ptree->setFrontOperation(&preopt);
	ptree->setPostOperation(&postopt);
	ptree->walk();
	ptree->setFrontOperation(0);
	ptree->setPostOperation(0);
}

////----<attach a xml file to the class>------------------------------------------------------------------------------------------
bool XMLGraphs::AttachXMLFile(std::string filename) 
{
	return xmldocexe.GetDocument().read(filename);
}

////----<access the collection of <string string> type graphs>---------------------------------------------------------------------
std::vector<GraphFromXML<std::string, std::string>>& XMLGraphs::getSringSGraphs()
{
	return SSGraphs;
}

////----<access the collection of <string double> type graphs>---------------------------------------------------------------------
std::vector<GraphFromXML<std::string, double>>& XMLGraphs::getStringDGraphs()
{
	return SDGraphs;
}

////----<access the collection of <double string> type graphs>---------------------------------------------------------------------
std::vector<GraphFromXML<double, std::string>>& XMLGraphs::getDoulbeSGraphs()
{
	return DSGraphs;
}

////----<access the collection of <double double> type graphs>---------------------------------------------------------------------
std::vector<GraphFromXML<double,double>>& XMLGraphs::getDoubleDGraphs()
{
	return DDGraphs;
}

////----<pre-operation to build different parts of a graph with the DFS>------------------------------------------------------------
void PreGraphGenerator::operator()(PNode<XMLValue>* pNode)
{
	std::string tagname = pNode->value().TagName();
	if(pNode->value().Type() == XMLValue::TAG)
	{
		if("graphs" == tagname)
		{
			return;
		}
		if("graph" == tagname)
		{
			BuildGraph(pNode->value());
			return;
		}
		if( "v" == tagname)
		{
			BuildVertex(pNode->value());
			return;
		}
		if( "e" == tagname)
		{
			BuildEge(pNode);
			return;
		}
	}
}

////----<a helper function to get the attribute value of the node according to the attribute name>----------------------------------
std::string PreGraphGenerator::getAttributeValue(const std::string name, XMLValue& value)
{
	for (size_t i = 0; i < value.attributes().size(); ++i)
		if((value.attributes())[i].AttrName == name)
		{
			std::string rst = (value.attributes())[i].AttrValue;
			if(rst.size() > 1 && rst.at(0) == '\"' && rst.at(rst.size() - 1) == '\"')
			{
				rst.erase(rst.begin());
				rst.erase(rst.end() - 1);
			}
			return rst;
		}
	return "";
}

////----<Create graphs according to the xml file>-----------------------------------------------------------------------------------
////according to the template type of current building graph
////, create different type of graphs and put it into the result collection
void PreGraphGenerator::BuildGraph(XMLValue& value)
{
	std::string v = getAttributeValue("vt",value);
	std::string e = getAttributeValue("et",value);
	if (v == "string" && e == "string")
	{
		this->gtype = SS;
		SSg->push_back(GraphFromXML<std::string,std::string>());
	}

	if (v == "string" && e == "double")
	{
		this->gtype = SD;
		SDg->push_back(GraphFromXML<std::string,double>());
	}

	if (v == "double" && e == "string")
	{
		this->gtype = DS;
		DSg->push_back(GraphFromXML<double, std::string>());
	}
	if (v == "double" && e == "double")
	{
		this->gtype =DD;
		DDg->push_back(GraphFromXML<double,double>());
	}
}

////----<create vertexes according to the xml and temporarily keep them into relating variables>------------------------------------------------------------------------------------------
void PreGraphGenerator::BuildVertex(XMLValue& value)
{
	std::string vtv = getAttributeValue("vtv",value);
	std::istringstream	vtvs(vtv);
	double vvalue;
	switch(this->gtype)
	{
	case SS:
		curVSS = zqyG::Vertex<std::string, std::string>(vtv);
		break;
	case SD:
		curVSD = zqyG::Vertex<std::string, double>(vtv);
		break;
	case DS:
		vtvs>>vvalue;
		curVDS = zqyG::Vertex<double, std::string>(vvalue);
		break;
	case DD:
		vtvs>>vvalue;
		curVDD = zqyG::Vertex<double,double>(vvalue);
		break;
	}
}

////----<create and add edge to current building vertex>------------------------------------------------------------------------------------------
void PreGraphGenerator::BuildEge(PNode<XMLValue>* pNode)
{
	if(!pNode->nextUnmarkedChild())
		return;
	//get the edge referrece and value, convert the referrence to right type
	std::string etv = getAttributeValue("etv",pNode->value());
	std::istringstream reffs(pNode->nextUnmarkedChild()->value().TagName());
	std::istringstream evalues(etv);
	double evalue;
	size_t reff;
	reffs>>reff;
	--reff;
	//add edge to current building vertex
	switch(this->gtype)
	{
	case SS:
		curVSS.addEdge(reff,etv);
		break;
	case SD:
		evalues>>evalue;
		curVSD.addEdge(reff,evalue);
		break;
	case DS:
		curVDS.addEdge(reff,etv);
		break;
	case DD:
		evalues>>evalue;
		curVDD.addEdge(reff,evalue);
		break;
	}
}


////----<test stub>------------------------------------------------------------------------------------------
#ifdef TEST_GRAPHFROMXML

void main()
{
	XMLGraphs xmlgraphs;
	xmlgraphs.AttachXMLFile("..\\graphs.xml");
	xmlgraphs.BuildGraphs();
	zqyG::CondensedGraph<std::string,std::string>* cdsg = xmlgraphs.getSringSGraphs()[0].GetCondensedGraph();
	xmlgraphs.getDoubleDGraphs()[0];
	xmlgraphs.getDoulbeSGraphs();
	xmlgraphs.getStringDGraphs();
	cdsg->CreateCondensed();
	std::ostringstream output;
	for(size_t i = 0; i < cdsg->GetCondensedGraph().size(); ++i)
	{
		output<<"(";
		for(size_t j = 0; j < cdsg->GetCondensedGraph()[i].Value().size(); ++j)
			output<<cdsg->GetCondensedGraph()[i].Value()[j]<<" ";
		output<<"; ";
		for(size_t j = 0; j < cdsg->GetCondensedGraph()[i].size(); ++j)
			output<<"->"<<cdsg->GetCondensedGraph()[i][j].first<<" ";
		output<<") ";
	}
	std::cout<<output.str();
	std::vector<Vertex<std::vector<size_t>,size_t>> sort = cdsg->TopSortByIndgree();

}

#endif