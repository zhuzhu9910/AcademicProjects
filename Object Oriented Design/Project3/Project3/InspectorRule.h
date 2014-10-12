#ifndef INSPECTORRULE_H
#define INSPECTORRULE_H
/////////////////////////////////////////////////////////////////////
//  InspectorRules.h - declares a rule for analyzing C++ code      //
//  ver 1.0                                                        //
//                                                                 //
//  Language:     Visual C++ 2008                                  //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project3 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module defines a rule class for analyzing C++ source code. The
  purpose of this class is as a base class for other rule class of C++
  source code analyzing. It provides some utilities to the job for such
  kind of classes.
  
  Public Interface:
  =================
  Toker t(someFile);              // create tokenizer instance
  SemiExp se(&t);                 // create a SemiExp attached to tokenizer
  Parser parser(se);              // now we have a parser
  InspectorRule<0> rule;          // create instance of a InspectorRule class
  parser.addRule(&r1);            // register rule with parser
  while(se.getSemiExp())          // get semi-expression
    parser.parse();               //   and parse it

  Build Process:
  ==============
  Required files
    - Parser.h, Parser.cpp, InspectorRule.h, InspectorRule.cpp,
      SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp
  Build commands (either one)
    - devenv Project3.sln
    - cl /EHsc /DTEST_PARSER parser.cpp InspectorRule.cpp \
         semiexpression.cpp tokenizer.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 1.0 : 01 Apr 10
  - first release

*/
//
#include <stack>
#include "Parser.h"
#include "ITokCollection.h"
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
////Class for handling the shared stack
template<typename T, int i = 0>
class SharedStack
{
public:
////----<access the static stack>-----------------------------------------------------------
	static std::stack<T>& stack()
	{
		return _stack;
	}
private:
	static std::stack<T> _stack;
};

template<typename T, int i>
std::stack<T> SharedStack<T,i>::_stack;

////////////////////////////////////////////////////////////////////////////////////////////
////struct for recording the information of a rule
struct RuleRecord
{
	std::string rdname;                                      //rule name
	std::vector<std::string> records;                        //records of this rule
	bool isfired;                                            //if this rule was fired or not
};

////////////////////////////////////////////////////////////////////////////////////////////
////Class of rule to inspect and analyze code in a C++ source code
template<int StackNum = 0>
class InspectorRule :public IRule
{
public:
	typedef SharedStack<std::pair<std::string,std::string>,StackNum> ScopeStack;
	typedef std::pair<std::string,std::string> ScopePair;
	InspectorRule();
	InspectorRule(std::vector<RuleRecord>* rules);
	bool isFunctionDefinition(ITokCollection* pTc);
	bool isClassDeclare(ITokCollection* pTc);
	bool isNameSpaceDeclare(ITokCollection* pTc);
	bool isStructDeclare(ITokCollection* pTc);
	bool isConstructor(ITokCollection* pTc);
	void RemoveBlankLines(ITokCollection* pTc);
	void RemoveHeadAcsCrtl(ITokCollection* pTc);
	void setRuleRecords(std::vector<RuleRecord>* rules);             //
	std::string getFunctionName(ITokCollection* pTc);
	std::vector<std::string> getParentClasses(ITokCollection* pTc);
	virtual bool doTest(ITokCollection* pTc);
	virtual ~InspectorRule(){}
protected:
	void RecordRule(const std::string& rulename,const std::string& record, bool isfired);//
	std::vector<RuleRecord>* prulerds;                               //
private:
	bool testFunDeclare(ITokCollection* pTc);
	bool testClsDeclare(ITokCollection* pTc);
	bool testNSDeclare(ITokCollection* pTc);
	bool testStctDeclare(ITokCollection* pTc);
	bool testOtherScope(ITokCollection* pTc);
	bool testEndScope(ITokCollection* pTc);
};

////----<void constructor for initializing data memeber>-----------------------------------
template<int StackNum>
InspectorRule<StackNum>::InspectorRule() :prulerds(0){}

////----<constructor of InspectorRule for initializing data memeber>-----------------------
template<int StackNum>
InspectorRule<StackNum>::InspectorRule(std::vector<RuleRecord>* rules) :prulerds(rules){}

////----<remove blank lines ("\n" tokens) in a semi-expression>-----------------------------
template<int StackNum>
void InspectorRule<StackNum>::RemoveBlankLines(ITokCollection *pTc)
{
	while(pTc->remove("\n")){}
}

