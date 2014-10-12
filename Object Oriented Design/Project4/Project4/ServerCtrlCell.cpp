/////////////////////////////////////////////////////////////////////
//  ServerCtrlCell.cpp - The cell for control in the server end    //
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project4 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
#include "ServerCtrlCell.h"
#include "ReceiverCell.h"
#include "SenderCell.h"
#include "FileTransCell.h"
#include "DisplayCell.h"
#include "InspectorCell.h"

////----<constructor of the class initializing its particular name>-------------------------
ServerCtrlCell::ServerCtrlCell() :Cell("ServerCtrl") {}

////----<processing and dispatch the messages>----------------------------------------------
bool ServerCtrlCell::CellFunction(Message &msg)
{
	msg.body() = this->deCode(msg.body());
	if(Cell::getField("Command:",msg.body()) == "Start")
	{
		this->returnMsg("From:ServerCtrl?To:Receiver?Operation:Listen?%",EndPoint(msg.returnAddress().address(),SERVER_PORT),"Receiver",false);
		return true;
	}
	if(msg.body() == "quit")
	{
		this->returnMsg(this->genAddrHead(Name(),"Sender").append("Operation:DisConnect?%"),EndPoint(msg.returnAddress().address(),CLIENT_PORT),"Sender",false);
		return true;
	}
	this->GenerateFile(msg);
	this->InspectFile(msg);
	this->RecordResult(msg);

	return true;
}

////----<cope with and report the errors from the functional cells>-------------------------
bool ServerCtrlCell::ErrorHandler(Message &msg)
{
	std::string err;
	if((err = Cell::getField("ERROR:",msg.body())) != "")
	{
		this->returnMsg(std::string("ERROR: ").append(err),msg.returnAddress(),"Display",false);
		return true;
	}
	return false;
}

////----<generate a file received from another computer or procedure>---------------------
void ServerCtrlCell::GenerateFile(Message &msg)
{
	std::string msgbody = Cell::getBody(msg.body());

	if(!this->ErrorHandler(msg) && Cell::getField("From:",msg.body()) == "Receiver" 
	&& Cell::getField("Command:",msgbody) == "Inspect"
	&& !this->ErrorHandler(msg))
	{
		std::string str = this->genAddrHead(Name(),"FileTransfer");
		str.append("FilePath:aly_").append(this->getField("FileName:",msgbody)).append("?Command:CreateFile?");
		str.append("FileName:").append(this->getField("FileName:",msgbody)).append("?%");
		str.append(this->getBody(msgbody));
		this->returnMsg(str,msg.returnAddress(),"FileTransfer");
	}
}

////----<inspect the new generated file when get the "success" message from file transfer>-
void ServerCtrlCell::InspectFile(Message &msg)
{
	if(!this->ErrorHandler(msg)
		&&Cell::getField("From:",msg.body()) == "FileTransfer"
		&&Cell::getField("Gen:",msg.body()) == "SUCCESS"
		&&Cell::getField("FilePath:",msg.body()).find(".log") == std::string::npos)
	{
		std::string str = this->genAddrHead(Name(),"Inspector");
		str.append("FilePath:").append(Cell::getField("FilePath:",msg.body()));
		str.append("?FileName:").append(Cell::getField("FileName:",msg.body()));
		str.append("?%");
		this->returnMsg(str,msg.returnAddress(),"Inspector");	
	}

}

////----<record the result of inspecting to a log and send it back to the client>---------
void ServerCtrlCell::RecordResult(Message &msg)
{
	if(Cell::getField("From:",msg.body()) == "Inspector")
	{
		//send back the report
		EndPoint endp(msg.returnAddress().address(),CLIENT_PORT);
		std::string head = this->genAddrHead(Name(),"Sender");
		std::string str = head;
		str.append("Operation:Connect?%");
		this->returnMsg(str,endp,"Sender",false);
		str = head;
		str.append("FileName:").append(Cell::getField("FileName:",msg.body())).append("?");
		str.append("Operation:Send?Command:Record?%").append(Cell::getBody(msg.body()));
		this->returnMsg(str,endp,"Sender",false);
		str = Cell::getBody(msg.body());
		this->returnMsg(str,msg.returnAddress(),"Display");
		//record the report into the log
		str = this->genAddrHead(Name(),"FileTransfer");
		std::string fname = this->getField("FileName:",msg.body());
		fname = fname.append(".log");
		str.append("FilePath:").append(fname).append("?Command:CreateFile?");
		str.append("FileName:").append(fname).append("?%");
		str.append(this->getBody(msg.body()));
		this->returnMsg(str,msg.returnAddress(),"FileTransfer");
	}
}

////----<test stub>-----------------------------------------------------------------------
#ifdef TEST_SERVERCTRLCELL

void main()
{
	SenderCell sdc;
	DisplayCell dsc;
	ReceiverCell rvc;
	FileTransCell ftc;
	InspectorCell itc;
	ServerCtrlCell scc;
	sdc.addMsgPass(&scc);
	dsc.addMsgPass(&scc);
	rvc.addMsgPass(&scc);
	ftc.addMsgPass(&scc);
	itc.addMsgPass(&scc);
	scc.addMsgPass(&sdc);
	scc.addMsgPass(&dsc);
	scc.addMsgPass(&rvc);
	scc.addMsgPass(&ftc);
	scc.addMsgPass(&itc);
	EndPoint endp("127.0.0.1",CLIENT_PORT);
	Message msg("Command:Start?%",endp,Message::text);
	msg.body() = Cell::enCode(msg.body());
	scc.postMessage(msg);
	::system("pause");
	std::cout<<"Server:\n";
	std::cout<<dsc.getOutputInfo();
	//::system("pause");
}

#endif