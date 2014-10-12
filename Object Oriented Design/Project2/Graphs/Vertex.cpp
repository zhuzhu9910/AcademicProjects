/////////////////////////////////////////////////////////////////////
//  Vertex.cpp - store information of vertexes for a graph         //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project2 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////

#include "Vertex.h"
#include <string>
#include <iostream>
using namespace zqyG;

////----<test stub>-----------------------------------------------------------------------------------------
#ifdef TEST_VERTEX
template <typename VT, typename ET>
void WalkGraph(zqyG::Vertex<VT,ET> vex[], size_t VSize)
{
	for (size_t i = 0; i<VSize; ++i)
	{
		std::cout<<vex[i].Value()<<" ";
		for(size_t j = 0; j < vex[i].size(); ++j)
			std::cout<<"("<<vex[i][j].first<<","<<vex[i][j].second<<"), ";
		std::cout<<std::endl;
	}
}


void main()
{
	const int VexSize = 6;
	zqyG::Vertex<std::string, std::string> vex[VexSize] = 
	   {zqyG::Vertex<std::string, std::string>("NO.1 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.2 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.3 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.4 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.5 Vertex"),
		zqyG::Vertex<std::string, std::string>("NO.6 Vertex")};
	
	
	vex[0].addEdge(1,"e1"); 
	vex[0].addEdge(2,"e2");  
	vex[0].addEdge(3,"e3");
	vex[1].addEdge(2,"e4");
	vex[1].addEdge(4,"e6");
	vex[1].addEdge(5,"e7");
	vex[2].addEdge(5,"e8");
	
	WalkGraph<std::string, std::string>(vex, VexSize);
	
	zqyG::Vertex<std::string, std::string>& vex2 = vex[2];
	vex[1].removeEdge(1);
	for(size_t i = 0; i<VexSize; ++i)
		vex[i].isMarked() = false;
	std::cout<<"\n\n";
	vex[1].removeEdge(2);
	zqyG::Vertex<std::string, std::string> vexc = vex[1];
	WalkGraph<std::string, std::string>(vex, VexSize);
	std::cout<<"\n\n";
	::system("pause");
}

#endif