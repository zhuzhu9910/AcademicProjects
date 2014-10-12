#ifndef CLIENTCTRLCELL_H
#define CLIENTCTRLCELL_H
/////////////////////////////////////////////////////////////////////
//  ClientCtrlCell.h - The cell for control in the client end      //
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
  SenderCell sdc;                           //create an instance of the sender cell
  DisplayCell dsc;                          //create an instance of the display cell
  ReceiverCell rvc;                         //create an instance of the receiver cell
  FileTransCell ftc;                        //create an instance of the file transfer cell
  ClientCtrlCell ccc;                       //create an instance of the client control cell
  sdc.addMsgPass(&ccc);                     //add the interface of the client control cell
  dsc.addMsgPass(&ccc);                     //add the interface of the client control cell
  rvc.addMsgPass(&ccc);                     //add the interface of the client control cell
  ftc.addMsgPass(&ccc);                     //add the interface of the client control cell
  ccc.addMsgPass(&sdc);                     //add the interface of the sender cell
  ccc.addMsgPass(&dsc);                     //add the interface of the display cell
  ccc.addMsgPass(&rvc);                     //add the interface of the receiver cell
  ccc.addMsgPass(&ftc);                     //add the interface of the file transfer cell
  EndPoint endp("127.0.0.1",SERVER_PORT);                                        //define the server address
  Message msg("Command:InspectFile?FilePath:../test.cpp?%",endp,Message::text);  //the command message for requesting inspecting a file
  msg.body() = Cell::enCode(msg.body());                                         //encode the message
  ccc.postMessage(msg);                                                          //post the message to the cell

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

////////////////////////////////////////////////////////////////////////////////////////////
////Class for controling and routing in the client end
class ClientCtrlCell :public Cell
{
public:
	ClientCtrlCell();
private:
	bool CellFunction(Message& msg);
	bool ErrorHandler(Message& msg);
	void OpenFile(Message& msg);
	void InspectRequest(Message& msg);
	void ReceiveReport(Message& msg,const std::string& msgbody);
	bool ClientCommand(Message& msg);
	bool isprocess;
};



#endif