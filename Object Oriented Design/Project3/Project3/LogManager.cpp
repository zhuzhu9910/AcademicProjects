/////////////////////////////////////////////////////////////////////
//  LogManager.cpp - declares an LogManager class for managing the //
//    logging proccess and LogManagerRegiser for creating and      //
//    keeping gobal shared static log managers.                    //
//  ver 1.0                                                        //
//                                                                 //
//  Language:     Visual C++ 2008                                  //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project3 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////
#include "LogManager.h"
using namespace LoggingProccessing;

//Definition of the log managers holder vector
Managers LogManagerRegister::logms;

////----<deconstructor for deallocating all created LogManager>-----------------------------
Managers::~Managers()
{
	for(size_t i = 0; i < logmanagers.size(); ++i)
	{
		delete logmanagers[i];
		logmanagers[i] = 0;
	}
	logmanagers.clear();
}

////----<create a log manager instance and attach the output log file>----------------------
bool LogManagerRegister::CreateManager(const std::string &filename)
{
	//logms.resize(logms.size() + 1);
	LogManager* plm = new LogManager;

	if(!plm->AttachFile(filename))
	{
		delete plm;
		return false;
	}
	plm->OutputFileHead();
	logms.logmanagers.push_back(plm);
	return true;
}

////----<send the log item to every log manager instance to log it>-------------------------
void LogManagerRegister::Log(LoggingProccessing::ILogger *log)
{
	for(size_t i = 0; i < logms.logmanagers.size(); ++i)
		logms.logmanagers[i]->Log(log);
}

////----<construct a log manager>-----------------------------------------------------------
LogManager::LogManager() :pOut(0) {}

////----<destruct a log manager and check if there is any unreleased pointer>---------------
LogManager::~LogManager()
{
	//check if there is any memory leak, display the unreleased memory
	if(!allocters.empty())
	{
		(*pOut)<<"\nMemery not released"<<"\n========================\nAddress \t Type";
		for(std::map<unsigned int,ILogger*>::iterator iter = allocters.begin(); iter != allocters.end(); ++iter)
		{
			AllocateLog* pall = dynamic_cast<AllocateLog*>(iter->second);
			(*pOut)<<"\n0x"<<std::hex<<pall->Address()<<"\t "<<pall->AlocType();
			delete pall;
		}
	}
	//claose the log file
	if(pOut)
	{
		pOut->clear();
		std::ofstream* pFs = dynamic_cast<std::ofstream*>(pOut);
		if(pFs)
		{
		  pFs->close();
		}
	}
	allocters.clear();
}

////----<attach a output log file to the log manager>---------------------------------------
bool LogManager::AttachFile(const std::string &filename)
{
  if(pOut)
  {
    pOut->clear();
    std::ofstream* pFs = dynamic_cast<std::ofstream*>(pOut);
    if(pFs)
    {
      pFs->close();
    }
  }
    pOut = new std::ofstream(filename.c_str());
  return pOut->good();
}

////----<output a record to the log file according to the log item>-------------------------
void LogManager::Log(LoggingProccessing::ILogger *log)
{
	if(pOut && log->type() != "deallocate")
		(*pOut)<<"\n"<<log->getLogRecord().c_str();

	if(log->type() == "allocater")
	{
		AllocateLog* pa = new AllocateLog(*(dynamic_cast<AllocateLog*>(log)));
		allocters[pa->Address()] = pa;
	}

	if(log->type() == "deallocate" && !allocters.empty())
	{
		std::map<unsigned int,ILogger*>::iterator pit = allocters.find(dynamic_cast<DeallocateLog*>(log)->Address());
		if(pit != allocters.end())
		{
			allocters.erase(pit);
			(*pOut)<<"\n"<<log->getLogRecord().c_str();
		}
	}
}

////----<write the head line to the log file>-----------------------------------------------
void LogManager::OutputFileHead()
{
	if(pOut)
	{
		(*pOut)<<"                                  CSE 687 Project#3 \n";
		(*pOut)<<"                                     Qinyun Zhu \n\n";
		(*pOut)<<"Object Inspector Tracing Record\n";
		(*pOut)<<"===========================================\n";
		(*pOut)<<"  Time "<<"\t\t\t"<<"Event";
	}
}



////----<test stub>-------------------------------------------------------------------------
#ifdef TEST_LOGMANAGER

void main()
{
	LogManagerRegister::CreateManager("../log.txt");
	FunctionLog alog("void main involked","13:52:55");
	AllocateLog allog(111,4,"int","pint");
	AllocateLog aallog(1111,16,"int","parint",true);
	DeallocateLog da(111);
	da.setTime("13:12:11");
	allog.setTime("13:22:11");
	aallog.setTime("13:11:11");
	LogManagerRegister::Log(&alog);
	LogManagerRegister::Log(&allog);
	LogManagerRegister::Log(&aallog);
	LogManagerRegister::Log(&da);

	::system("pause");
}

#endif