/////////////////////////////////////////////////////////////////////
//  FileTransCell.cpp - open and send back the content of a file or//
//                  create a file                                  //
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project4 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////

#include "FileTransCell.h"
#include <fstream>
////----<constructor of this class>-------------------------------------------------------
FileTransCell::FileTransCell() :Cell("FileTransfer"){}

////----<open the file specified in the message and generate a return message>------------
void FileTransCell::OpenFile(Message &msg)
{
	std::string path = Cell::getField("FilePath:",msg.body());
	if(path != "")
	{
		std::ifstream inputfile(path.c_str());
		std::string to = Cell::getField("From:",msg.body());
		if(!inputfile.good())
		{
			returnMsg("ERROR:Cannot open the specified file!!?%",msg.returnAddress(),Cell::getField("From:",msg.body()),false);
			return;
		}
		std::string buffer;
		while(inputfile.good())
			buffer.push_back(inputfile.get());
		buffer.erase(buffer.begin() + buffer.length() - 1);
		std::string filetext = "FileName:";
		size_t namepos = path.find_last_of("\\/");
		filetext.append(path.substr(namepos + 1)).append("?").append(genAddrHead(Name(),to));
		filetext.append("%").append(buffer);
		returnMsg(filetext,msg.returnAddress(),to);
	}
}

////----<create a file according to the information in the input message>-----------------
void FileTransCell::GenFile(Message &msg)
{
	std::string filename = Cell::getField("FilePath:",msg.body());
	if(filename == "")
	{
		returnMsg("ERROR:Cannot create the file!!?%",msg.returnAddress(),Cell::getField("From:",msg.body()),false);
		return;
	}
	std::ofstream outf(filename.c_str());
	if(!outf.good())
	{
		returnMsg("ERROR:Cannot create the file!!?%",msg.returnAddress(),Cell::getField("From:",msg.body()),false);
		return;
	}
	outf<<Cell::getBody(msg.body()).c_str();
	outf.flush();
	outf.close();
	//return a message to say that the file is created successfully
	std::string str = this->genAddrHead(Name(),Cell::getField("From:",msg.body()));
	str.append("FileName:").append(Cell::getField("FileName:",msg.body())).append("?");
	str.append("FilePath:").append(Cell::getField("FilePath:",msg.body())).append("?Gen:SUCCESS?%");
	this->returnMsg(str,msg.returnAddress(),Cell::getField("From:",msg.body()),false);
}

////----<according to the command in the message, dispatch the message to one of the function>----
bool FileTransCell::CellFunction(Message &msg)
{
	msg.body() = this->deCode(msg.body());
	if(Cell::getField("Command:",msg.body()) == "OpenFile")
		OpenFile(msg);
	if(Cell::getField("Command:",msg.body()) == "CreateFile")
		GenFile(msg);
	return true;
}

////----<test stub>-----------------------------------------------------------------------
#ifdef TEST_FILETRANSCELL
void main()
{
	Cell cell("main");
	FileTransCell ftc;
	ftc.addMsgPass(&cell);
	std::string cnt = "From:main?To:FileTransfer?FilePath:../test.cpp?Command:OpenFile?%";
	EndPoint rend("main",0);
	Message msg(cnt,rend,Message::text);
	cnt = "From:main?To:FileTransfer?FilePath:../tttt.cpp?Command:CreateFile?%hahaha";
	Message msg1(cnt,rend,Message::text);
	ftc.postMessage(msg);
	ftc.postMessage(msg1);
	::system("pause");
}

#endif