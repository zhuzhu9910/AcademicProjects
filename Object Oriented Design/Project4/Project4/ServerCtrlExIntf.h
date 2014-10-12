#ifndef SERVERCTRLEXINTF_H
#define SERVERCTRLEXINTF_H
/////////////////////////////////////////////////////////////////////
//  ServerCtrlExIntf.h - Interface for configuring the server end  //
//                  and factory for generating the configurer      //                 
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project4 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module defines a cell class for controling and organizing the 
  function cells in the server end. It receives the command from the 
  user, processes and routes the messages. 
  
  Public Interface:
  =================
  ServerCtrlExIntf* psce = CreateServerCtrl();  //create an instance of the client end configurer
  psce->build();                                //build the server cells
  psce->Start();                                //start the server
  std::cout<<psce->getInfo();                   //get one line of the report to display

  Build Process:
  ==============
  Build commands (either one)
    - devenv Project4.sln

  Maintenance History:
  ====================
  ver 1.0 : 27 Apr 10
  - first release

*/
//
#include <string>
//////////////////////////////////////////////////////////////////////////////////////////
////Interface of the server end cells configurer
struct ServerCtrlExIntf
{
	virtual bool build() =0;
	virtual void Start() = 0;
	virtual std::string getInfo() = 0;
	virtual unsigned int infosize() = 0;
	virtual ~ServerCtrlExIntf(){}

};

////----<create an instance of the configure and return the pointer to the interface>-----
////the declaration
	ServerCtrlExIntf* CreateServerCtrl();


#endif