#ifndef LOGMANAGER_H
#define LOGMANAGER_H
/////////////////////////////////////////////////////////////////////
//  LogManager.h - declares an LogManager class for managing the   //
//    logging proccess and LogManagerRegiser for creating and      //
//    keeping gobal shared static log managers.                    //
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
  This module defines a logger interface to define some standard logging
  operations. The class of function logger and allocator logger provides
  the standard logging operations and keeping the infomation for log.
  
  Public Interface:
  =================
  LogManagerRegister::CreateManager("../log.txt");     //create a LogManager with output log file path
  FunctionLog alog("void main involked","13:52:55");   //create a function log instance
  LogManagerRegister::Log(&alog);                      //log the function log item
	
  Build Process:
  ==============
  Build commands
    - devenv Project3.sln

  Maintenance History:
  ====================
  ver 1.0 : 01 Apr 10
  - first release
  ver 1.1 : 06 Apr 10
  - add the Managers struct 
  and modify the constructors
  of classes in this file  

*/
//
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <cctype>
#include <fstream>
#include <sstream>
#include "Loggers.h"

namespace LoggingProccessing
{

////////////////////////////////////////////////////////////////////////////////////////////
////Class LogManager for managing the logging the log item to a log file
class LogManager
{
public:
	LogManager();
	~LogManager();
	bool AttachFile(const std::string& filename);
	void OutputFileHead();
	void Log(ILogger* log);
private:
	LogManager& operator = (const LogManager& lm);
	LogManager(const LogManager& lm);
	std::ostream* pOut;
	std::map<unsigned int,ILogger*> allocters;
};

////////////////////////////////////////////////////////////////////////////////////////////
////struct for holding log manager pointers and responsible for deallocating them
struct Managers
{
	~Managers();
	std::vector<LogManager*> logmanagers;
};

////////////////////////////////////////////////////////////////////////////////////////////
////Class for creating and holding log managers
class LogManagerRegister
{
public:
	static bool CreateManager(const std::string& filename);
	static void Log(ILogger* log);
private:
	static Managers logms;
};

}
#endif