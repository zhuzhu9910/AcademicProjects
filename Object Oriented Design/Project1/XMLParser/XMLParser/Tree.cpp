/////////////////////////////////////////////////////////////////////
//  Tree.cpp - the parse tree of XML                               //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project1 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////


#include "Tree.h"
#include <vector>
//----<construct XMLTree with assigning the pointer of root node to the empty tree>----
XMLTree::XMLTree(PNode<XMLValue>* proot) :pRoot(proot), pFOper(NULL),pPOper(NULL) {}

//----<copy constructor for copying the member parameters>----
XMLTree::XMLTree(const XMLTree& xmltree) :pRoot(NULL)
{
	this->operator=(xmltree);
}

//----<assignment operator>---
XMLTree& XMLTree::operator= (const XMLTree& xmltree)
{
	this->pRoot = xmltree.pRoot;
	PNode<XMLValue>* newroot = new PNode<XMLValue>(xmltree.pRoot->value());
	//initialize the stack for copying and push the new root node to the stack
	std::stack<XMLNode*> xmlstack;
	xmlstack.push(newroot);
	//copy the nodes of the tree
	CopyTreePreOper* preopt = new CopyTreePreOper(xmlstack);
	CopyTreePostOper* postopt = new CopyTreePostOper(xmlstack);
	this->setFrontOperation(preopt);
	this->setPostOperation(postopt);
	this->walk();                        //walk through the tree to copy the nodes
	this->setFrontOperation(NULL);
	this->setPostOperation(NULL);
	delete preopt;
	delete postopt;
	//put the new linked tree root to the new copied XMLTree
	this->pRoot = newroot;
	//clear the vector of deleted nodes
	this->deletednodes.clear();
	return *this;
}

//----<set the root of the XML Tree>
void XMLTree::setRoot(PNode<XMLValue>* proot)
{
	pRoot = proot;
}

//----<find out whether the node possesses the specified attribute or tag name>---
inline bool XMLTree::NodeFindCompare(PNode<XMLValue>* pPNode, std::string tagName, std::string attrName, std::string attrValue)
{
	//if tag name is not null then compare the tag names 
	//else looking for the specified attribute in the node
	if(0 != tagName.compare(""))
		return (0 == pPNode->value().TagName().compare(tagName));
	else
		return (pPNode->value().Find(attrName,attrValue) >= 0);
}

//----<add a node as a child node to a specified parent node and return the pointer of the added node>---
PNode<XMLValue>* XMLTree::add(PNode<XMLValue>* pPNode, PNode<XMLValue>* pPar)
{
	if(pPar != NULL)
	{
		pPar->add(pPNode);
		return pPNode;
	}
	return NULL;
}

//----<remove a node from the tree and return its parent node pointer>---
PNode<XMLValue>* XMLTree::remove(PNode<XMLValue>* pPNode)
{
	//when the specified node is the root node
	if(pRoot == pPNode)
	{
		deletednodes.push_back(pRoot);
		removeSubtree(pRoot);
		return pPNode;
	}
	//when the specified node is no the root node
	//find it and delete it
	return deleteFind(pRoot, pPNode);
}

//----<get the pointer of the root node>---
PNode<XMLValue>* XMLTree::getroot()
{
	return pRoot;
}

//----<search for the nodes which have the specified attribute>---
std::vector<PNode<XMLValue>*> XMLTree::find (std::string attrName, std::string attrValue)
{
	std::vector<PNode<XMLValue>*> rnodes;
	DFSFind(pRoot,rnodes,"",attrName,attrValue);
	return rnodes;
}

//----<search for the nodes which have specified tag name>---
std::vector<PNode<XMLValue>*> XMLTree::find (std::string tagName)
{
	std::vector<PNode<XMLValue>*> rnodes;
	DFSFind(pRoot,rnodes,tagName);
	return rnodes;
}

//----<search for the node which has the specified id>---
PNode<XMLValue>* XMLTree::findbyID(std::string id)
{
	//format the content of id to "id value" 
	std::string formatID;
	formatID.append("\"").append(id).append("\"");
	//search for specified id
	std::vector<PNode<XMLValue>*> idholder = find("ID",formatID);
	if(idholder.empty())
		idholder = find("id",formatID);
	if(idholder.empty())
		idholder = find("Id",formatID);
	//return the unique result
	if(!idholder.empty())
		return idholder[0];
	else
		return NULL;
}

//----<delete the nodes in the sub-tree of a specified node>---
void XMLTree::removeSubtree(PNode<XMLValue>* pPNode)
{
	PNode<XMLValue >* pTemp;
	pPNode->clearMarks();
	while (pTemp = pPNode->nextUnmarkedChild())
	{
		pTemp->setMarkState(true);
		deletednodes.push_back(pTemp);
		removeSubtree(pTemp);
	}
}

//----<get the deleted nodes>----
std::vector<PNode<XMLValue>*> XMLTree::getdeletednodes()
{
	return deletednodes;
}

//----<search for the node need deleted using depth first search>---
PNode<XMLValue>* XMLTree::deleteFind(PNode<XMLValue>* currentNode, PNode<XMLValue>* tobedelete)
{
	PNode<XMLValue >* pTemp;
	currentNode->clearMarks();
	while (pTemp = currentNode->nextUnmarkedChild())
	{
		//if find the nodes then remove it from the tree, 
		//push it into the deletednodes vector and also delete
		//its subtree nodes and then stop searching.
		if(tobedelete == pTemp)
		{
			deletednodes.push_back(pTemp);
			currentNode->remove(pTemp);
			removeSubtree(pTemp);
			return currentNode;
		}
		pTemp->setMarkState(true);
		pTemp = deleteFind(pTemp,tobedelete);
		//if there is a nonempty pointer returned from 
		//next layer search, it indicates that the node
		//need to be deleted is found
		if(NULL != pTemp)
			return pTemp;
	}
	return NULL;
}