////----<remove front access control keywords>----------------------------------------------
template<int StackNum>
void InspectorRule<StackNum>::RemoveHeadAcsCrtl(ITokCollection *pTc)
{
	if(pTc->length() < 2)
		return;
	if((*pTc)[0] == "public" && (*pTc)[1] == ":")
	{
		pTc->remove("public");
		pTc->remove(":");
	}
	if((*pTc)[0] == "private" && (*pTc)[1] == ":")
	{
		pTc->remove("private");
		pTc->remove(":");
	}
	if((*pTc)[0] == "protected" && (*pTc)[1] == ":")
	{
		pTc->remove("protected");
		pTc->remove(":");
	}
}

////----<test if the semi-expresssion is a class declaration>-------------------------------
template<int StackNum>
bool InspectorRule<StackNum>::isClassDeclare(ITokCollection *pTc)
{
	return (pTc->length() > 1) && (pTc->find("class") != pTc->length()) && (*pTc)[pTc->length() - 1] == "{";
}

////----<test if the semi-expression is a function definition>------------------------------
template<int StackNum>
bool InspectorRule<StackNum>::isFunctionDefinition(ITokCollection *pTc)
{
	RemoveBlankLines(pTc);
	if((pTc->length() > 0 
		&& (*pTc)[pTc->length() - 1] != "{")
		||pTc->length() <= 0
		|| (!ScopeStack::stack().empty()
		&& (ScopeStack::stack().top().first != "namespace"
		&& ScopeStack::stack().top().first != "class"
		&& ScopeStack::stack().top().first != "struct")))
		return false;

	if((*pTc)[0] == "enum" || (*pTc)[0] == "#" || isClassDeclare(pTc)
		|| isNameSpaceDeclare(pTc) || isStructDeclare(pTc)
		|| pTc->find("(") == pTc->length())
		return false;
	return true;
}

////----<test if the semi-expresssion is a constructor definition>--------------------------
template<int StackNum>
bool InspectorRule<StackNum>::isConstructor(ITokCollection *pTc)
{
	if(!isFunctionDefinition(pTc))
		return false;
	
	std::string fname = getFunctionName(pTc);
	std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
	if(!ScopeStack::stack().empty()
		&& (ScopeStack::stack().top().first == "class" || ScopeStack::stack().top().first == "struct")
		&& fname == ScopeStack::stack().top().second)
		return true;

	size_t cpos = fname.find("::");
	if(cpos != std::string::npos && (fname.substr(0,cpos) == fname.substr(cpos + 2, fname.size() - cpos - 2)))
		return true;
	return false;
	
}

////----<test if the semi-expresssion is a namespace declaration>---------------------------
template<int StackNum>
bool InspectorRule<StackNum>::isNameSpaceDeclare(ITokCollection *pTc)
{
	return (pTc->length() > 1) && (pTc->find("namespace") != pTc->length()) && (*pTc)[pTc->length() - 1] == "{";
}

////----<test if the semi-expresssion is a struct declaration>------------------------------
template<int StackNum>
bool InspectorRule<StackNum>::isStructDeclare(ITokCollection *pTc)
{
	return (pTc->length() > 1) && (pTc->find("struct") != pTc->length()) && (*pTc)[pTc->length() - 1] == "{";
}

////----<get the name of a function from the semi-expression>-------------------------------
template<int StackNum>
std::string InspectorRule<StackNum>::getFunctionName(ITokCollection *pTc)
{
	int pos = pTc->find("(");
	if(pos == pTc->length())
		return "";
	if((pos - 3 >= 0) && ((*pTc)[pos - 2] == "::" || (*pTc)[pos - 2] == "::~"))
		return std::string((*pTc)[pos - 3]).append((*pTc)[pos - 2]).append((*pTc)[pos - 1]);
	if((pos - 2 >= 0) && (*pTc)[pos - 2] == "~")
		return std::string((*pTc)[pos - 2]).append((*pTc)[pos - 1]);
	return (*pTc)[pos - 1];
}

////----<get the parent classes from the class declaration>---------------------------------
template<int StackNum>
std::vector<std::string> InspectorRule<StackNum>::getParentClasses(ITokCollection *pTc)
{
	int pos = pTc->find(":");
	std::vector<std::string> prtcls;
	RemoveBlankLines(pTc);
	for(int i = pos; i < pTc->length(); ++i)
	{
		if((*pTc)[i] == "," || (*pTc)[i] == "{")
			prtcls.push_back((*pTc)[i - 1]);
	}
	return prtcls;
}

////----<test the test rules>---------------------------------------------------------------
template<int StackNum>
bool InspectorRule<StackNum>::doTest(ITokCollection *pTc)
{
	pTc->trimFront();
	if(isConstructor(pTc))
		std::cout<<"\nConstructor: "<<getFunctionName(pTc)<<std::endl;

	return testClsDeclare(pTc) || testNSDeclare(pTc) || testStctDeclare(pTc)
		|| testFunDeclare(pTc) || testOtherScope(pTc) || testEndScope(pTc);
}

