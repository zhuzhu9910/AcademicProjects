/////////////////////////////////////////////////////////////////////
//  Loggers.cpp - declares an interface of logger and a class of   //
//       function log and allocator log for logging operations     //
//  ver 1.0                                                        //
//                                                                 //
//  Language:     Visual C++ 2008                                  //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project3 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
#include "Loggers.h"
#include <iostream>
#include <sstream>
using namespace LoggingProccessing;
////----<set the logging time>--------------------------------------------------------------
void FunctionLog::setTime(const std::string &stime)
{
	_time = stime;
}
////----<produce a text string for logging>-------------------------------------------------
std::string FunctionLog::getLogRecord()
{
	std::string rd;
	return rd.append(_time).append(" \t").append(_funevent);
}
////----<return the type of the log instance>-----------------------------------------------
std::string FunctionLog::type()
{
	return "FunctionLog";
}
////----<set the logging time>--------------------------------------------------------------
void AllocateLog::setTime(const std::string &stime)
{
	_time = stime;
}
////----<produce a text string for logging>-------------------------------------------------
std::string AllocateLog::getLogRecord()
{
	std::string rd;
	std::ostringstream ostr;
	ostr<<std::hex<<Address();
	rd.append(_time).append("\t");
	//if it is a array, add the text "array of" something to demonstrate it
	if(_isarray)
		rd.append("Array of ");
	rd.append("Type \'").append(AlocType()).append("\' instance \'").append(PraName()).append("\'");
	rd.append(" allocated at address 0x").append(ostr.str());
	return rd;
}
////----<return the type of the log instance>-----------------------------------------------
std::string AllocateLog::type()
{
	return "allocater";
}
////----<get the address of the allocated memory>-------------------------------------------
unsigned int AllocateLog::Address()
{
	return _address;
}
////----<get the parameter name of the pointer>---------------------------------------------
std::string& AllocateLog::PraName()
{
	return _name;
}
////----<get the data type of the allocated memroy>-----------------------------------------
std::string& AllocateLog::AlocType()
{
	return _type;
}

////----<set the logging time>--------------------------------------------------------------
void DeallocateLog::setTime(const std::string &stime)
{
	_time = stime;
}
////----<return the type of the log instance>-----------------------------------------------
std::string DeallocateLog::type()
{
	return "deallocate";
}
////----<produce a text string for logging>-------------------------------------------------
std::string DeallocateLog::getLogRecord()
{
	std::string rd;
	std::ostringstream ostr;
	ostr<<std::hex<<_address;
	rd.append(_time).append("\t").append("Deallocate Memory 0x").append(ostr.str());
	return rd;
}
////----<get the address of the deallocated memory>-----------------------------------------
unsigned int DeallocateLog::Address()
{
	return _address;
}

////----<test stub>-------------------------------------------------------------------------
#ifdef TEST_LOGGERS
void main()
{
	FunctionLog flog("void main() involked", "13:59:12");
	flog.setTime("13:12:12");
	std::cout<<flog.type()<<" " <<flog.getLogRecord().c_str()<<std::endl;
	AllocateLog alll(0,4,"int","pint");
	alll.setTime("13:12:12");
	std::cout<<alll.type()<<" "<<alll.getLogRecord().c_str()<<" "<<alll.AlocType()<<std::endl;
	AllocateLog alall(0,4,"int","pint",true);
	alall.setTime("13:12:12");
	std::cout<<alall.type()<<" "<<alall.getLogRecord().c_str()<<" "<<alall.AlocType()<<std::endl;
	DeallocateLog dl(0);
	dl.setTime("0:0:0:0");
	std::cout<<dl.type()<<" " <<dl.getLogRecord().c_str()<<std::endl;

	::system("pause");
}

#endif