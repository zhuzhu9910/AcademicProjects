/////////////////////////////////////////////////////////////////////
//  XMLDisplay.cpp - Output parse tree and rebuilt XML for display //
//			     - or the parse tree to file or string             //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project1 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////

#include "XMLDisplay.h"
#include "XMLConfigurParser.h"
#include "Parser.h"
#include "Tree.h"
#include <string>
#include <iostream>
#include <cctype>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <assert.h>

//----<construct a display class with a parse tree>---
XMLDisplay::XMLDisplay(XMLTree* ptree) :pTree(ptree), pOut(NULL){}

XMLDisplay::~XMLDisplay()
{
  if(pOut)
  {
    pOut->clear();
    std::ofstream* pFs = dynamic_cast<std::ofstream*>(pOut);
    if(pFs)
    {
      pFs->close();
    }
  }
}

//----<attach output to a file>--
bool XMLDisplay::AttachFile(const std::string& name)
{
  if(pOut)
  {
    pOut->clear();
    std::ofstream* pFs = dynamic_cast<std::ofstream*>(pOut);
    if(pFs)
    {
      pFs->close();
    }
  }
    pOut = new std::ofstream(name.c_str());
  return pOut->good();
}

//----<rebuild the xml docuemnt from the parse tree>---
void XMLDisplay::rebuildXML()
{
	//create the operations to rebuild the xml document
	RebuitXMLFrontOper* fopt = new RebuitXMLFrontOper(xmlstr);
	RebuitXMLPostOper* popt = new RebuitXMLPostOper(xmlstr);

	//set the operations
	pTree->setFrontOperation(fopt);
	pTree->setPostOperation(popt);

	//rebuild the xml document by DFS
	pTree->walk();

	//delete the operations
	pTree->setFrontOperation(NULL);
	pTree->setPostOperation(NULL);
	delete fopt;
	delete popt;
}

//----<DFS the tree and turn the information in the nodes to a format for displaying and store them in a string>---
void XMLDisplay::buildTreeDisplay()
{
	//create operation
	OutputTreeOper* oper = new OutputTreeOper;
	//set operation
	pTree->setFrontOperation(oper);
	pTree->setPostOperation(NULL);
	//DFS the tree
	pTree->walk();
	//put the root into the string
	treestr = "\nNode: <root>\n";
	//append the child nodes into the display string
	if(!pTree->getroot()->getChildren().empty())
	{
		treestr.append("Child nodes: ");
		std::vector<XMLValue> mychildren = pTree->getroot()->getChildren();
		for(size_t i = 0; i < mychildren.size(); ++i)
		{
			//if a child node is a xml element node with tag then display <tag name> else display its node type
			if(XMLValue::COMMENT != mychildren[i].Type())
				treestr.append("<").append(mychildren[i].TagName()).append(">, ");
			else
				treestr.append("COMMENT").append(", ");
		}
		treestr.append("\n---------------------\n");
	}
	//get the string for displaying the tree
	treestr.append(oper->getOutput());
	//delete the operation
	pTree->setFrontOperation(NULL);
	delete oper;
}

//----<Output the tree to the attached file and return the string for displaying the tree>---
std::string XMLDisplay::OutputTree()
{
	//Output he tree information to the attached file (if there is an attached one)
	if (NULL != pOut && pOut->good())
	{
		(*pOut)<<treestr.c_str();
		std::cout<<"Outputing to file succeed!"<<std::endl;
	}
	return treestr;

}

//----<Output the rebuilt xml to the attached file and return the string of rebuilt xml>
std::string XMLDisplay::OutputXML()
{
	//Output he rebuilt xml to the attached file (if there is an attached one)
	if (NULL != pOut && pOut->good())
	{
		(*pOut)<<xmlstr.c_str();
		std::cout<<"Outputing to file succeed!"<<std::endl;
	}
	return xmlstr;
}

//----<test stub>---
#ifdef TEST_XMLDISPLAY

void main()
{
	try
	{
	std::stack<PNode<XMLValue>*> *nStack = new std::stack<PNode<XMLValue>*>;
	PNode<XMLValue>* rootnode = new PNode<XMLValue>(XMLValue(XMLValue::TAG,"Document"));
	XMLTree* xtree = new XMLTree(rootnode);
	ParserState* ps = new ParserState;
	
	Toker* pToker = new Toker;
	pToker->setMode(Toker::xml);          //XML parser mode
    XmlParts* pSemi = new XmlParts(pToker);
    Parser* pParser = new Parser(pSemi);

	ElementEnd* pElementendrule = new ElementEnd(nStack,ps);
	ElementStart* pElementstartrule = new ElementStart(nStack,ps);
	ElementSelfClosed* pElementselfcrule = new ElementSelfClosed(nStack,ps);
	Text* ptextrule = new Text(nStack,ps);
	Comment* pCommentrule = new Comment(nStack,ps);
	ProcInstr* pIinstrrule = new ProcInstr(nStack,ps);

	EndFound* pEndact = new EndFound(nStack,xtree);
	StartFound* pStartact = new StartFound(nStack,xtree);
	SelfClosedFound* pSelfClosedact = new SelfClosedFound(nStack,xtree);
	TextFound* pTextact = new TextFound(nStack,xtree);
	CommentFound* pCommentact = new CommentFound(nStack,xtree);
	ProcInstrFound* pInstract = new ProcInstrFound(nStack,xtree);
	
	pElementendrule->addAction(pEndact);
	pElementstartrule->addAction(pStartact);
	pElementselfcrule->addAction(pSelfClosedact);
	ptextrule->addAction(pTextact);
	pCommentrule->addAction(pCommentact);
	pIinstrrule->addAction(pInstract);
	
	pParser->addRule(pElementendrule);
	pParser->addRule(pElementstartrule);
	pParser->addRule(pElementselfcrule);
	pParser->addRule(ptextrule);
	pParser->addRule(pCommentrule);
	pParser->addRule(pIinstrrule);
	    

	nStack->push(rootnode);

	if(pParser)
    {
		if(! pToker->attach("..\\LectureNote.xml"))
        {
	      std::cout << "\n  could not open file "  << std::endl;
          return;
        }
     }

	while(pParser->next())
       pParser->parse();
	
	//display the tree and the rebuilt xml
	XMLDisplay disply(xtree);
	//generate the string of tree and rebuilt xml
	disply.buildTreeDisplay();
	disply.rebuildXML();
	//attach the output file to the display object
	disply.AttachFile("..\\output.txt");
	//display them
	std::cout<<"=======================Parser Tree====================="<<std::endl;
	std::cout<<disply.OutputTree();
	
      std::cout << "\n\n";

	std::cout<<"=====================Rebuild XML====================="<<std::endl;
	std::cout<<disply.OutputXML();
	
	delete pEndact;
	delete pStartact;
	delete pSelfClosedact;
	delete pTextact;
	delete pCommentact;
	delete pInstract;
	
	delete pElementendrule;
	delete pElementstartrule;
	delete pElementselfcrule;
	delete ptextrule;
	delete pCommentrule;
	delete pIinstrrule;

	delete pToker;
	delete pSemi;
	delete pParser;

	delete ps;
	delete nStack;
	
	xtree->remove(xtree->getroot());
	std::vector< PNode<XMLValue>* > deletednodes = xtree->getdeletednodes();
	for(size_t i = 0; i < deletednodes.size(); ++i)
		delete deletednodes[i];
	delete xtree;

  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
  }	
  ::system("pause");
}

#endif