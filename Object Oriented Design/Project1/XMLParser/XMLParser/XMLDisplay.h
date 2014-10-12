#ifndef XMLDISPLAY_H
#define XMLDISPLAY_H
/////////////////////////////////////////////////////////////////////
//  XMLDisplay.h - Output parse tree and rebuilt XML for display   //
//			     - or the parse tree to file or string             //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project1 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module defines the class for displaying the parse tree in two
  ways: one is directly display the content of the nodes in the tree
  in a DFS order or rebuild the xml document from the parse tree and
  display it. This module provides the funcionality to display the 
  result of parsing to a string or file and rebuild the xml document
  from the parse tree.

  Public Interface:
  =================
  XMLDisplay disply(config.GetTree());     //create a new display object with a parse tree
  disply.rebuildXML();                     //rebuild the xml document from the parse tree
  disply.buildTreeDisplay();               //generate the output string for displaying the parse tree
  disply.AttachFile(somefile);             //attach an output file
  disply.OutputTree();                     //output the parse tree
  disply.OutputXML();                      //output the rebuilt xml document

  Build Process:
  ==============
  Required files
    - XMLDisplay.h, XMLDisplay.cpp, XMLConfigurParser.h, XMLConfigurParser.cpp,
	Parser.h, Parser.cpp, XMLActionAndRules.h, XMLActionAndRules.cpp,
    xmlElementParts.h, xmlElementParts.cpp, tokenizer.h, tokenizer.cpp
  Build commands (either one)
    - devenv XMLParser.sln
    - cl /EHsc /DTEST_PARSER XMLDisplay.h XMLDisplay.cpp\
		 XMLConfigurParser.cpp parser.cpp \
         XMLActionAndRules.cpp \
         xmlElementParts.cpp tokenizer.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 1.0 : 05 Feb 10
  - first release

*/
//

#include "Tree.h"
#include <string>
#include <iostream>
////////////////////////////////////////////////////////////////////////////////
////Operation class for front operation of rebuilding xml document
class RebuitXMLFrontOper : public Operation
{
public:
	//--<construct the class with a string to store the output string>--
	RebuitXMLFrontOper(std::string& xmlstr) :newxml(xmlstr){};

	//--<the () operator for reconstructing the start element>--
	void operator()(PNode<XMLValue>* pNode)
	{
		//reconstruct the element according its type
		switch(pNode->value().Type())
		{
		case XMLValue::TAG:
			newxml.append("<").append(pNode->value().TagName());
			rebuildAttr(pNode);
			newxml.append(">\n");
			break;
		case XMLValue::COMMENT:
			newxml.append("<!--").append(pNode->value().TagName());
			newxml.append("-->\n");
			break;
		case XMLValue::SELF_CLOSE_TAG:
			newxml.append("<").append(pNode->value().TagName());
			rebuildAttr(pNode);
			newxml.append("/>\n");
			break;
		case XMLValue::TEXT:
			newxml.append(pNode->value().TagName()).append("\n");
			break;
		case XMLValue::PROC_INSTR:
			newxml.append("<?").append(pNode->value().TagName());
			rebuildAttr(pNode);
			newxml.append("?>\n");
			break;
		}
	}

	//--<set the output string>--
	void setOutputStr(std::string& xmlstr)
	{
		newxml = xmlstr;
	}
private:
	//--<represent the attribute of an element in the form of Attribute Name = Attribute Value>--
	void rebuildAttr(PNode<XMLValue>* pNode)
	{
		for(size_t i = 0; i<pNode->value().attributes().size(); ++i)
		{
			newxml.append(" ");
			newxml.append(pNode->value().attributes()[i].AttrName);
			newxml.append("=");
			newxml.append(pNode->value().attributes()[i].AttrValue);
		}
	}
	std::string& newxml;
};

////////////////////////////////////////////////////////////////////////////////
////Operation class for post-operation of rebuilding xml document
class RebuitXMLPostOper : public Operation
{
public:
	//--<construct the class with a string to store the output string>--
	RebuitXMLPostOper(std::string& xmlstr) :newxml(xmlstr){};

	//--<the () operator for reconstructing the close element>--
	void operator()(PNode<XMLValue>* pNode)
	{
		if(XMLValue::TAG == pNode->value().Type())
		{
			newxml.append("</").append(pNode->value().TagName());
			newxml.append(">\n");
		}
	}

	//--<set the output string>--
	void setOutputStr(std::string& xmlstr)
	{
		newxml = xmlstr;
	}
private:
	std::string& newxml;
};

////////////////////////////////////////////////////////////////////////////////
////Operation class for output a tree, represent the tree into a string
class OutputTreeOper : public Operation
{
public:
	//--<the () operator for arranging and storing the node information into the string>--
	void operator()(PNode<XMLValue>* pNode)
	{
		std::string type = GetType(pNode->value().Type());
		//append the element name, node type to one string for displaying
		if (XMLValue::TAG == pNode->value().Type())
			ostr.append("Node Name: ").append("<").append(pNode->value().TagName()).append(">");
		else
			ostr.append("Node Name: ").append(pNode->value().TagName());
			ostr.append(", Node Type: ").append(type.c_str()).append("\n");
		//append information of attributes into this display string
		if(!pNode->value().attributes().empty())
		{
			ostr.append("Attributes: ");
			for(size_t i = 0; i < pNode->value().attributes().size(); ++i)
				ostr.append(pNode->value().attributes()[i].AttrName).append(" = ").append(pNode->value().attributes()[i].AttrValue).append(", ");
			ostr.append("\n");
		}
		
		//append the child nodes into the display string
		if(!pNode->getChildren().empty())
		{
			ostr.append("Child nodes: ");
			std::vector<XMLValue> mychildren = pNode->getChildren();
			for(size_t i = 0; i < mychildren.size(); ++i)
			{
				//if a child node is a xml element node with tag then display <tag name> else display its node type
				if(XMLValue::TAG == mychildren[i].Type())
					ostr.append("<").append(mychildren[i].TagName()).append(">, ");
				else
					ostr.append(GetType(mychildren[i].Type())).append(", ");
			}
			ostr.append("\n");
		}
		ostr.append("---------------------\n");
	}
	//--<get the output string>--
	std::string getOutput()
	{
		return ostr;
	}
	//--<turn the type of the node to a string>--
	std::string GetType(XMLValue::NodeType type)
	{
		switch(type)
		{
		case XMLValue::COMMENT:
			return "Comment";
		case XMLValue::TAG:
			return "Element";
		case XMLValue::TEXT:
			return "Text";
		case XMLValue::PROC_INSTR:
			return "Process Instruct";
		case XMLValue::SELF_CLOSE_TAG:
			return "Element";
		default:
			return "Comment";
		}
	}
private:
	std::string ostr;
};


////////////////////////////////////////////////////////////////////////////////
////Class for xml displaying
class XMLDisplay
{
public:
	XMLDisplay(XMLTree* ptree);
	~XMLDisplay();
	bool AttachFile(const std::string& name);
	void rebuildXML();
	void buildTreeDisplay();
	std::string OutputTree();
	std::string OutputXML();
private:
	XMLTree* pTree;
	std::ostream* pOut;
	std::string xmlstr;
	std::string treestr;
};













#endif