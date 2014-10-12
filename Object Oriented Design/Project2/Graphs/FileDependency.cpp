/////////////////////////////////////////////////////////////////////
//  FileDependency.cpp - analyzing file and package dependencies   //
//             and put the analyzed information in the classes     //
//  ver 1.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Lenovo T400, Windows 7                          //
//  Application:   Project2 of CSE687                              //
//  Author:        Qinyun Zhu, Syracuse University,                //
//                 (315) 251-4249, qzhu02@syr.edu                  //
/////////////////////////////////////////////////////////////////////


#include "FileDependency.h"

////----<constructor of the class with input of the attached file>-------------------------------------------------------------------
FileDependency::FileDependency(const std::string &filename) :pIn(0)
{
	pIn = new std::ifstream(filename.c_str());
	path = filename;
	_name = getNameFromPath(path);
	if(!pIn->good())
	{
		std::cout<<"\ncan not open specified file!!!\n";
	}
}

////----<deconstruct the class and close the file>-----------------------------------------------------------------------------------
FileDependency::~FileDependency()
{
	if(!pIn)
	{
		pIn->close();
		delete pIn;
	}
}

////----<find dependencies of this file>---------------------------------------------------------------------------------------------
bool FileDependency::AnalyzeDependencies()
{
	std::string curr;
	if(!pIn->is_open())
	{
		pIn->clear();
		pIn->open(path.c_str());
	}
	if(!pIn->good())
	{
		std::cout<<"\n can not open file!!!\n";
		return false;
	}
	dependencies.clear();

	//find dependencies in "include" headers
	while (pIn->peek() != -1)
	{
		*pIn>>curr;
		if(curr == "#include")
		{
			*pIn>>curr;
			if((pIn->peek() == '\n' || pIn->peek() == ' ') && curr.at(0) == '\"' && curr.at(curr.size() -1) == '\"')
			{
				curr.erase(curr.begin());
				curr.erase(curr.end() - 1);
				dependencies.push_back(curr);
			}
		}
	}
	pIn->close();
	return true;
}

////----<index operator for accessing dependencis of the file>--------------------------------------------------------------------
std::string& FileDependency::operator [](size_t pos)
{
	return dependencies.at(pos);
}

////----<the amount of dependent files this file has>------------------------------------------------------------------------------
size_t FileDependency::size()
{
	return dependencies.size();
}

////----<find file name in the path of the file>------------------------------------------------------------------------------------
std::string FileDependency::getNameFromPath(const std::string& filepath)
{
	std::string fname = filepath;
	const size_t pathsize = filepath.size();
	for(size_t i = pathsize - 1; i >= 0 && i != -1 ; --i)
	{
		if(filepath[i] == '\\' || filepath[i] == '/')
		{
			fname.clear();
			for(size_t j = i + 1; j < pathsize; ++j)
				fname.push_back(filepath[j]);
			break;
		}
	}
	return fname;
}

////----<get the file name>---------------------------------------------------------------------------------------------------------
std::string FileDependency::name()
{
	return _name;
}

////----<get the file name for recognizing dependency relationship>-----------------------------------------------------------------
std::string FileDependency::DependencyName()
{
	return _name;
}

////----<find the position of a specified file name in the dependencies vector>-----------------------------------------------------
size_t FileDependency::findDependency(const std::string& depfile)
{
	for(size_t i = 0; i < dependencies.size(); ++i)
		if(getNameFromPath(dependencies[i]) == getNameFromPath(depfile))
			return i;
	return -1;
}

////----<find if this file is depended on a specified file>--------------------------------------------------------------------------
bool FileDependency::hasDependencyWith(const std::string& fname)
{
	if(findDependency(fname) == -1)
		return false;
	return true;
}

////----<return relative path from current file to dependency file>-----------------------------------------------------------------
////no match found return "", local relative path return "."
std::string FileDependency::GetEdgeValue(const std::string& fname)
{
	size_t deppos = findDependency(fname);
	if(deppos == -1)
		return "";
	std::string rp;
	const size_t pathsize = dependencies[deppos].size();
	for(size_t i = pathsize - 1; i >= 0 && i != -1 ; --i)
	{
		if(dependencies[deppos].at(i) == '\\' || dependencies[deppos].at(i) == '/')
		{
			for(size_t j = 0; j <= i; ++j)
				rp.push_back(dependencies[deppos].at(j));
			break;
		}
	}
	if(rp.empty())
		return ".";
	else
		return rp;
}

