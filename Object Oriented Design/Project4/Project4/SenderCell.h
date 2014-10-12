#ifndef SENDERCELL_H
#define SENDERCELL_H
/////////////////////////////////////////////////////////////////////
//  Sender.h - Connect the specified address and port and send     //
//            messages through soccket.                            //
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
  This module defines a cell class for sending message through socket. 
  It connects, sends and disconnects to the address according to the 
  input command messages.

  Public Interface:
  =================
  SenderCell sender;                                    //create an instance of the sender cell
  EndPoint endp("127.0.0.1",8000);                      //the address the sender will connect to
  //command message ot connect to the specified address
  Message msg0("From:main?To:Sender?Operation:Connect?Command:Haha?%Hello!",endp,Message::text);
  //message send to the address
  Message msg1("From:main?To:Sender?Operation:Send?Command:Haha?%Hello!",endp,Message::text);
  //message send to the address
  Message msg2("From:main?To:Sender?Operation:Send?Command:quit?%Hello2!",endp,Message::text);
  //command message to disconnect
  Message msg3("From:main?To:Sender?Operation:DisConnect?Command:quit?%Hello2!",endp,Message::text);
  sender.postMessage(msg0);                             //post the message to the sender
  sender.postMessage(msg1);                             //post the message to the sender
  sender.postMessage(msg2);                             //post the message to the sender
  sender.postMessage(msg3);                             //post the message to the sender

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
////Class of the cell to send messages through socket
class SenderCell :public Cell
{
public:
	SenderCell();
	~SenderCell();
private:
	bool CellFunction(Message& msg);
	void Connect(Message& msg);
	void DisConnect(Message& msg);
	void Send(Message& msg);
	void LocalAddrTransform(Message& msg);
	Sender* psender;
};



#endif