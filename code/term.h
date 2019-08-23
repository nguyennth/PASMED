#pragma once
#define BOOST_ALL_DYN_LINK
#define BOOST_IOSTREAMS_NO_LIB

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>

#include <queue>
#include <list>
#include <sstream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/filesystem.hpp>

using namespace std;

namespace io = boost::iostreams;
//#include "sentence.h"

//using namespace std;
#define MAX_TOKEN 100


class common
{
public:
	static vector<wstring> splitUTF8(wstring, wstring);
	static vector<string> split(string, string);
	static string tolowerStr(string);
	static string normalizeStr(string);
	static string normalizeStr2(string);
};

class Term
{ //term can be a cons or a tok in a sentence
public:
	int start, end; //start and end position in text file
	string type; // cons; tok, gene_prod, disease ...
	string content, base; //enju output
	string id;
	string netag; 
	bool check; 
	string POS,conceptID, preferred;

public:
	
	bool operator < (const Term& str) const
	{
		return (start < str.start);
	}
	void parse(string strIn);
	bool parseGene(string strIn, int sStart, int sEnd,bool);
	bool parseEnt(string strIn, int sStart, int sEnd);
	bool parseBiodivEnt (string strIn, int sStart, int sEnd);
	bool parseEnt2(string strIn, const string strText, const vector<Term>& arrTok, int& pos, std::map<string,int>& ,io::filtering_ostream&, int&, int& );
	Term(void);
	~Term(void);
};

class Relation
{
public:
	int start, end;
	int arg1, arg2;
	string name;
	string semType; 
	bool check;
public:
	Relation(void);
	~Relation(void);
	bool parse(string strLine, int, int);
	bool operator = (const Relation& rel) const
	{
		return (start == rel.start && end == rel.end);
	}
};



