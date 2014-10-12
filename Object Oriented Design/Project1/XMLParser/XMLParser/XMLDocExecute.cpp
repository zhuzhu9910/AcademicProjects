/////////////////////////////////////////////////////////////////////
//  XMLDocExecute.cpp - Demostration for project 1                 //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project1 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////

#include "XMLActionAndRules.h"
#include "XMLConfigurParser.h"
#include "XMLDisplay.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "XMLDocExecute.h"
#include <iostream>
#include <string>

//----< constructor initializes pointers to all parts >----------
XMLDocExecute::XMLDocExecute() :pConfigure(NULL),pDoc(NULL), pParser(NULL) {}

//----< destructor releases all parts >--------------------------
XMLDocExecute::~XMLDocExecute()
{
	delete pConfigure;
	delete pDoc;
}


//----< configure the parser and display, assemble all parts >------------
bool XMLDocExecute::Build()
{
	pDoc = new XMLDocument();
	pConfigure = new XMLConfigParser(pDoc);
	pParser = pConfigure->Build();
	if(pParser)
		return true;
	else
		return false;
}

//----<parse the input xml file>----------
void XMLDocExecute::Parse()
{
	while(pParser->next())
       pParser->parse();
	pParser->parse();
}


//----<get the xml document>---
XMLDocument& XMLDocExecute::GetDocument()
{
	return *pDoc;
}

#ifdef EXECUTE

int main(int argc, char* argv[])
{
  // collecting tokens from files, named on the command line

  if(argc < 2)
  {
    std::cout 
      << "\n  please enter name of file to process on command line\n\n";
	::system("pause");
    return 1;
  }
	
	//create document and parser
	XMLDocExecute xmlexef;
	xmlexef.Build();
	XMLDocument& document = xmlexef.GetDocument();
	
	//create input and output string
	std::string input;
	std::string output;
	document.write(output,false);
	//string to store the argument specifying the input type
	std::string typestr;

    try
    {	
		if(2 == argc || "file" == (typestr = argv[2]))
		{
			//get the file name from the command line and parse the file
			std::cout << "\n  Processing file " << argv[1];
			std::cout << "\n  " << std::string(16 + strlen(argv[1]),'-');
			if(!document.read(argv[1]))
			{
			  std::cout << "\n  could not open file " << argv[1] << std::endl;
			}
		}
		else if (3 == argc && "string" == (typestr = argv[2]))
		{
			//get string from command line and parse it
			input = argv[1];
			document.read(input,false);
		}
		else
		{
			std::cout << "\n  Please enter properate argument. First argument is for a file name "
				<<"or xml string. Second one is either file or string to specify the input type." <<std::endl;
			::system("pause");
			return 1;
		}
		  // now that parser the input
		xmlexef.Parse();
		
		//show the original parse tree
		std::cout<<"\n======================This is the Demo of Project 1===========================\n";
		std::cout<<"All requirements in project 1 has been met and this is a simple demo\n";
		std::cout<<"First, display the corresponding tree structure of the specified xml file\n";
		std::cout<<"===============================================================================\n";
		document.DisplayTree();
		std::cout<<output.c_str();

		//add a element and show the parse tree again
		std::cout<<"===============================================================================\n";
		std::cout<<"\nNow, add a child element with tag \"testTag\" to an element with id \"testId\"\n";
		std::cout<<"Display the resulting tree\n";
		std::cout<<"===============================================================================\n";
		XMLElement testelm;
		testelm.makeElement(XMLValue::TAG,"testTag");
		XMLElement targetelm = document.GetElementbyId("testId");
		document.AddElement(testelm,targetelm);
		document.DisplayTree();
		std::cout<<output.c_str();

		//remove the added element and show the parse tree
		std::cout<<"===============================================================================\n";
		std::cout<<"\nNow, remove the added element and redisplay the tree\n";
		std::cout<<"===============================================================================\n";
		document.RemoveElement(testelm);
		document.DisplayTree();
		std::cout<<output.c_str();
		std::cout<<"===============================================================================\n";
		std::cout<<"\nDisplay the rebuilt xml\n";
		std::cout<<"===============================================================================\n";

		//show the rebuilt xml document
		document.DisplayXML();
		std::cout<<output.c_str();

		std::cout<<"===============================================================================\n";
		std::cout<<"\nCreate two new XMLDocuments as copies of the former document object\n";
		std::cout<<"===============================================================================\n";
		XMLDocument doc_cp = document;
		XMLDocument doc_as;
		doc_as = document;

		//test the copy (using copy constructor) outputing to a file
		std::cout<< "\nAttach one new copied document to an output file named output.txt,\n the other still output to string.\n";
		std::cout<<"Output to file(display rebuilt xml):\n"<< std::string(10,'-') << std::endl;
		doc_cp.write("..\\XMLParser\\output.txt");
		doc_cp.DisplayXML();

		//test the assigned copy outputing to the string
		std::cout<<std::string(10,'-') <<"\nOutput to string(display rebuilt xml):\n"<< std::string(10,'-') << std::endl;
		output.clear();                                      //clear output
		doc_as.DisplayXML();
		std::cout<<output.c_str();

		//test finding by tag
		std::cout<<"\n"<<std::string(10,'=')<<"\nFind Element test by tag:\n"<< std::string(10,'-') << std::endl;
		std::vector<XMLElement> elmsfound = document.GetElementbyTag("test");
		if(elmsfound.empty())
			std::cout<<"NO PROPERATE FOUND\n";
		for (size_t i = 0; i < elmsfound.size(); ++i)
		{
			std::cout<<elmsfound[i].PrintElement().c_str()<<std::string(10,'-') << std::endl;
		}

		//add attribute
		std::cout<<"\n"<<std::string(10,'=')<<"\nAdd an attribute Project = 1 to the testTag:\n"<< std::string(10,'-') << std::endl;
		testelm.getValue().AddAttr("Project","1");
		//remove the attribute
		std::cout<<testelm.PrintElement().c_str();
		std::cout<<"\n"<<std::string(10,'=')<<"\nRemove the attribute Project = 1 from the testTag:\n"<< std::string(10,'-') << std::endl;
		testelm.getValue().RemoveAttr("Project","1");
		std::cout<<testelm.PrintElement().c_str();



    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
 // }
  ::system("pause");
}
#endif