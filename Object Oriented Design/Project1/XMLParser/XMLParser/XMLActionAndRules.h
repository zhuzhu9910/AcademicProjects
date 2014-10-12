#ifndef ACTIONSANDRULES_H
#define ACTIONSANDRULES_H
/////////////////////////////////////////////////////////////////////
//  XMLActionAndRules.h - rules for parser and                     //
//						- corresponding actions                    //
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
  This module defines several rule and action classes. The rules are 
  for the parser to match and actions are for excuting corresponding
  actions to build the xml parse tree.

  Public Interface:
  =================
  Toker t(someFile);              // create tokenizer instance
  XmlParts se(&t);                // create a XmlParts attached to tokenizer
  Parser parser(se);              // now we have a parser
  Rule1 r1;                       // create instance of a derived Rule class
  Action1 a1;                     // create a derived action
  r1.addAction(&a1);              // register action with the rule
  parser.addRule(&r1);            // register rule with parser
  while(se.getSemiExp())          // get semi-expression
    parser.parse();               //   and parse it

  Build Process:
  ==============
  Required files
    - Parser.h, Parser.cpp, XMLActionAndRules.h, XMLActionAndRules.cpp,
      xmlElementParts.h, xmlElementParts.cpp, tokenizer.h, tokenizer.cpp
  Build commands (either one)
    - devenv Project1HelpS06.sln
    - cl /EHsc /DTEST_PARSER parser.cpp ActionsAndRules.cpp \
         semiexpression.cpp tokenizer.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 1.0 : 03 Feb 10
  - first release

*/
//
#include <queue>
#include <string>
#include "Parser.h"
#include "ITokCollection.h"

////////////////////////////////////////////////////////////////////////
/////Record the state of the parser
class ParserState
{
public:
	enum PState {START, PARSE, END, COMMENT, ERROR, NORMALEND};
	//--<construct a parserstate with START as its initial state>--
	ParserState() {state = START;};
	
	//--<test and switch the parser state and return the latest state>--
	PState TestState(ITokCollection* pitc,std::stack<PNode<XMLValue>*>* pstack)
	{
		switch (state)
		{
		case START: 
			//if stack is not empty then switch state from START to PARSE
			if(!pstack->empty()) 
				state = PARSE; 
			break;
		//under a non-start state, when stack becomes empty or no more token input
		//the parse process ends and parser state switch to END 
		case PARSE:
			if(pstack->empty() || 0 == pitc->length())
				state = END; 
			break;
		case COMMENT:
			if(pstack->empty() || 0 == pitc->length())
				state = END; 
			break;
		}	
		return state;
	}

	//--<test if the parse process ends normally or there may be some syntax errors in the xml string or file>--
	bool NormalEnd(ITokCollection* pitc,std::stack<PNode<XMLValue>*>* pstack)
	{
		if( pstack->size() < 2 && 0 == pitc->length()) 
			return true; 
		else 
			return false;
	}
	//--<switch to COMMENT state>--
	void comment(){state = COMMENT;}

	//--<back to PARSE state>--
	void uncomment(){state = PARSE;}

