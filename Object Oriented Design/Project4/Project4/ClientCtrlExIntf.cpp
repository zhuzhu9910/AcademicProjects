/////////////////////////////////////////////////////////////////////
//  ClientCtrlExIntf.cpp - Interface for configuring the client end//
//                  and factory for generating the configurer      //                 
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project4 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////

#include "ClientCtrlExIntf.h"
#include "ClientCtrlCell.h"
#include "ReceiverCell.h"
#include "SenderCell.h"
#include "FileTransCell.h"
#include "DisplayCell.h"


//////////////////////////////////////////////////////////////////////////////////////////
////Class of Client end configurer and executive, implementation of the configurer interface
class ClientCtrlExeutive :public ClientCtrlExIntf
{
public:
	ClientCtrlExeutive();
	~ClientCtrlExeutive();
	bool build();
	void InspectFile(std::string filepath);
	void setServer(std::string addr);
	std::string getInfo();
	unsigned int infosize();
	void Connect();
	void DisConnect();
private:
	ClientCtrlExeutive(const ClientCtrlExeutive& cce);
	ClientCtrlExeutive& operator = (const ClientCtrlExeutive& cce);
	SenderCell* psdc;
	DisplayCell* pdsc;
	ReceiverCell* prvc;
	FileTransCell* pftc;
	ClientCtrlCell* pccc;
	EndPoint sverEnd;
};

////----<constructor of the class>--------------------------------------------------------
ClientCtrlExeutive::ClientCtrlExeutive() : psdc(0),pdsc(0),prvc(0),pftc(0),pccc(0),sverEnd("127.0.0.1",SERVER_PORT){}

////----<deconstructor of the class>------------------------------------------------------
ClientCtrlExeutive::~ClientCtrlExeutive()
{
	delete pccc;
	delete psdc;
	delete pdsc;
	delete prvc;
	delete pftc;
}

////----<connect to the server>------------------------------------------------------------
void ClientCtrlExeutive::Connect()
{
	if (pccc)
	{
		Message msg("Command:Connect?%",this->sverEnd,Message::text);
		msg.body() = Cell::enCode(msg.body());
		pccc->postMessage(msg);
	}
}

////----<disconnect to the server>---------------------------------------------------------
void ClientCtrlExeutive::DisConnect()
{
	if (pccc)
	{
		Message msg("Command:DisConnect?%",this->sverEnd,Message::text);
		msg.body() = Cell::enCode(msg.body());
		pccc->postMessage(msg);
	}
}


////----<create the cells to build the client end system>---------------------------------
bool ClientCtrlExeutive::build()
{
	try{
		psdc = new SenderCell;
		pdsc = new DisplayCell;
		prvc = new ReceiverCell;
		pftc = new FileTransCell;
		pccc= new ClientCtrlCell;
		psdc->addMsgPass(pccc);
		pdsc->addMsgPass(pccc);
		prvc->addMsgPass(pccc);
		pftc->addMsgPass(pccc);
		pccc->addMsgPass(psdc);
		pccc->addMsgPass(pdsc);
		pccc->addMsgPass(prvc);
		pccc->addMsgPass(pftc);
	}catch(std::exception& ex)
	{
		delete pccc;
		delete psdc;
		delete pdsc;
		delete prvc;
		delete pftc;

		pccc = 0;
		psdc = 0;
		pdsc = 0;
		prvc = 0;
		pftc = 0;
		std::cout<<ex.what()<<std::endl;
		return false;
	}
	return true;
}

////----<generate a command to inspect a file and post it into the client controler's message queue>-----
void ClientCtrlExeutive::InspectFile(std::string filepath)
{
	if (pccc)
	{
		std::string cmd = "Command:InspectFile?FilePath:";
		cmd.append(filepath).append("?%");
		Message msg(cmd,this->sverEnd,Message::text);
		msg.body() = Cell::enCode(msg.body());
		pccc->postMessage(msg);
	}
}

////----<set the server address>----------------------------------------------------------
void ClientCtrlExeutive::setServer(std::string addr)
{
	sverEnd = EndPoint(addr,SERVER_PORT);
}

////----<get one piece of display string from the display cell>---------------------------
std::string ClientCtrlExeutive::getInfo()
{
	if(pdsc)
	{
		return pdsc->getOutputInfo();
	}
	return "";
}

////----<get the size of the display string vector>---------------------------------------
unsigned int ClientCtrlExeutive::infosize()
{
	return pdsc->QSize();
}



////----<create an instance of the configure and return the pointer to the interface>-----
////the definition
ClientCtrlExIntf* CreateClientCtrl()
{
	return new ClientCtrlExeutive;
}

////----<test stub>-----------------------------------------------------------------------
#ifdef TEST_CLIENTSERVICE
int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		std::cout <<"illeagal command format!\n";
		return 1;
	}
	
	ClientCtrlExIntf* pcce = CreateClientCtrl();
	if(!pcce->build())
	{
		std::cout<<"Build Client Service Failed!\n";
		::system("pause");
		return 1;
	}
	pcce->setServer(argv[1]);
	pcce->Connect();
	std::cout<<"Client is Running\n*************************\n";
	for(int i = 2; i < argc; ++i)
	{
		pcce->InspectFile(argv[i]);
	}
	int timer = 0;
	pcce->DisConnect();
	while((timer < (argc - 2) * 2) || pcce->infosize() > 0)
	{
		std::cout<<pcce->getInfo();
		timer ++;
	}
	::system("pause");
	delete pcce;

	
}
#endif