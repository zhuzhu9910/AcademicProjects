/////////////////////////////////////////////////////////////////////
//  XMLDocument.cpp - The class holds the tree and manange the     //
//                  document's output and input.                   //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2005                                 //
//  Platform:      Lenovo ThinkPad T400, Windows 7 Professional    //
//  Application:   Project 1 of CSE687                             //
//  Author:        Qinyun Zhu, Syracuse University                 //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////



#include "XMLDocument.h"
#include "Tree.h"
#include "Nodes.h"
#include "xmlElementParts.h"
#include "XMLDisplay.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "XMLActionAndRules.h"
#include "XMLConfigurParser.h"
#include <string>
#include <iostream>

//----<construct the XMLDocument, create an empty inner parse tree and object for displaying>---
XMLDocument::XMLDocument()
{
	try
	  {
		pTree = new XMLTree(new PNode<XMLValue>(XMLValue(XMLValue::TAG,"Document")));
		display = new XMLDisplay(pTree);
		sout = NULL;
	  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
  }
}

//----<deallocate the inner parse tree and the displaying object>---
XMLDocument::~XMLDocument()
{
	delete display;
	removeTreeNodes();
	delete pTree;
}

//----<copy constructor>---
XMLDocument::XMLDocument(const XMLDocument& xmldoc)
{
	try
	{
		this->pTree = new XMLTree(NULL);	
		*(this->pTree) = *(xmldoc.pTree);
		this->pToker = xmldoc.pToker;
		this->display = new XMLDisplay(pTree);
		this->sout = xmldoc.sout; 
	 }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
  }
}

//----<assignment operator, copy the inner tree and the tokenizer pointer>---
XMLDocument& XMLDocument::operator= (const XMLDocument& xmldoc)
{
	try
	{
		this->removeTreeNodes();
		*(this->pTree) = *(xmldoc.pTree);
		delete this->display;
		this->display = new XMLDisplay(pTree);
		this->pToker = xmldoc.pToker;
		this->sout = xmldoc.sout;
	}
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
  }
	return *this;
}

//----<attach the output file or string to the displayer>---
bool XMLDocument::write(std::string& name, bool isFile)
{
	if (isFile)
	{
		sout = NULL;
		return display->AttachFile(name);
	}
	else
		sout = &name;           //attach the output string
	return true;
}

//----<attach the output file to the displayer>---
bool XMLDocument::write(const std::string& name, bool isFile)
{
	if (isFile)
		return display->AttachFile(name);
	return false;
}

//----<attch the input file or string to the tonkenizer>---
bool XMLDocument::read (const std::string& name, bool isFile)
{
	return pToker->attach(name, isFile);
}

//----<return the pointer of the tree>---
XMLTree* XMLDocument::GetTree()
{
	return this->pTree;
}

//----<attach a tokenizer to the xml document>---
void XMLDocument::SetToker(Toker* toker)
{
	pToker = toker;
}

//----<display the tree to the specified file or string>---
void XMLDocument::DisplayTree()
{
	//build a string to display the structure of the tree
	display->buildTreeDisplay();
	//output the string 
	if(sout != NULL)
		*sout = display->OutputTree();
	else
		display->OutputTree();
	
}

//----<rebuild a xml document from the tree and display it>
void XMLDocument::DisplayXML()
{
	//rebuild the xml
	display->rebuildXML();
	//display the new xml document to a file or string
	if(sout != NULL)
		*sout =display->OutputXML();
	else
		display->OutputXML();

}

//----<clear the nodes in the tree and make a new empty tree>---
void XMLDocument::Clear()
{
	removeTreeNodes();
	pTree =  new XMLTree(new PNode<XMLValue>(XMLValue(XMLValue::TAG,"Document")));
}

//----<search for a element by ID in the tree>---
XMLElement XMLDocument::GetElementbyId(std::string id)
{
	return	XMLElement(pTree->findbyID(id));
}

