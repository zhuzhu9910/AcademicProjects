#ifndef ACTIONSANDRULES_H
#define ACTIONSANDRULES_H
/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.h - declares new parsing rules and actions     //
//  ver 1.2                                                        //
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
  This module defines several rule and action classes for the class 
  inspector and annotator to analyze the input source code and generate
  target source code with object inspector.
  
  Public Interface:
  =================
  Toker t(someFile);              // create tokenizer instance
  SemiExp se(&t);                 // create a SemiExp attached to tokenizer
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
    - Parser.h, Parser.cpp, ActionsAndRules.h, ActionsAndRules.cpp,
      SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp
  Build commands (either one)
    - devenv Project3.sln
    - cl /EHsc /DTEST_PARSER parser.cpp ActionsAndRules.cpp \
         semiexpression.cpp tokenizer.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 1.2 : 05 Apr 10
  - added rules and actions for class inspector and object inspector
  ver 1.1 : 17 Jan 09
  - changed to accept a pointer to interfaced ITokCollection instead
    of a SemiExpression
  ver 1.0 : 12 Jan 06
  - first release

*/
//
#include <queue>
#include <string>
#include <map>
#include "Parser.h"
#include "ITokCollection.h"
#include "InspectorRule.h"


////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect the Manual page and Maintenance page
template<int StackNum = 0>
class ManualAndMaintenancePages : public InspectorRule<StackNum>
{
public:

////----<constructor>-----------------------------------------------------------------------
	ManualAndMaintenancePages(std::vector<RuleRecord>* rules) :InspectorRule<StackNum>(rules)
	{
		RuleRecord rrd1,rrd2;
		rrd1.rdname = "ManualPage";
		rrd1.isfired = false;
		rrd2.rdname = "MaintenancePage";
		rrd2.isfired = false;
		prulerds->push_back(rrd1);
		prulerds->push_back(rrd2);
	}

////----<test if the comment is a matenance page or a manu page>----------------------------
  bool doTest(ITokCollection* pTc)
  {
	pTc->trimFront();
	//record successive comment
	std::string cmt = findComment(pTc);
	if(!cmt.empty())
	{
		comments.push_back(cmt);
		return true;
	}
	//test if comment found is maintenance page or manu page
	if(!comments.empty())
	{
		if(isMaintenancePage())
		{
			RecordRule("MaintenancePage","** Maintenance Page Exists **",true);
		}
		if(isManuPage())
		{
			RecordRule("ManualPage","** Manu Page Exists **",true);
		}
		comments.clear();
		return true;
	}
	return false;

  }
private:
	std::vector<std::string> comments;

////----<test if any element of an array of keywords exist in the comment>------------------
	bool KeyWordsExist(const std::string keywords[],const size_t size)
	{
		bool result = true;
		bool isrulefired = false;
		for(size_t j =0; j < size; ++j)
		{
			for(size_t i = 0; i < comments.size(); ++i)
			{
				isrulefired = isrulefired || (comments[i].find(keywords[j]) != std::string::npos);
			}
			result = result && isrulefired;
			isrulefired = false;
		}
		return result;
	}

////----<test if the comment is a maintenance page>-----------------------------------------
	bool isMaintenancePage()
	{
		const int size_rl = 3;
		std::string rules[size_rl] = {"Author","Language","Platform"};
		if(comments.size() < 5)
			return false;

		return KeyWordsExist(rules,size_rl);
	}

////----<test if the comment is a manu page>------------------------------------------------
	bool isManuPage()
	{
		const int size_rl = 2;
		std::string rules[size_rl] = {"Public Interface","Maintenance History"};
		if(comments.size() < 6)
			return false;
		return KeyWordsExist(rules,size_rl);
	}

////----<find the comment in a semiexpression>----------------------------------------------
	std::string findComment(ITokCollection* pTc)
	{
		for(int i = 0; i < pTc->length(); ++i)
		{
			if((*pTc)[i].size() < 2)
				continue;
			if((*pTc)[i][0] == '/' && ((*pTc)[i][1] == '/' || (*pTc)[i][1] == '*'))
			{
				return (*pTc)[i];
			}
		}
		return "";
	}
};


