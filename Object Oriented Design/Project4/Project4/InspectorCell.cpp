/////////////////////////////////////////////////////////////////////
//  InspectorCell.cpp - open and send back the content of a file or//
//                  create a file                                  //
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project4 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////

#include "InspectorCell.h"
#include "ClassInspector.h"

////----<constructor of the class>--------------------------------------------------------
InspectorCell::InspectorCell() :Cell("Inspector"){}

////----<use the class inspector to inspect the specified source file and return the report>-------
bool InspectorCell::CellFunction(Message &msg)
{
	msg.body() = this->deCode(msg.body());
	if(msg.getMsgType() == Message::file)
	{
		std::string filename = Cell::getField("FilePath:",msg.body());
		if(filename != "")
		{
			std::string to = Cell::getField("From:",msg.body());
			ClassInspector clsinspr;
			clsinspr.addFile(filename);
			clsinspr.Analyze();
			std::string rstr = genAddrHead(Name(),to).append("FileName:");
			rstr.append(Cell::getField("FileName:",msg.body())).append("?%");
			rstr.append(clsinspr.Show());
			returnMsg(rstr,msg.returnAddress(),to,false);
		}
	}
	return true;
}

////----<test stub>-----------------------------------------------------------------------
#ifdef TEST_INSPECTORCELL
void main()
{
	Cell cell("main");
	InspectorCell intc;
	intc.addMsgPass(&cell);
	EndPoint endp("127.0.0.0",0);
	Message msg("FilePath:ttt.cpp?From:main?To:Inspector?%#include<iostream>\nint a;\nvoid main() {}",endp,Message::file);
	msg.body() = Cell::enCode(msg.body());
	intc.postMessage(msg);
	::Sleep(1000);
	::system("pause");
}
#endif