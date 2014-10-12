/////////////////////////////////////////////////////////////////////
//  ConfigureParser.cpp - builds and configures parsers            //
//  ver 1.0                                                        //
//                                                                 //
//  Language:     Visual C++ 2008                                  //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project3 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////

#include "Parser.h"
#include "SemiExpression.h"
#include "Tokenizer.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"

//----< constructor initializes pointers to all parts >--------------

ConfigParseForClassInspector::ConfigParseForClassInspector() 
  : pToker(0), pSemi(0), pParser(0),isreset(false){}

//----< destructor releases all parts >------------------------------

ConfigParseForClassInspector::~ConfigParseForClassInspector()
{
  // when Builder goes out of scope, everything must be deallocated


  delete pMAMP;
  delete pPreProsStat;
  delete pTestStub;
  delete pnConstGlobalData;
  delete pnDftCstrWtOutInitL;
  delete pnExtAbsDestruc;
  delete pOverloadsVlt;

  delete pFunctionDef;
  delete pClassDeclare;
  delete pNamespaceDeclare;
  delete pStructDeclare;
  delete pOtherScope;
  delete pScopeEnd;

  delete pParser;
  delete pSemi;
  delete pToker;
}
//----< attach toker to a file stream or stringstream >------------

bool ConfigParseForClassInspector::Attach(const std::string& name, bool isFile)
{
  if(pToker == 0)
    return false;
  _filename = name;
	pFunctionDef->resetlines();
	pFunctionDef->setCurFile(name);
  if(name.find(".h") != std::string::npos)
  {
	  pPreProsStat->setHeadFile();
  }

  if(name.find(".cpp") != std::string::npos)
	  pTestStub->SetCppFile();
  return pToker->attach(name, isFile);
}

//----< access the function information records >------------------
std::map<std::string ,std::vector<FunInfo>>& ConfigParseForClassInspector::getFunctions()
{
	return pFunctionDef->GetFunctions();
}

//
//----< Here's where all the parts get assembled >-----------------

Parser* ConfigParseForClassInspector::Build()
{
  try
  {
    //when reset the configure, rebuild the parser, semi-expression and tokenizer
	if(isreset)
	{
		delete pParser;
		delete pSemi;
		delete pToker;
		records.clear();
	}
    pToker = new Toker;
	pToker->returnComments();
    pSemi = new SemiExp(pToker);
	pSemi->makeCommentSemiExp();
    pParser = new Parser(pSemi);
	//when reset the configure, it does not rebuild the rules and actions
	if(!isreset)
	{
		createRulesAndActions();
	}
    //add rules and actions to the parser
	addRulesAndActions();
	isreset = true;
    return pParser;
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
    return 0;
  }
}

////----<add the rules and actions to parser>---------------------
void ConfigParseForClassInspector::addRulesAndActions()
{
	pParser->addRule(pnConstGlobalData);
	pParser->addRule(pOverloadsVlt);
	pParser->addRule(pnDftCstrWtOutInitL);
	pParser->addRule(pnExtAbsDestruc);
	pParser->addRule(pOtherScope);
	pParser->addRule(pStructDeclare);
	pParser->addRule(pClassDeclare);
	pParser->addRule(pFunctionDef);
	pParser->addRule(pNamespaceDeclare);
	pParser->addRule(pScopeEnd);

	pParser->addRule(pMAMP);
	pParser->addRule(pPreProsStat);
	pParser->addRule(pTestStub);
}
////----<create rules and actions>---------------------------------
void ConfigParseForClassInspector::createRulesAndActions()
{
	pMAMP = new ManualAndMaintenancePages<>(&records);
	pPreProsStat = new PreProsStat<>(&records);
	pTestStub = new TestStub<>(&records);
	pFunctionDef = new FunctionDef<>;
	pClassDeclare = new ClassDeclare<>;
	pNamespaceDeclare = new NamespaceDeclare<>;
	pStructDeclare = new StructDeclare<>;
	pOtherScope = new OtherScope<>;
	pScopeEnd = new ScopeEnd<>;
	pnConstGlobalData = new nConstGlobalData<>(&records);
	pnDftCstrWtOutInitL = new nDftCstrWtOutInitL<>(&records);
	pnExtAbsDestruc = new nExtAbsDestruc<>(&records);
	pOverloadsVlt = new OverloadsVlt<>(&records);

	pMAMP->setRuleRecords(&records);
	pPreProsStat->setRuleRecords(&records);
	pTestStub->setRuleRecords(&records);
	pFunctionDef->setRuleRecords(&records);
	pClassDeclare->setRuleRecords(&records);
	pNamespaceDeclare->setRuleRecords(&records);
	pStructDeclare->setRuleRecords(&records);
	pOtherScope->setRuleRecords(&records);
	pScopeEnd->setRuleRecords(&records);
	pnConstGlobalData->setRuleRecords(&records);
	pnDftCstrWtOutInitL->setRuleRecords(&records);
	pnExtAbsDestruc->setRuleRecords(&records);
	pOverloadsVlt->setRuleRecords(&records);

}

