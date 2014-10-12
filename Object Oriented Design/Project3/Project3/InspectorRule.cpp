/////////////////////////////////////////////////////////////////////
//  InspectorRules.cpp - declares a rule for analyzing C++ code    //
//  ver 1.0                                                        //
//                                                                 //
//  Language:     Visual C++ 2008                                  //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project3 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
#include "InspectorRule.h"

////----<test stub>-------------------------------------------------------------------------
#ifdef TEST_INSPECTORRULE
#include "Tokenizer.h"
#include "SemiExpression.h"


void main()
{
    Toker* pToker = new Toker;
	pToker->returnComments();
    SemiExp* pSemi = new SemiExp(pToker);
	pSemi->makeCommentSemiExp();
    Parser* pParser = new Parser(pSemi);

	InspectorRule<>* pIR = new InspectorRule<>;
	pParser->addRule(pIR);
	pToker->attach("../Project3/InspectorRule.h");
    while(pParser->next())
      pParser->parse();
	delete pIR;
	delete pParser;
	delete pSemi;
	delete pToker;
}


#endif