////////////////////////////////////////////////////////////////////////////////////////////

////----<test of function declaration>------------------------------------------------------
template<int StackNum>
 bool InspectorRule<StackNum>::testFunDeclare(ITokCollection *pTc)
{
	std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
	if(isFunctionDefinition(pTc))
	{
		stk.push(ScopePair("function",getFunctionName(pTc)));
		std::cout<<"\n Function Declare: "<<getFunctionName(pTc).c_str()<<std::endl;
		return true;
	}
	return false;
}

 ////----<test of class declaration>--------------------------------------------------------
 template<int StackNum>
 bool InspectorRule<StackNum>::testClsDeclare(ITokCollection *pTc)
{
	std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
	if(isClassDeclare(pTc))
	{
		stk.push(ScopePair("class",(*pTc)[pTc->find("class") + 1]));
		std::vector<std::string> pps = getParentClasses(pTc);
		std::cout<<"\n Class Declare: "<<(*pTc)[pTc->find("class") + 1].c_str()<<std::endl;
		return true;
	}
	return false;
}

////----<test of end scope>-----------------------------------------------------------------
template<int StackNum>
 bool InspectorRule<StackNum>::testEndScope(ITokCollection *pTc)
{
	std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
	if(pTc->find("}") != pTc->length())
	{
		std::cout<<"\n Go out of Scope:"<<stk.top().first.c_str()<<": "<<stk.top().second.c_str()<<std::endl;
		stk.pop();
		return true;
	}
	return false;
}

 ////----<test and print namespace>---------------------------------------------------------
 template<int StackNum>
 bool InspectorRule<StackNum>::testNSDeclare(ITokCollection *pTc)
{
	std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
	if(isNameSpaceDeclare(pTc))
	{
		stk.push(ScopePair("namespace",(*pTc)[pTc->find("namespace") + 1]));
		std::cout<<"\n NameSpace Declare: "<<(*pTc)[pTc->find("namespace") + 1].c_str()<<std::endl;
		return true;
	}
	return false;
}

 ////----<test and print the other scope beginning>-----------------------------------------
 template<int StackNum>
 bool InspectorRule<StackNum>::testOtherScope(ITokCollection *pTc)
{
	std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
	if(pTc->length() > 1 && (*pTc)[pTc->length() - 1] == "{")
	{
		stk.push(ScopePair("other scope",""));
		std::cout<<"\n Other Scope Found"<<std::endl;
		return true;
	}
	return false;
}

 ////----<test and print the struct declaration>--------------------------------------------
 template<int StackNum>
 bool InspectorRule<StackNum>::testStctDeclare(ITokCollection *pTc)
{
	std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
	if(isStructDeclare(pTc))
	{
		stk.push(ScopePair("struct",(*pTc)[pTc->find("struct") + 1]));
		std::cout<<"\n Struct Declare: "<<(*pTc)[pTc->find("struct") + 1].c_str()<<std::endl;
		return true;
	}
	return false;
}

////----<set the rule record pointer>-------------------------------------------------------
template<int StackNum>
void InspectorRule<StackNum>::setRuleRecords(std::vector<RuleRecord> *rules)
{
	prulerds = rules;
}

////----<add a new record to the rule records>----------------------------------------------
template<int StackNum>
void InspectorRule<StackNum>::RecordRule(const std::string& rulename,const std::string& record, bool isfired)
{
	if(prulerds == 0)
		return;
	std::vector<RuleRecord>::iterator iter;
	for(iter = prulerds->begin(); iter != prulerds->end(); ++iter)
	{
		if(iter->rdname == rulename)
			break;
	}
	if(iter != prulerds->end())
	{
		iter->records.push_back(record);
		iter->isfired = isfired;
	}
	else
	{
		RuleRecord rrd;
		rrd.rdname = rulename;
		rrd.records.push_back(record);
		rrd.isfired = isfired;
		prulerds->push_back(rrd);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
////Struct for recording function infomation
struct FunInfo
{
	unsigned int funline;           //the line the function appear in the file
	std::string funname;            //the name of the function
	std::string filename;           //the file name

////----<== operator for comparing two infomation structure>--------------------------------
	bool operator == (const FunInfo& info) const
	{
		return funline == info.funline && funname == info.funname && filename == info.filename;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
////sturct of recording the infomation of a class
struct ClassObj
{
	std::string name;                                         //class name
	std::vector<std::string> parents;                         //parent list
	std::vector<std::pair<std::string,std::string>> nomFun;   //normal function list
	std::vector<std::pair<std::string,std::string>> vtFun;    //virtual function list
};


#endif