/////////////////////////////////////////////////////////////////////
//  ServerCtrlExIntf.cpp - Interface for configuring the server end//
//                  and factory for generating the configurer      //                 
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project4 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
#include "ServerCtrlExIntf.h"
#include "ServerCtrlCell.h"
#include "ReceiverCell.h"
#include "SenderCell.h"
#include "FileTransCell.h"
#include "DisplayCell.h"
#include "InspectorCell.h"

#include <sstream>

//////////////////////////////////////////////////////////////////////////////////////////
////Class of Server end configurer and executive, implementation of the configurer interface
class ServerCtrlExecutive :public ServerCtrlExIntf
{
public:
	ServerCtrlExecutive();
	~ServerCtrlExecutive();
	bool build();
	void Start();
	std::string getInfo();
	unsigned int infosize();
private:
	ServerCtrlExecutive(const ServerCtrlExecutive& sce);
	ServerCtrlExecutive& operator = (const ServerCtrlExecutive& sce);
	SenderCell* psdc;
	DisplayCell* pdsc;
	ReceiverCell* prvc;
	FileTransCell* pftc;
	ServerCtrlCell* pscc;
	InspectorCell* pitc;
	EndPoint cltEnd;
};

////----<constructor of the class>--------------------------------------------------------
ServerCtrlExecutive::ServerCtrlExecutive() :psdc(0),pdsc(0),prvc(0),pftc(0),pscc(0),pitc(0),cltEnd("127.0.0.1",CLIENT_PORT){}

////----<deconstructor of the class>------------------------------------------------------
ServerCtrlExecutive::~ServerCtrlExecutive()
{
	delete pscc;
	delete psdc;
	delete pdsc;
	delete prvc;
	delete pftc;
	delete pitc;
}

////----<create the cells to build the server end system>---------------------------------
bool ServerCtrlExecutive::build()
{
	try{
		psdc = new SenderCell;
		pdsc = new DisplayCell;
		prvc = new ReceiverCell;
		pftc = new FileTransCell;
		pscc = new ServerCtrlCell;
		pitc = new InspectorCell;
		psdc->addMsgPass(pscc);
		pdsc->addMsgPass(pscc);
		prvc->addMsgPass(pscc);
		pftc->addMsgPass(pscc);
		pitc->addMsgPass(pscc);
		pscc->addMsgPass(psdc);
		pscc->addMsgPass(pdsc);
		pscc->addMsgPass(prvc);
		pscc->addMsgPass(pftc);
		pscc->addMsgPass(pitc);
	}catch(std::exception& ex)
	{
		delete pscc;
		delete psdc;
		delete pdsc;
		delete prvc;
		delete pftc;
		delete pitc;

		pscc = 0;
		psdc = 0;
		pdsc = 0;
		prvc = 0;
		pftc = 0;
		pitc = 0;
		std::cout<<ex.what()<<std::endl;
		return false;
	}
	return true;
}

////----<start listening to the server port>----------------------------------------------
void ServerCtrlExecutive::Start()
{
	if(pscc)
	{
		Message msg("Command:Start?%",cltEnd,Message::text);
		msg.body() = Cell::enCode(msg.body());
		pscc->postMessage(msg);
	}
}


////----<get one piece of display string from the display cell>---------------------------
std::string ServerCtrlExecutive::getInfo()
{
	if(pdsc)
	{
		return pdsc->getOutputInfo();
	}
	return "";
}

////----<get the size of the display string vector>---------------------------------------
unsigned int ServerCtrlExecutive::infosize()
{
	return pdsc->QSize();
}

////----<create an instance of the configure and return the pointer to the interface>-----
////the definition
ServerCtrlExIntf* CreateServerCtrl()
{
	return new ServerCtrlExecutive;
}

////----<test stub>-----------------------------------------------------------------------
#ifdef TEST_SERVERSERVICE
int main(int argc, char* argv[])
{
	bool isinfinite = false;
	unsigned int time = 0;
	if(argc < 2)
	{
		isinfinite = true;
	}
	else
	{
		std::istringstream ist(argv[1]);
		ist>>time;
	}
	ServerCtrlExIntf* psce = CreateServerCtrl();
	if(!psce->build())
	{
		std::cout<<"Build Server Service Failed!\n";
		::system("pause");
		return 1;
	}
	psce->Start();
	std::cout<<"Server is Running\n*************************\n";
	unsigned timer = 0;
	while((timer < time) || isinfinite)
	{
		if(psce->infosize() > 0)
			std::cout << psce->getInfo();
		timer++;
		::Sleep(100);
	}
	::system("pause");
	delete psce;
	
}

#endif