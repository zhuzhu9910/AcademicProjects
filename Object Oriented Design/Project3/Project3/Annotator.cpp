/////////////////////////////////////////////////////////////////////
//  Annotator.cpp - class for annotating input source code files   //
//  ver 1.0                                                        //
//                                                                 //
//  Language:     Visual C++ 2008                                  //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project3 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////

#include "Annotator.h"
#include <algorithm>

using namespace LoggingProccessing;
////----<annotator constructor inputing the function list, output directory, log file path>-
Annotator::Annotator(const Annotator::FunMap &finfo, const std::string &outcat, const std::string &logpath)
:funinfos(finfo), OutPutPath(outcat), LogPath(logpath){}

////----<get the file name from a path>-----------------------------------------------------
std::string Annotator::getFileName(const std::string &path)
{
	std::string filename;
	size_t posl = path.find_last_of("\\");
	size_t posr = path.find_last_of("/");
	size_t pos;
	if((posl > posr && posl != std::string::npos) || posr == std::string::npos)
		pos = posl;
	else
		pos = posr;

	if(pos == std::string::npos)
		return path;
	else
		return path.substr(pos + 1);
}

////----<execute annoating the code>--------------------------------------------------------
void Annotator::Annotate()
{
	for(FunMap::iterator iter = funinfos.begin(); iter != funinfos.end(); ++iter)
	{
		ConfigParseForAnnotator configure(&(iter->second),LogPath);
		Parser* pParser = configure.Build();
		try{
			if(pParser)
			{
				std::string output;
				output.append(OutPutPath).append(getFileName(iter->first));
				if(!configure.Attach(iter->first))
				{
					std::cout << "\n  could not open file " << iter->first << std::endl;
					continue;
				}

				if(!configure.AttachOutput(output))
				{
					std::cout << "\n  could not open or create file " << output << std::endl;
					continue;
				}
				configure.getOutputFileStream()<<"#include \"Inspector.h\"\n"<<"#include \"LogManager.h\"\n" ;
				while(pParser->next())
					pParser->parse();
			}
		}catch(std::exception& ex)
		{
		  std::cout << "\n\n    " << ex.what() << "\n\n";
		}
	}
}

////----<add a function record into the function list to annotate>--------------------------
void Annotator::AddFunction(const FunInfo &functioninfo)
{
	funinfos[functioninfo.filename].push_back(functioninfo);
}

////----<add a set of function records into the function list to annotate>------------------
void Annotator::AddFunctions(const std::vector<FunInfo> &functioninfos)
{
	for(size_t i = 0; i < functioninfos.size(); ++i)
		AddFunction(functioninfos[i]);
}

////----<remove a function record from the function list>-----------------------------------
bool Annotator::RemoveFunction(const FunInfo &functioninfo)
{
	FunMap::iterator mapiter = funinfos.find(functioninfo.filename);
	if(mapiter == funinfos.end())
		return false;
	std::vector<FunInfo>::iterator veciter = std::find(mapiter->second.begin(),mapiter->second.end(),functioninfo);
	if(veciter == mapiter->second.end())
		return false;
	mapiter->second.erase(veciter);
	return true;
}

////----<remove a set of function records from the function list>---------------------------
bool Annotator::RemoveFunctions(const std::vector<FunInfo> &functioninfos)
{
	bool issucc = false;
	for(size_t i = 0; i < functioninfos.size(); ++i)
		issucc = issucc || RemoveFunction(functioninfos[i]);
	return issucc;
}

////----access the function list>-----------------------------------------------------------
Annotator::FunMap& Annotator::GetFunctions()
{
	return funinfos;
}


////----<test stub>-------------------------------------------------------------------------
#ifdef TEST_ANNOTATOR

int main(int argc, char* argv[])
{
  std::cout << "\n  Testing Parser class\n "
            << std::string(22,'=') << std::endl;
  if(argc < 2)
  {
    std::cout 
      << "\n  please enter name of file to process on command line\n\n";
    return 1;
  }
    ConfigParseForClassInspector configure;
  for(int i=1; i<argc; ++i)
  {
    std::cout << "\n  Processing file " << argv[i];
    std::cout << "\n  " << std::string(16 + strlen(argv[i]),'-');
	Parser* pParser = configure.Build();
    try
    {
      if(pParser)
      {
        if(!configure.Attach(argv[i]))
        {
          std::cout << "\n  could not open file " << argv[i] << std::endl;
          continue;
        }
      }
      else
      {
        std::cout << "\n\n  Parser not built\n\n";
        return 1;
      }
      while(pParser->next())
        pParser->parse();
      std::cout << "\n\n";
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
  }
  Annotator ann(configure.getFunctions(),"../TestCode/NewGen/","../mylog.log");
  std::vector<FunInfo> funvec;
  funvec.push_back(ann.GetFunctions()["..\\TestCode\\test.cpp"][0]);
  ann.RemoveFunctions(funvec);
  ann.AddFunctions(funvec);
  ann.Annotate();
  ::system("pause");
}

#endif