#ifndef INSPECTOR_H
#define INSPECTOR_H
/////////////////////////////////////////////////////////////////////
//  Inspector.h - declares inspectors for logging functions and    //
//         new and delete                                          //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project3 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module defines classes for inspect and trace specified objects.
  Function inspector trace the calling of functions. Allocator inspector
  traces the allocate and deallocate memory and detect the memory leak.
  The new and delete operators are overloaded in this module for tracing
  purpose.
  
  Public Interface:
  =================
  FunctionInspector("void main()");              //create a function inspector
  int* pint = new("int","pint") int;             //call the overloaded new operator
  delete pint;

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
#include <iostream>
#include <string>


namespace LoggingProccessing
{

////////////////////////////////////////////////////////////////////////////////////////////
////Base class of Inspectors sharing a function to get system time
class Inspector
{
protected:
	std::string getCurTime();
};

////////////////////////////////////////////////////////////////////////////////////////////
////Function inspector class for tracing the calling and leaving a function
class FunctionInspector :public Inspector
{
public:
	FunctionInspector(const std::string& funname);
	~FunctionInspector();
private:
	std::string _funname;
};
////////////////////////////////////////////////////////////////////////////////////////////
////Allocator inspector class for recording the allocate and deallocate operations
class AllocatorInspector: public Inspector
{
public:
	void logallocate(unsigned int address, size_t size, const std::string& type, const std::string& name,bool isarray =false);
	void logdeallocate(unsigned int address);
};



}

////////////////////////////////////////////////////////////////////////////////////////////
////Declaration of overloaded operators
void* operator new(size_t size,const std::string& type, const std::string& name);
void* operator new[](size_t size,const std::string& type, const std::string& name);
void operator delete(void* pt);
void operator delete[](void* pt);


#endif