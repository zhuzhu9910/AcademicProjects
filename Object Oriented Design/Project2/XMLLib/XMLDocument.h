#ifndef XMLDOCUMENT_H
#define XMLDOCUMENT_H
/////////////////////////////////////////////////////////////////////
//  XMLDocument.h - The class holds the tree and manange the       //
//                  document's output and input.                   //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2005                                 //
//  Platform:      Lenovo ThinkPad T400, Windows 7 Professional    //
//  Application:   Project 1 of CSE687                             //
//  Author:        Qinyun Zhu, Syracuse University                 //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module holds the tree and is responsible for building and destroying
  it. This module also has the operations to load a string or file to a toker
  and output the tree.

  Public Interface:
  =================
  Toker toker;                                      //initialize toker   
  toker.setMode(Toker::xml);                        //XML parser mode
  XmlParts Semi(pToker);                            //initialize xml parts
  XMLDocument doc;                                  //initialize xml document
  Parser pParser(Semi);                             //initialize parser
  doc.SetToker(toker);                              //attach toker to the document
  doc.read(some file);                              //set the file or string to load into the toker
  doc.write(some file);                             //set the file or string for outputing
  while(pParser->next())                            //parse the file
     pParser->parse();
  doc.DisplayTree();                                //display the tree structure
  doc.DisplayXML();
  XMLElement elmbyid = doc.GetElementbyId();        //find the xml element by id
  doc.RemoveElement(elmbyid);                       //remove the specified element


  Build Process:
  ==============
  Required files
    - Tree.h, Tree.cpp, Nodes.h, Nodes.cpp,
      XMLDocument.h, XMLDocument.cpp,
      xmlElementParts.h, xmlElementParts.cpp, tokenizer.h, tokenizer.cpp
  Build commands (either one)
    - devenv XMLParser.sln
    - cl /EHsc /DTEST_PARSER XMLDocument.cpp XMLDocument.cpp \
         Tree.cpp Nodes.cpp \
         xmlElementParts.cpp tokenizer.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 1.0 : 12 Feb 10
  - first release

*/

//
#include "Tree.h"
#include "xmlElementParts.h"
#include "XMLDisplay.h"
#include "Tokenizer.h"
#include <string>
#include <iostream>
#include <stack>

/////////////////////////////////////////////////////////////////////////////////
////Structure of holding the XMLNode pointer,
////show proper contents of node to user
struct XMLElement
{
	friend class XMLDocument;
	XMLElement(XMLNode* pnode = NULL);
	void makeElement(const XMLValue::NodeType tagtype,const std::string tagname);
	bool isempty();
	XMLValue& getValue();
	std::string PrintElement();
private:
	XMLNode* pNode;
};

//----<construct the element with the XMLNode pointer it is assigned to hold >---
inline XMLElement::XMLElement(XMLNode* pnode) :pNode(pnode) {}

//----<get the node value>---
inline XMLValue& XMLElement::getValue()
{
	return pNode->value();
}

//----<make a new node>---
inline void XMLElement::makeElement(const XMLValue::NodeType tagtype,const std::string tagname)
{
	pNode = new PNode<XMLValue>(XMLValue(tagtype, tagname));
}

//----<test if the element is empty>---
inline bool XMLElement::isempty()
{
	if(NULL == pNode)
		return true;
	else
		return false;
}

//----<print the content of the element value to a string>--
inline std::string XMLElement::PrintElement()
{
	std::string sout;
	sout.append("Name: ").append("<").append (this->getValue().TagName()).append(">").append("\nAttributes: ");
	if(!this->getValue().attributes().empty())
	{
		for(size_t i=0; i<this->getValue().attributes().size(); ++i)
			sout.append (this->getValue().attributes()[i].AttrName).append(" = ").append(this->getValue().attributes()[i].AttrValue).append(", ");
	}
	else
		sout.append("NO ATTRIBUTES  ");
	sout.erase(sout.size() - 2,2);
	sout.append("\n");
	return sout;
}

/////////////////////////////////////////////////////////////////////////////////
////Class as XML document
////for managing the tree and specify the input and output
class XMLDocument
{
public:
	XMLDocument();
	~XMLDocument();
	
	bool read (const std::string& name, bool isFile = true);
	bool write (std::string& name, bool isFile = true);
	bool write (const std::string& name, bool isFile = true);
	XMLTree* GetTree();
	void SetToker(Toker* toker);

	void DisplayTree();
	void DisplayXML();
	void Clear();
	XMLElement GetElementbyId(std::string id);
	std::vector<XMLElement> GetElementbyTag(std::string tag);
	XMLElement AddElement(XMLElement elm, XMLElement parelm);
	XMLElement RemoveElement(XMLElement elm);

	XMLDocument(const XMLDocument& xmldoc);
	XMLDocument& operator= (const XMLDocument& xmldoc);
private:
	XMLTree* pTree;
	Toker* pToker;
	XMLDisplay* display;
	std::string* sout;
	void removeTreeNodes();
};

//----<remove all nodes of the tree>---
inline void XMLDocument::removeTreeNodes() 
{
	if(NULL != this->pTree->getroot())
	{
		this->pTree->remove(this->pTree->getroot());                                                      //remove root node
		std::vector< PNode<XMLValue>* > deletednodes = this->pTree->getdeletednodes();                    //remove all other nodes
		for(size_t i = 0; i < deletednodes.size(); ++i)                                                   //delete all removed nodes
			delete deletednodes[i];
	}
}

#endif