#ifndef XMLDOCEXCUTE_H
#define XMLDOCEXCUTE_H
/////////////////////////////////////////////////////////////////////
//  XMLDocExecute.h - Demostration for project 1                   //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project1 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module defines the class only for the project 1 demonstration.
  It provide the interface to build the parser and display module and
  set the input output file and excuting parsing and displaying.
  This project meets all requirements.

  Public Interface:
  =================
  XMLDocExecute docexc;                    //create a excuting object
  docexc.Build();                          //initialize the parts for parser and create a display object
  docexc.GetDocument().read(somefile);     //attach a input xml file
  docexc.GetDocument().write(somfile);     //attach a output file
  docexc.Parse();                          //parse the input and build the parse tree
  docexc.DisplayTree();                    //display the parse tree
  docexc.DisplayXML();                     //rebuild the xml and display it

  Build Process:
  ==============
  Required files
    -XMLDocExecute.h, XMLDocExecute.cpp, XMLDocument.h,XMLDocument.cpp,
	XMLDisplay.h, XMLDisplay.cpp, XMLConfigurParser.h, XMLConfigurParser.cpp,
	Parser.h, Parser.cpp, XMLActionAndRules.h, XMLActionAndRules.cpp, 
	xmlElementParts.h, xmlElementParts.cpp, tokenizer.h, tokenizer.cpp
  Build commands (either one)
    - devenv XMLParser.sln
    - cl /EHsc /DTEST_PARSER XMLDocExecute.h XMLDocExecute.cpp\
		XMLDocument.h XMLDocument.cpp\
		XMLDisplay.h XMLDisplay.cpp\
		 XMLConfigurParser.cpp parser.cpp \
         XMLActionAndRules.cpp \
         xmlElementParts.cpp tokenizer.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 1.0 : 08 Feb 10
  - first release

*/
//
#include "XMLActionAndRules.h"
#include "XMLConfigurParser.h"
#include "XMLDisplay.h"
#include "Tokenizer.h"
#include "XMLDocExecute.h"
#include "Parser.h"
#include <iostream>
#include <string>


/////////////////////////////////////////////////////////////////////////////////
/////Class for demonstrating the stuff of this project
class XMLDocExecute
{
public:
	XMLDocExecute();
	~XMLDocExecute();
	bool Build();
	void Parse();
	XMLDocument& GetDocument();
	
private:
	XMLDocument* pDoc;
	XMLConfigParser* pConfigure;
	Parser* pParser;
};

#endif