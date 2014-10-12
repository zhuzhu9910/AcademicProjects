/////////////////////////////////////////////////////////////////////
//  ClassInspector.cpp - check a C++ source code against a set of  //
//         a plugable rule                                         //
//  ver 1.0                                                        //
//                                                                 //
//  Language:     Visual C++ 2008                                  //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project3 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
#include "ClassInspector.h"
#include <string>
#include <iostream>

////----<constructor for initializing the parse configure>----------------------------------
ClassInspector::ClassInspector() :pConfigParseForClassInspector(0)
{
	try
	{
		pConfigParseForClassInspector = new ConfigParseForClassInspector;
	}
	catch (std::exception &ex)
	{
		std::cout << "\n\n    " << ex.what() << "\n\n";
		delete pConfigParseForClassInspector;
		pConfigParseForClassInspector = 0;
		throw ex;
		return;
	}
}

////----<deconstructor of ClassInspector>---------------------------------------------------
ClassInspector::~ClassInspector()
{
	delete pConfigParseForClassInspector;
}

////----<add a file for analysis>-----------------------------------------------------------
void ClassInspector::addFile(const std::string& filename)
{
	std::pair<std::string,ClassInspector::RuleRecords> finfo;
	finfo.first = filename;
	files.push_back(finfo);
}

////----<return the detected functions>-----------------------------------------------------
ClassInspector::FunMap& ClassInspector::getFunctions()
{
	return pConfigParseForClassInspector->getFunctions();
}

////----<analyze the files>-----------------------------------------------------------------
void ClassInspector::Analyze()
{
  for(size_t i = 0; i < files.size(); ++i)
  {

	Parser* pParser = pConfigParseForClassInspector->Build();

    try
    {
      if(pParser)
      {
        if(!pConfigParseForClassInspector->Attach(files[i].first))
        {
			std::cout << "\n  could not open file " << files[i].first << std::endl;
          continue;
        }
      }
      else
      {
        std::cout << "\n\n  Parser not built\n\n";
        return;
      }
      // now that parser is built, use it

      while(pParser->next())
        pParser->parse();
		
	  files[i].second = pConfigParseForClassInspector->getRuleRecords();
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
  }
}

////----<get the records of rule violations>------------------------------------------------
std::vector<std::pair<std::string,ClassInspector::RuleRecords>>& ClassInspector::getRecords()
{
	return files;
}

////----<get the rule records in a form of string for displaying>---------------------------
std::string ClassInspector::Show()
{
	std::string rst;
	for(size_t i = 0; i < files.size(); ++i)
	{
		rst.append("\n  Processing file ").append(files[i].first);
		rst.append( "\n  ").append( std::string(16 + files[i].first.length(),'-'));
		for(size_t j = 0; j < files[i].second.size(); ++j)
		{
			for(size_t z = 0; z < files[i].second[j].records.size(); ++z)
				rst.append("\n").append(files[i].second[j].records[z]);
		}
		rst.append("\n");
	}
	return rst;
}

////----<test stub>-------------------------------------------------------------------------
#ifdef TEST_CLASSINSPECTOR

void ShowFunctions(ClassInspector::FunMap& themap)
{
	ClassInspector::FunMap::iterator iter;
	for(iter = themap.begin(); iter != themap.end(); ++iter)
	{
		std::cout<<"\n\n"<<"File: "<<iter->first<<"\n-------------";
		for(size_t i = 0; i < iter->second.size(); ++i)
			std::cout<<"\nFunction Name: "<<iter->second[i].funname
			<<"\nLine: "<<iter->second[i].funline;
	}
}

void main()
{
	ClassInspector clin;
	clin.addFile("../Project3/Parser.h");
	clin.addFile("../Project3/Parser.cpp");
	clin.Analyze();
	std::vector<std::pair<std::string,ClassInspector::RuleRecords>> rds = clin.getRecords();
	std::cout<<clin.Show();
	ShowFunctions(clin.getFunctions());
}

#endif