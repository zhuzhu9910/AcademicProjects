/////////////////////////////////////////////////////////////////////
//  ClientCtrlCell.cpp - The cell for control in the client end    //
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project4 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
#include "ClientCtrlCell.h"
#include "ReceiverCell.h"
#include "SenderCell.h"
#include "FileTransCell.h"
#include "DisplayCell.h"

////----<constructor of the class initializing its particular name>-------------------------
ClientCtrlCell::ClientCtrlCell() :Cell("ClientCtrl"),isprocess(false){}

////----<cope with and report the errors from the functional cells>-------------------------
bool ClientCtrlCell::ErrorHandler(Message &msg)
{
	std::string err;
	if((err = Cell::getField("ERROR:",msg.body())) != "")
	{
		this->returnMsg(std::string("ERROR: ").append(err),msg.returnAddress(),"Display",false);
		return true;
	}
	return false;
}

////----<processing and dispatch the messages>----------------------------------------------
bool ClientCtrlCell::CellFunction(Message &msg)
{
	msg.body() = this->deCode(msg.body());
	std::string msgbody = Cell::getBody(msg.body());
	//receive the commands
	if(this->ClientCommand(msg))
		return true;
	//get the message returned from the file transer and now send the file to server
	if((Cell::getField("From:",msg.body()) == "FileTransfer"))
	{
		this->InspectRequest(msg);
		return true;
	}
	//get the report from the server and send it to the display cell for displaying
	if(!ErrorHandler(msg) && Cell::getField("From:",msg.body()) == "Receiver" && Cell::getField("Command:",msgbody) == "Record")
	{
		ReceiveReport(msg,msgbody);
		return true;
	}
	return true;
}

////----<send a command to the file transfer cell to get the content of a file>-------------
void ClientCtrlCell::OpenFile(Message &msg)
{
	std::string addr;
	std::string body = this->genAddrHead(this->Name(),"FileTransfer");
	body.append("Command:OpenFile?FilePath:").append(Cell::getField("FilePath:",msg.body()));
	body.append("?%").append(Cell::getBody(msg.body()));
	this->returnMsg(body,msg.returnAddress(),"FileTransfer",false);
}

////----<send the file to the server using the sender cell for file inspection>-------------
void ClientCtrlCell::InspectRequest(Message &msg)
{
	std::string bk = "From:ClientCtrl?To:Sender?";
	if(!ErrorHandler(msg))
	{
		bk.append("FileName:").append(Cell::getField("FileName:",msg.body())).append("?");
		bk.append("Operation:Send?Command:").append("Inspect?%").append(Cell::getBody(msg.body()));
	}
	else
	{
		bk.append("ERROR:").append(Cell::getField("ERROR:",msg.body())).append("?%");
	}
	this->returnMsg(bk,msg.returnAddress(),"Sender",(msg.getMsgType() == Message::file));
	//this->returnMsg("From:ClientCtrl?To:Sender?Operation:DisConnect?%",msg.returnAddress(),"Sender",false);
}

////----<receive the report from server and display it>-------------------------------------
void ClientCtrlCell::ReceiveReport(Message &msg,const std::string& msgbody)
{
	//display report
	std::string title = "Result of Inspecting ";
	title.append(Cell::getField("FileName:",msgbody));
	title.append("\n==========================");
	this->returnMsg(title,msg.returnAddress(),"Display",false);
	this->returnMsg(Cell::getBody(msgbody),msg.returnAddress(),"Display",false);
	this->isprocess = false;
}

////----<receive and process commands for the clients>--------------------------------------
bool ClientCtrlCell::ClientCommand(Message &msg)
{
	//open the specified file and send to the server
	if(Cell::getField("Command:",msg.body()) == "InspectFile")
	{
		if(!this->isprocess)
		{
			this->OpenFile(msg);
			this->isprocess =true;
		}
		else
		{
			msg.body() = Cell::enCode(msg.body());
			this->postMessage(msg);
		}
		return true;
	}
	//connect to the server
	if(Cell::getField("Command:",msg.body()) == "Connect")
	{
		this->returnMsg("From:ClientCtrl?To:Receiver?Operation:Listen?%",EndPoint(msg.returnAddress().address(),CLIENT_PORT),"Receiver",false);
		this->returnMsg("From:ClientCtrl?To:Sender?Operation:Connect?%",msg.returnAddress(),"Sender",false);
		return true;
	}
	//disconnect current connection to the server
	if(Cell::getField("Command:",msg.body()) == "DisConnect")
	{   //only when all specified file has been processed and results reported,
		//disconnect the connection or repost the command
		if(!this->isprocess && this->isInQEmpty())
			this->returnMsg("From:ClientCtrl?To:Sender?Operation:DisConnect?%",msg.returnAddress(),"Sender",false);
		else
		{   
			msg.body() = Cell::enCode(msg.body());
			this->postMessage(msg);
		}
		return true;
	}
	return false;
}

////----<test stub>-------------------------------------------------------------------------
#ifdef TEST_CLIENTCTRLCELL
void main()
{
	
	SenderCell sdc;
	DisplayCell dsc;
	ReceiverCell rvc;
	FileTransCell ftc;
	ClientCtrlCell ccc;
	sdc.addMsgPass(&ccc);
	dsc.addMsgPass(&ccc);
	rvc.addMsgPass(&ccc);
	ftc.addMsgPass(&ccc);
	ccc.addMsgPass(&sdc);
	ccc.addMsgPass(&dsc);
	ccc.addMsgPass(&rvc);
	ccc.addMsgPass(&ftc);
	EndPoint endp("127.0.0.1",SERVER_PORT);
	Message msg("Command:InspectFile?FilePath:../test.cpp?%",endp,Message::text);
	msg.body() = Cell::enCode(msg.body());
	ccc.postMessage(msg);
	Message msg1("Command:InspectFile?FilePath:../test.h?%",endp,Message::text);
	msg1.body() = Cell::enCode(msg1.body());
	ccc.postMessage(msg1);
	ccc.Suspend();
	ccc.Resume();
	while(true)
	{
		std::cout <<dsc.getOutputInfo();
	}
	::system("pause");

}

#endif