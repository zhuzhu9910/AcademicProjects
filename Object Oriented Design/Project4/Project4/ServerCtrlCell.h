#ifndef SERVERCTRLCELL_H
#define SERVERCTRLCELL_H
/////////////////////////////////////////////////////////////////////
//  ServerCtrlCell.h - The cell for control in the server end      //
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
  SenderCell sdc;                           //create an instance of the sender cell
  DisplayCell dsc;                          //create an instance of the display cell
  ReceiverCell rvc;                         //create an instance of the receiver cell
  FileTransCell ftc;                        //create an instance of the file transfer cell
  InspectorCell itc;                        //create an instance of the inspector cell
  ServerCtrlCell scc;                       //create an instance of the server control cell
  sdc.addMsgPass(&scc);                     //add the interface of the server control cell
  dsc.addMsgPass(&scc);                     //add the interface of the server control cell
  rvc.addMsgPass(&scc);                     //add the interface of the server control cell
  ftc.addMsgPass(&scc);                     //add the interface of the server control cell
  itc.addMsgPass(&scc);                     //add the interface of the server control cell
  scc.addMsgPass(&sdc);                     //add the interface of the sender cell
  scc.addMsgPass(&dsc);                     //add the interface of the display cell
  scc.addMsgPass(&rvc);                     //add the interface of the receiver cell
  scc.addMsgPass(&ftc);                     //add the interface of the file transfer cell
  scc.addMsgPass(&itc);                     //add the interface of the inspector cell
  EndPoint endp("127.0.0.1",0);                                                  //any address and port is ok
  Message msg("Command:Start?%",endp,Message::text);                             //the command message to start instening in the server port
  msg.body() = Cell::enCode(msg.body());                                         //encode the message
  scc.postMessage(msg);                                                          //post the message to the cell

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
////Class for controling and routing in the server end
class ServerCtrlCell :public Cell
{
public:
	ServerCtrlCell();
private:
	bool CellFunction(Message& msg);
	bool ErrorHandler(Message& msg);
	void GenerateFile(Message& msg);
	void InspectFile(Message& msg);
	void RecordResult(Message& msg);
};


#endif