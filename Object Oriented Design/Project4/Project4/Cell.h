#ifndef CELL_H
#define CELL_H
/////////////////////////////////////////////////////////////////////
//  Cell.h - Interface for message passing and base class for cells//
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008                                  //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project4 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module defines a interface for message passing and a base class to provide
  some basic operations for functional cells using a message passing mechanism to
  communicate.
  The messages passing amoung the cells have to follow the form that
  From:XXX?To:XXXX?OtherFields:XXX?%....
  For example, From: is the name of the From field and XXX is standard for the value
  of this field and ? is the end mark of this field. The last % is end mark of the
  header and the following part of the % is the body of message content.
  This module provide the operations for geting the values of the header, body of the
  message and encoding and decoding the message content.
  
  Public Interface:
  =================
  Cell cell1("cell1");                                  //create an instance of Cell
  Cell cell2("cell2");                                  //create an instance of Cell
  cell1.addMsgPass(&cell2);                             //add cell2 to cell1 as a object which cell1 can pass message to
  Message msg("Hello");                                 //create a message instance
  cell1.postMessage(msg);                               //send the message to cell1

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
#include <map>
#include "..\Sockets\Sockets.h"  // must be first
#include "..\Threads\Threads.h"
#include "..\BlockingQueue\BlockingQueue.h"
#include "..\Communicator\Message.h"
#define DEFAULT_PORT 8500
#define CLIENT_PORT 8200
#define SERVER_PORT 8000


////////////////////////////////////////////////////////////////////////////////////////////
////Interface for message passing
struct IMsgPass
{
	virtual void postMessage(const Message& msg) = 0;
	virtual void addMsgPass(IMsgPass* msgpass) = 0;
	virtual void removeMsgPass(IMsgPass* msgpass) = 0;
	virtual std::string Name() = 0;
	virtual ~IMsgPass(){}
};

////////////////////////////////////////////////////////////////////////////////////////////
////Base class for message passing cells
class Cell :public IMsgPass, public Thread_Processing<Cell>
{
public:
	virtual ~Cell();
	Cell(const std::string& name);
	void run();
	void postMessage(const Message& msg);
	void addMsgPass(IMsgPass* msgpass);
	void removeMsgPass(IMsgPass* msgpass);
	std::string Name();
	void Resume();
	void Suspend();
	static std::string getField(const std::string& field,const std::string& text);
	static std::string getBody(const std::string& text);
	static std::string deCode(const std::string& code);
	static std::string enCode(const std::string& content);

protected:
	typedef std::map<std::string,IMsgPass*> CellMap;
	virtual bool CellFunction(Message& msg);
	std::string genAddrHead(const std::string& from,const std::string& to);
	void returnMsg(const std::string& content,EndPoint& endp,const std::string& to, bool isfile = true);
	bool isInQEmpty();
	//data members
	std::map<std::string,IMsgPass*> locOCells;           //interfaces of other cells for message passing
	thread* pFunThread;                                 
private:
	std::string _name;                                   //name of this cell (it is the unique name for each cell)
	BQueue<Message> inQ;                                 //input message queue
};

#endif