////----<eniminate the extension of the file name>-----------------------------------------------------------------------------------
std::string FileDependency::getNameWithoutExp(const std::string& fname)
{
	std::string myname = fname;
	size_t eppos = myname.find(".");
	const size_t mynamesize = myname.size();
	for(size_t i =eppos; i < mynamesize; ++i)
	{
		myname.erase(myname.begin() + eppos);
	}
	return myname;
}

////----<constructor of PackageDpendency with inputing package files>----------------------------------------------------------------
PackageDependency::PackageDependency(FileDependency* headfile, FileDependency* sourcefile) 
:header(headfile),source(sourcefile){}

////----<get header file name to recognize dependency relationships>------------------------------------------------------------------
std::string PackageDependency::DependencyName()
{
	if (!header)
		return "";
	return header->name();
}

////----<get package name>------------------------------------------------------------------------------------------------------------
std::string PackageDependency::name()
{
	if (!header)
		return "";
	//eniminate the extension of the head file
	std::string myname = header->name();
	size_t eppos = myname.find(".");
	const size_t mynamesize = myname.size();
	for(size_t i =eppos; i < mynamesize; ++i)
	{
		myname.erase(myname.begin() + eppos);
	}
	return myname;
}

////----<get a pointer to head file dependency class>-----------------------------------------------------------------------------------
FileDependency* PackageDependency::getheadfile()
{
	return header;
}

////----<get a pointer to source file dependency class>-----------------------------------------------------------------------------------
FileDependency* PackageDependency::getsourcefile()
{
	return source;
}

////----<check if the package is depended on a specified file>----------------------------------------------------------------------------
bool PackageDependency::hasDependencyWith(const std::string &fname)
{
	if(header->findDependency(fname) != -1)
		return true;
	if(source != 0 && source->findDependency(fname) != -1)
		return true;
	return false;
}

////----<reanalyze the dependencies of the files in the package>--------------------------------------------------------------------------
bool PackageDependency::reAnalyzeDependencies()
{
	bool result = header->AnalyzeDependencies();
	if(source != 0)
		result = result && source->AnalyzeDependencies();
	return result;
}

////----<return relative path from current package to dependency package>-----------------------------------------------------------------
std::string PackageDependency::GetEdgeValue(const std::string& fname)
{
	std::string rfh;
	std::string rfs;
	if(header)
		rfh = header->GetEdgeValue(fname);
	if(source)
		rfs = source->GetEdgeValue(fname);
	if(rfh.size() > rfs.size())
		return rfh;
	else
		return rfs;
}

////----<test stub>------------------------------------------------------------------------------------------
#ifdef TEST_FILEDEPENDENCY
void main()
{
	FileDependency fd("..\\Graphs\\GraphFromXML.h");
	FileDependency fds("..\\Graphs\\GraphFromXML.cpp");
	std::cout<<fd.name()<<std::endl;
	fd.AnalyzeDependencies();
	fd.AnalyzeDependencies();
	fds.AnalyzeDependencies();
	size_t pos = fd.findDependency("XML.H");
	if(pos == -1)
		std::cout<<"None";
	PackageDependency pd(&fd,&fds);
	PackageDependency pd2(&fd);
	std::string packagename = pd.DependencyName();
	bool isdp = pd.hasDependencyWith("CondensedGraph.h");

	packagename = pd2.DependencyName();
	isdp = pd2.hasDependencyWith("CondensedGraph.h");
	isdp = pd.reAnalyzeDependencies();
	isdp = pd2.reAnalyzeDependencies();
	bool isdpf = fd.hasDependencyWith("CondensedGraph.h");
	IDependency<std::string>* id[2] = {&fd,&pd};
	isdpf = id[0]->hasDependencyWith("XML.H");
	std::cout<<isdpf<<std::endl;
	isdpf = id[1]->hasDependencyWith("CondensedGraph.h");
	std::cout<<isdpf<<std::endl;
	std::cout<<id[0]->name()<<" "<<id[1]->name();

	
}

#endif