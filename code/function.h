
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
//#include <codecvt> //for Win
#include <locale> //for Linux

#include "term.h"
#include "sentence.h"


//#include <boost/iostreams/filtering_stream.hpp>
//#include <boost/iostreams/filter/gzip.hpp>
//#include <boost/iostreams/filtering_streambuf.hpp>
//#include <boost/iostreams/copy.hpp>
//#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/bimap.hpp>
//
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/progress.hpp>
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/support/lambda.hpp>
#include <boost/bimap/multiset_of.hpp>

using namespace std;
//namespace io = boost::iostreams;
namespace fs = boost::filesystem;
using namespace boost::bimaps;



class func
{
public:

	static void InsertArray(vector<Term>& arrTerm, Term term);
	static std::map<string,string> ReadSemNet(string semNetFile);
	static std::map<string,int> ReadSemDic(string semNetFile);
	static void FilterEntity(string txtName, string entName, string enjuName, string filterName, std::map<string,int>& entDic, int& total, int& filter);
	static string wsdEnt(string strType, std::map<string,string> semNet);
	static void FilterDic(string entName, string filterName, const std::map<string,string>& entDic,
	int& total, int& filter, std::map<string,int>& semDic, const std::map<string,string>& semNet );
	static void wsd(string entName, string filterName, std::map<string,int>semDic,int typeEnt);
	static void extractOccurrence(const string&, const string&, const string&, const string&, statistics& );
	static void ProcessFiles2(const string& txtName, const string& entName, const string& enjuName, const string& strFileOut, 
	std::map<string,string>& semNet, std::map<string,string>& metaRel, statistics& stat);
	static void ProcessFiles3(const string& txtName, const string& entName, const string& enjuName, const string& strFileOut, 
	std::map<string,string>& semNet, std::map<string,string>& metaRel, statistics& stat);
	static void ExtractNP(const string& txtName, const string& enjuName, const string& strFileOut,statistics& stat);
	static void ExtractRelNP(const string& entFile, const string& relFileName, map<string,string> semNet);

	static void readMetaRel2(string strIn, string strFileOut);
	static std::map<string,string> readMetaRel(string strIn);
	static void FilterRel(string strFileIn, string strFileOut, std::map<string,string> dicRel);
	static void readDic(string strFileName, std::map<std::string, long int >& entDic, long int& total, long int& remained);	
	static void readAllDic(string strDir, string fileDic);
	static void countFile (string strFileName, std::map<string,long int>& dicRel);
	static void countRelation(string semNetFile, string strDir);
	static bool isInSemList(string);
	static string disambiguateSem(string strSem);
	static void RandomizeCorpus(string strDir, string strFileOut,int numSent);
	static void sortEntityFiles(string fileIn, string fileOut);
	static void ReadReVerb(string strText, string strFileName, string strFileNP, string strFileMM, float threshold);
	static void ReadOLLIE(string strText, string strFileName, string strFileNP, string strFileMM, float threshold);	
	static void readSemRep(string fileIn, string fileOut);

	static void createLinkLDAInput(string fileIn, map<string,string>& relHash, int&);
	static void readAllRel(string strDir, string fileLDA,int threshold);

	
	static void CheckCoverage(string filename, statistics& stat);

	static void ChooseRelName(string fileRel, string fileOut, std::map<string,string>& relOnto);

	static bool checkOrder(std::pair<string,int> first, std::pair<string,int> second)
	{
		if(first.second >= second.second)
			return true;
		return false;
	}
	static void mergeFileforEvaluate(string strGold, string strAuto, string strFinal);
	template< class MapType >
	static void print_map(const MapType & map,
		const std::string & separator,
		std::ostream & os )
	{
		typedef typename MapType::const_iterator const_iterator;

    for( const_iterator i = map.begin(), iend = map.end(); i != iend; ++i )
    {
        os << i->first << separator << i->second << std::endl;
    }
	}
	static void ReadMM(string strNPFile, string strMMFile, string strRelFile);
};
