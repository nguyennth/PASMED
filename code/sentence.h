#include "term.h"
#include "statistics.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>


namespace io = boost::iostreams;

#define LIMIT_LENGTH (100)
class Sentence
{

public:	
	vector<Term> arrTerm;
	vector<Term> arrTok;	
	vector<Term> arrEnt;
	vector<Relation> arrRel, arrRelA;
	//string arrPred[MAX_TOKEN][MAX_TOKEN];
	//list<int> *vertex;
	//int vertex[MAX_TOKEN][MAX_TOKEN];
	string id;
	int start, end,count, numOfNPpairs;
	std::map<string,string> semNet;

public:
	vector<Term>::size_type lookUpTerm(const vector<Term>&, const string&);
	Term lookUpHead( const string&);

	string parse(string);

	bool isInSemList(string);
	bool CheckRule(const wstring&,statistics&);
	bool CheckRule1(vector<Term>::iterator,const Term&,Term&,const wstring&, statistics&);
	bool CheckRule2(vector<Term>::iterator,const Term&,const wstring&,statistics&);
	bool CheckRule3(vector<Term>::iterator,const Term&,const wstring&, statistics&);
	bool CheckRule1_4(vector<Term>::iterator, vector<Term>::size_type,
		vector<Term>::size_type, const wstring& , statistics&);
	bool CheckRule1_3(vector<Term>::iterator it, Term termNP1, Term termPP, const wstring& strText, statistics& stat);
	bool CheckRule4(vector<Term>::iterator it, const wstring&,statistics&);
	bool CheckTransitive(vector<Term>::iterator, const wstring&,statistics&);
	bool CheckIntransitive(vector<Term>::iterator, const wstring&,statistics&);
	void CheckEntity(const Term&, const Term&, const Term&, const wstring&);
	void CheckEntity(const Term&, const Term&, const Term&, const Term&, const wstring&);
	void CheckEntity2(const Term&, const Term&, const Term&);
	void CheckEntity2(const Term&, const Term&, const Term&, const Term&);
	bool InsertRelation(Relation);
	void Evaluate(statistics& );	
	bool CheckSemRel(Relation& rel, std::map<string,string>&);
	bool CheckMetaRel(Relation& rel, std::map<string,string>&);
	bool CheckSemantic(Relation& rel);
	bool CheckSemantic_NCSTOX(Relation& rel);
	bool CheckSemantic_oldschema(Relation& rel);
	void PrintRel(const string& strText, std::ofstream& out);
	void PrintOccurence(const wstring& strText, std::ofstream& out, int&);
	void AssignNETag();
	void PrintEnt(const string& strText, std::ofstream& out);
	

	//these following functions are used to extract NP-NP pairs
	bool eCheckRule(const string&, statistics& stat, io::filtering_ostream& os);
	bool eCheckRule1(vector<Term>::iterator, const Term&, const Term&, const string&, statistics&, io::filtering_ostream& os);
	bool eCheckRule2(vector<Term>::iterator, const Term&, const string&, statistics&,io::filtering_ostream& os);
	bool eCheckRule3(vector<Term>::iterator, const Term&, const string&, statistics&, io::filtering_ostream& os);
	bool eCheckRule1_4(vector<Term>::iterator, vector<Term>::size_type, vector<Term>::size_type, const string&, statistics &, io::filtering_ostream& os);
	bool eCheckRule1_3(vector<Term>::iterator, Term termNP1, Term termPP, const string&, statistics&, io::filtering_ostream& os);
	bool eCheckRule4(vector<Term>::iterator, const string&, statistics&, io::filtering_ostream& os);
	bool eCheckTransitive(vector<Term>::iterator, const string&, statistics&, io::filtering_ostream& os);
	bool eCheckIntransitive(vector<Term>::iterator, const string&, statistics&, io::filtering_ostream& os);
	

	
	//Sentence(std::map<string,string> sem);
	Sentence(void);
	~Sentence(void);
	//static int countPred;
};
