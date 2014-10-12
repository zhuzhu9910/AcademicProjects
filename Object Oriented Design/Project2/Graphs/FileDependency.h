#ifndef FILEDEPENDENCY_H
#define FILEDEPENDENCY_H
/////////////////////////////////////////////////////////////////////
//  FileDependency.h - analyzing file and package dependencies and //
//                put the analyzed information in the classes      //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project2 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations:
  ==================
  Provides a FileDependency class to read information from a file and
  find the "include" heads to recognize the dependency relationship of
  this file to other files.Provides a package class to hold relating 
  file dependency relationships to store the package relationship information.
  Provide a public interface for dependency classes for operations of 
  getting the names and check the dependency relationships.
  
  Public Interface:
  =================
  FileDependency fd("..\\Graphs\\GraphFromXML.h");              //create FileDependency class with an attached file
  FileDependency fds("..\\Graphs\\GraphFromXML.cpp");           //create FileDependency class with an attached file
  fd.AnalyzeDependencies();                                     //find dependencies of the file
  bool isdp = fd.hasDependencyWith("CondensedGraph.h");         //check if the file is depended on CondensedGraph.h
  std::string filename = fd.name();                             //get the file name
  PackageDependency pd(&fd,&fds);                               //create a PackageDependency class with two file dependency objects
  isdp = pd.hasDependencyWith("CondensedGraph.h");              //check if the package is depended on a file    
  std::string packageheadname = pd.DependencyName();            //get the package header file name
  std::string packagename =pd.name()                            //get the package name

  Build Process:
  ==============
  cl /D:TEST_FILEDEPENDENCY FileDependency.h FileDependency.cpp
  
  Maintenance History:
  ====================
  ver 1.0 : 14 Mar 10
  - first release
 */

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////Interface for dependency analyzing classes for some common operations
template<typename T>
struct IDependency
{
	virtual bool hasDependencyWith(const T& fname) = 0;
	virtual T name() = 0;
	virtual T DependencyName() = 0;
	virtual T GetEdgeValue(const T& fname) = 0;
	virtual ~IDependency(){};
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////class to analyze dependency relashionships of a file and store relative information
class FileDependency :public IDependency<std::string>
{
public:
	FileDependency(const std::string& filename);
	~FileDependency();
	bool AnalyzeDependencies();
	std::string& operator [] (size_t pos);
	size_t size();
	std::string name();
	std::string DependencyName();
	std::string GetEdgeValue(const std::string& fname);
	bool hasDependencyWith(const std::string& fname);
	size_t findDependency(const std::string& depfile);
	static std::string getNameFromPath(const std::string& filepath);
	static std::string getNameWithoutExp(const std::string& fname);
private:
	FileDependency(const FileDependency& fdp);
	FileDependency& operator = (const FileDependency& fdp);
	std::string _name;
	std::string path;
	std::vector<std::string> dependencies;
	std::ifstream* pIn;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////class to store package dependency information
class PackageDependency : public IDependency<std::string>
{
public:
	PackageDependency(FileDependency* headfile, FileDependency* sourcefile = 0);
	std::string DependencyName();
	std::string name();
	bool hasDependencyWith(const std::string& fname);
	bool reAnalyzeDependencies();
	std::string GetEdgeValue(const std::string& fname);
	FileDependency* getheadfile();
	FileDependency* getsourcefile();
private:
	FileDependency* header;
	FileDependency* source;
};

#endif