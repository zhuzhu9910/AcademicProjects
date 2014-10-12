#ifndef XMLCONFIGUREPARSER_H
#define XMLCONFIGUREPARSER_H
/////////////////////////////////////////////////////////////////////
//  ConfigureParser.h - builds and configures parsers              //
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
  This module builds and configures parsers.  It builds the parser
  parts and configures them with application specific rules and actions.

  Public Interface:
  =================
  XMLConfigParser conConfig;
  conConfig.Build();
  conConfig.Attach(someFileName);

  Build Process:
  ==============
  Required files
    - XMLConfigurParser.h, XMLConfigurParser.cpp, Parser.h, Parser.cpp,
      XMLActionAndRules.h, XMLActionAndRules.cpp,
      xmlElementParts.h, xmlElementParts.cpp, tokenizer.h, tokenizer.cpp
  Build commands (either one)
    - devenv XMLParser.sln
    - cl /EHsc /DTEST_PARSER XMLConfigurParser.cpp parser.cpp \
         XMLActionAndRules.cpp \
         xmlElementParts.cpp tokenizer.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 1.0 : 05 Feb 10
  - first release

*/

//

//#include "Parser.h"
#include "XmlElementParts.h"
#include "Tokenizer.h"
#include "XMLActionAndRules.h"
#include "XMLDocument.h"




///////////////////////////////////////////////////////////////
// build parser that writes its output to console

class XMLConfigParser : IBuilder
{
public:
  XMLConfigParser(XMLDocument* xmldoc);
  ~XMLConfigParser();
//  bool Attach(const std::string& name, bool isFile=true);
  Parser* Build();
 // XMLTree* GetTree();

private:
  // Builder must hold onto all the pieces

  Toker* pToker;
  XMLDocument* pDoc;
  XmlParts* pSemi;
  Parser* pParser;
///////////////////////////
  std::stack<PNode<XMLValue>*>* pStack; 
  ParserState* pState;
/////////////////////////////
  // and the the Rules and Actions as well

	ElementEnd* pElementendrule;
	ElementStart* pElementstartrule;
	ElementSelfClosed* pElementselfcrule;
	Text* ptextrule;
	Comment* pCommentrule;
	ProcInstr* pIinstrrule;
	CommentStart* pCommentstartrule;
	ParseComment* pParsecommentrule;
	CommentEnd* pCommentEndrule;
	ErrorTest* pErrortestrule; 

	EndFound* pEndact;
	StartFound* pStartact;
	SelfClosedFound* pSelfClosedact;
	TextFound* pTextact;
	CommentFound* pCommentact;
	ProcInstrFound* pInstract;
	StartCommentFound* pStartCommentact;
	ParseCommentAct* pParseCommentact;
	ErrorAlert* pErroralertact;
  //helper fucntions
	void creatRulesAndActions();
  // prohibit copies and assignments

  XMLConfigParser(const XMLConfigParser&);
  XMLConfigParser& operator=(const XMLConfigParser&);
};

#endif
