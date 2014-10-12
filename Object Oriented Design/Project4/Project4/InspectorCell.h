#ifndef INSPECTORCELL_H
#define INSPECTORCELL_H
/////////////////////////////////////////////////////////////////////
//  InspectorCell.h - open and send back the content of a file or  //
//                  create a file                                  //
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
  This module defines a cell class for inspecting the input file text
  from a message

  Public Interface:
  =================
  InspectorCell intc;                                             //create an instance of the inspector cell
  EndPoint endp("127.0.0.0",0);                                   //message return address
                         //create an instance of the message with path of the file
  Message msg("FilePath:ttt.cpp?FileName:ttt.cpp?From:main?To:Inspector?%",endp,Message::MsgType::file);
  msg.body() = Cell::enCode(msg.body());
  intc.postMessage(msg);                                          //post the message                                                      

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
#include "Cell.h"

//////////////////////////////////////////////////////////////////////////////////////////
////Class of the cell to inspect a source file
class InspectorCell :public Cell
{
public:
	InspectorCell();
private:
	bool CellFunction(Message& msg);
};


#endif