/////////////////////////////////////////////////////////////////////
//  DisplayCell.cpp - Cell for collecting displaying messages      //                 
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project4 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
#include "DisplayCell.h"
#include <iostream>
////----<constructor of the cell class>---------------------------------------------------
DisplayCell::DisplayCell() :Cell("Display"){}

////----<get the string for displaying>---------------------------------------------------
std::string DisplayCell::getOutputInfo()
{
	return outq.deQ();
}
////----<put the message content for displaying into the displaying queue>----------------
bool DisplayCell::CellFunction(Message &msg)
{
	msg.body() = this->deCode(msg.body());
	std::string output;
	output.append(msg.body()).append("\n");
	outq.enQ(output);
	return true;
}

////----<get the size of the displaying queue>--------------------------------------------
size_t DisplayCell::QSize()
{
	return outq.size();
}

/////-----<test stub>---------------------------------------------------------------------
#ifdef TEST_DISPLAYCELL
void main()
{
	DisplayCell dc;
	dc.postMessage(Message("Wahahah"));
	dc.postMessage(Message("ihehehehe"));
	::system("pause");
}
#endif