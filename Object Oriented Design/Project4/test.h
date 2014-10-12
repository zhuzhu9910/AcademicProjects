#ifndef TEST_H
#define TEST_H
/////////////////////////////////////////////////////////////////////
//  Test.h - classes for testing the object inspector              //
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
  Some classes for testing.
  
  Public Interface:
  =================
  Some code....
  .....
  .....

  Build Process:
  ==============
  Build commands
    - devenv Test.sln

  Maintenance History:
  ====================
  ver 1.0 : 05 Apr 10
  - first release

*/
//
struct test
{
	virtual void f(){}
	virtual int fcc(int c) { return c;}

};

struct testc
{
public:
	virtual void tc(){}
	struct testnested
	{
		virtual void vtest() {}
	};
	void tc(int c)
	{
		c = 0;
	}
	virtual ~testc(){}
};

class test1 :public test
{
public:
	test1(int k)
	{
		myf = k;
	}

	void f()
	{
		char* pc = new char;
		delete pc;
	}
	
private:
	int myf;
};

class test2 :public test
{
public:
	test2(int m);
	int test2k(char c, int i, long h);
	void f(int k) {}
private:
	int mm;
};



#endif