//----<search for a element by tag in the tree>---
std::vector<XMLElement> XMLDocument::GetElementbyTag(std::string tag)
{
	std::vector<XMLNode*> findednodes;
	std::vector<XMLElement> result;
	findednodes = pTree->find(tag);
	for(size_t i = 0; i< findednodes.size(); ++i)
	{
		result.push_back(XMLElement(findednodes[i]));
	}

	return	result;
}

//----<add an element to a specified parent node of the tree>---
XMLElement XMLDocument::AddElement(XMLElement elm, XMLElement parelm)
{
	return	XMLElement(pTree->add(elm.pNode, parelm.pNode));
}

//----<remove a element from the tree>---
XMLElement XMLDocument::RemoveElement(XMLElement elm)
{
	return XMLElement(pTree->remove(elm.pNode));
}


//----<test stub>---
#ifdef XMLDOCUMENT_TEST
int main(int argc, char* argv[])
{

  std::cout << "\n  Testing ConfigureParser module\n "
            << std::string(32,'=') << std::endl;

  // collecting tokens from files, named on the command line

  if(argc < 2)
  {
    std::cout 
      << "\n  please enter name of file to process on command line\n\n";
    return 1;
  }

  for(int i=1; i<argc; ++i)
  {
    std::cout << "\n  Processing file " << argv[i];
    std::cout << "\n  " << std::string(16 + strlen(argv[i]),'-');
	XMLDocument document;
	XMLConfigParser config(&document);
	Parser* pParser = config.Build();
	std::string output;
	document.write(output,false);
    try
    {
      if(pParser)
      {
        if(!document.read(argv[i]))
        {
          std::cout << "\n  could not open file " << argv[i] << std::endl;
          continue;
        }

      }
      else
      {
        std::cout << "\n\n  Parser not built\n\n";
        return 1;
      }
      // now that parser is built, use it

	while(pParser->next())
       pParser->parse();
	pParser->parse();
	
	std::cout<<"\n======================Test XMLDocument===========================\n";
	std::cout<<"===============================================================================\n";
	document.DisplayTree();
	std::cout<<"===============================================================================\n";
	std::cout<<"\nNow, add a child element with tag \"testTag\" to an element with id \"testId\"\n";
	std::cout<<"Display the resulting tree\n";
	std::cout<<"===============================================================================\n";
	XMLElement testelm;
	if (!testelm.isempty())
		std::cout<<testelm.getValue().TagName().c_str()<<std::endl;
	testelm.makeElement(XMLValue::TAG,"testTag");
	XMLElement targetelm = document.GetElementbyId("testId");
	std::cout<<"Display the elements: \n";
	if (!targetelm.isempty())
		std::cout<<targetelm.getValue().TagName().c_str()<<std::endl;
	document.AddElement(testelm,targetelm);
	document.DisplayTree();
	std::cout<<output.c_str();
	std::cout<<"===============================================================================\n";
	std::cout<<"\nNow, remove the added element and redisplay the tree\n";
	std::cout<<"===============================================================================\n";
	document.RemoveElement(testelm);
	document.DisplayTree();
	std::cout<<output.c_str();
	std::cout<<"===============================================================================\n";
	std::cout<<"\nFinally, display the rebuilt xml\n";
	std::cout<<"===============================================================================\n";
	document.DisplayXML();
	std::cout<<output.c_str();
	document.write("\\output.txt");
	document.DisplayXML();
	std::cout<<"\nNew copied XMLDocument display the rebuilt xml\n";
	std::cout<<"===============================================================================\n";
	XMLDocument doctest_copy(document);
	doctest_copy.write(output,false);
	doctest_copy.DisplayXML();
	std::cout<<output.c_str();
	std::cout<<"\nNew assigned XMLDocument display the rebuilt xml\n";
	std::cout<<"===============================================================================\n";
	XMLDocument doctest_assignment;
	doctest_assignment = document;
	doctest_assignment.write(output,false);
	doctest_assignment.DisplayXML();
	
	std::cout<<output.c_str();
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
  }
  ::system("pause");
}

#endif