/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.cpp - declares new parsing rules and actions   //
//  ver 1.2                                                        //
//                                                                 //
//  Language:     Visual C++ 2008                                  //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project3 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////


////----<test stub>-------------------------------------------------------------------------
#ifdef TEST_ACTIONSANDRULES

#include <iostream>
#include "ActionsAndRules.h"
#include "Tokenizer.h"
#include "SemiExpression.h"

int main(int argc, char* argv[])
{
  std::cout << "\n  Testing ActionsAndRules class\n "
            << std::string(30,'=') << std::endl;

  try
  {
    Toker* pToker = new Toker;
	pToker->returnComments();
    SemiExp* pSemi = new SemiExp(pToker);
	pSemi->makeCommentSemiExp();
    Parser* pParser = new Parser(pSemi);

	ManualAndMaintenancePages* pIR = new ManualAndMaintenancePages;
	pParser->addRule(pIR);
	pToker->attach("../Project3/ActionsAndRules.h");
    while(pParser->next())
      pParser->parse();
	delete pIR;
	delete pParser;
	delete pSemi;
	delete pToker;
    std::cout << "\n\n";
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
  }
}
#endif
