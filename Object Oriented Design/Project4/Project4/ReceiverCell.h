#ifndef RECEIVERCELL_H
#define RECEIVERCELL_H
/////////////////////////////////////////////////////////////////////
//  ReceiverCell.h - Listen to a port and receive messages from    //
//                    soccket.                                     //
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
  This module defines a cell class for listening to a port and receive
  messages from the socket.

  Public Interface:
  =================
  ReceiverCell Revcc;                                                       //create an instance of the Reciever
  EndPoint endp("127.0.0.1",8000);                                          //define the listening port
  Message msg("From:main?To:Receiver?Operation:Listen?%",endp,Message::text);//create a message to ask the Receiver "Listen" to the specified port
  msg.body() = Cell::enCode(msg.body());                                     //encode the message body
  Revcc.postMessage(msg);                                                    //post the messge to the cell

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
#include "../Communicator/Comm.h"

//////////////////////////////////////////////////////////////////////////////////////////
////Class of the receiver cell for receiving messages from socket
class ReceiverCell :public Cell
{
public:
	ReceiverCell();
private:
	bool CellFunction(Message& msg);
	EndPoint removecon(Message& msg);
};




#endif