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

#include "XmlElementParts.h"
#include "Tokenizer.h"
#include "XMLConfigurParser.h"
#include "XMLDisplay.h"

//----< constructor initializes pointers to all parts >--------------

XMLConfigParser::XMLConfigParser(XMLDocument* xmldoc) 
  :pDoc(xmldoc), pParser(0), pElementendrule(0), pElementstartrule(0), 
    pElementselfcrule(0), ptextrule(0), pCommentrule(0), pIinstrrule(0), 
	pEndact(0), pStartact(0), pSelfClosedact(0), pTextact(0), pCommentact(0),
	pInstract(0), pStack(0), pState(0){}

//----< destructor releases all parts >------------------------------

XMLConfigParser::~XMLConfigParser()
{
  // when Builder goes out of scope, everything must be deallocated

	delete pEndact;
	delete pStartact;
	delete pSelfClosedact;
	delete pTextact;
	delete pCommentact;
	delete pInstract;
	delete pStartCommentact;
	delete pParseCommentact;
	delete pErroralertact;
	
	delete pElementendrule;
	delete pElementstartrule;
	delete pElementselfcrule;
	delete ptextrule;
	delete pCommentrule;
	delete pIinstrrule;
	delete pCommentstartrule;
	delete pParsecommentrule;
	delete pCommentEndrule;
	delete pErrortestrule;

	delete pParser;
	delete pSemi;
	delete pToker;

	delete pState;
	delete pStack;
                                                           
}

//
//----< Here's where alll the parts get assembled >----------------

Parser* XMLConfigParser::Build()
{
  try
  {
    // configure to detect and act on preprocessor statements
	pStack = new std::stack<PNode<XMLValue>*>;
	pState = new ParserState;
	pToker = new Toker;
	pToker->setMode(Toker::xml);          //XML parser mode
	pSemi = new XmlParts(pToker);
    pParser = new Parser(pSemi);
	pDoc->SetToker(pToker);               //attach the toker to document
	creatRulesAndActions();               //create rules and actions
	pStack->push(pDoc->GetTree()->getroot());    //Push the root node into stack
    return pParser;
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
    return 0;
  }
}

//----<create rules and actions>
void XMLConfigParser::creatRulesAndActions()
{
	XMLTree* pTree = pDoc->GetTree();
	//create rules
	pElementendrule = new ElementEnd(pStack,pState);
	pElementstartrule = new ElementStart(pStack,pState);
	pElementselfcrule = new ElementSelfClosed(pStack,pState);
	ptextrule = new Text(pStack,pState);
	pCommentrule = new Comment(pStack,pState);
	pIinstrrule = new ProcInstr(pStack,pState);
	pCommentstartrule = new CommentStart(pStack,pState);
	pParsecommentrule = new ParseComment(pStack,pState);
	pCommentEndrule = new CommentEnd(pStack,pState);
	pErrortestrule = new ErrorTest(pStack,pState);

	//create actions
	pEndact = new EndFound(pStack,pTree);
	pStartact = new StartFound(pStack,pTree);
	pSelfClosedact = new SelfClosedFound(pStack,pTree);
	pTextact = new TextFound(pStack,pTree);
	pCommentact = new CommentFound(pStack,pTree);
	pInstract = new ProcInstrFound(pStack,pTree);
	pStartCommentact = new StartCommentFound(pStack,pTree);
	pParseCommentact = new ParseCommentAct(pStack,pTree);
	pErroralertact = new ErrorAlert(pStack,pTree);

	//add actions to rules
	pElementendrule->addAction(pEndact);
	pElementstartrule->addAction(pStartact);
	pElementselfcrule->addAction(pSelfClosedact);
	ptextrule->addAction(pTextact);
	pCommentrule->addAction(pCommentact);
	pIinstrrule->addAction(pInstract);
	pCommentstartrule->addAction(pStartCommentact);
	pParsecommentrule->addAction(pParseCommentact);
	pCommentEndrule->addAction(pEndact);
	pErrortestrule->addAction(pErroralertact);

	//add rules to the parser
	pParser->addRule(pElementendrule);
	pParser->addRule(pElementstartrule);
	pParser->addRule(pElementselfcrule);
	pParser->addRule(ptextrule);
	pParser->addRule(pCommentrule);
	pParser->addRule(pIinstrrule);
	pParser->addRule(pCommentstartrule);
	pParser->addRule(pParsecommentrule);
	pParser->addRule(pCommentEndrule);
	pParser->addRule(pErrortestrule);
}


 
//
//----<test stub>---
#ifdef TEST_CONFIGUREPARSER

#include <queue>
#include <string>

void WalkTree(PNode<XMLValue >* pPNode)
{
	std::cout <<"\n  NodeName: "<<pPNode->value().TagName().c_str()<<" ;Type: "<<pPNode->value().Type();
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

int main(int argc, char* argv[])
{
  std::cout << "\n  Testing ConfigureParser module\n "
            << std::string(32,'=') << std::endl;

  // collecting tokens from files, named on the command line

  for(int i=1; i<argc; ++i)
  {
    std::cout << "\n  Processing file " << argv[i];
    std::cout << "\n  " << std::string(16 + strlen(argv[i]),'-');

	XMLDocument document;
	XMLConfigParser config(&document);
	XMLConfigParser config;
	Parser* pParser = config.Build();
	
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
	std::cout<<"============================Parser Tree====================="<<std::endl;
	WalkTree(config.GetTree()->getroot());

      std::cout << "\n\n";

	std::cout<<"============================Rebuild XML====================="<<std::endl;
	std::string outputstring;
	RebuitXMLFrontOper* fopt = new RebuitXMLFrontOper(outputstring);
	RebuitXMLPostOper* popt = new RebuitXMLPostOper(outputstring);
	config.GetTree()->setFrontOperation(fopt);
	config.GetTree()->setPostOperation(popt);
	config.GetTree()->walk();
	std::cout<<"\n"<<outputstring;
	delete fopt;
	delete popt;
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
  }

}

#endif
