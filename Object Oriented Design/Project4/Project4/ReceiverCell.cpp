/////////////////////////////////////////////////////////////////////
//  ReceiverCell.cpp - Listen to a port and receive messages from  //
//                    soccket.                                     //
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project4 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
#include "ReceiverCell.h"

/////----<the constructor of the class>---------------------------------------------------
ReceiverCell::ReceiverCell() :Cell("Receiver"){}

/////----<get the command from the input message to listen and return messages from the specified port>----
bool ReceiverCell::CellFunction(Message &msg)
{
	msg.body() = this->deCode(msg.body());
	if(Cell::getField("Operation:",msg.body()) == "Listen")
	{
		Message remsg;
		try{
			Receiver Recv(msg.returnAddress().port());
			//listen and receive message from the specified port
			do{
				remsg = Recv.GetMsg();
				remsg = Message(this->deCode(remsg.body()),this->removecon(remsg),remsg.getMsgType());
				std::string to = getField("From:",msg.body());
				std::string msgcnt = this->genAddrHead(Name(),to);
				if(remsg.body() == "quit")
				{
					this->returnMsg("quit",remsg.returnAddress(),to,false);
					break;
				}
				msgcnt.append("%").append(Cell::getBody(remsg.body()));
				this->returnMsg(msgcnt,remsg.returnAddress(),to,(remsg.getMsgType() == Message::file));
			}while(true);
			
		}
		catch(std::exception& ex)
		{
			std::cout<<"receive failed: "<<ex.what()<<std::endl;
		}
	}
	::Sleep(100);
	return true;
}

////----<remove the quotation marks from the addrees>-------------------------------------
EndPoint ReceiverCell::removecon(Message &msg)
{
	std::string addr = msg.returnAddress().address();
	if(addr.size() > 0)
	{
		if(addr.at(0) == '\"')
			addr.erase(addr.begin());
		if(addr.at(addr.size() - 1) == '\"')
			addr.erase(addr.begin() + addr.size() - 1);
	}
	
	return EndPoint(addr,msg.returnAddress().port());
}

////----<test stub>----------------------------------------------------------------------
#ifdef TEST_RECEIVER
void main()
{
	try{
	ReceiverCell Revcc;
	Cell cell("main");
	Revcc.addMsgPass(&cell);
	EndPoint endp("127.0.0.1",8000);
	Message msg("From:main?To:Receiver?Operation:Listen?%",endp,Message::text);
	msg.body() = Cell::enCode(msg.body());
	Revcc.postMessage(msg);
	::Sleep(5000);
	}	catch(std::exception& ex)
		{
			std::cout<<"receive failed: "<<ex.what()<<std::endl;
		}

	::system("pause");

}

#endif