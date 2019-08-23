//============================================================================
// Name        : ExtractOccurrence.cpp
// Author      : Nhung Nguyen
// Version     : v1.0
// Copyright   : Your copyright notice
// Description : Extract occurrences from biodiversity documents
//============================================================================

#include "function.h"
#include <dirent.h>

using namespace std;

int main( int argc, char* argv[] )
{
	bool freq = false, pos= false, pattern = false, sem=false;
	std::map<string,string> dic;
	statistics stat;
	std::map<string,string> semNet;
	std::map<string,int> semDic;
	std::map<string,string> metaRel;
	std::map<string, int> entDic;
	int total = 0, filter = 0;


	if (argc == 5) //extract relations --> this function is used in case we extract relations after NER
	{
		//extract occurrences

		pattern = true;
		func::extractOccurrence(argv[1],argv[2],argv[3],argv[4],stat);
		return 0;
	}
	else
	{
		cerr<<"Usage: extractRel text_file entity_file enju_file relation_file\n";
		cerr<<"text_file: path to a text file\n";
		cerr<<"entity_file: path to a file contains named entities corresponding to the text file in brat format\n";
		cerr<<"enju_file: path to a .gz file contains PAS results corresponding to the text file by Enju parser\n";
		cerr<<"relation_file: path to save the extracted relations\n";
		return -1;
	}

}


void pickUp(string strAll, string strFiltered, string strMovedOut)
{
	map<string,int> entDic = func::ReadSemDic(strFiltered);
	std::ifstream fFilter(strAll.c_str(), std::ios_base::in);
	std::ofstream fOut (strMovedOut.c_str(),std::ios_base::out);
	string strLine;

	while(getline(fFilter,strLine))
	{
		size_t pos = strLine.find("\t");
		string temp = strLine.substr(0,pos);
		//if(temp.length() > 8 && temp.find("&apos;s")==temp.length()-7)
		//	continue;*/
		if(strLine.find("other ")==0)
			continue;
		if(entDic.find(strLine.substr(0,pos))==entDic.end())
		{
			fOut << strLine << endl;
		}
	}
	fFilter.close();
	fOut.close();

}

void sortDic(string strFileIn, string strFileOut)
{
	std::map<int,string> freq;
	std::ifstream file(strFileIn.c_str(), std::ios_base::in);
	string strLine;
	std::ofstream fileOut(strFileOut.c_str(), std::ios_base::out);
	if(!fileOut)
	{
		cerr << "Error when opening file " << strFileOut << endl;
		return;
	}
	while(getline(file,strLine))
	{
		if(strLine.find("Total")!=string::npos)
		{
			continue;
		}
		size_t pos = strLine.find("\t");
		if(pos==string::npos)
			continue;
		string strKey = strLine.substr(pos+1) + ", ";
		int value = atoi(strLine.substr(0,pos).c_str());

		vector<string> arrKey; // = NULL; //common::split(strKey,", ");
		for(int i=0 ; i< arrKey.size(); i++)
		{
			fileOut << common::normalizeStr(arrKey[i]) << "\t" << value << endl;
		}

	}

	fileOut.close();

}

