#ifndef DISPLAYCELL_H
#define DISPLAYCELL_H
/////////////////////////////////////////////////////////////////////
//  DisplayCell.h - Cell for collecting displaying messages        //                 
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
  This module defines a cell class for displaying
  The message to the cell should be withou the message header.

  Public Interface:
  =================
  DisplayCell dc;                            //create an instanc of the display cell
  dc.postMessage(Message("Hello"));          //post a message for displaying

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
////Class of displaying cell
class DisplayCell :public Cell
{
public:
	DisplayCell();
	std::string getOutputInfo();
	size_t QSize();
private:
	bool CellFunction(Message &msg);
	BQueue<std::string> outq;
};


#endif