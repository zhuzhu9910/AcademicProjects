/////////////////////////////////////////////////////////////////////
//  Inspector.cpp - declares inspectors for logging functions and  //
//         new and delete                                          //
//  ver 1.0                                                        //
//                                                                 //
//  Language:     Visual C++ 2008                                  //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project3 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
#include "Inspector.h"
#include "windows.h"
#include "LogManager.h"
using namespace LoggingProccessing;

////----<constructor of function inspector for logging the calling of a function>-----------
FunctionInspector::FunctionInspector(const std::string &funname) :_funname(funname)
{
	std::string evtlog;
	evtlog.append(funname).append(" invoked");
	FunctionLog flog(evtlog,getCurTime());
	LogManagerRegister::Log(&flog);
}
////----<deconstruct the function inspector and log the leaving of a function>--------------
FunctionInspector::~FunctionInspector()
{
	std::string evtlog;
	evtlog.append(_funname).append(" ended");
	FunctionLog flog(evtlog,getCurTime());
	LogManagerRegister::Log(&flog);
}

////----<get the current system time>-------------------------------------------------------
std::string Inspector::getCurTime()
{
	SYSTEMTIME st;
	::GetLocalTime(&st);
	std::ostringstream stime;
	stime<<st.wHour<<":"<<st.wMinute<<":"<<st.wSecond<<":"<<st.wMilliseconds;
	return stime.str();
}

////----<record the address, size of the allocated memory and the pointer name and type>----
void AllocatorInspector::logallocate(unsigned int address, size_t size, const std::string &type, const std::string &name,bool isarray)
{
	AllocateLog log = AllocateLog(address,size,type,name,isarray);
	log.setTime(getCurTime());
	LogManagerRegister::Log(&log);
}
////----<record the deallocation operation and the deallocated address >--------------------
void AllocatorInspector::logdeallocate(unsigned int address)
{
	DeallocateLog log = DeallocateLog(address);
	log.setTime(getCurTime());
	LogManagerRegister::Log(&log);
}

////----<overloaded new operator with type and name input>----------------------------------
void* operator new(size_t size,const std::string& type, const std::string& name)
{
	void* add = malloc(size);
	AllocatorInspector all;
	all.logallocate((unsigned int)add,size,type,name);
	return add;
}
////----<overloaded new[] operator with type and name input>--------------------------------
void* operator new[](size_t size,const std::string& type, const std::string& name)
{
	void* add = malloc(size);
	AllocatorInspector all;
	all.logallocate((unsigned int)add,size,type,name,true);
	return add;
}

////----<overloaded delete operator>--------------------------------------------------------
void operator delete(void* pt)
{
	AllocatorInspector dall;
	dall.logdeallocate((unsigned int)pt);
	free(pt);
}
////----<overloaded delete[] operator>------------------------------------------------------
void operator delete[](void* pt)
{
	AllocatorInspector dall;
	dall.logdeallocate((unsigned int)pt);
	free(pt);
}

////----<test stub>-------------------------------------------------------------------------
#ifdef TEST_INSPECTOR

void main()
{
	LogManagerRegister::CreateManager("../log-inspector.txt");
	FunctionInspector("void main()");
	int* pint = new("int","pint") int;
	int* pinta = new("int","pinta") int[3];
	delete pint;
	delete[] pinta;
}
#endif