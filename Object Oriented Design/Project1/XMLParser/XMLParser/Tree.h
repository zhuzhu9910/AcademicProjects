#ifndef TREE_H
#define TREE_H
/////////////////////////////////////////////////////////////////////
//  Tree.h - the parse tree of XML                                 //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project1 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations:
  ==================
  Provides a xml Tree class that wrap the tree which holds a finite number
  of connected nodes and supports visitation of each node, using depth 
  first search.It provides the operation of add and remove a node to or 
  from the tree and find nodes in the tree with specified ID, tag or attribute.
  
  Public Interface:
  =================
  XMLValue xmlv(XMLValue::TAG, "MyTag");                           //xml tree value
  PNode<XMLValue> xmlroot(xmlv);                                   //make root node of the tree
  PNode<XMLValue> xmlcld1(XMLValue(XMLValue::TAG,"xmlChild1"));    //make a child node
  XMLTree tree(&xmlroot);                                          //create a tree and attach the root node
  tree.add(&xmlcld1,xmlroot);                                      //add a child node to the root node
  std::vector<PNode<XMLValue>*> finded = tree.find("xmlChild1");   //find the node which has a tag named "xmlChild1"
  tree.remove(&xmlcld1);                                           //remove a node from the tree

  Build Process:
  ==============
  cl /D:TEST_TREE Tree.cpp Nodes.cpp
  
  Maintenance History:
  ====================
  ver 1.0 : 27 Jan 10
  - first release
 */
/*
 * Notes:
 * - Tree destructor is not deleting nodes because the test stub is creating
 *   on the stack, e.g., local to main.  If you create nodes on the heap
 *   then you should delete all nodes from the tree (the deleted nodes will 
 *   be stored in the deleted nodes vector) and delete all the contents 
 *   pointed to by the deleted-nodes vector.
 */




#include <iostream>
#include "Nodes.h"
#include <vector>
#include <stack>
////////////////////////////////////////////////////////////////////////////////
////Class of outside defined operation applying during walking through the tree 
class Operation
{
public:
	virtual ~Operation(){}
	virtual void operator() (PNode<XMLValue>* pNode)
	{
		std::cout<<"\n "<<pNode->value().TagName();
	}
};

////////////////////////////////////////////////////////////////////////////////
////Operation class for front-operation of copying xml tree
class CopyTreePreOper : public Operation
{
public:
	//--<construct the class with a stack>--
	CopyTreePreOper(std::stack<XMLNode*>& nodestack) :Stack(nodestack){};

	//--<the () operator for copying the tree>--
	void operator()(PNode<XMLValue>* pNode)
	{
		PNode<XMLValue>* newnode = new PNode<XMLValue>(pNode->value());
		Stack.top()->add(newnode);
		Stack.push(newnode);
	}
private:
	std::stack<XMLNode*>& Stack;
};

////////////////////////////////////////////////////////////////////////////////
////Operation class for front-operation of copying xml tree
class CopyTreePostOper : public Operation
{
public:
	//--<construct the class with a stack>--
	CopyTreePostOper(std::stack<XMLNode*>& nodestack) :Stack(nodestack){};

	//--<the () operator for poping a node out of the stack when leaving a tree during copying>--
	void operator()(PNode<XMLValue>* pNode)
	{
		Stack.pop();
	}
private:
	std::stack<XMLNode*>& Stack;
};

///////////////////////////////////////////////////////////////////////////////////////
////Class of XML tree, possessing the nodes in the tree and some operations on the tree
class XMLTree
{
public:
	XMLTree(PNode<XMLValue>* proot);
	/////////////////////////////////////
	XMLTree(const XMLTree& xmltree);
	XMLTree& operator= (const XMLTree& tree);
	void setRoot(PNode<XMLValue>* proot);

	PNode<XMLValue>* getroot();
	std::vector<PNode<XMLValue>*> getdeletednodes();

	///add and remove operation
	PNode<XMLValue>* add(PNode<XMLValue>* pPNode, PNode<XMLValue>* pPar = NULL);
	PNode<XMLValue>* remove(PNode<XMLValue>* pPNode);

	///search and find
	std::vector<PNode<XMLValue>*> find (std::string attrName, std::string attrValue);
	std::vector<PNode<XMLValue>*> find (std::string tagName);
	PNode<XMLValue>* findbyID(std::string id);

   ///outside operation set and depth first search applying ouside defined operation
	void setFrontOperation(Operation* pOper);
	void setPostOperation(Operation* pOper);
	void walk(PNode<XMLValue>* pStart);
	void walk();

private:

	///member parameters
	PNode<XMLValue>* pRoot;
	std::vector< PNode<XMLValue>* > deletednodes;
	Operation* pFOper;
	Operation* pPOper;

	///helper functions
	void removeSubtree(PNode<XMLValue>* pPNode);
	PNode<XMLValue>* deleteFind(PNode<XMLValue>* currentNode, PNode<XMLValue>* tobedelete);
	void DFSFind(PNode<XMLValue>* currentNode, std::vector< PNode<XMLValue>* >& findedNodes,
		std::string tagName, std::string attrName = "", std::string attrValue = "");
	bool NodeFindCompare(PNode<XMLValue>* pPNode, std::string tagName, std::string attrName = "", std::string attrValue = "");
	
};

#endif