/////////////////////////////////////////////////////////////////////
//  ExcutiveTest.cpp - demonstrating module for project3           //
//  ver 1.0                                                        //
//                                                                 //
//  Language:     Visual C++ 2008                                  //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project3 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////

#include "ClassInspector.h"
#include "Annotator.h"
using namespace LoggingProccessing;


#ifdef TEST_EXCUTIVE
////----<display the functions detected recorded in the map>--------------------------------
void ShowFunctions(ClassInspector::FunMap& themap)
{
	ClassInspector::FunMap::iterator iter;
	for(iter = themap.begin(); iter != themap.end(); ++iter)
	{
		std::cout<<"\n\n"<<"File: "<<iter->first<<"\n-------------";
		for(size_t i = 0; i < iter->second.size(); ++i)
			std::cout<<"\nFunction Name: "<<iter->second[i].funname;
	}
}
////----<pick up the first function which is not main in the function set>------------------
FunInfo PickupaFunction(ClassInspector::FunMap& themap)
{
	ClassInspector::FunMap::iterator iter;
	for(iter = themap.begin(); iter != themap.end(); ++iter)
	{
		for(size_t i = 0; i < iter->second.size(); ++i)
			if(iter->second[i].funname.find("main") == std::string::npos)
				return iter->second[i];
	}
	FunInfo failtofind = {0,"",""};
	return failtofind;
}
////----<display the interpretation for requirements 1 - 3>---------------------------------
void DisplayTest1()
{
	std::cout<<"====================\nRequirement 1 to 2:"
		<<"\n Please check my code to see I have sutisfied them\n========================";
	std::cout<<"\n\n====================\nRequirement 3:"
		<<"\n The following program will: "
		<<"\na. declare if the presence of manual and maintenance pages"
		<<"\nb. declare if a header file has #ifndef and #define preprocessor statements"
		<<"\nc. declare if a implementation file has test stub with #ifdef TEST preprocessor statement"
		<<"\nd. declare if there is non-const global data"
		<<"\ne. declare if a non-default constructor does not use initialization lists"
		<<"\nf. declare if there is no virtual destructor in a class which has virtual functions"
		<<"\ng. declare if there are any overloads of virtual functions or across class scopes"
		<<"\n========================";
}
////----<display the interpretation for requirements 4,5,8>---------------------------------
void DisplayTest2()
{
	::system("pause");
	std::cout<<"\n====================\nRequirement 4:"
		<<"\n Please see my source code in Loggers.h for this requirement\n========================";
	std::cout<<"\n====================\nRequirement 5:"
		<<"\n Please see my source code in LogManager.h, LogManager.cpp, Inspector.h, "
		<<"\nInspector.cpp for this requirement\n========================";
	std::cout<<"\n====================\nRequirement 8:"
		<<"\nInclude or exclude a subset of inspected functions for logging\n========================";
	std::cout<<"\n Inspected Functions: ";
}

////----<the main function>-----------------------------------------------------------------
int main(int argc, char* argv[])
{
	if(argc < 4)
	{
		std::cout<<"Please enter correct arguments!!\n";
		return 1;
	}
	
	DisplayTest1();
	ClassInspector clsIntr;
	for(int i = 3; i < argc; ++i)
		clsIntr.addFile(argv[i]);
	clsIntr.Analyze();
	std::cout<<clsIntr.Show();
	DisplayTest2();
	Annotator ant(clsIntr.getFunctions(),argv[1],argv[2]);
	ShowFunctions(ant.GetFunctions());
	FunInfo testfun1 = PickupaFunction(ant.GetFunctions());
	ant.RemoveFunction(testfun1);
	FunInfo testfun2 = PickupaFunction(ant.GetFunctions());
	ant.RemoveFunction(testfun2);
	FunInfo testfun3 = PickupaFunction(ant.GetFunctions());
	ant.RemoveFunction(testfun3);
	std::cout<<"\nExclude Functions: \n"<<testfun1.funname<<"\n"
		<<testfun2.funname<<"\n"<<testfun3.funname<<"\n";
	std::cout<<"\n !!!!!!!!!!!!!!!!!!!!!!!!!!\nThe rest functions are:";
	ShowFunctions(ant.GetFunctions());
	std::vector<FunInfo> funs;
	funs.push_back(testfun1);
	funs.push_back(testfun2);
	ant.AddFunctions(funs);
	std::cout<<"\nInclude Functions: \n"<<testfun1.funname<<"\n"
		<<testfun2.funname<<"\n";
	std::cout<<"\n!!!!!!!!!!!!!!!!!!!!!!!!!!\nThe rest functions are: ";
	ShowFunctions(ant.GetFunctions());
	ant.Annotate();
	std::cout<<"\n====================\nRequirement 6,7:"
		<<"\n Please see the generated source code and log for ths requirement\n========================";
	::system("pause");
}

#endif