std::vector<RuleRecord>& ConfigParseForClassInspector::getRuleRecords()
{
	return records;
}

//
//----< constructor initializes pointers to all parts >--------------

ConfigParseForAnnotator::ConfigParseForAnnotator(std::vector<FunInfo>* info, const std::string& logname) 
  : pToker(0), pSemi(0), pParser(0), funinfo(info),pOut(0), logfile(logname) {}

//----< destructor releases all parts >------------------------------

ConfigParseForAnnotator::~ConfigParseForAnnotator()
{
  // when Builder goes out of scope, everything must be deallocated

  delete pModifyNew;
  delete pAnnotatorRule;
  delete pParser;
  delete pSemi;
  delete pToker;

  if(pOut)
  {
	  pOut->close();
	  delete pOut;
  }
}
//----< attach toker to a file stream or stringstream >------------

bool ConfigParseForAnnotator::Attach(const std::string& name, bool isFile)
{
  if(pToker == 0)
    return false;
  return pToker->attach(name, isFile);
}
//----< attach a output file stream to the output rules >----------
bool ConfigParseForAnnotator::AttachOutput(const std::string& path)
{
  if(!pAnnotatorRule)
	  return false;
  if(pOut)
  {
    pOut->clear();
    pOut->close();
  }
  pOut = new std::ofstream(path.c_str());
  pAnnotatorRule->setOutPut(pOut);
  pModifyNew->setOutPut(pOut);
  return pOut->good();
}


//----< Here's where all the parts get assembled >-----------------

Parser* ConfigParseForAnnotator::Build()
{
  try
  {
    pToker = new Toker;
	pToker->returnComments();
    pSemi = new SemiExp(pToker);
	pSemi->makeCommentSemiExp();
    pParser = new Parser(pSemi);
	pAnnotatorRule = new AnnotatorRule<>(funinfo,logfile);
	pModifyNew = new ModifyNew;
	pAnnotatorRule->addAction(pModifyNew);
	pParser->addRule(pAnnotatorRule);


    return pParser;
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
    return 0;
  }
}

//----< get the output file stream >------------------------------
std::ofstream& ConfigParseForAnnotator::getOutputFileStream()
{
	return *pOut;
}

//----<test stub>-------------------------------------------------
#ifdef TEST_CONFIGUREPARSER

#include <queue>
#include <string>


////----<test ConfigParseForClassInspector>-----------------------
int test1(int i, char* argv[],ConfigParseForClassInspector& configure)
{

    std::cout << "\n  Processing file " << argv[i];
    std::cout << "\n  " << std::string(16 + strlen(argv[i]),'-');

    Parser* pParser = configure.Build();
    try
    {
      if(pParser)
      {
        if(!configure.Attach(argv[i]))
        {
          std::cout << "\n  could not open file " << argv[i] << std::endl;
          return 1;
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
      std::cout << "\n\n";
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
	return 0;
}

////----<test ConfigParseForAnnotator>-------------------------------
int test2(int i, char* argv[],ConfigParseForClassInspector& configure)
{
    std::cout << "\n  Processing file for Queued Outputs " << argv[i];
    std::cout << "\n  " << std::string(35 + strlen(argv[i]),'-');
	Parser* pParser;
    ConfigParseForAnnotator Qconfigure(&(configure.getFunctions()[argv[i]]),"../log-conf.log");
    pParser = Qconfigure.Build();
    try
    {
      if(pParser)
      {
        if(!Qconfigure.Attach(argv[i]))
        {
          std::cout << "\n  could not open file " << argv[i] << std::endl;
          return 1;
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
      std::cout << "\n\n";
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
    std::cout << "\n\n";
	return 0;
}
 
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
    ConfigParseForClassInspector configure;
	if(test1(i,argv,configure) == 1)
		continue;
	if(test2(i,argv,configure) == 1)
		continue;
  }
}

#endif
