#ifndef PNODES_H
#define PNODES_H
/////////////////////////////////////////////////////////////////////
//  Nodes.h - store XML elements in the tree structure             //
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
  This module defines a PNode class for the XML parsing tree. It holds a
  finite number pointers to child nodes, and a visitation predicate.
  The XMLValue class holds the information of a XML element. It stores the
  name, type of the XML node and the pairs of attribute and value. It has methods
  to add, remove and find a specified attribute.
  The Attribute structure stores the attribute and value pair and provides
  method and operator to compare them.
  

  Public Interface:
  =================
  XMLValue t_value(XMLValue::TAG, "MyTag");               //create a node value store a tag element
  XMLValue t_value_cld(XMLValue::TAG, "MyTag1");          //another element
  t_value.AddAttr("ID",""1234"");                         //add a attribute to a node value
  PNode node(t_value);                                    //create a node with value t_value
  PNode node_cld(t_value_cld);                            //create a node with value t_value_cld
  node.add(&node_cld);                                    //add the node node_cld to node as a child node
  node.value().find("ID","1234");                         //find an attribute in node
  node.remove(&node_cld);                                 //remove the node_cld from the child nodes of node

  Build Process:
  ==============
  cl /D:TEST_NODES Nodes.cpp

  Maintenance History:
  ====================
  ver 1.0 : 25 Jan 10
  - first release
*/

#include <vector>
#include <string>
///////////////////////////////////////////////////////////
////Store the pairs of attribute and value
struct Attribute
{
	Attribute(const std::string name,const std::string value);
	bool operator== (const Attribute& attr);
	bool compare(const std::string name, const std::string value);
	std::string AttrName;
	std::string AttrValue;
};

//----<construct attribute with attribute name and value pair>--------
inline Attribute::Attribute(const std::string name,const std::string value) : AttrName(name), AttrValue(value){}

//----<using == operator to compare two attributes (both attribute name and value)>-----
inline bool Attribute::operator== (const Attribute& attr) 
{
	if(0 == this->AttrName.compare(attr.AttrName) && 0 == this->AttrValue.compare(attr.AttrValue))
		return true;
	else
		return false;
}

//---<compare a pair of attribute name and value with that in the attribute>----
inline bool Attribute::compare(const std::string name, const std::string value)
{
	if(0 == this->AttrName.compare(name) && 0 == this->AttrValue.compare(value))
		return true;
	else
		return false;
}

///////////////////////////////////////////////////////
//////Value of a XML node
class XMLValue
{
public:
	enum NodeType {TEXT, TAG, SELF_CLOSE_TAG, COMMENT, PROC_INSTR};
	XMLValue(const NodeType tagtype,const std::string tagname);
	void AddAttr(std::string name, std::string value);
	bool RemoveAttr (std::string name, std::string value);
	int Find(std::string name, std::string value);

	NodeType& Type();
	std::string& TagName();
	std::vector<Attribute>& attributes(); 
private:
	NodeType _Type;
	std::string _TagName;
	std::vector<Attribute> _attributes; 
};
//----<construct XMLValue with specified type (e.g. TEXT, TAG) of XML Node and the name or content string of this node>------
inline XMLValue::XMLValue(const NodeType tagtype,const std::string tagname) :_Type(tagtype), _TagName(tagname) {};

//----<add an attribute to this node with specified attribute name and value>------
inline void XMLValue::AddAttr(std::string name, std::string value){_attributes.push_back(Attribute(name,value));}

//----<remove an attribute from this node with specified attribute name and value. Successful finding the attribute and removing it returns true else return false>----
inline bool XMLValue::RemoveAttr (std::string name, std::string value)
{
	std::vector<Attribute>::iterator iter = _attributes.begin();
	for (iter=_attributes.begin(); iter!=_attributes.end(); ++iter)
	{
		if(iter->compare(name,value))
		{
			_attributes.erase(iter);
			return true;
		}
	}
	return false;
}

//---<get the type of this node>---
inline XMLValue::NodeType& XMLValue::Type()
{
	return _Type;
}

//---<get the tag name or content of this node>---
inline std::string& XMLValue::TagName()
{
	return _TagName;
}

//---<get the attributes of this node>---
inline std::vector<Attribute>& XMLValue::attributes()
{
	return _attributes;
}

//---<find out the position of the attribute with specified attribute name and value in the vector>---
inline int XMLValue::Find(std::string name, std::string value)
{
	std::vector<Attribute>::iterator iter = _attributes.begin();
	int cnt = 0;
	for (iter=_attributes.begin(); iter!=_attributes.end(); ++iter)
	{
		if(name == iter->AttrName && value == iter->AttrValue)
			return cnt;
		++cnt;
	}
	return -1;
}


///////////////////////////////////////////////////////////////////
////The node of the parser tree
template <typename T>
class PNode
{
public:
	PNode (const T& t);
	PNode<T>* add(PNode<T>* pPNode);
	PNode<T>* remove(PNode<T>* pPNode);
	PNode<T>* nextUnmarkedChild();
	T& value();
	bool isMarked();
	void setMarkState(const bool newstate);
	void clearMarks();
	std::vector<T> getChildren();
private:
	std::vector<PNode<T>*> children;     //child nodes of this node
	T t_;                                //value of the node
	bool marked_;                        //the mark to show whether this node is visited in a tree search process
};

typedef PNode<XMLValue> XMLNode;        //rename PNode<XMLValue> to a simple name XMLNode

//----<construct the node with its value>---
template <typename T>
inline PNode<T>::PNode(const T& t) :t_(t), marked_(false) {};

//----<add a node to this node as a child>---
template <typename T>
inline PNode<T>* PNode<T> ::add(PNode<T>* pPNode)
{
	children.push_back(pPNode);
	return pPNode;
}

//----<get the value of this node>---
template <typename T>
inline T& PNode<T>::value() { return t_;}

//----<if this node is marked?>---
template <typename T>
inline bool PNode<T>::isMarked(){ return marked_; }

//----<remove a child node from this node>---
template <typename T>
inline PNode<T>* PNode<T>::remove(PNode<T>* pPNode)
{
	std::vector<PNode<T>*>::iterator iter = children.begin();
	for (iter=children.begin(); iter!=children.end(); ++iter)
	{
		if(pPNode == *iter)
		{
			children.erase(iter);
			return this;
		}
	}
	return NULL;
}

//----<set whether this node is marked, true means marked and false means unmarked>---
template <typename T>
void PNode<T>::setMarkState(const bool newstate)
{
	marked_ = newstate;
}

//----<find out the pointer to the next unmarked child node>---
template <typename T>
PNode<T>* PNode<T>::nextUnmarkedChild()
{
	for(size_t i=0;i<children.size(); ++i)
		if(!children[i]->isMarked())
			return children[i];
	return 0;
}

//----<get the vector of the values of child nodes>---
template <typename T>
inline std::vector<T> PNode<T> ::getChildren()
{
	std::vector<T> cldvalues;
	for(size_t i=0;i<children.size(); ++i)
		cldvalues.push_back (children[i]->value());
	return cldvalues;
}

//----<clear the marks of the child nods of this node>---
template <typename T>
inline void PNode<T> ::clearMarks()
{
	for(size_t i = 0; i < children.size(); ++i)
		children[i]->setMarkState(false);
}

#endif