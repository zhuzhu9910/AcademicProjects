#ifndef CLASSINSPECTOR_H
#define CLASSINSPECTOR_H
/////////////////////////////////////////////////////////////////////
//  ClassInspector.h - check a C++ source code against a set of    //
//         a plugable rule                                         //
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
  This module defines a class for parsing the source code and check it 
  against a set of plugable rules and output the detection results of
  the inspector proccess. It also records the infomation of function
  definitions while analyzing the file.
  
  Public Interface:
  =================
  ClassInspector clin;                      //create an instance of ClassInspector
  clin.addFile(some file path);             //add a file to check
  clin.Analyze();                           //analyze the file
  std::cout<<clin.Show();                   //output the result to screen

  Build Process:
  ==============
  Build commands (either one)
    - devenv Project3.sln

  Maintenance History:
  ====================
  ver 1.0 : 03 Apr 10
  - first release

*/
//
#include "ConfigureParser.h"
#include "InspectorRule.h"
#include <string>
////////////////////////////////////////////////////////////////////////////////////////////
////Class Inspector to analyze a C++ source code
class ClassInspector
{
public:
	typedef std::map<std::string ,std::vector<FunInfo>> FunMap;
	typedef std::vector<RuleRecord> RuleRecords;
	ClassInspector();
	std::vector<std::pair<std::string,RuleRecords>>& getRecords();
	std::string Show();
	void addFile(const std::string& filename);
	void Analyze();
	FunMap& getFunctions();
	~ClassInspector();
private:
	ConfigParseForClassInspector* pConfigParseForClassInspector;
	std::vector<std::pair<std::string,RuleRecords>> files;
};




#endif