////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect the presence of the #ifndef and #define preporcessor statements in a
////header file
template<int StackNum = 0>
class PreProsStat :public InspectorRule<StackNum>
{
public:
////----<constructor>-----------------------------------------------------------------------
	PreProsStat(std::vector<RuleRecord>* rules) :isheader(false), InspectorRule<StackNum>(rules)
	{
		RuleRecord rrd;
		rrd.isfired = false;
		rrd.rdname = "PreProsStat";
		rules->push_back(rrd);
	}

////----<set if the file is parsing now is a header file>-----------------------------------
	void setHeadFile(bool ishf = true)
	{
		isheader = ishf;
	}

////----<test the rule>---------------------------------------------------------------------
	bool doTest(ITokCollection* pTc)
	{
		if(isheader)
		{
			pTc->trimFront();
			if(pTc->length() >= 3 
				&& (*pTc)[0] == "#" 
				&& (*pTc)[1] == "ifndef")
			{
				pkname = (*pTc)[2];
			}

			if(pTc->length() >= 3 
				&& (*pTc)[0] == "#" 
				&& (*pTc)[1] == "define"
				&& !pkname.empty()
				&& (*pTc)[2] == pkname)
			{
				RecordRule("PreProsStat","** Header File Preproccessor Statements Exist **",true);
			}
			return true;
		}
		return false;
	}
private:
	bool isheader;
	std::string pkname;
};

////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect the presence of a test stub in a cpp file
template<int StackNum = 0>
class TestStub :public InspectorRule<StackNum>
{
public:

////----<constructor>-----------------------------------------------------------------------
	TestStub(std::vector<RuleRecord>* rules) 
		:iscppfile(false),iftest(false),extmain(false), InspectorRule<StackNum>(rules)
	{
		RuleRecord rrd;
		rrd.isfired = false;
		rrd.rdname = "TestStub";
		rules->push_back(rrd);
	}

////----<set if the file is parsing now is a cpp file>--------------------------------------
	void SetCppFile(bool iscpp = true)
	{
		iscppfile = iscpp;
	}

////----<test the rule>---------------------------------------------------------------------
	bool doTest(ITokCollection* pTc)
	{
		if(iscppfile)
		{
			if(findiftest(pTc))
				iftest = true;
			if(isMain(pTc) && iftest)
				extmain = true;
			if (extmain && iftest)
			{
				this->RecordRule("TestStub","** Test Stub Exists **",true);
				iscppfile = false;
				iftest = false;
			}
			return true;
		}
		return false;
	}

private:
	bool iscppfile;
	bool iftest;
	bool extmain;

////----<find the macro "#ifdef TEST_..." in current semiexpression>------------------------
	bool findiftest(ITokCollection* pTc)
	{
		if( pTc->length() > 2
			&&(*pTc)[0] == "#"
			&& (*pTc)[1] == "ifdef"
			&& (*pTc)[2].find("TEST") != std::string::npos)
			return true;
		return false;
	}

////----<test if current semiexpression is a main function definition>----------------------
	bool isMain(ITokCollection* pTc)
	{
		for(int i = 0; i < pTc->length() - 1; ++i)
		{
			if( i < pTc->length() - 3
				&& ((*pTc)[i] == "void" || (*pTc)[i] == "int")
				&& (*pTc)[i + 1] == "main"
				&& (*pTc)[i + 2] == "("
				)
				return true;
		}
		return false;
	}
};


