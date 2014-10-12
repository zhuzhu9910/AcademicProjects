#ifndef ANNOTATOR_H
#define ANNOTATOR_H
/////////////////////////////////////////////////////////////////////
//  Annotator.h - class for annotating input source code files     //
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
  This module defines a class for parsing and insert object inspector 
  utilities into the source code lines and generate a new code file with
  declaration of logging instance in functions. It can include and exclude
  a subset of the detected functions to add inspector code.
  
  Public Interface:
  =================
  Annotator ann(Function list,generating code director path, log file path);   //create a Annotator instance
  std::vector<FunInfo> funvec;                                                 //define a vector of FunInfo
  funvec.push_back(ann.GetFunctions()[some file name][0]);                     //push a function record into the vector
  ann.RemoveFunctions(funvec);                                                 //remove the functions in the vector
  ann.AddFunctions(funvec);                                                    //add functions in the vector
  ann.Annotate();                                                              //annoatate the code

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

namespace LoggingProccessing
{

////////////////////////////////////////////////////////////////////////////////////////////
////Class for annotating a C++ source code
class Annotator
{
public:
	typedef std::map<std::string ,std::vector<FunInfo>> FunMap;
	Annotator(const FunMap& finfo, const std::string &outcat, const std::string& logpath);
	void Annotate();
	void AddFunction(const FunInfo& functioninfo);
	bool RemoveFunction(const FunInfo& functioninfo);
	void AddFunctions(const std::vector<FunInfo>& functioninfos);
	bool RemoveFunctions(const std::vector<FunInfo>& functioninfos);
	FunMap& GetFunctions();
private:
	FunMap funinfos;
	std::string getFileName(const std::string& path);
	std::string OutPutPath;
	std::string LogPath;
};


}

#endif