//----<using DFS search to find the specified nodes and push them to a collecting vector>----
void XMLTree::DFSFind(PNode<XMLValue>* currentNode, std::vector< PNode<XMLValue>* >& findedNodes, std::string tagName, std::string attrName, std::string attrValue)
{
	PNode<XMLValue >* pTemp;
	currentNode->clearMarks();
	while (pTemp = currentNode->nextUnmarkedChild())
	{
		if(NodeFindCompare(pTemp,tagName,attrName,attrValue))
			findedNodes.push_back(pTemp);

		pTemp->setMarkState(true);
		DFSFind(pTemp,findedNodes,tagName,attrName,attrValue);
	}
}

//----<walk through the tree from the root node using depth first search>----
void XMLTree::walk()
{
	pRoot->clearMarks();
	PNode<XMLValue >* pTemp;
	while (pTemp = pRoot->nextUnmarkedChild())
	{
		pTemp->setMarkState(true);
		walk(pTemp);
	}
}

//----<walk through the tree and apply outside defined opearation to every node>----
void XMLTree::walk(PNode<XMLValue>* pStart)
{
	pStart->clearMarks();
	PNode<XMLValue >* pTemp;
	//front-operation when go to the node
	if(pFOper != NULL)
	{
		(*pFOper)(pStart);
	}
	//depth-first search
	while (pTemp = pStart->nextUnmarkedChild())
	{
		pTemp->setMarkState(true);
		walk(pTemp);
	}
	//post-operation when leave the node
	if(pPOper != NULL)
	{
		(*pPOper)(pStart);
	}
}

//----<set the front operation when go to a node>----
void XMLTree::setFrontOperation(Operation* pOper)
{
	pFOper = pOper;
}

//----<set the operation when leave a node>----
void XMLTree::setPostOperation(Operation* pOper)
{
	pPOper = pOper;
}

//----<test stub>---
#ifdef TEST_TREE

void main()
{
	std::cout<<"===========================XMLTree Test==================================="<<std::endl;
	XMLValue xmlv(XMLValue::TAG, "MyTag");
	xmlv.AddAttr("ATTR1","1");
	xmlv.AddAttr("ATTR2","2");
	xmlv.AddAttr("ATTR3","3");
	
	PNode<XMLValue> xmlroot(xmlv);
	PNode<XMLValue> xmlcld1(XMLValue(XMLValue::TAG,"xmlChild1"));
	PNode<XMLValue> xmlcld2(XMLValue(XMLValue::TAG,"xmlChild2"));
	PNode<XMLValue> xmlcld3(XMLValue(XMLValue::TAG,"xmlChild3"));
	PNode<XMLValue> xmlcld4(XMLValue(XMLValue::TAG,"xmlChild4"));
	
	xmlcld2.value().AddAttr("ATTR2","2");
	xmlcld2.value().AddAttr("ID","\"2\"");
	//add nodes
	XMLTree xmltree(&xmlroot);
	xmltree.add(&xmlcld1,&xmlroot);
	xmltree.add(&xmlcld2,&xmlroot);
	xmltree.add(&xmlcld3,&xmlcld2);
	xmltree.add(&xmlcld4,&xmlcld3);
	XMLTree treeopt(new XMLNode(xmlv) ) ;
	treeopt = xmltree;
	XMLTree tree = xmltree;
	
	
	std::cout<<"The XML Parse Tree:\n"<<"Root: "<<tree.getroot()->value().TagName()<<std::endl;

	std::vector<PNode<XMLValue>*> findout = tree.find("ATTR2","2");
	for(size_t i = 0; i< findout.size(); ++i)
		std::cout<<"Finded Nodes with atrribute ATTR2 = \"2\": "<<findout[i]->value().TagName()<<std::endl;
	//search
	std::vector<PNode<XMLValue>*> findout1 = tree.find("xmlChild4");
	for(size_t i = 0; i< findout1.size(); ++i)
		std::cout<<"Finded Nodes with tagname xmlChild4: "<<findout1[i]->value().TagName()<<std::endl;

	std::cout<<"Finded Node with ID = 2: "<<tree.findbyID("2")->value().TagName()<<std::endl;
	//delete node
	std::cout<<"Deleted Node's parent: "<<xmltree.remove(&xmlcld2)->value().TagName()<<std::endl;
	std::cout<<"Deleted Node's parent(copy constructor test): "<<tree.remove(tree.findbyID("2"))->value().TagName()<<std::endl;
	std::cout<<"Deleted Node's parent(assignment operator test): "<<treeopt.remove(treeopt.findbyID("2"))->value().TagName()<<std::endl;

	std::vector<XMLValue> rootcld = tree.getroot()->getChildren();
	std::vector<PNode<XMLValue>*> deletednodes = tree.getdeletednodes();
	for(size_t i = 0; i < rootcld.size(); ++i)
		std::cout<<"Children of root: "<<rootcld[i].TagName()<<std::endl;
	for (size_t i = 0; i < deletednodes.size(); ++i)
		std::cout<<"Deleted Nodes: "<<deletednodes[i]->value().TagName()<<std::endl;

	Operation opt;
	tree.setFrontOperation(&opt);
	tree.walk();
	::system("pause");
}

#endif