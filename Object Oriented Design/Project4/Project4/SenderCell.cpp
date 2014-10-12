/////////////////////////////////////////////////////////////////////
//  Sender.cpp - Connect the specified address and port and send   //
//            messages through soccket.                            //
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project4 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
#include "SenderCell.h"

////----<constructor of the class>--------------------------------------------------------
SenderCell::SenderCell() :Cell("Sender"),psender(0){}
////----<deconstructor of the class>------------------------------------------------------
SenderCell::~SenderCell()
{
	delete psender;
}

////----<connect to the address and port specified in the message>------------------------
void SenderCell::Connect(Message &msg)
{
	if(psender == 0)
		psender = new Sender;
	else
		return;
	this->LocalAddrTransform(msg);
	//if failed then send back the error information
	if(!psender->connect(msg.returnAddress()))
	{
		std::string errbk = this->genAddrHead(Name(),Cell::getField("From:",msg.body()));
		errbk.append("ERROR: Cannot Connect to EndPoint?%");
		this->returnMsg(errbk,msg.returnAddress(),Cell::getField("From:",msg.body()),false);
		delete psender;
		psender = 0;
	}
}

////----<disconnect to the address and send the "quit" command to make the receiver quit>-
void SenderCell::DisConnect(Message &msg)
{
	if(psender)
	{
		psender->PostMsg(Message(Cell::enCode("quit")));
		psender->wait();
		delete psender;
		psender = 0;
	}
}

////----<send message to the connected address>-------------------------------------------
void SenderCell::Send(Message &msg)
{
	if(psender)
	{
		std::string sdbody = this->genAddrHead("Sender","Receiver");
		sdbody.append("?%").append(msg.body());
		EndPoint myend(psender->myIP(),msg.returnAddress().port());
		sdbody = this->enCode(sdbody);
		Message sdmsg(sdbody,myend,msg.getMsgType());
		psender->PostMsg(sdmsg);
	}
	else
	{
		std::string errbk = this->genAddrHead(Name(),Cell::getField("From:",msg.body()));
		errbk.append("ERROR: No Connection?%");
		this->returnMsg(errbk,msg.returnAddress(),Cell::getField("From:",msg.body()),false);
	}
}

////----<take sending actions according to the input message>-----------------------------
bool SenderCell::CellFunction(Message &msg)
{
	msg.body() = this->deCode(msg.body());
	std::string opter = Cell::getField("Operation:",msg.body());

	if(opter == "Connect")
		this->Connect(msg);
	if(opter == "Send")
		this->Send(msg);
	if(opter == "DisConnect")
		this->DisConnect(msg);
	return true;
}

////----<if the destination address is local then chage it to loopback address>-----------
void SenderCell::LocalAddrTransform(Message &msg)
{
	if(this->psender && msg.returnAddress().address() == this->psender->myIP())
	{
		msg.returnAddress() = EndPoint("127.0.0.1",msg.returnAddress().port());
	}
}

////----<test stub>-----------------------------------------------------------------------

#ifdef TEST_SENDER
void main()
{
	Cell cell("main1");
	SenderCell sender;
	sender.addMsgPass(&cell);
	EndPoint endp("127.0.0.1",8000);
	Message msg0("From:main?To:Sender?Operation:Connect?Command:Haha?%Hello!",endp,Message::text);
	Message msg1("From:main?To:Sender?Operation:Send?Command:Haha?%Hello!",endp,Message::text);
	Message msg2("From:main?To:Sender?Operation:Send?Command:quit?%Hello2!",endp,Message::text);
	Message msg3("From:main?To:Sender?Operation:DisConnect?Command:quit?%Hello2!",endp,Message::text);
	sender.postMessage(msg0);
	sender.postMessage(msg1);
	sender.postMessage(msg2);
	sender.postMessage(msg3);
	::Sleep(5000);
	::system("pause");

}
#endif