	//--<reset the state to START>--
	void reStart(){state = START;}
private:
	PState state;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
////Rule class for XML
class XMLRule : public IRule
{
public:
	//--<construct the XMLRule class with pointers to parser stack and parser state>--
	XMLRule(std::stack<PNode<XMLValue>*>* pstack, ParserState* pstate) :pStack(pstack), pState(pstate){};

protected:
	std::stack<PNode<XMLValue>*>* pStack;   //pointer to the parser stack
	ParserState* pState;                    //pointer to the parser state
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
////Rule for the end of a tag element
class ElementEnd : public XMLRule
{
public:
	ElementEnd(std::stack<PNode<XMLValue>*>* pstack, ParserState* pstate) :XMLRule(pstack, pstate) {};
	bool doTest(ITokCollection* pTc)
	{	
		if(	pTc->length() > 1
			&&0 == (*pTc)[0].compare("<") 
			&& 0 == (*pTc)[1].compare("/")
			&& ParserState::END != pState->TestState(pTc,pStack)
			&& ParserState::COMMENT != pState->TestState(pTc,pStack))
		{
			doActions(pTc);
			return true;
		}
		return false;
	  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect a start of a tag element
class ElementStart : public XMLRule
{
public:
	ElementStart(std::stack<PNode<XMLValue>*>* pstack, ParserState* pstate) :XMLRule(pstack, pstate) {};
	bool doTest(ITokCollection* pTc)
	{	

		if(pTc->length() > 1
			&&0 == (*pTc)[0].compare("<") 
			&& 0 != (*pTc)[1].compare("/")
			&& 0 != (*pTc)[1].compare("?")
			&& 0 != (*pTc)[1].compare("!")
			&& 0 != (*pTc)[pTc->length() - 2].compare("/")
			&& ParserState::END != pState->TestState(pTc,pStack)
			&& ParserState::COMMENT != pState->TestState(pTc,pStack))
		{
			doActions(pTc);
			return true;
		}
		return false;
	  }

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect a self-closed tag element
class ElementSelfClosed : public XMLRule
{
public:
	ElementSelfClosed(std::stack<PNode<XMLValue>*>* pstack, ParserState* pstate) :XMLRule(pstack, pstate) {};
	bool doTest(ITokCollection* pTc)
	{	

		if(pTc->length() > 1
			&&0 == (*pTc)[0].compare("<")
			&& 0 == (*pTc)[pTc->length() - 2].compare("/")
			&& 0 != (*pTc)[1].compare("/")
			&& 0 != (*pTc)[1].compare("?")
			&& 0 != (*pTc)[1].compare("!")
			&& ParserState::END != pState->TestState(pTc,pStack)
			&& ParserState::COMMENT != pState->TestState(pTc,pStack))
		{
			doActions(pTc);
			return true;
		}
		return false;
	  }

};

////////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect a text element in xml
class Text: public XMLRule
{
public:
	Text(std::stack<PNode<XMLValue>*>* pstack, ParserState* pstate) :XMLRule(pstack, pstate) {};
	bool doTest(ITokCollection* pTc)
	{	

		if(pTc->length() > 0
			&&0 != (*pTc)[0].compare("<") 
			&& ParserState::END != pState->TestState(pTc,pStack)
			&& ParserState::COMMENT != pState->TestState(pTc,pStack))
		{
			doActions(pTc);
			return true;
		}
		return false;
	  }
};


/////////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect a normal comment element (without symbols like '<','>')
class Comment: public XMLRule
{
public:
	Comment(std::stack<PNode<XMLValue>*>* pstack, ParserState* pstate) :XMLRule(pstack, pstate) {};
	bool doTest(ITokCollection* pTc)
	{	

		if(pTc->length() > 2
			&&0 == (*pTc)[0].compare("<") 
			&& 0 == (*pTc)[1].compare("!")
			&& 0 == (*pTc)[2].compare("--")
			&& 0 == (*pTc)[pTc->length() - 2].compare("--")
			&& 0 == (*pTc)[pTc->length() - 1].compare(">")
			&& ParserState::END != pState->TestState(pTc,pStack)
			&& ParserState::COMMENT != pState->TestState(pTc,pStack))
		{
			doActions(pTc);
			return true;
		}
		return false;
	  }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
////detect a start of a comment with symbols like '<' and '>'
class CommentStart: public XMLRule
{
public:
	CommentStart(std::stack<PNode<XMLValue>*>* pstack, ParserState* pstate) :XMLRule(pstack, pstate) {};
	bool doTest(ITokCollection* pTc)
	{	

		if(pTc->length() > 2
			&&0 == (*pTc)[0].compare("<") 
			&& 0 == (*pTc)[1].compare("!")
			&& 0 == (*pTc)[2].compare("--")
			&& (0 != (*pTc)[pTc->length() - 2].compare("--")
			|| 0 != (*pTc)[pTc->length() - 1].compare(">"))
			&& ParserState::END != pState->TestState(pTc,pStack)
			&& ParserState::COMMENT != pState->TestState(pTc,pStack))
		{
			pState->comment();
			doActions(pTc);
			return true;
		}
		return false;
	  }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
////Rule to cope with wrong detected xml element parts when parse comment with '<' or '>'
class ParseComment: public XMLRule
{
public:
	ParseComment(std::stack<PNode<XMLValue>*>* pstack, ParserState* pstate) :XMLRule(pstack, pstate) {};
	bool doTest(ITokCollection* pTc)
	{	

		if( ParserState::COMMENT == pState->TestState(pTc,pStack)
			&& !(0 == (*pTc)[0].compare("<") 
			&& 0 == (*pTc)[1].compare("!")
			&& 0 == (*pTc)[2].compare("--"))
			&&( pTc->length() < 2 
			|| (pTc->length() >= 2
			&& !(0 == (*pTc)[pTc->length() - 2].compare("--")
			&& 0 == (*pTc)[pTc->length() - 1].compare(">"))))
			)
		{
			doActions(pTc);
			return true;
		}
		return false;
	  }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect a comment end of the comments with symbols like '<' and '>'
class CommentEnd: public XMLRule
{
public:
	CommentEnd(std::stack<PNode<XMLValue>*>* pstack, ParserState* pstate) :XMLRule(pstack, pstate) {};
	bool doTest(ITokCollection* pTc)
	{	

		if( ParserState::COMMENT == pState->TestState(pTc,pStack)
			&&(pTc->length() >= 2
			&& 0 == (*pTc)[pTc->length() - 2].compare("--")
			&& 0 == (*pTc)[pTc->length() - 1].compare(">"))
			)
		{
			pState->uncomment();
			doActions(pTc);
			return true;
		}
		return false;
	  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect a process instruct element
class ProcInstr : public XMLRule
{
public:
	ProcInstr(std::stack<PNode<XMLValue>*>* pstack, ParserState* pstate) :XMLRule(pstack, pstate) {};
	bool doTest(ITokCollection* pTc)
	{	

		if(pTc->length() > 1
			&&0 == (*pTc)[0].compare("<") 
			&& 0 == (*pTc)[1].compare("?")
			&& ParserState::END != pState->TestState(pTc,pStack)
			&& ParserState::COMMENT != pState->TestState(pTc,pStack))
		{
			doActions(pTc);
			return true;
		}
		return false;
	  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
////Rule to test if there are some errors when the parser process ends
class ErrorTest: public XMLRule
{
public:
	ErrorTest(std::stack<PNode<XMLValue>*>* pstack, ParserState* pstate) :XMLRule(pstack, pstate) {};
	bool doTest(ITokCollection* pTc)
	{	

		if( ParserState::END == pState->TestState(pTc,pStack)
			&& !pState->NormalEnd(pTc, pStack))
		{
			doActions(pTc);
			return true;
		}
		return false;
	  }
};


//////////////////////////////////////////////////////////////////////////////////////////////////
////Action class for XML
class XMLAction : public IAction
{
public:
	//--<construct the class with the parse stack and parse tree>--
	XMLAction(std::stack<PNode<XMLValue>*>* pstack, XMLTree* ptree) :pStack(pstack), pTree(ptree){};
protected:
	std::stack<PNode<XMLValue>*>* pStack;    //pointer to the parse stack
	XMLTree* pTree;                          //pointer to the parse tree
};

/////////////////////////////////////////////////////////////////////////////////////////////////
////Actions to deal with a end tag element. Pop the top element from the stack.
class EndFound : public XMLAction
{
public:
	EndFound(std::stack<PNode<XMLValue>*>* pstack, XMLTree* ptree): XMLAction(pstack, ptree){};
  void doAction(ITokCollection* pTc)
  {
    pStack->pop();
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////
////Actions to deal with a start tag element. Create a new node, store the element information in
////it and push it into the stack.
class StartFound : public XMLAction
{
public:
	StartFound(std::stack<PNode<XMLValue>*>* pstack, XMLTree* ptree): XMLAction(pstack, ptree){};
  void doAction(ITokCollection* pTc)
  {
	//create a new xml node
	XMLValue* xmlvalue = new XMLValue(XMLValue::TAG, (*pTc)[1]);
    PNode<XMLValue>* pnewnode = new PNode<XMLValue>(*xmlvalue);
	
	//find attribute name and value pairs and add them to the new created node
	for (int i = 0; i < pTc->length(); ++i)
	{
		if (0 == (*pTc)[i].compare("="))
		{
			pnewnode->value().AddAttr((*pTc)[i - 1], (*pTc)[i + 1]);
		}
	}
	//make new node as a child node of the top node in the stack
	pStack->top()->add(pnewnode);
	//push the new node into the stack
	pStack->push(pnewnode);
	
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////
////Actions to deal with a self-closed tag element. Create a new node, store the element 
////information in it and add it into the child nodes of the top node in the stack.
class SelfClosedFound : public XMLAction
{
public:
	SelfClosedFound(std::stack<PNode<XMLValue>*>* pstack, XMLTree* ptree): XMLAction(pstack, ptree){};
  void doAction(ITokCollection* pTc)
  {
    //create new node
	XMLValue* xmlvalue = new XMLValue(XMLValue::TAG, (*pTc)[1]);
    PNode<XMLValue>* pnewnode = new PNode<XMLValue>(*xmlvalue);
	//strore attributes into the node
	for (int i = 0; i < pTc->length(); ++i)
	{
		if (0 == (*pTc)[i].compare("="))
		{
			pnewnode->value().AddAttr((*pTc)[i - 1], (*pTc)[i + 1]);
		}
	}
	//add it to the child nodes of the top node in the stack
	pStack->top()->add(pnewnode);
	
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////
////Actions to deal with a text element. Create a new node, store the element 
////information in it and add it into the child nodes of the top node in the stack.
class TextFound : public XMLAction
{
public:
	TextFound(std::stack<PNode<XMLValue>*>* pstack, XMLTree* ptree): XMLAction(pstack, ptree){};
  void doAction(ITokCollection* pTc)
  {
	//create a new node
    XMLValue* xmlvalue = new XMLValue(XMLValue::TEXT, "");
    PNode<XMLValue>* pnewnode = new PNode<XMLValue>(*xmlvalue);
	//store the text into the node
	for (int i = 0; i < pTc->length(); ++i)
	{
		pnewnode->value().TagName().append((*pTc)[i]);
		pnewnode->value().TagName().append(" ");

	}
	//add the node to the top node in the stack as a child node
	pStack->top()->add(pnewnode);
	
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////
////Actions to deal with the normal comment.create a new node, store the information, add it to 
////the top node in the stack as a child node.
class CommentFound : public XMLAction
{
public:
	CommentFound(std::stack<PNode<XMLValue>*>* pstack, XMLTree* ptree): XMLAction(pstack, ptree){};
  void doAction(ITokCollection* pTc)
  {
	//create new node
	XMLValue* xmlvalue = new XMLValue(XMLValue::COMMENT, "");
    PNode<XMLValue>* pnewnode = new PNode<XMLValue>(*xmlvalue);
	//store the comment text into the node and ignore the xml symbols
	for (int i = 3; i < pTc->length() - 2; ++i)
	{
		pnewnode->value().TagName().append((*pTc)[i]);
		pnewnode->value().TagName().append(" ");

	}
	//add the new node to the top node in the stack as a child node
	pStack->top()->add(pnewnode);
	
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////
////Actions to deal with process instruction. create a node for the element and add it to the root
////node as a childe.
class ProcInstrFound : public XMLAction
{
public:
	ProcInstrFound(std::stack<PNode<XMLValue>*>* pstack, XMLTree* ptree): XMLAction(pstack, ptree){};
  void doAction(ITokCollection* pTc)
  {
	//create new node
	XMLValue* xmlvalue = new XMLValue(XMLValue::PROC_INSTR, (*pTc)[2]);
    PNode<XMLValue>* pnewnode = new PNode<XMLValue>(*xmlvalue);
	//find and add attributes to the node
	for (int i = 0; i < pTc->length() - 1; ++i)
	{
		if (0 == (*pTc)[i].compare("="))
		{
			pnewnode->value().AddAttr((*pTc)[i - 1], (*pTc)[i + 1]);
		}
	}
	//add the node to the root node as a child
	pTree->getroot()->add(pnewnode);
	
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////
////Actions to deal with the start of comment with symbols like '<' and '>'. create a new node, 
////strore the comment text in the node, add it as a child of the top node in the stack and push
////it into the stack.
class StartCommentFound : public XMLAction
{
public:
	StartCommentFound(std::stack<PNode<XMLValue>*>* pstack, XMLTree* ptree): XMLAction(pstack, ptree){};
  void doAction(ITokCollection* pTc)
  {
	//create a new node
	XMLValue* xmlvalue = new XMLValue(XMLValue::COMMENT, "");
    PNode<XMLValue>* pnewnode = new PNode<XMLValue>(*xmlvalue);
	//append the text and store the text into the node
	std::string cmt;
	for (int i = 3; i < pTc->length(); ++i)
	{
		cmt.append((*pTc)[i]);
		cmt.append(" ");
	}
	xmlvalue->TagName() = cmt;
	//add the node as a child of the top node in the stack
	pStack->top()->add(pnewnode);
	//push the node into the stack
	pStack->push(pnewnode);
	
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////
////Actions to deal with the wrong returned xml element parts during parsing a special comment 
////element. append and store the text into the top node in the stack
class ParseCommentAct : public XMLAction
{
public:
	ParseCommentAct(std::stack<PNode<XMLValue>*>* pstack, XMLTree* ptree): XMLAction(pstack, ptree){};
  void doAction(ITokCollection* pTc)
  {
	//store and append the text in the top node in the stack
	std::string cmt;
	pStack->top()->value().TagName().append(pTc->show());
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////
////Actions to deal with a error found in the end of the parse process. Show the warn and empty 
////the stack.
class ErrorAlert : public XMLAction
{
public:
	ErrorAlert(std::stack<PNode<XMLValue>*>* pstack, XMLTree* ptree): XMLAction(pstack, ptree){};
  void doAction(ITokCollection* pTc)
  {
	  std::cout<<"\n"<<"Syntax Error! Please check your XML code!"<<std::endl;
	  if(!pStack->empty())
	  {
		  pStack->pop();
	  }
  }
};

#endif