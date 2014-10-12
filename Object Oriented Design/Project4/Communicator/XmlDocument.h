#ifndef XMLDOCUMENT_H
#define XMLDOCUMENT_H
/////////////////////////////////////////////////////////////////////
//  XmlDocument.h - Wrapper for XML documents                      //
//  ver 1.3                                                        //
//                                                                 //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Prototype for CSE687 Pr1, Sp09                  //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This package provides an XmlDocument class.  Its function is to build
  and internal parse tree representation of an XML document, and support
  loading, editing, and saving documents.

  Build Process:
  ==============
  Required files
    - XmlDocument.h, XmlDocument.cpp, XmlElementParts.h, XmlElementParts.cpp,
      XmlNodes.h, XmlNodes.cpp, MTree.h, MTree.cpp, Tokenizer.h, Tokenizer.cpp
  Build commands (either one)
    - devenv XmlProcessing.sln
    - cl /EHsc XmlDocument.cpp XmlElementParts.cpp XmlNodes.cpp Tokenizer.cpp

  Maintenance History:
  ====================
  ver 1.3 : 18 Mar 10
  - In response to bug reported by Karthick Soundararaj, added checks for
    empty XmlNodeStack before popping and checking top.  Bug caused unhandled
    structured exception for some cases of invalid XML.
  - also added attribute check in XmlProcInstrNode::addAttribue(...) in the
    XmlNode package
  ver 1.2 : 27 Feb 10
  - provided enQuote function to prevent tokenizer from rearranging XML
    text node's text
  - fixed bug in findelement
  ver 1.1 : 22 Feb 10
  - made Parse() a private function
  - reduced the number of find functions by using an enumeration:
    enum findType { tag, attribName, attribValue }; 
  ver 1.0 : 13 Feb 10
  - first release

*/
#include <stack>
#include <vector>
#include "XmlNodes.h"
#include "MTree.h"
#include "XmlElementParts.h"

namespace XmlProcessing
{
  class XmlDocument
  {
  public:
    enum findType { tag, attribName, attribValue };
    XmlDocument() {}
    bool& verbose() { return verbose_; }
    // compiler generated copy and assignment are correct for XmlDocument
    void loadFile(const std::string& filename);
    void loadString(const std::string& xmlStr);
    void clear();
    std::vector<XmlNode*> findElements(const std::string& arg, findType ft);
    XmlNode* findElement(const std::string& arg, findType ft, XmlNode* pNode=0);
    XmlNode* insertElement(XmlNode* pParent, XmlNode* pNewChild);
    XmlNode* deleteElement(XmlNode* pParent, XmlNode* pChild);
    std::string ToString(bool format=true);
    bool ToFile(const std::string& fileName);
  private:
    std::string load;
    std::string enQuoteXmlText(const std::string& XmlText);
    void parse(bool fromFile=true);
    void parseParts(std::stack<XmlNode*>&, XmlParts&);
    void ProcessComment(std::stack<XmlNode*>& XmlNodeStack, XmlParts& parts);
    void ProcessText(std::stack<XmlNode*>& XmlNodeStack, XmlParts& parts);
    void ProcessProcInstr(std::stack<XmlNode*>& XmlNodeStack, XmlParts& parts);
    void ProcessElement(std::stack<XmlNode*>& XmlNodeStack, XmlParts& parts);
    void ProcessEndElement(std::stack<XmlNode*>& XmlNodeStack, XmlParts& parts);
    TMTree::MTree<XmlNode> tree;
    bool format_;
    bool verbose_;
  };
  //----< save filename for toker attachment >-----------------------------

  inline void XmlDocument::loadFile(const std::string& filename)
  {
    load = filename;
    parse();
  }
  //----< save XML string for toker attachment >---------------------------

  inline void XmlDocument::loadString(const std::string& XmlText)
  {
    load = XmlText;
    parse(false);
  }
  //----< remove all elements of the XML parse tree >----------------------

  inline void XmlDocument::clear() { tree.clear(); }
}
#endif
