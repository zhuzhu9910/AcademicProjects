/////////////////////////////////////////////////////////////////////
//  XMLActionAndRules.cpp - rules for parser and                   //
//						- corresponding actions                    //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project1 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////


//----<test stub>---
#ifdef TEST_XMLACTIONANDRULES

#include "XMLActionAndRules.h"
#include "XmlElementParts.h"
#include "Tokenizer.h"

//----<DFS nodes of the tree for test>--
void WalkTree(PNode<XMLValue >* pPNode)
{
	std::cout <<"\n  NodeName: "<<pPNode->value().TagName().c_str()<<" ;Type(): "<<pPNode->value().Type();
	if(!pPNode->value().attributes().empty())
	{
		std::cout <<"; Attributes: ";
		for(size_t i = 0; i < pPNode->value().attributes().size(); ++i)
			std::cout <<pPNode->value().attributes()[i].AttrName <<" = "<<pPNode->value().attributes()[i].AttrValue<<", ";
	}
	PNode<XMLValue >* pTemp;
	while (pTemp = pPNode->nextUnmarkedChild())
	{
		pTemp->setMarkState(true);
		WalkTree(pTemp);
	}
}


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

	std::cout<<"============================Parser Tree====================="<<std::endl;
	WalkTree(xtree->getroot());
	
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