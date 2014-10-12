/////////////////////////////////////////////////////////////////////
//  Cell.cpp - Interface for message passing and base class        //
//         for cells                                               //
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project4 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
#include "Cell.h"
#include <iostream>
#include <string>
#include "Base64.h"

////----<constructor of the Cell class to initialize cell name and the working thread>------
Cell::Cell(const std::string& name)
: pFunThread(0), _name(name)
{
	pFunThread = new thread(*this);
	pFunThread->start();
}

////----<deconstructor to release the thread>-----------------------------------------------
Cell::~Cell()
{
	delete pFunThread;
}

////----<running function for the working thread>-------------------------------------------
void Cell::run()
{
	while(CellFunction(inQ.deQ()));
	
}

////----<post a message into the input message queue>---------------------------------------
void Cell::postMessage(const Message& msg)
{
	inQ.enQ(msg);
}

////----<add a message passing interface of another cell so this cell can pass message to it>
void Cell::addMsgPass(IMsgPass *msgpass)
{
	locOCells[msgpass->Name()] = msgpass;
}

////----<remove a interface pointer keeping in the cell>------------------------------------
void Cell::removeMsgPass(IMsgPass *msgpass)
{
	std::map<std::string,IMsgPass*>::iterator iter = locOCells.find(msgpass->Name());
	if(iter != locOCells.end())
	{
		locOCells.erase(iter);
	}
}

////----<get the name of this cell>---------------------------------------------------------
std::string Cell::Name()
{
	return _name;
}

////----<define the function of this cell, process each input message>----------------------
bool Cell::CellFunction(Message &msg)
{
	msg.body() = this->deCode(msg.body());
	std::cout<<msg.body()<<std::endl;
	if(msg.body() != "12132:232321:3223")
		return true;
	else
		return false;
}

////----<get the value of a specified field of the message head>----------------------------
std::string Cell::getField(const std::string &field,const std::string& text)
{
	size_t endpos = text.find("%");
	size_t flpos = text.find(field);
	if(flpos == std::string::npos || endpos == std::string::npos || endpos < flpos)
		return "";
	size_t start = text.find(":",flpos) + 1;
	size_t end = text.find("?",flpos);
	return text.substr(start,end - start);
}

////----<get the body of the message without head fields>-----------------------------------
std::string Cell::getBody(const std::string &text)
{
	if (text.find("%") == std::string::npos)
		return "";
	return text.substr(text.find("%")+1);
}

////----<generate the address fields for the message header>--------------------------------
std::string Cell::genAddrHead(const std::string &from, const std::string &to)
{
	std::string addr = "From:";
	addr.append(from).append("?").append("To:").append(to).append("?");
	return addr;
}

////----<reply meesage to a cell with the specified name>-----------------------------------
void Cell::returnMsg(const std::string& content,EndPoint& endp,const std::string& to, bool isfile)
{
	Message::MsgType msgtype;
	if(isfile)
		msgtype = Message::file;
	else
		msgtype = Message::text;
	std::string code = this->enCode(content);
	Message rmsg(code,endp,msgtype);
	CellMap::iterator iter = this->locOCells.find(to);
	if(iter != locOCells.end())
		iter->second->postMessage(rmsg);
}

////----<decode a message body>-------------------------------------------------------------
std::string Cell::deCode(const std::string &code)
{
	std::vector<char> cv = Base64::decode(code);
	std::string content;
	for(size_t i = 0; i < cv.size(); ++i)
		content.push_back(cv[i]);
	return content;
}

////----<encode a message body>-------------------------------------------------------------
std::string Cell::enCode(const std::string &content)
{
	std::vector<char> cv;
	for(size_t i = 0; i < content.size(); ++i)
		cv.push_back(content.at(i));
	return Base64::encode(cv);
}

////----<suspend the working thread of the cell>--------------------------------------------
void Cell::Suspend()
{
	pFunThread->suspend();
}

////----<resume the working thread of the cell>---------------------------------------------
void Cell::Resume()
{
	pFunThread->resume();
}

////----<test if input queue is empty>------------------------------------------------------
bool Cell::isInQEmpty()
{
	if(inQ.size() == 0)
		return true;
	return false;
}

////----<test stub>-------------------------------------------------------------------------
#ifdef TEST_CELL

void main()
{
	Cell cell("main");
	Cell cell2("test");
	cell.addMsgPass(&cell2);
	cell.removeMsgPass(&cell2);
	std::cout<<Cell::getField("Type","From:home?To:school?Name:cat?Type:human?%Happy!")<<std::endl;
	std::cout<<Cell::getBody("From:home?To:school?Name:cat?Type:human?%Happy!")<<std::endl;
	Message msg1("12132:232321:3220");
	Message msg2("12132:232321:3221");
	Message msg3("12132:232321:3222");
	Message msg4("12132:232321:3223");
	cell.postMessage(msg1);
	cell.postMessage(msg2);
	cell.postMessage(msg3);
	cell.postMessage(msg4);
	::system("pause");
}

#endif