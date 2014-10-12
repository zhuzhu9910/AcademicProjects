#ifndef LOGGERS_H
#define LOGGERS_H
/////////////////////////////////////////////////////////////////////
//  Loggers.h - declares an interface of logger and a class of     //
//       function log and allocator log for logging operations     //
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
  FunctionLog flog("void main() involked", "13:59:12");   //create a function log instance
  flog.setTime("13:12:12")                                //set the logging time
  AllocateLog alll(0,4,"int","pint");                     //create a allocator log instance
  alll.setTime("13:12:12");                               //set the logging time
  DeallocateLog dl(0);                                    //create a deallocating log instance
  dl.setTime("0:0:0:0");                                  //set the logging time

  Build Process:
  ==============
  Build commands
    - devenv Project3.sln

  Maintenance History:
  ====================
  ver 1.0 : 01 Apr 10
  - first release

*/
//
#include <string>
namespace LoggingProccessing
{
////////////////////////////////////////////////////////////////////////////////////////////
////Logger interface for logging operations
struct ILogger
{
	virtual std::string getLogRecord() = 0;
	virtual void setTime(const std::string& stime) = 0;
	virtual std::string type() = 0;
	virtual ~ILogger(){}
};

////////////////////////////////////////////////////////////////////////////////////////////
////Class for logging function infomation
class FunctionLog :public ILogger
{
public:
	FunctionLog(const std::string& funevent,const std::string& stime = "");
	void setTime(const std::string& stime);
	std::string getLogRecord();
	std::string type();
private:
	std::string _funevent;
	std::string _time;
};
////////////////////////////////////////////////////////////////////////////////////////////
////Class for logging memeory allocating information
class AllocateLog :public ILogger
{
public:
	AllocateLog(unsigned int address, size_t size, const std::string& type, const std::string& name, bool isarray = false);
	void setTime(const std::string& stime);
	std::string getLogRecord();
	std::string type();
	unsigned int Address();
	std::string& AlocType();
	std::string& PraName();
	size_t AlocSize();

protected:
	unsigned int _address;
	size_t _size;
	std::string _type;
	std::string _name;
	std::string _time;
	bool _isarray;
};


////////////////////////////////////////////////////////////////////////////////////////////
////Class for logging memory deallocating information
class DeallocateLog :public ILogger
{
public:
	DeallocateLog(unsigned int address);
	void setTime(const std::string& stime);
	std::string getLogRecord();
	std::string type();
	unsigned int Address();
private:
	unsigned int _address;
	std::string _time;
};

////----<construct function log with function name and invloked time>-----------------------
inline FunctionLog::FunctionLog(const std::string &funevent, const std::string &stime)
:_funevent(funevent),_time(stime) {}
////----<construct allocation log with address, size, type, parameter name and an array mark>-
inline AllocateLog::AllocateLog(unsigned int address, size_t size, const std::string& type, const std::string& name, bool isarray)
:_address(address), _size(size), _type(type), _name(name), _isarray(isarray ){}

////----<construct a deallocating log with the memory address>------------------------------
inline DeallocateLog::DeallocateLog(unsigned int address) :_address(address){}


}




#endif