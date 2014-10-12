#ifndef FILETRANSCELL_H
#define FILETRANSCELL_H
/////////////////////////////////////////////////////////////////////
//  FileTransCell.h - open and send back the content of a file or  //
//                  create a file                                  //
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project4 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module defines a cell class for some file operations. It can open
  the specified file in the input message and return the content of the
  file. It can create a file according to the specified path and content
  in the input message.

  Public Interface:
  =================
  FileTransCell ftc;                                                                     //create an instance of the file transfer cell
  std::string cnt = "From:main?To:FileTransfer?FilePath:../test.cpp?Command:OpenFile?%"; //the content of the message as a command to open the specified file
  EndPoint rend("127.0.0.1",0);                                                          //create the end point instance
  Message msg(cnt,rend,Message::MsgType::text);                                          //create the instance of the message for opening file
  cnt = "From:main?To:FileTransfer?FilePath:../tttt.cpp?Command:CreateFile?%hahaha";     //the content of the message as a command to create a file
  Message msg1(cnt,rend,Message::MsgType::text);                                         //create the instance of the meessage for creating file
  ftc.postMessage(msg);                                                                  //post the message for opening file to the file transfer cell
  ftc.postMessage(msg1);                                                                 //post the message for creating file to the file transfer cell

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
#include "Cell.h"

//////////////////////////////////////////////////////////////////////////////////////////
////Class of the cell for file transfering
class FileTransCell: public Cell
{
public:
	FileTransCell();
private:
	bool CellFunction(Message& msg);
	void OpenFile(Message& msg);
	void GenFile(Message& msg);
};

#endif