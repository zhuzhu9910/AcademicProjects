#ifndef CONFIGUREPARSER_H
#define CONFIGUREPARSER_H
/////////////////////////////////////////////////////////////////////
//  ConfigureParser.h - builds and configures parsers              //
//  ver 1.0                                                        //
//                                                                 //
//  Language:     Visual C++ 2008                                  //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project3 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module builds and configures parsers.  It builds the parser
  parts and configures them with application specific rules and actions.
  It defines two builders, one is for class inspector and another is 
  for the annotator.
  

  Public Interface:
  =================
  ConfigParseForClassInspector conConfig;           //create a parse configure for class inspector
  conConfig.Build();                                //build the parser
  conConfig.Attach(someFileName);                   //attach an input file
  std::vector<FunInfo> fv;                          //vector of function infomation records
  ConfigParseForAnnotator cpfa(&fv,some log path);  //create a parse configure for annotator
  cpfa.Build();                                     //build the parser
  cpfa.Attach(someFileName);                        //attach an input file
  cpfa.AttachOutput(someFileName);                  //attach an output file

  Build Process:
  ==============
  Build commands
    - devenv Project1HelpS06.sln

  Maintenance History:
  ====================
  ver 1.0 : 01 Apr 10
  - first release

*/

//
#include "Parser.h"
#include "SemiExpression.h"
#include "Tokenizer.h"
#include "ActionsAndRules.h"

///////////////////////////////////////////////////////////////
// build parser for class inspector

class ConfigParseForClassInspector : IBuilder
{
public:
  ConfigParseForClassInspector();
  ~ConfigParseForClassInspector();
  bool Attach(const std::string& name, bool isFile=true);
  std::map<std::string ,std::vector<FunInfo>>& getFunctions();
  Parser* Build();
  std::vector<RuleRecord>& getRuleRecords();

private:
	
	std::string _filename;
	bool isreset;
	std::vector<RuleRecord> records;
  // Builder must hold onto all the pieces

  Toker* pToker;
  SemiExp* pSemi;
  Parser* pParser;

  
  // and the the Rules and Actions as well

  ManualAndMaintenancePages<>* pMAMP;
  PreProsStat<>* pPreProsStat;
  TestStub<>* pTestStub;
  nConstGlobalData<>* pnConstGlobalData;
  nDftCstrWtOutInitL<>* pnDftCstrWtOutInitL;
  nExtAbsDestruc<>* pnExtAbsDestruc;

  FunctionDef<>* pFunctionDef;
  ClassDeclare<>* pClassDeclare;
  NamespaceDeclare<>* pNamespaceDeclare;
  StructDeclare<>* pStructDeclare;
  OtherScope<>* pOtherScope;
  ScopeEnd<>* pScopeEnd;
  OverloadsVlt<>* pOverloadsVlt;

  // prohibit copies and assignments

  ConfigParseForClassInspector(const ConfigParseForClassInspector&);
  ConfigParseForClassInspector& operator=(const ConfigParseForClassInspector&);

  //helper utilities
  void addRulesAndActions();
  void createRulesAndActions();
};

//
///////////////////////////////////////////////////////////////
// build parser for annotator

class ConfigParseForAnnotator : IBuilder
{
public:
	ConfigParseForAnnotator(std::vector<FunInfo>* info, const std::string& logname);
  ~ConfigParseForAnnotator();
  bool Attach(const std::string& name, bool isFile=true);
  bool AttachOutput(const std::string& path);
  Parser* Build();
  std::ofstream& getOutputFileStream();

private:
  std::vector<FunInfo>* funinfo;
  std::ofstream* pOut;
  std::string logfile;
  // Builder must hold onto all the pieces

  Toker* pToker;
  SemiExp* pSemi;
  Parser* pParser;
  
  // and the the Rules and Actions as well
  AnnotatorRule<>* pAnnotatorRule;
  ModifyNew* pModifyNew;

  // prohibit copies and assignments

  ConfigParseForAnnotator(const ConfigParseForAnnotator&);
  ConfigParseForAnnotator& operator=(const ConfigParseForAnnotator&);
};

#endif
