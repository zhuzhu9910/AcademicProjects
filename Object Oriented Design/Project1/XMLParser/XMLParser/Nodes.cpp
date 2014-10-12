/////////////////////////////////////////////////////////////////////
//  Nodes.cpp - store XML elements in the tree structure           //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project1 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////


//----<test stub>---
#ifdef TEST_NODES
#include <iostream>
#include "Nodes.h"

//----<DFS nodes of the tree for test>--
void WalkTree(PNode<std::string >* pPNode)
{
	pPNode->clearMarks();
	std::cout <<"\n  "<<pPNode->value();
	PNode<std::string >* pTemp;
	while (pTemp = pPNode->nextUnmarkedChild())
	{
		pTemp->setMarkState(true);
		WalkTree(pTemp);
	}
}

void main()
{
	std::cout<< "\n PNode<T> class demonstration";
	std::cout<< "\n ============================\n";
	//initialize
	PNode<std::string > root("root");
	PNode<std::string > child1("child1");
	PNode<std::string > child2("child2");
	PNode<std::string > grandchild11("grandchild11");
	PNode<std::string > grandchild21("grandchild21");
	PNode<std::string > grandchild22("grandchild22");
	PNode<std::string > grandgrandchild211("grandgrandchild211");
	PNode<std::string > child3("child3");
	
	//test adding
	child1.add(&grandchild11);
	grandchild21.add(&grandgrandchild211);
	child2.add(&grandchild21);
	child2.add(&grandchild22);
	root.add(&child1);
	root.add(&child2);
	root.add(&child3);
	WalkTree(&root);
	
	//test removing, mark operation
	std::cout<<"\n\n removing first child";
	root.remove(&child1);
	std::cout<<"\n unmarked all nodes";
	root.setMarkState(true);
	child1.setMarkState(true);
	grandchild11.setMarkState(true);
	child2.setMarkState(true);
	grandchild21.setMarkState(true);
	grandchild22.setMarkState(true);
	grandgrandchild211.setMarkState(true);
	child3.setMarkState(true);

	//show the tree
	WalkTree(&root);
	std::cout<<"\n\n";
	
	std::cout<<"===========================XMLValues Test==================================="<<std::endl;
	XMLValue xmlv(XMLValue::TAG, "MyTag");
	Attribute attr1("ATTR1","1");
	Attribute attr2("ATTR2","2");
	Attribute attr3("ATTR3","3");
	xmlv.AddAttr("ATTR1","1");
	xmlv.AddAttr("ATTR2","2");
	xmlv.AddAttr("ATTR3","3");

	//test == operator
	std::cout<<"Attribute Compare:\n"<<"attr1 == attr1: "<< (attr1 == attr1) <<"; attr2 == attr1: "<< (attr2 == attr1)<<std::endl;

	//test search and find attributes
	for(size_t i=0; i< xmlv.attributes().size();++i)
		std::cout<<xmlv.attributes().at(i).AttrName.c_str()<<"  "<<xmlv.attributes().at(i).AttrValue.c_str()  <<std::endl;
	std::cout<<"Find ATTR2's index: "<<xmlv.Find("ATTR2","2")<<std::endl;
	std::cout<<"Find ATTR4's index: "<<xmlv.Find("ATTR4","4")<<std::endl;
	//test removing attributes
	xmlv.RemoveAttr("ATTR2","2");
	std::cout<<"====================ATTR2 Removed==========================\n";
	for(size_t i=0; i< xmlv.attributes().size();++i)
		std::cout<<xmlv.attributes().at(i).AttrName.c_str()<<"  "<<xmlv.attributes().at(i).AttrValue.c_str()  <<std::endl;
	std::cout<<"=================Nodes with XMLValues======================"<<std::endl;
	PNode<XMLValue> xmlroot(xmlv);
	PNode<XMLValue> xmlcld1(XMLValue(XMLValue::TAG,"xmlChild1"));
	PNode<XMLValue> xmlcld2(XMLValue(XMLValue::TAG,"xmlChild2"));
	xmlroot.add(&xmlcld1);
	xmlroot.add(&xmlcld2);
	PNode<XMLValue> *pTemp;

	//test searching through the tree
	while (pTemp = xmlroot.nextUnmarkedChild())
	{
		std::cout<<"The child of root: "<<pTemp->value().TagName()<<std::endl; 
		pTemp->setMarkState(true);
	}

	std::cout<<"=================Children of Nodes======================"<<std::endl;
	for (size_t i = 0; i < xmlroot.getChildren().size(); i++)
		std::cout<<"a Child of ROOT: "<<xmlroot.getChildren()[i].TagName()<<std::endl;
	::system("pause");
}

#endif