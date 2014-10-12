#ifndef CLIENTCTRLEXINTF_H
#define CLIENTCTRLEXINTF_H
/////////////////////////////////////////////////////////////////////
//  ClientCtrlExIntf.h - Interface for configuring the client end  //
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
  function cells in the client end. It receives the command from the 
  user, processes and routes the messages. 
  
  Public Interface:
  =================
  ClientCtrlExIntf* pcce = CreateClientCtrl();  //create an instance of the client end configurer
  pcce->build();                                //build the client cells
  pcce->setServer(some address);                //set the address of the server
  pcce->Connect()                               //connect to the server
  pcce->InspectFile(some file path);            //send a file to server to inspect
  std::cout<<pcce->getInfo();                   //get one line of the report to display
  pcce->DisConnect()                            //disconnect the connection

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
////Interface of the client end cells configurer
struct ClientCtrlExIntf
{
	virtual void InspectFile(std::string filepath) = 0;
	virtual std::string getInfo() = 0;
	virtual bool build() = 0;
	virtual void setServer(std::string addr) = 0;
	virtual unsigned int infosize() = 0;
	virtual void Connect() = 0;
	virtual void DisConnect() = 0;
	virtual ~ClientCtrlExIntf(){}
	
};

////----<create an instance of the configure and return the pointer to the interface>-----
////the declaration
ClientCtrlExIntf* CreateClientCtrl();

#endif