////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect non-const global data
template<int StackNum = 0>
class nConstGlobalData :public InspectorRule<StackNum>
{
public:
////----<constructor>-----------------------------------------------------------------------
	nConstGlobalData(std::vector<RuleRecord>* rules) :InspectorRule<StackNum>(rules)
	{
		RuleRecord rrd;
		rrd.isfired = false;
		rrd.rdname = "Non-ConstGlobalData";
		rules->push_back(rrd);
	}
////----<test the rule>---------------------------------------------------------------------
	bool doTest(ITokCollection *pTc)
	{
		pTc->trimFront();
		std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
		if(isnConstGlobaldata(pTc))
		{
			doActions(pTc);
			std::string rst = "** Non-Const Global Data Found **\n";
			rst.append(pTc->show());
			this->RecordRule("Non-ConstGlobalData", rst,true);
			return true;
		}
		return false;
		
	}
private:

////----<test if current semiexpression is a non-const global data>-------------------------
	bool isnConstGlobaldata(ITokCollection *pTc)
	{
		//an array of keywords to exclude
		const int keynum = 10;
		std::string keywords[keynum] = {"namespace", "enum", "#", "typedef", "const", "template", "inline", "struct", "class", "using"};
		//are we in global scope now?
		if(pTc->length() < 3 || (!ScopeStack::stack().empty() && ScopeStack::stack().top().first != "namespace"))
			return false;
		//exclude the code which include the kewords
		for(int i = 0; i < keynum; ++i)
		{
			if((*pTc)[0] == keywords[i])
				return false;
		}
		//test if it is a definition of some kind of global data
		if(((*pTc)[pTc->length() - 1] == ";" 
			|| ((*pTc)[pTc->length() - 1] == "{" 
			&& pTc->find("[") != pTc->length() && pTc->find("(") == pTc->length()))
			&& (*pTc)[0] != "/")
			return true;

		return false;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect non-default constructor which does not use initialization list
template<int StackNum = 0>
class nDftCstrWtOutInitL :public InspectorRule<StackNum>
{
public:

////----<constructor>-----------------------------------------------------------------------
	nDftCstrWtOutInitL(std::vector<RuleRecord>* rules) :InspectorRule<StackNum>(rules)
	{
		RuleRecord rrd;
		rrd.isfired = false;
		rrd.rdname = "ConstructorWithoutInitializationList";
		rules->push_back(rrd);
	}

////----<test the rule>---------------------------------------------------------------------
	bool doTest(ITokCollection *pTc)
	{
		//remove front bank space and line and access keywords
		pTc->trimFront();
		RemoveHeadAcsCrtl(pTc);
		//test if it is a non-default constructor without initialization list
		if(isConstructor(pTc) && ((pTc->find("(") + 1) < (pTc->find(")")))  && (pTc->find(":") == pTc->length()))
		{
			std::string rst = "** Non-default Constructor without Initialization List Found :";
			rst.append(getFunctionName(pTc)).append(" **");
			this->RecordRule("ConstructorWithoutInitializationList",rst,true);
			doActions(pTc);
			return true;
		}
		return false;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect abstract destructor in an abstract class
template<int StackNum = 0>
class nExtAbsDestruc :public InspectorRule<StackNum>
{
public:
////----<constructor>-----------------------------------------------------------------------
	nExtAbsDestruc(std::vector<RuleRecord>* rules)
		:InspectorRule<StackNum>(rules)
	{
		RuleRecord rrd;
		rrd.isfired = false;
		rrd.rdname = "AbstractDestractor";
		rules->push_back(rrd);
	}
////----<test the rule>---------------------------------------------------------------------
	bool doTest(ITokCollection *pTc)
	{
		std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
		bool isfired = false;
		//if the tokenizer is leaving a class, test if it violate this rule
		if(!curclass.empty() 
			&& (stk.empty() 
			|| (((stk.top().first == "class" || stk.top().first == "struct")) 
			&& pTc->find("}") != pTc->length())))
		{
			if(!clsvb.empty() && clsvb.top().first && !clsvb.top().second)
			{
				doActions(pTc);
				std::string rst = "** No Abstract Destructor Found in this Abstract Class: \'";
				rst.append(curclass).append("\' **");
				this->RecordRule("AbstractDestractor",rst,true);
				isfired = true;
			}
			curclass.clear();
			if(!clsvb.empty())
				clsvb.pop();
		}
		//push new class information to record stack
		if(this->isClassDeclare(pTc) || this->isStructDeclare(pTc))
		{
			std::pair<bool,bool> vbc(false,false);
			clsvb.push(vbc);
		}
		//mark if a virtual destructor or a virtual function found in this class
		if(!stk.empty() && (stk.top().first == "class" || stk.top().first == "struct"))
		{
			curclass = stk.top().second;
			if(pTc->find("virtual") != pTc->length())
			{
				clsvb.top().first = true;
				if(pTc->find("~") != pTc->length())
					clsvb.top().second = true;
			}
		}
		return isfired;
	}
private:
	std::stack<std::pair<bool,bool>> clsvb;
	std::string curclass;
};


////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect overloads of virtual function and across class scope
template<int StackNum = 0>
class OverloadsVlt :public InspectorRule<StackNum>
{
public:
////----<constructor>-----------------------------------------------------------------------
	OverloadsVlt(std::vector<RuleRecord>* rules) :InspectorRule<StackNum>(rules)
	{
		RuleRecord rrd;
		rrd.isfired = false;
		rrd.rdname = "OverLoads";
		rules->push_back(rrd);
	}

////----<detect the rule>-------------------------------------------------------------------
	bool doTest(ITokCollection *pTc)
	{
		RemoveBlankLines(pTc);
		std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
		//record current class
		regclass(pTc);
		//record class functions of current class
		if(!stk.empty() && (stk.top().first == "class" || stk.top().first == "struct"))
		{
			int length = pTc->length();
			if(isFunctionDefinition(pTc) 
				|| (length > 3 && (*pTc)[length - 1] == ";" 
				&& pTc->find("(") != length))
			{
				std::pair<std::string,std::string> fun;
				fun.first = getFunctionName(pTc);
				fun.second = getParamSect(pTc);
				if(pTc->find("virtual") != length)
					classes[find(stk.top().second)]->vtFun.push_back(fun);
				else
					classes[find(stk.top().second)]->nomFun.push_back(fun);
			}
			//when go out of current class scope, check the overload rules
			if(pTc->find("}") != length)
			{
				if(find(stk.top().second) != -1 && hasAbstractOVL(*(classes[find(stk.top().second)])))
				{
					std::string rst = "** Class \'";
					rst.append(stk.top().second).append("\' has Abstract Function Overloads **");
					this->RecordRule("OverLoads",rst,true);
				}
				if(find(stk.top().second) != -1 && hasAcrossScopeOVL(*(classes[find(stk.top().second)])))
				{
					std::string rst = "** Class \'";
					rst.append(stk.top().second).append("\' has Across Scope Overloads **");
					this->RecordRule("OverLoads",rst,true);
				}
				
			}
			return true;
		}
		return false;
	}
////----<deconstructor to clear data>-------------------------------------------------------
	~OverloadsVlt()
	{
		clear();
	}
private:
	std::vector<ClassObj*> classes;
////----<test if a function is overloaded in a class>---------------------------------------
	bool findoverloads(const std::string& funname,const std::string& funparams, const ClassObj& clsobj)
	{
			int occnum = 0;
			for(size_t j = 0; j < clsobj.vtFun.size(); ++j)
				if(funname == clsobj.vtFun[j].first
					&& funparams != clsobj.vtFun[j].second)
					occnum++;
			for(size_t j = 0; j < clsobj.nomFun.size(); ++j)
				if(funname == clsobj.nomFun[j].first
					&& funparams != clsobj.nomFun[j].second)
					occnum++;
			if(occnum > 0)
				return true;
			return false;
	}
////----<test if there are any overloads amoung functions between two classes>--------------
	bool findOVLinClass(const ClassObj& clstocheck,const ClassObj& tarcls)
	{
		for(size_t i = 0; i < clstocheck.nomFun.size(); ++i)
			if(findoverloads(clstocheck.nomFun[i].first,clstocheck.nomFun[i].second, tarcls))
				return true;
		for(size_t i = 0; i < clstocheck.vtFun.size(); ++i)
			if(findoverloads(clstocheck.vtFun[i].first,clstocheck.vtFun[i].second, tarcls))
				return true;
		return false;
	}

////----<detect abstract function overloads in current class>-------------------------------
	bool hasAbstractOVL(const ClassObj& clsobj)
	{
		for(size_t i = 0; i < clsobj.vtFun.size(); ++i)
		{
			if(findoverloads(clsobj.vtFun[i].first,clsobj.vtFun[i].second,clsobj))
				return true;
		}
		return false;
	}
	
////----<detect overloads across class scope using DFS>-------------------------------------
	bool hasAcrossScopeOVL(const ClassObj& clsobj)
	{
		std::stack<ClassObj*> stk;
		for(size_t i = 0; i < clsobj.parents.size(); ++i)
		{
			size_t pa = find(clsobj.parents[i]);
			if(pa != -1)
				stk.push(classes[pa]);
		}
		while(!stk.empty())
		{
			ClassObj* curcls = stk.top();
			stk.pop();
			for(size_t i = 0; i < curcls->parents.size(); ++i)
			{
				size_t pa = find(curcls->parents[i]);
				if(pa != -1)
					stk.push(classes[pa]);
			}
			if(findOVLinClass(clsobj,*curcls))
				return true;
			
		}
		return false;
	}
////----<create a class record object and register it>--------------------------------------
	void regclass(ITokCollection *pTc)
	{
		if(isClassDeclare(pTc) || isStructDeclare(pTc))
		{
			int poscls = pTc->find("class");
			int posstrt = pTc->find("struct");
			ClassObj* clsobj = new ClassObj;
			if(poscls != pTc->length())
				clsobj->name = (*pTc)[poscls + 1];
			else
				clsobj->name = (*pTc)[posstrt + 1];
			clsobj->parents = this->getParentClasses(pTc);
			classes.push_back(clsobj);
		}
	}
////----<delete all class record objects and clear the vector>------------------------------
	void clear()
	{
		for(size_t i = 0; i < classes.size(); ++i)
		{
			delete classes[i];
		}
		classes.clear();
	}

////----<find a class by its class name>----------------------------------------------------
	size_t find(std::string clsname)
	{
		for(size_t i = 0; i < classes.size(); ++i)
			if(classes[i]->name == clsname)
				return i;
		return -1;
	}

////----<get the function argument list from the semiexpression>----------------------------
	std::string getParamSect(ITokCollection *pTc)
	{
		bool paset = false;
		std::string paramset;
		for(int i = 0; i < pTc->length(); ++i)
		{
			if((*pTc)[i] == "(")
				paset = true;
			if(paset)
			{
				paramset.append((*pTc)[i]);
			}
			if((*pTc)[i] == ")")
				return paramset;
		}
		return "";
	}
};





////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect function
template<int StackNum = 0>
class FunctionDef :public InspectorRule<StackNum>
{
public:
////----<constructor>-----------------------------------------------------------------------
	FunctionDef() :funline(0){}
////----<test the rule>---------------------------------------------------------------------
	bool doTest(ITokCollection *pTc)
	{
		RemoveBlankLines(pTc);
		funline++;
		std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
		if(isFunctionDefinition(pTc))
		{
			//record the function and do the actions
			doActions(pTc);
			std::string fun;
			if(!stk.empty() && stk.top().first == "class")
				fun.append("In Class ").append(stk.top().second).append(" : ");
			if(!stk.empty() && stk.top().first == "struct")
				fun.append("In Struct ").append(stk.top().second).append(" : ");
			fun.append( pTc->show());
			fun.erase(fun.end() - 1);
			FunInfo info = {funline, fun, curfile};
			funinfos[curfile].push_back(info);
			stk.push(ScopePair("function",getFunctionName(pTc)));
			return true;
		}
		return false;
	}
////----<reset the line counter>------------------------------------------------------------
	void resetlines()
	{
		funline = 0;
	}
////----<get the function list>-------------------------------------------------------------
	std::map<std::string ,std::vector<FunInfo>>& GetFunctions()
	{
		return funinfos;
	}
////----<let the class know current file path>----------------------------------------------
	void setCurFile(const std::string& fname)
	{
		curfile = fname;
	}
private:
	std::map<std::string ,std::vector<FunInfo>> funinfos;
	unsigned int funline;
	std::string curfile;
};

////////////////////////////////////////////////////////////////////////////////////////////
/////Rule to detect class
template<int StackNum = 0>
class ClassDeclare :public InspectorRule<StackNum>
{
public:
////----<detect class and record the entering of this scope>--------------------------------
	bool doTest(ITokCollection *pTc)
	{
		pTc->trimFront();
		std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
		if(isClassDeclare(pTc))
		{
			stk.push(ScopePair("class",(*pTc)[pTc->find("class") + 1]));
			doActions(pTc);
			return true;
		}
		return false;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
/////Rule to detect namespace
template<int StackNum = 0>
class NamespaceDeclare :public InspectorRule<StackNum>
{
public:

////----<detect namespace and record the entering of this scope>----------------------------
	bool doTest(ITokCollection *pTc)
	{
		pTc->trimFront();
		std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
		if(isNameSpaceDeclare(pTc))
		{
			stk.push(ScopePair("namespace",(*pTc)[pTc->find("namespace") + 1]));
			doActions(pTc);
			return true;
		}
		return false;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
/////Rule to detect struct
template<int StackNum = 0>
class StructDeclare :public InspectorRule<StackNum>
{
public:
////----<detect struct and record the entering of this scope>-------------------------------
	bool doTest(ITokCollection *pTc)
	{
		pTc->trimFront();
		std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
		if(isStructDeclare(pTc))
		{
			stk.push(ScopePair("struct",(*pTc)[pTc->find("struct") + 1]));
			doActions(pTc);
			return true;
		}
		return false;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect the entering of other non-specified scope
template<int StackNum = 0>
class OtherScope :public InspectorRule<StackNum>
{
public:
////----<detect entering other scope and record the entering of this scope>-----------------
	bool doTest(ITokCollection *pTc)
	{
		RemoveBlankLines(pTc);
		std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
		if(pTc->length() > 1 && (*pTc)[pTc->length() - 1] == "{" 
			&& !isFunctionDefinition(pTc) && !isNameSpaceDeclare(pTc) 
			&& !isStructDeclare(pTc) && !isClassDeclare(pTc))
		{
			stk.push(ScopePair("other scope",""));
			return true;
		}
		return false;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
////Rule to detect the end of a scope and pop the stack
template<int StackNum = 0>
class ScopeEnd :public InspectorRule<StackNum>
{
public:
////----<test if it is the end of a scope and pop the stack>--------------------------------
	bool doTest(ITokCollection *pTc)
	{
		pTc->trimFront();
		std::stack<std::pair<std::string,std::string>>& stk = ScopeStack::stack();
		if(pTc->find("}") != pTc->length())
		{
			stk.pop();
			return true;
		}
		return false;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
////Action to replace the new operator to the form this project defined
class ModifyNew :public IAction
{
public:
////----<replace the new operator>----------------------------------------------------------
	 void doAction(ITokCollection* pTc)
	 {
		 //find the parameter name
		 int epos = pTc->find("=");
		 std::string paraname;
		 if(epos != pTc->length() && epos > 0)
			 paraname = (*pTc)[epos - 1];
		//replace the new operator with arguments to specify the type and name
		for(int i = 0; i < pTc->length(); ++i)
		{
			if( (*pTc)[i] == "new" && i + 1 < pTc->length())
			{
				std::string newstr = " new(\"";
				newstr.append((*pTc)[i+1]).append("\",\"");
				newstr.append(paraname).append("\")");
				(*pOut)<<newstr.c_str();
			}
			else
				(*pOut)<<" "<<(*pTc)[i].c_str();
		}
		(*pOut)<<"\n";
	 }
////----<get the output object from outside of this class>----------------------------------
 	void setOutPut(std::ofstream *pout)
	{
		pOut = pout;
	}
private:
	std::ofstream *pOut;
};


////////////////////////////////////////////////////////////////////////////////////////////
////Class in a form of rule to replace new and insert logging sentence into functions
template<int StackNum = 1>
class AnnotatorRule :public InspectorRule<StackNum>
{
public:
////----<constructor with input of function list and the name of log file>------------------
	AnnotatorRule(std::vector<FunInfo>* info,const std::string& logfile) 
		:funinfos(info),_logfile(logfile),line(0){}
////----<find new and replace new, find function and insert inspector>----------------------
	bool doTest(ITokCollection *pTc)
	{
		line++;
		//detect and replace new
		if(pTc->find("new") !=pTc->length())
		{
			doActions(pTc);
			return true;
		}

		(*pOut)<<pTc->show()<<"\n";
		RemoveBlankLines(pTc);
		//detect main function and insert the declaration of log manager
		if(pTc->find("main") != pTc->length() && isFunctionDefinition(pTc))
		{
			(*pOut)<<"\nLoggingProccessing::LogManagerRegister::CreateManager(\""<<_logfile.c_str()<<"\");\n";
		}
		//detect recorded function and insert dclaration of inspector instance
		for(size_t i = 0; i < funinfos->size(); ++i)
		{
			if((*funinfos)[i].funline == line)
			{
				std::string insert = "\n";
				insert.append(" LoggingProccessing::FunctionInspector FunIsp(\"").append((*funinfos)[i].funname).append("\");\n");
				(*pOut)<<insert.c_str();
				break;
			}
		}
		return false;
	}
/////----<get the output stream from outside of this class>---------------------------------
	void setOutPut(std::ofstream *pout)
	{
		pOut = pout;
	}
private:
	std::ofstream *pOut;
	std::string _logfile;
	unsigned int line;
	std::vector<FunInfo>* funinfos;
};

#endif
