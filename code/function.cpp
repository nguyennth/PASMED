
#include "function.h"



void func::InsertArray(vector<Term>& arrTerm, Term term)
{
	int start = term.start, end = term.end;
	for(unsigned int i=0; i< arrTerm.size(); i++)
	{
		Term t = arrTerm[i];
		if(t.end == start && t.end == end)
		{
			arrTerm[i].type = arrTerm[i].type + "|" + term.type;
			return;
		}
	}
	arrTerm.push_back(term);
}



std::map<string,string> func::ReadSemNet(string semNetFile)
{
	std::map<string,string> semNet;
	std::ifstream file(semNetFile.c_str(),std::ios_base::in);
	if(!file)
	{
		cerr << "Error of opening file " << semNetFile << endl;
		return semNet;
	}
	string strLine="";
	while(getline(file,strLine))
	{
		size_t pos = strLine.find("\t");
		
		if(pos==string::npos)
			continue;
		string strKey = strLine.substr(0,pos);
		/*if(strKey.length() > 3 && (strKey.find("of ")==0 || strKey.find("in ")==0||strKey.find("as ")==0))
		{
			strKey = strKey.substr(3);
		}
		else if(strKey.length() > 3 && (strKey.find(" of")==strKey.length()-3 ||strKey.find(" in")==strKey.length()-3)|| strKey.find(" as")==strKey.length()-3)
		{
			strKey = strKey.substr(0, strKey.length() - 3);
		}*/
		if(semNet.find(strKey)==semNet.end())
			semNet.insert(std::pair<string,string>(strKey, strLine.substr(pos+1)));
		else
			semNet[strKey] += ";" +  strLine.substr(pos+1);
	}

	file.close();

	return semNet;
}

std::map<string,int> func::ReadSemDic(string semNetFile)
{
	std::map<string,int> semNet;
	std::ifstream file(semNetFile.c_str(),std::ios_base::in);
	if(!file)
	{
		cerr << "Errod of opening file " << semNetFile << endl; 
		return semNet;
	}
	string strLine="";
	while(getline(file,strLine))
	{
		size_t pos = strLine.find("\t");
		if(pos==string::npos)
			continue;
		int freq =  atoi(strLine.substr(pos+1).c_str());
		semNet.insert(std::pair<string,int>(strLine.substr(0,pos), freq));
	}

	file.close();

	return semNet;
}

void func::FilterEntity(string txtName, string entName, string enjuName, string filterName, std::map<string,int>& entDic, int& total, int& filter)
{
	std::ofstream fEnt("ent.txt", std::ios_base::out);
	std::fstream is(txtName.c_str(), std::fstream::in);
	if(!is)
	{
		cerr << "Error when opening " << txtName << endl;
		return;
	}
		
	std::ifstream entFile(entName.c_str(), std::ios_base::in | std::ios_base::binary);
	if(!entFile)
	{
		cerr << "Error when opening " << entName<< endl;
		return;
	}
	boost::iostreams::filtering_istream entF;
	entF.push(boost::iostreams::gzip_decompressor());
	entF.push(entFile);

	std::ifstream enjuFile(enjuName.c_str(), std::ios_base::in | std::ios_base::binary);

	if(!enjuFile) {
		cerr << "Error when opening file" << enjuName << endl;	
		return;
	}
	boost::iostreams::filtering_istream enjuF;
	enjuF.push(boost::iostreams::gzip_decompressor());
	enjuF.push(enjuFile);


	is.seekg (0, is.end);
  size_t length = is.tellg();
  is.seekg (0, is.beg);

  char * buffer = new char [length];
	is.read(buffer,length);

	string strText(buffer);
	length = strText.length();
	
	io::filtering_ostream flog;
	flog.push(io::gzip_compressor());
	flog.push(io::file_descriptor_sink(filterName));
	
	//out.write (memblock, size);
	//ofstream flog(filterName);

	string strPat = "";

	string strLine, strEnt, cate;
	//int countPred = 0, countUnk=0;
	//long int count= 0;
	int posTok=0;

	getline(enjuF,strLine);	
	
	while(strLine.find("parse_status=\"success\"")==string::npos)
		getline(enjuF,strLine);	

	getline(entF, strEnt);	
	total++;
	
	//ofstream out;
	do
	{	
		if(strLine.length() < 2) { 	break; }
		//cerr << "Loop1\t" << count << endl ;
		Sentence sent;
		sent.arrRelA.clear();
		sent.arrEnt.clear();
		posTok = 0;
		while(strLine.find("parse_status=\"success\"")==string::npos 
			&& strLine.find("parse_status=\"fragmental parse\"")==string::npos)
		{
			getline(enjuF,strLine);
			if(strLine == "")
				return;
		}
		cate = sent.parse(strLine);
		//cout << strText.substr(sent.start, sent.end - sent.start) << endl;
		do{ //loop on enju file
			getline(enjuF, strLine);
			//cerr << "Loop2" << endl;
			if(strLine.length() < 1) break;

			/*if(strLine.find("id=\"s0.762\"")!=string::npos)
				strLine = strLine;*/
		
			Term term;
			term.parse(strLine);
			term.check = false;
			if(term.start == term.end && term.start==0) break;
			if(term.type == "tok")
			{
				sent.arrTok.push_back(term);
			}
			else if(term.type=="cons")
			{
				sent.arrTerm.push_back(term);
			}
			
			cate = term.type;
	
		}while(cate != "sentence");
		
//		int sGene, eGene;	
		Term preEnt;
		do{ // loop on entity file
			//cerr << "Loop4" << endl;
			if(strEnt.find("sent") == 0 || strEnt.find("NP1") == 0 || strEnt.find("NP2")==0 || strEnt.find("rel")==0)
			{
					flog << strEnt << endl;
					continue;
			}

			Term ent;
			int sGene, eGene;		
			do{				// this loop is used to skip entities that don't belong to any sentences in Enju output
				istringstream geneIs(strEnt.c_str());
				geneIs >> sGene >> eGene;
			//	cerr << "Loop3" << endl;
			} while (sGene < sent.start && getline(entF,strEnt)&& total++);
			
			//bool res = ent.parseEnt(strEnt,sent.start, sent.end, true);			
			bool res = ent.parseEnt2(strEnt,strText,sent.arrTok,posTok,entDic, flog,total, filter);			
			if(res)
			{				
				//sent.arrEnt.push_back(ent);				
				preEnt = ent;
			}
			else { 
				break; 
			}
			
		}while(getline(entF, strEnt) && total++);
		
		std::sort(sent.arrEnt.begin(),sent.arrEnt.end());		

	} while(cate == "sentence");
		
	entFile.close();	
	enjuFile.close();
	flog.flush();
	flog.reset();
	
}
string func::wsdEnt(string strType, std::map<string,string> semNet)
{
	//replace child semantic by its parent semantic if there exists
	//this is not a perfect algorithm since it doesn't cover every case.
	if(strType.find(",")==string::npos)
	{
		return strType;
	}
	strType += ",";
	vector<string> arrSem; // = common::split(strType,",");
	for(unsigned int i=0; i< arrSem.size()-1; i++)
	{
		string type1 = arrSem[i];
		for(unsigned int j=i+1; j< arrSem.size(); j++)
		{
			string type2 = arrSem[j];
			string combine = type1 + "-" + type2;
			std::map<string,string>::iterator it = semNet.find(combine);
			if(it!=semNet.end())
			{
				if(it ->second == "isa")
				{
					size_t pos = strType.find(type1);
					if(pos==string::npos)
						continue;
					strType = strType.replace(pos,type1.length() + 1, "");
				}
			}
			else 
			{
				combine = type2 + "-" + type1;
				it = semNet.find(combine);
				if(it!=semNet.end())
				{
					if(it ->second == "isa")
					{
						size_t pos = strType.find(type2);
						if(pos==string::npos)
							continue;
						strType = strType.replace(pos,type2.length() + 1, "");
					}
				}
			}
		}
	}

	return strType;
}

void func::FilterDic(string entName, string filterName, const std::map<string,string>& entDic,
	int& total, int& filter, std::map<string,int>& semDic, const std::map<string,string>& semNet )
{
	std::ifstream entFile(entName.c_str(), std::ios_base::in | std::ios_base::binary);
	if(!entFile)
	{
		cerr << "Error when opening " << entName<< endl;
		return;
	}
	boost::iostreams::filtering_istream entF;
	entF.push(boost::iostreams::gzip_decompressor());
	entF.push(entFile);

	io::filtering_ostream flog;
	flog.push(io::gzip_compressor());
	flog.push(io::file_descriptor_sink(filterName));

	string strLine;
	int index=0;
	int change = 0;
	while(getline(entF,strLine))
	{
		change = 0;
		if(strLine.find("sent") == 0 || strLine.find("NP1") == 0 || strLine.find("NP2")==0 || strLine.find("rel")==0 || strLine.find("unCon")!=string::npos)
		{
			flog << strLine << endl;
			continue;
		}
		

		vector<string> arrTerm = common::split(strLine+"\t", "\t");
		
		//cout << arrTerm[0] << "\t" << arrTerm[2] << "\t" << arrTerm[3] << "\t" << arrTerm[4] << endl;
		/*if(arrTerm[0]=="2143")
			change = change;*/
		string strNorm = arrTerm[arrTerm.size()-2];
		if(strNorm.length() > 3 && (strNorm.find("of ")==0 || strNorm.find("in ")==0|| strNorm.find("as ")==0|| strNorm.find("to ")==0 || strNorm.find("at ")==0))
		{
			strNorm = strNorm.substr(3);
			arrTerm[2] = arrTerm[2].substr(3);
			change = 1;
		}
		else if(strNorm.length() > 3 && (strNorm.find(" of")==strNorm.length()-3 || strNorm.find(" in")==strNorm.length()-3|| strNorm.find(" as")==strNorm.length()-3
			|| strNorm.find(" at")==strNorm.length()-3|| strNorm.find(" to")==strNorm.length()-3))
		{
			strNorm = strNorm.substr(0,strNorm.length()-3);
			arrTerm[2] = arrTerm[2].substr(0,arrTerm[2].length() - 3);
			change = 2;
		}
		else if(strNorm.find("for ")==0)
		{
			strNorm = strNorm.substr(4);
			arrTerm[2] = arrTerm[2].substr(4);
			change = 3;
		}
		else if(strNorm.find("about ")==0)
		{
			strNorm = strNorm.substr(6);
			arrTerm[2] = arrTerm[2].substr(6);
			change = 4;
		}
		if(entDic.find(strNorm)!=entDic.end())
		{
			//save ent+sem dic
			string strType = arrTerm[3];
			strType = wsdEnt(strType,semNet); //--> this is greedy check --> takes a lot of time!!!!
			//start end entity-text semantic-types conceptID normalized-text
			int start = atoi(arrTerm[0].c_str()), end = atoi(arrTerm[1].c_str());
			if(change == 1)
			{
				start -= 3;
			}
			else if(change == 2)
			{
				end -=3;
			} else if(change == 3) start -=4;
			else if(change == 4) start -=6;

			char strStart[50];
			sprintf(strStart,"%d\t%d",start, end);
			string strOut = string(strStart) + "\t" + arrTerm[2] 
				+ "\t" + strType + "\t" + arrTerm[arrTerm.size()-3] + "\t" + strNorm + "\n";
			/*char strIndex[50];
			sprintf(strIndex,"entity-%d",index);
			string strOut = arrTerm[0] + "\t" + strType + "\t"  ;
			strOut += strIndex;
			strOut += "\t" + arrTerm[1] + "\t" + arrTerm[2] + "\t" +  strType + "\t" + arrTerm[4] + "\t" + arrTerm[5] + "\n";*/
			flog.write(strOut.c_str(), strOut.length());
			//cout << strOut << endl;
			index++;

			if(strType.find(",")!=string::npos)
			{
				vector<string> arrSem = common::split(strType,",");
				for(int i=0; i< arrSem.size(); i++)
				{
					strType = strNorm + "-" + arrSem[i];
					if(semDic.find(strType)==semDic.end())
					{
						semDic.insert(std::pair<string,int>(strType,1));
					}
					else
						semDic[strType]++;
					//cout << strType << endl;
				}
			}
			else
			{
				strNorm = strNorm +"-"+strType;
				if(semDic.find(strNorm)==semDic.end())
				{
					semDic.insert(std::pair<string,int>(strNorm,1));
				}
				else
					semDic[strNorm]++;
			}
		}
		total++;
	}
	filter += index + 1;

	entFile.close();
	flog.flush();
	flog.reset();

}

void func::wsd(string entName, string filterName, std::map<string,int>semDic, int typeEnt)
{
	std::ifstream entFile(entName.c_str(), std::ios_base::in | std::ios_base::binary);
	if(!entFile)
	{
		cerr << "Error when opening " << entName<< endl;
		return;
	}
	boost::iostreams::filtering_istream entF;
	entF.push(boost::iostreams::gzip_decompressor());
	entF.push(entFile);

	io::filtering_ostream flog;
	flog.push(io::gzip_compressor());
	flog.push(io::file_descriptor_sink(filterName));

	string strLine;
	bool flag = false;
	int index = 0;
	char strIndex[20];
	while(getline(entF,strLine))
	{
		/*if(strLine.find("Endothelins")!=-1)
		{
			flag = flag;
		}*/
		flag = false;
		if(strLine.find("sent") == 0 || strLine.find("NP1") == 0 || strLine.find("NP2")==0 || strLine.find("rel")==0 )
		{
			flog << strLine << endl;
			continue;
		}
		vector<string> arrTerm = common::split(strLine + "\t", "\t");
		//cout << arrTerm[0] << "\t" << arrTerm[2] << "\t" << arrTerm[3] << "\t" << arrTerm[4] << endl;
		//start end entity-text semantic-types conceptID normalized-text
		//string strEnt = arrTerm[2];
		string strType = arrTerm[3];
		strType = disambiguateSem(strType);
		if(strType=="")
			continue;
	
		string strNorm = arrTerm[arrTerm.size()-2];
		string strOut = arrTerm[0] + "\t" + arrTerm[1] + "\t";

		if(typeEnt == 1 && (strType.find("aapp")!=string::npos || strType.find("amas")!=string::npos  || strType.find("gngm")!=string::npos || strType.find("enzy")!=string::npos || strType.find("nnon")!=string::npos
			|| strType.find("nusq")!=string::npos))// || strType.find("celf")!=string::npos  || strType.find("horm")!=string::npos)
		{
			strType =  "gene";
			flag = true;
		}
		else if (typeEnt == 2 && (strType.find("antb")!=string::npos || strType.find("horm")!=string::npos || strType.find("hops")!=string::npos || strType.find("imft")!=string::npos || 
		strType.find("nsba")!=string::npos || strType.find("orch")!=string::npos || strType.find("phsu")!=string::npos))
		{
			strType = "drug";	
			flag = true;
		}
		else if(typeEnt==3)
		{
			flag = true;		
			if(strType.find(",")!=string::npos)
			{
				vector<string> arrSem = common::split(strType,",");
				int max = 0, maxIndex = 0;
				for(int i=0; i< arrSem.size(); i++)
				{
					strType = strNorm + "-" + arrSem[i];
					std::map<string,int>::iterator it = semDic.find(strType);
					if(it!=semDic.end())
					{
						if(max < it ->second)
						{
							max = it ->second; maxIndex = i;
						}
					}
			
				}
				if(maxIndex!=-1)
				{
					strType = arrSem[maxIndex];					
				}		
			}
			
		}

		if(flag==true)
		{
			strOut += strType + "\t" ;
			sprintf(strIndex,"entity-%d",index);
			strOut += strIndex;
			//strOut += "\t" + arrTerm[arrTerm.size()-2] + "\t" + arrTerm[2];
			if(arrTerm[arrTerm.size()-1].find("unCon")==0)
				strOut += "\t" + arrTerm[4] + "\t" + arrTerm[arrTerm.size()-2];
			else
				strOut += "\t" + arrTerm[4] + "\t" + arrTerm[arrTerm.size()-1];
			strOut += "\n";			
			flog.write(strOut.c_str(), strOut.length());
			index++;
		}
	}
	
	entFile.close();
	flog.flush();
	flog.reset();
}
void func::ProcessFiles2(const string& txtName, const string& entName, const string& enjuName, const string& strFileOut, 
	std::map<string,string>& semNet, std::map<string,string>& metaRel, statistics& stat)
{
	//this function is used to process the whole MEDLINE with metamap output
	std::ofstream fEnt("ent.txt", std::ios_base::out);
	std::fstream is(txtName.c_str(), std::fstream::in);
	if(!is)
	{
		cerr << "Error when opening " << txtName << endl;
		return;
	}
		
	std::ifstream entFile(entName.c_str(), std::ios_base::in | std::ios_base::binary);
	if(!entFile)
	{
		cerr << "Error when opening " << entName<< endl;
		return;
	}
	boost::iostreams::filtering_istream entF;
	entF.push(boost::iostreams::gzip_decompressor());
	entF.push(entFile);

	
	//fstream enjuF(temp.c_str(),std::fstream::in);
	std::ifstream enjuFile(enjuName.c_str(), std::ios_base::in | std::ios_base::binary);
	//fstream enjuF("test.txt");

	if(!enjuFile) {
		cerr << "Error when opening file" << enjuName << endl;	
		return;
	}
	boost::iostreams::filtering_istream enjuF;
	enjuF.push(boost::iostreams::gzip_decompressor());
	enjuF.push(enjuFile);

	std::ofstream fileOut(strFileOut.c_str(),std::ofstream::out);

	if(!fileOut) {
		cerr << "Error when opening file" << strFileOut << endl;	
		return;
	}


	is.seekg (0, is.end);
	size_t length = is.tellg();
	is.seekg (0, is.beg);

	char * buffer = new char [length];
	is.read(buffer,length);

	string strText(buffer);
	length = strText.length();
	string strPat = "";

	string strLine, strEnt, cate;
	int countPred = 0, countUnk=0;
	long int count= 0;
	int posTok=0;

	getline(enjuF,strLine);	
	while(strLine.find("parse_status=\"success\"")==string::npos 
			&& strLine.find("parse_status=\"fragmental parse\"")==string::npos)
		{
			getline(enjuF,strLine);
			if(strLine == "")
				return;
			if(strLine.find("sentence id=")!=string::npos)
			{
				//fileOut << "Sentence" << stat.totalSent << endl;
				stat.totalSent++;
			}
		}
	
	getline(entF, strEnt);	
	
	//ofstream out;
	do
	{	
		if(strLine.length() < 2) { 	break; }
	
		Sentence sent;
		sent.arrRelA.clear();
		posTok = 0;
		
		while(strLine.find("parse_status=\"success\"")==string::npos 
			&& strLine.find("parse_status=\"fragmental parse\"")==string::npos)
		{
			getline(enjuF,strLine);
			if(strLine == "")
				return;
			if(strLine.find("sentence id=")!=string::npos)
			{
				//fileOut << "Sentence" << stat.totalSent << endl;
				stat.totalSent++;

			}
		}
	

		cate = sent.parse(strLine);

		do{ //loop on enju file
			getline(enjuF, strLine);
			
			if(strLine.length() < 1) break;
			Term term;
			term.parse(strLine);
			term.check = false;
			term.netag = "O";
			if(term.start == term.end && term.start==0) break;
			if(term.type == "tok")
			{
				sent.arrTok.push_back(term);
			}
			else
			{
				sent.arrTerm.push_back(term);
			}
			
			cate = term.type;
	
		}while(cate != "sentence");
		
//		int sGene, eGene;	
		Term preEnt;
		do{ // loop on entity file
			Term ent;
			int sGene, eGene;		
			do{				// this loop is used to skip entities that don't belong to any sentences in Enju output
				istringstream geneIs(strEnt.c_str());
				geneIs >> sGene >> eGene;
			} while (sGene < sent.start && getline(entF,strEnt));
			
			//bool res = ent.parseEnt(strEnt,sent.start, sent.end, 1);			//--> for gene
			//bool res = ent.parseGene(strEnt,sent.start,sent.end, true);			// --> for golden entity of PPI and DDI
			//bool res = ent.parseEnt(strEnt,sent.start, sent.end, 2);			//--> for drug
			bool res = ent.parseEnt(strEnt,sent.start, sent.end);			//--> for all
			
			if(res)
			{		
				if(ent.type!="NONE")
				{
					sent.arrEnt.push_back(ent);				
					preEnt = ent;
				}
			}
			else { break; }
			
		}while(getline(entF, strEnt));

		if(count++ % 100 ==0)
		{
			cerr << ".";
		}
		//cout << "***** Sentence " << sent.id << "\n";	
	
		std::sort(sent.arrEnt.begin(),sent.arrEnt.end());

		
		//if(sent.arrEnt.size() <=1 && sent.arrRel.size()>0)
			//cout << "This sentence seems to be not consistent!!" << endl;
		//if(sent.arrEnt.size() > 1)
		{
				//  if(sent.id=="s10")
			/*if(sent.arrRel.size()>0)
				cout << "Has relations" << endl;*/
			//  if(sent.id=="s268")
			//if(count < 1956) //--> AIMed
			//	if(count >= 1956 && count <= 3055)// --> BioInfer
		//	if(count >= 3056) // --> LLL
			//if(count < 1302) // --> MEDLINE DDI
			//if(count > 1302) // --> DRUGBANK DDI
		//	string tempSent = strText.substr(sent.start, sent.end - sent.start);
			//if(tempSent.find(" not ")==string::npos && tempSent.find("n't")==string::npos && tempSent.find(" no ")==string::npos && tempSent.find("No ")==string::npos)
			//{
			cout << "***" << "Sentence" << stat.totalSent << ": " <<strText.substr(sent.start, sent.end - sent.start) << endl;
			/*if(sent.start==842473)
				sent.start = 842473;*/
				//sent.CheckRule(strText,stat); //temporarily closed for UTF-8
				//sent.AssignNETag();
				fileOut << "Sentence" << stat.totalSent << ": " << strText.substr(sent.start, sent.end - sent.start)<< endl; //--> for evaluation golden corpus
				sent.PrintRel(strText,fileOut);				
				stat.totalSent++;
				/*if(stat.totalSent==2190)
					stat.totalSent = stat.totalSent;*/
				//sent.PrintEnt(strText,fEnt);
				//sent.Evaluate(stat);
			//}
			/*else 
			{
				stat.falseneg += sent.arrRel.size();
				cout << "Negative sentence." << endl;
			}*/
		//	countCover += sent.count;
			//sent.extractEntPattern(strText,"disease","gene");
		}

		//	sent.FindPred(strText, countPred, countUnk,verbDic);			
			
		//	strPat = "\n" + sent.createPattern(strText) + "\n\n";
			
//			sent.extractEntPattern(strText);
			
	//		string strSent = strText.substr(sent.start, sent.end - sent.start);
		//	out.write(strSent.c_str(), strSent.length());
		//	out.write(strPat.c_str(), strPat.length());
	//	}

	} while(cate == "sentence");
	
	
	cerr << endl;
		
	/*re = tp * 1.0 / (tp+fn); pre = tp * 1.0 /(tp+fp); fscore = 2*re*pre/(re+pre);
	cout << "True positive:" << tp << "; false positive:" << fp << "; false negative:" << fn << endl;
	cout << "Precision=" <<pre <<"; "<<"Recall=" << re << "; F-score=" << fscore << endl;

	fscore = countCover * 1.0 / (tp + fn);
	cout << "Covered relations:" << countCover << "(" << fscore << ")" << endl;*/


	entFile.close();	
	enjuFile.close();
	fileOut.close();
	fEnt.close();
	
}
void func::readMetaRel2(string strIn, string strFileOut)
{
	
	std::ifstream is(strIn.c_str(),std::ifstream::in);
	if(!is)
	{

		cerr << "Error when opening " << strIn << endl;
		return ;
	}
	std::ofstream os(strFileOut.c_str(),std::ifstream::out);
	if(!os)
	{

		cerr << "Error when opening " << strFileOut << endl;
		return ;
	}

	string strLine;

	while(getline(is,strLine))
	{
		vector<string> arrTerm = common::split(strLine,"|");
		os << arrTerm[0] <<"\t" <<arrTerm[3] << "\t" << arrTerm[4] << "\t" << arrTerm[7] << endl;
//		string strRel = arrTerm[0] + "-" + arrTerm[4];
//		if(dicRel.find(strRel)==dicRel.end())
//			dicRel.insert(std::pair<string,string>(strRel,arrTerm[7]));
	}
	is.close();
	os.close();
//	return dicRel;
}

std::map<string,string> func::readMetaRel(string strIn)
{
	std::map<string,string> dicRel;
	std::ifstream is(strIn.c_str(),std::ifstream::in);
	if(!is)
	{

		cerr << "Error when opening " << strIn << endl;
		return dicRel;
	}

	string strLine;

	while(getline(is,strLine))
	{
		istringstream arrTerm(strLine.c_str());
		string concept1, concept2, rel, rela;
		arrTerm >> concept1 >> rel >> concept2 >> rela;
		concept1 += "-" + concept2;
		if(dicRel.find(concept1)==dicRel.end())
			dicRel.insert(std::pair<string,string>(concept1,rel + ":" + rela));
		else
		{
			string strRel = dicRel[concept1];
			rel += ":" + rela;
			if(strRel.find(rel)==string::npos)
				dicRel[concept1] += ";" + rel;
		}
			
	}
	is.close();
	return dicRel;
}
void func::FilterRel(string strFileIn, string strFileOut, std::map<string,string> dicRel)
{
	std::ifstream is(strFileIn.c_str(),std::ifstream::in);
	if(!is)
	{
		cerr << "Error when opening " << strFileIn << endl;
		return;
	}
	std::ofstream os(strFileOut.c_str(),std::ofstream::out);
	if(!os)
	{
		cerr << "Error when opening " << strFileOut << endl;
		return ;
	}
	string strLine;
	int total=0, filter=0;
	while(getline(is,strLine))
	{
		total++;
		string concept1, concept2, relName;	
		int start, end; 
		istringstream txtIs(strLine.c_str());
		txtIs >> start >> end >> relName >> concept1 >> concept2;
		//string strFind = concept1 + "-" + concept2;
		if(dicRel.find(concept1 + "-" + concept2)!=dicRel.end())
		{
			os << strLine << "\t" << dicRel[concept1+"-"+concept2] << endl;
			filter ++;
		}
		else if(dicRel.find(concept2 + "-" + concept1)!=dicRel.end())
		{
			os << strLine << endl;
			filter ++;
		}
	}

	is.close();
	os.close();
	cout << "Total: " << total << "\t Remained: "<< filter << endl;
}


void func::readDic(string strFileName, std::map<string,long int>& entDic, long int& total, long int& remained)
{
	cerr << strFileName << endl;
	std::ifstream file(strFileName.c_str(), std::ios_base::in);
	string strLine;

	while(getline(file,strLine))
	{
		size_t pos = strLine.find("\t");
		if(pos==string::npos)
			continue;
		if(strLine.find("Total")!=string::npos)
		{
			//Total:129092	 Remained:93405			
			size_t pos2 = strLine.find(":");
			total += atoi(strLine.substr(pos2+1, pos-pos2-1).c_str());
			pos2 = strLine.rfind(":");
			remained += atoi(strLine.substr(pos2+1).c_str());
			continue;
		}
		string strKey = strLine.substr(0,pos);
		int value = atoi(strLine.substr(pos+1).c_str());
		
		if(strKey =="&apos;s" || strKey == "-LRB-" || strKey == "%")
		{
			cout << "Error1" << strKey << " " << value << endl;
			continue;
		}
		if(entDic.find(strKey)==entDic.end())
			entDic.insert(std::pair<string,long int>(strKey,value));
		else
			entDic[strKey] += value;
	}
	file.close();

}

void func::countFile (string strFileName, std::map<string,long int>& dicRel)
{
	std::ifstream file(strFileName.c_str(), std::ios_base::in);
	string strLine;

	while(getline(file,strLine))
	{		
		vector<string> arrTerm = common::split(strLine,"\t");
		if(arrTerm.size() < 5)
		{
			cerr << strLine << endl;
			continue;
		}
		string relTerm = arrTerm[5];// + ";";
		if(relTerm[relTerm.length() - 1]==';')
			relTerm = relTerm.substr(0, relTerm.length() -1);
		if(dicRel.find(relTerm)==dicRel.end())
		{
			dicRel.insert(std::pair<string,long int>(relTerm,1));
		}
		else
		{
			dicRel[relTerm] ++;
		}
		/*vector<string> arrRel = common::split(relTerm,";");
		for(vector<string>::iterator it = arrRel.begin(); it!=arrRel.end(); it++)
		{
			string iRel = *it + "|";
			vector<string> arriRel = common::split(iRel,"|");
			for(vector<string>::iterator jt = arriRel.begin(); jt!=arriRel.end(); jt++)
			{
				string rel = *jt;
				if(dicRel.find(rel)!=dicRel.end())
				{
					dicRel[rel] ++;
				}

			}
		}*/
	}
	file.close();
}
/*void func::readAllRel(string strDir, string fileLDA, int threshold)
{
	std::map<string,string> relHash;
	int countRel = 0;
	fs::path full_path( fs::initial_path<fs::path>() );
	full_path = fs::system_complete( fs::path( strDir) );
	if ( !fs::exists( full_path ) )
  {
		std::cout << "\nNot found: " << full_path.filename() << std::endl;
    return;
  }

  if ( fs::is_directory( full_path ) )
  {
    fs::directory_iterator end_iter;
    for ( fs::directory_iterator dir_itr( full_path );
          dir_itr != end_iter;
          ++dir_itr )
    {
      try
      {
        if ( fs::is_regular_file( dir_itr->status() ) )
        {
					std::cout << dir_itr->path().filename() << "\n";
					string filename = dir_itr->path().parent_path().string() + "\\" + dir_itr->path().filename().string();
					createLinkLDAInput(filename,relHash,countRel);

				}
			}catch( const std::exception & ex )
      {
				std::cout << "Error2: " << ex.what() << endl;
        //std::cout << dir_itr->path().filename() << " " << ex.what() << std::endl;
      }
			
		}
	}
	
	
	std::ofstream fileOut(fileLDA.c_str(), std::ios_base::out);
	if(!fileOut)
	{
		cout << "Error when opening file " << fileLDA << endl;
		return;
	}

	for(map<string,string>::iterator it = relHash.begin(); it != relHash.end(); it++)
	{
		std::map<string,int> argHash;
		vector<string> arrTerm = common::split(it->second + "#","#");
		if(arrTerm.size() < threshold)
			continue;
		for(vector<string>::iterator j = arrTerm.begin(); j!= arrTerm.end(); j++)
		{
			string temp = *j;
			if(argHash.find(temp)==argHash.end())
			{
				argHash.insert(std::pair<string,int>(temp,1));
			}
			else
			{
				argHash[temp] ++;
			}
		}
		
		bool start = true;		

		for(map<string,int>::iterator z = argHash.begin(); z != argHash.end(); z++)
		{			
			vector<string> item = common::split(z->first + "\t", "\t");
			if(item.size()!=2)
			{
				cout << "ReadAllRel error: " << z->first << endl;
				continue;
			}
			if(z->second <= 2)
				continue;
			if(start == true)
			{
				fileOut << it ->first;
				start = false;
			}

			item[0] = common::normalizeStr2(item[0]);
			item[1] = common::normalizeStr2(item[1]);
			//item[2] = common::normalizeStr2(item[2]);

			//fileOut << "\targ1:\"" + item[0] + "\" arg1type:\"" + item[1] << "\" arg2:\"" << item[2] + "\" arg2type:\"" + item[3] << "\" " << z ->second;
			fileOut << "\targ1:\"" + item[0] +  "\" arg2:\"" + item[1] + "\" " << z ->second;
		}

		if(start == false)
			fileOut << endl;
	}

	fileOut << "Total Relations:" << countRel << endl;
}*/
/*void func::readAllDic(string strDir, string fileDic)
{
	long int total = 0, remained = 0;
	std::map<string,long int> entDic;
	fs::path full_path( fs::initial_path<fs::path>() );
	full_path = fs::system_complete( fs::path( strDir) );
	if ( !fs::exists( full_path ) )
  {
		std::cout << "\nNot found: " << full_path.filename() << std::endl;
    return;
  }

  if ( fs::is_directory( full_path ) )
  {
    fs::directory_iterator end_iter;
    for ( fs::directory_iterator dir_itr( full_path );
          dir_itr != end_iter;
          ++dir_itr )
    {
      try
      {
        if ( fs::is_regular_file( dir_itr->status() ) )
        {
					std::cout << dir_itr->path().filename() << "\n";
					string filename = dir_itr->path().parent_path().string() + "\\" + dir_itr->path().filename().string();
					readDic(filename, entDic, total, remained);

				}
			}catch( const std::exception & ex )
      {
				std::cout << "Error2: " << ex.what() << endl;
        //std::cout << dir_itr->path().filename() << " " << ex.what() << std::endl;
      }
			
		}
	}
	
	std::ofstream fileOut(fileDic.c_str(), std::ios_base::out);
	if(!fileOut)
	{
		cout << "Error when opening file " << fileDic << endl;
		return;
	}
//	boost::bimap<std::string, multiset_of<int>> biDic;
//	for(std::map<string,long int>::iterator it = entDic.begin(); it!=entDic.end(); it++)
//	{
//		//cout << it->first << "\t" << it->second << endl;
//		//fileOut << it->first << "\t" << it->second << endl;		
//		//string strKey = it->first;
//		//int value = it -> second;
////		biDic.left.insert(strKey,value);
//		biDic.insert(boost::bimap<std::string,multiset_of<int>>::value_type(it->first,it -> second));
//		
//		//biDic.insert(boost::bimap<std::string,int>::value_type(it->first,it->second));
//	
//	}

	//func.print_map(biDic.right,"\t",fileOut);
	vector<std::pair<std::string,int> > sortDic(entDic.begin(),entDic.end());
	std::sort(sortDic.begin(), sortDic.end(),&func::checkOrder);
	for(vector<std::pair<string,int> >::iterator it= sortDic.begin(); it != sortDic.end(); it++)
	{
		fileOut << it->first << "\t" << it->second << endl;	
	}

	entDic.clear();
	sortDic.clear();
	fileOut << "Total:" << total;
	fileOut << "\t Remained:" << remained << endl;
	//fileOut << 1 - (remained*1.0)/total << endl;
	fileOut.close();
}*/

/*void func::countRelation(string semNetFile, string strDir)
{
	std::map<string,long int> dicRel;
	//std::ifstream file(semNetFile.c_str(),std::ios_base::in);
	//if(file==NULL)
	//{
	//	cerr << "Errod of opening file " << semNetFile << endl; 
	//	return;
	//}
	//string strLine="";
	int count = 0;
	//while(getline(file,strLine))
	//{
	//	size_t pos = strLine.find("\t");
	//	if(pos==string::npos)
	//		continue;
	//	//if(dicRel.find(strLine.substr(0,pos))!=dicRel.end())
	//	//{
	//	//	cout << strLine << endl;
	//	//}
	//	dicRel.insert(std::pair<string,int>(strLine.substr(0,pos), 0));
	//	//count++;
	//}
//	cout << count << endl;
	//file.close();
	fs::path full_path( fs::initial_path<fs::path>() );
	full_path = fs::system_complete( fs::path( strDir) );
	if ( !fs::exists( full_path ) )
  {
		std::cerr << "\nNot found: " << full_path.filename() << std::endl;
    return;
  }

  if ( fs::is_directory( full_path ) )
  {
    fs::directory_iterator end_iter;
    for ( fs::directory_iterator dir_itr( full_path );
          dir_itr != end_iter;
          ++dir_itr )
    {
      try
      {
        if ( fs::is_regular_file( dir_itr->status() ) )
        {
					std::cerr << dir_itr->path().filename() << "\n";
					string filename = dir_itr->path().parent_path().string() + "\\" + dir_itr->path().filename().string();
					countFile(filename, dicRel);

				}
			}catch( const std::exception & ex )
      {
				std::cerr << ex.what() << endl;
        //std::cout << dir_itr->path().filename() << " " << ex.what() << std::endl;
      }
			
		}
	}
	
	for(std::map<string,long int>::iterator it = dicRel.begin(); it!=dicRel.end(); it++)
	{
		cout << it->first << "\t" << it->second << endl;
	}

		
}
*/

void func::mergeFileforEvaluate(string strGold, string strAuto, string strFinal)
{
	std::ifstream gold(strGold.c_str(), std::ios_base::in);
	std::ifstream au(strAuto.c_str(), std::ios_base::in);
	std::ofstream fin(strFinal.c_str(), std::ios_base::out);

	string goldEnt, autoEnt;
	while(getline(gold,goldEnt))
	{
		if(goldEnt=="")
		{				
			fin << endl;
			getline(au,autoEnt);
			continue;
		}		
		getline(au,autoEnt);
		size_t pos = autoEnt.rfind(" ");
		goldEnt += autoEnt.substr(pos);
		fin << goldEnt << endl;
	}

	gold.close();
	au.close();
	fin.close();
}

void func::ExtractNP(const string& txtName, const string& enjuName, const string& strFileOut,  statistics& stat)
{
	//this function is used to extract NP-NP pairs
	std::fstream is(txtName.c_str(), std::fstream::in);
	if(!is)
	{
		cerr << "Error when opening " << txtName << endl;
		return;
	}		

	std::ifstream enjuFile(enjuName.c_str(), std::ios_base::in | std::ios_base::binary);
	
	if(!enjuFile) {
		cerr << "Error when opening file" << enjuName << endl;	
		return;
	}
	boost::iostreams::filtering_istream enjuF;
	enjuF.push(boost::iostreams::gzip_decompressor());
	enjuF.push(enjuFile);

	/*std::ofstream fileOut(strFileOut.c_str(),std::ofstream::out);

	if(fileOut == NULL) {
		cerr << "Error when opening file" << strFileOut << endl;	
		return;
	}*/

	io::filtering_ostream fileOut;
	try{
		fileOut.push(io::gzip_compressor());
		fileOut.push(io::file_descriptor_sink(strFileOut));
	}catch(exception& ex)
	{
		cerr << ex.what() << endl;
		return;
	}

	is.seekg (0, is.end);
  size_t length = is.tellg();
  is.seekg (0, is.beg);

  char * buffer = new char [length];
	is.read(buffer,length);

	string strText(buffer);
	length = strText.length();
	string strPat = "";

	string strLine, strEnt, cate;
	int countPred = 0, countUnk=0;
	int count= 0;
	int posTok=0;

	getline(enjuF,strLine);	
	while(strLine.find("parse_status=\"success\"")==string::npos 
			&& strLine.find("parse_status=\"fragmental parse\"")==string::npos)
		{
			Sentence sent;
			cate = sent.parse(strLine);
			if(cate == "sentence")
			{
				fileOut << "sent\t" << sent.start << "\t" << sent.end << endl;
			}
			getline(enjuF,strLine);
			if(strLine == "")
				return;
			/*if(strLine.find("sentence id=")!=string::npos)
			{
				fileOut << "Sentence" << stat.totalSent << endl;
				stat.totalSent++;
			}*/
		}
	//ofstream out;
	do
	{	
		if(strLine.length() < 2) { 	break; }
	
		Sentence sent;
		sent.arrRelA.clear();
		posTok = 0;
		
		while(strLine.find("parse_status=\"success\"")==string::npos 
			&& strLine.find("parse_status=\"fragmental parse\"")==string::npos)
		{
			cate = sent.parse(strLine);
			if(cate == "sentence")
			{
				fileOut << "sent\t" << sent.start << "\t" << sent.end << endl;
				//count ++;
			}
			getline(enjuF,strLine);
			if(strLine == "")
				return;
			/*if(strLine.find("sentence id=")!=string::npos)
			{
				fileOut << "Sentence" << stat.totalSent << endl;
				stat.totalSent++;

			}*/
		}
		

		cate = sent.parse(strLine);

		fileOut << "sent\t" << sent.start << "\t" << sent.end << endl;
		
		do{ //loop on enju file
			getline(enjuF, strLine);
			
			if(strLine.length() < 1) break;
			Term term;
			term.parse(strLine);
			term.check = false;
			term.netag = "O";
			if(term.start == term.end && term.start==0) break;
			if(term.type == "tok")
			{
				sent.arrTok.push_back(term);
			}
			else
			{
				sent.arrTerm.push_back(term);
			}
			
			cate = term.type;
	
		}while(cate != "sentence");
		
		//cout << "***" <<strText.substr(sent.start, sent.end - sent.start) << endl;
		sent.eCheckRule(strText,stat,fileOut);		
		count += sent.numOfNPpairs;

	} while(cate == "sentence");
	
	cerr <<"Number of NP pairs:" << count ;
	cerr << endl;	

	enjuFile.close();
//	fileOut.close();
	fileOut.flush();
	fileOut.reset();
	
}
bool func::isInSemList(string strSem)
{
	//"aapp","amph","anim","antb","bacs","bact","bdsu","bird","bodm","bpoc","bsoj","carb","celc",
	//"cell","cgab","chem","chvf","chvs","clnd","dsyn","eico","elii","enzy","fish","fngs","food",
	//"geoa","gngm","hops","horm","humn","inch","irda","lipd","mamm","nnon","nsba","opco","orch",
	//"orgm","phsu","plnt","podg","rcpt","rept","sbst","strd","tisu","virs","vita","vtbt"
	/*if(strSem=="aapp"|| strSem=="amph"|| strSem=="anim"|| strSem=="antb"|| strSem=="bacs"|| 
		strSem=="bact"|| strSem=="bdsu"|| strSem=="bird"|| strSem=="bodm"|| strSem=="bpoc"|| 
		strSem=="bsoj"|| strSem=="carb"|| strSem=="celc"|| strSem=="cell"|| strSem=="cgab"|| 
		strSem=="chem"|| strSem=="chvf"|| strSem=="chvs"|| strSem=="clnd"|| strSem=="dsyn"|| 
		strSem=="eico"|| strSem=="elii"|| strSem=="enzy"|| strSem=="fish"|| strSem=="fngs"|| 
		strSem=="food"|| strSem=="geoa"|| strSem=="gngm"|| strSem=="hops"|| strSem=="horm"|| 
		strSem=="humn"|| strSem=="inch"|| strSem=="irda"|| strSem=="lipd"|| strSem=="mamm"|| 
		strSem=="nnon"|| strSem=="nsba"|| strSem=="opco"|| strSem=="orch"|| strSem=="orgm"|| 
		strSem=="phsu"|| strSem=="plnt"|| strSem=="podg"|| strSem=="rcpt"|| strSem=="rept"|| 
		strSem=="sbst"|| strSem=="strd"|| strSem=="tisu"|| strSem=="virs"|| strSem=="vita"|| 
		strSem=="vtbt")*/
	if(strSem=="clas"||strSem=="cnce"||strSem=="ftcn"||strSem=="grpa"||strSem=="idcn"
		||strSem=="inpr"||strSem=="lang"||strSem=="qlco"||strSem=="qnco"||strSem=="rnlw"
		||strSem=="spco"||strSem=="tmco"||strSem=="acty"||strSem=="bhvr"||strSem=="dora"
		||strSem=="evnt"||strSem=="gora"||strSem=="inbe"||strSem=="mcha"||strSem=="ocac"
		||strSem=="socb"||strSem=="clas"||strSem=="cnce"||strSem=="ftcn"||strSem=="grpa"
		||strSem=="idcn"||strSem=="inpr"||strSem=="lang"||strSem=="qlco"||strSem=="qnco"
		||strSem=="rnlw"||strSem=="spco"||strSem=="tmco"||strSem=="bmod"||strSem=="ocdi"
		||strSem=="hcro"||strSem=="orgt"||strSem=="pros"||strSem=="shro")
		return false;
	return true;
}
string func::disambiguateSem(string strSem)
{
	int count = 0;
	if(strSem.find(",")==string::npos)
	{
		count = 1;
		if(isInSemList(strSem)==false)
			return "";
		//return "";
	}
	else
	{
		//split string by ','
		vector<string> arrSem = common::split(strSem +",", ",");
		for(int i = 0; i< arrSem.size(); i++)
			if(isInSemList(arrSem[i])==true)
			{
				count++;
			}
		
	}
	if(count == 0)
		return "";
	
	return strSem;
}

void func::CheckCoverage(string filename, statistics& stat)
{
	/*int tp=0, fp=0,fn=0, countCover=0;
	double pre=0.0, re = 0.0, fscore=0.0;*/
	string temp = filename + ".txt";	
	std::fstream is(temp.c_str(), std::fstream::in);
	if(!is)
	{
		cerr << "Error when opening " << temp << endl;
		return;
	}
	//fstream geneF("medline12n0684.gene-ne-so");
	temp = filename+ ".gene-ne-so.gz";
	//fstream geneF(temp.c_str(),std::fstream::in );
	std::ifstream geneFile(temp.c_str(), std::ios_base::in | std::ios_base::binary);
	if(!geneFile)
	{
		cerr << "Error when opening " << temp << endl;
		return;
	}
	boost::iostreams::filtering_istream geneF;
	geneF.push(boost::iostreams::gzip_decompressor());
	geneF.push(geneFile);

	//fstream otherF("medline12n0684.other-ne-so");
	temp = filename + ".other-ne-so.gz";

	//fstream otherF(temp.c_str(),std::fstream::in);
	std::ifstream otherFile(temp.c_str(), std::ios_base::in | std::ios_base::binary);

	if(!otherFile)
	{
		cerr << "Error when opening " << temp << endl;
		return;
	}
	boost::iostreams::filtering_istream otherF;
	otherF.push(boost::iostreams::gzip_decompressor());
	otherF.push(otherFile);


	//fstream gdaF("medline12n0684.dga-so");
	temp = filename + ".dga-so.gz";
	//fstream gdaF(temp.c_str(), std::fstream::in);
	std::ifstream gdaFile(temp.c_str(), std::ios_base::in | std::ios_base::binary);
	if(!gdaFile)
	{
		cerr << "Error when opening " << temp << endl;
		return;
	}
	boost::iostreams::filtering_istream gdaF;
	gdaF.push(boost::iostreams::gzip_decompressor());
	gdaF.push(gdaFile);
	//	fstream enjuF("medline12n0684.enju-so");
	temp = filename + ".enju-so.gz";
	//fstream enjuF(temp.c_str(),std::fstream::in);
	std::ifstream enjuFile(temp.c_str(), std::ios_base::in | std::ios_base::binary);
	//fstream enjuF("test.txt");

	if(!enjuFile) {
		cerr << "Error when opening file" << temp << endl;	
		return;
	}
	boost::iostreams::filtering_istream enjuF;
	enjuF.push(boost::iostreams::gzip_decompressor());
	enjuF.push(enjuFile);


	is.seekg (0, is.end);
  int length = is.tellg();
  is.seekg (0, is.beg);

  char * buffer = new char [length];
	is.read(buffer,length);

	string strText(buffer);
	length = strText.length();
	string strPat = "";
	
	map<string,int> verbDic;
	int countGeneDisease = 0;

	string strLine, cate, strGDA,strGene, strOther;
	int countPred = 0, countUnk=0;
	long int count= 0;

	getline(enjuF,strLine);	
	getline(otherF, strOther); 	
	getline(geneF, strGene);	
	getline(gdaF, strGDA);
	//fstream out("pattern.txt");
	std::ofstream out;
	out.open("pattern-1.txt");
	do
	{	
		if(strLine.length() < 2) { 	break; }
		
		Sentence sent;
		cate = sent.parse(strLine);

		do{ //loop on enju file
			getline(enjuF, strLine);
			
			if(strLine.length() < 1) break;
			Term term;
			term.parse(strLine);
			if(term.start == term.end && term.start==0) break;
			if(term.type == "tok")
			{
				sent.arrTok.push_back(term);
			}
			else
			{
				sent.arrTerm.push_back(term);
			}
			
			cate = term.type;
	
		}while(cate != "sentence");
		
//		int sGene, eGene;	
		Term preGene;
		do{ // loop on gene file
			Term gene;
			bool res;// = gene.parseEnt(strGene,sent.start, sent.end, true);			
			if(res)
			{
				if(gene.id.find("aentity")!=string::npos)
					continue;
			/*	if(sent.arrEnt.size()>0)
				{
					if(preGene.start==gene.start && preGene.end==gene.end)
						continue;
				}*/
				sent.arrEnt.push_back(gene);
				//InsertArray(sent.arrEnt, gene);
				countGeneDisease++;

				preGene = gene;
			}
			else { break; }
			
		}while(getline(geneF, strGene));
		
//		int sEnt, eEnt;
		Term preEnt;
		do //loop on other_entity file
		{
			Term ent;
			bool res;// = ent.parseEnt(strOther, sent.start, sent.end, false);
			if(res )
			{
				if(ent.type=="disease")
				{
					/*if(sent.arrEnt.size() > 0)
					{
						if(preEnt.start == ent.start && preEnt.end == ent.end)
						{
							if(preEnt.type != ent.type)
							{
								sent.arrEnt[sent.arrEnt.size()-1].type = sent.arrEnt[sent.arrEnt.size()-1].type + "|" + ent.type;
							}
							continue;
						}
					}*/
					sent.arrEnt.push_back(ent);
					//InsertArray(sent.arrEnt,ent);
					preEnt = ent;
				}
				else  {	continue;}
			}
			else { break;}			
			
		}while( getline(otherF, strOther));

		//loop on gda file
		do
		{
			Relation rel; 
			bool res = rel.parse(strGDA, sent.start, sent.end);
			if(res)
			{
				sent.arrRel.push_back(rel);
			}
			else { break;}
		} while(getline(gdaF, strGDA));

		if(count++ % 100 ==0)
		{
			cerr << ".";
		}
		cout << "Processing sentence " << sent.id << "\n";
	
		
		std::sort(sent.arrEnt.begin(),sent.arrEnt.end());

		
			
		//	sent.constructPredMatrix();
		stat.totalSent++;
		if(sent.arrEnt.size() > 1)
		{
				//  if(sent.id=="s10")
			/*if(sent.arrRel.size()>0)
				cout << "Has relations" << endl;*/
			//  if(sent.id=="s268")
			//if(count < 1956) //--> AIMed
			//	if(count >= 1956 && count <= 3055)// --> BioInfer
		//	if(count >= 3056) // --> LLL
			//if(count < 1302) // --> MEDLINE DDI
			//if(count > 1302) // --> DRUGBANK DDI
			string tempSent = strText.substr(sent.start, sent.end - sent.start);
			//if(tempSent.find(" not ")==string::npos && tempSent.find("n't")==string::npos && tempSent.find(" no ")==string::npos && tempSent.find("No ")==string::npos)
			{//sent.CheckRule(strText,stat);
			sent.Evaluate(stat);}
			/*else 
			{
				stat.falseneg += sent.arrRel.size();
				cout << "Negative sentence." << endl;
			}*/
		//	countCover += sent.count;
			//sent.extractEntPattern(strText,"disease","gene");
		}

		//	sent.FindPred(strText, countPred, countUnk,verbDic);			
			
		//	strPat = "\n" + sent.createPattern(strText) + "\n\n";
			
//			sent.extractEntPattern(strText);
			
	//		string strSent = strText.substr(sent.start, sent.end - sent.start);
		//	out.write(strSent.c_str(), strSent.length());
		//	out.write(strPat.c_str(), strPat.length());
	//	}

	} while(cate == "sentence");
	
	cerr << endl;
		
	/*re = tp * 1.0 / (tp+fn); pre = tp * 1.0 /(tp+fp); fscore = 2*re*pre/(re+pre);
	cout << "True positive:" << tp << "; false positive:" << fp << "; false negative:" << fn << endl;
	cout << "Precision=" <<pre <<"; "<<"Recall=" << re << "; F-score=" << fscore << endl;

	fscore = countCover * 1.0 / (tp + fn);
	cout << "Covered relations:" << countCover << "(" << fscore << ")" << endl;*/


	geneFile.close();
	otherFile.close();
	enjuFile.close();
	gdaFile.close();
}

void func::ExtractRelNP(const string& entFileName, const string& relFileName, map<string,string> semNet)
{
	std::ofstream relFile(relFileName.c_str(), std::ios_base::out);
	std::ifstream entFile(entFileName.c_str(), std::ios_base::in | std::ios_base::binary);
	if(!entFile)
	{
		cerr << "Error when opening " << entFileName << endl;
		return;
	}
	vector<Term> arrEnt1, arrEnt2;
	boost::iostreams::filtering_istream entF;
	entF.push(boost::iostreams::gzip_decompressor());
	entF.push(entFile);
	string strLine;
	int index = 0;
	int pair = 0;
	while(getline(entF,strLine))
	{
		if(strLine.find("Sentence")==0)
		{
			relFile << strLine << endl;
			index++;
			continue;
		}
		if(strLine == "NP1")
		{
			//pair ++;
			while(getline(entF, strLine) && strLine.find("NP2")!=0)
			{
				vector<string> arrTerm = common::split(strLine + "\t" , "\t");
				Term ent;
				ent.start = atoi(arrTerm[0].c_str()); ent.end = atoi(arrTerm[1].c_str());
				ent.type = arrTerm[2]; ent.conceptID = arrTerm[3]; ent.content = arrTerm[4];
				arrEnt1.push_back(ent);
			}
			while(getline(entF, strLine) && strLine.find("NP1")!=0)
			{
				vector<string> arrTerm = common::split(strLine + "\t" , "\t");
				Term ent;
				ent.start = atoi(arrTerm[0].c_str()); ent.end = atoi(arrTerm[1].c_str());
				ent.type = arrTerm[2]; ent.conceptID = arrTerm[3]; ent.content = arrTerm[4];
				arrEnt2.push_back(ent);
				
				//assign relations

			}
		}
		
	}


	entFile.close();
}



void func::ReadReVerb(string textName, string strFileName, string strFileNP, string strFileMM, float threshold)
{
	std::ifstream text(textName.c_str(),std::ios_base::in);

	std::ifstream file(strFileName.c_str(), std::ios_base::in);

	std::ofstream fileNP(strFileNP.c_str(), std::ios_base::out);

	std::ofstream fileMM(strFileMM.c_str(), std::ios_base::out);
	
	string strLine;
	string strSent;
	/*while(getline(file,strLine))
	{
		vector<string> arrTerm = common::split(strLine+"\t", "\t");	
		strSent = common::normalizeStr(arrTerm[12]);
		fileNP << strSent << endl;
	}*/

	
	getline(file,strLine);
	vector<string> arrTerm;// = common::split(strLine+"\t", "\t");	
	int sentNum;// = atoi(arrTerm[1].c_str());

	int index = 1;
	int globalPos = 0 ;
	while(getline(text,strSent))
	{
		/*if(index==494)
		{
			index = index;
		}*/
		fileNP << "sent" << "\t" << globalPos << "\t" << globalPos + strSent.length()<<endl;
		do
		{
			if(strLine=="")
			{
				getline(file,strLine);
				break;
			}
			arrTerm = common::split(strLine+"\t", "\t");
			if(arrTerm.size()!=18)
			{
				cerr << "Format is not correct " << strLine;
				break;				
			}

			sentNum = atoi(arrTerm[1].c_str());				
			if(sentNum > index)
			{
				break;
			}

			if(atof(arrTerm[11].c_str()) < threshold)
				continue;
			string temp1 = common::normalizeStr(arrTerm[2]);
			int start1 = strSent.find(temp1);
			if(start1==-1)
			{				
				cerr << "**arg1:" << strSent << endl << arrTerm[2] << endl;
				continue;
			}
			string temp2 = common::normalizeStr(arrTerm[4]);
			int start2 = strSent.find(temp2, start1 + temp1.length());
			if(start2==-1)
			{
				cerr << "**arg2:" << strSent << endl << arrTerm[4] << endl;
				continue;
			}

			//fileNP << atoi(arrTerm[5].c_str()) + globalPos << "\t" << atoi(arrTerm[6].c_str()) + globalPos << "\t" << arrTerm[2] << endl;
			fileNP << start1 + globalPos << "\t" << start1 + globalPos + temp1.length() << "\t" << temp1 << endl;
			fileMM << temp1 << endl;

			fileNP << "rel" << "\t" << arrTerm[3] << endl;

			//fileNP << atoi(arrTerm[7].c_str()) + globalPos << "\t" << atoi(arrTerm[8].c_str()) + globalPos << "\t" << arrTerm[4] << endl;
			fileNP << start2 + globalPos << "\t" << start2 + globalPos + temp2.length()<< "\t" << temp2 << endl;
			fileMM << temp2 << endl;

		}while(getline(file,strLine) && sentNum == index);

		index ++;
		globalPos += strSent.length() + 1;
	}
	
	
	file.close();
	fileNP.close();
	fileMM.close();
}

void func::ReadOLLIE(string textFile, string strFileName, string strFileNP, string strFileMM, float threshold)
{
	std::ifstream text(textFile.c_str(),std::ios_base::in);

	std::ifstream file(strFileName.c_str(), std::ios_base::in);

	std::ofstream fileNP(strFileNP.c_str(), std::ios_base::out);

	std::ofstream fileMM(strFileMM.c_str(), std::ios_base::out);
	
	string strLine;
	string strSent;

	std::fstream is(textFile.c_str(), std::fstream::in);
	is.seekg (0, is.end);
  size_t length = is.tellg();
  is.seekg (0, is.beg);

  char * buffer = new char [length];
	is.read(buffer,length);
	string strText(buffer);
	length = strText.length();

	int globalPos = 0;
	int prePos = 0;
	int localPos = 0;
	vector<string> arrTerm;
	while(getline(file,strLine))
	{
		if(strLine.find("> ")==0)
		{
			strSent = strLine.substr(2);
			globalPos = strText.find(strSent,prePos);
			if(globalPos == string::npos)
			{
				cerr << "Wrong sentence " << strSent << endl;
				return;
			}
			fileNP << "sent\t" << globalPos << "\t" << globalPos + strSent.length() << endl;
			while(getline(file,strLine))
			{
				if(strLine == "")
					break;
				//0.844: (The atrioventricular bundle; ran through; the fibrous trigone)
				int pos = strLine.find(":");
				if(pos==string::npos)
					continue;
				if(atof(strLine.substr(0,pos).c_str()) < threshold)
					continue;
				int rpos = strLine.find("[enabler=");
				if(rpos!=string::npos)
				{
					strLine = strLine.substr(0,rpos);				}

				strLine = strLine.substr(pos+3, strLine.length() - pos - 3 - 1); //skip the first " (" and the last ')'
				arrTerm = common::split(strLine + ";", ";");
				if(arrTerm.size() != 3)
					continue;
				int start1 = strSent.find(arrTerm[0]);
				if(start1 == -1)
					continue;
				int start2 = strSent.find(arrTerm[2].substr(1));
				if(start2 == -1)
					continue;
				int start3 = strSent.find(arrTerm[1]);
				/*if(start3 == -1)
					continue;*/
				fileNP << start1 + globalPos << "\t" << start1 + globalPos + arrTerm[0].length()<<  "\t" << arrTerm[0]<< endl;
				fileMM << arrTerm[0] << endl;

				fileNP << "rel\t" <<  arrTerm[1].substr(1) << endl;

				fileNP << start2 + globalPos << "\t" << start2 + globalPos + arrTerm[2].length()-1 <<  "\t" << arrTerm[2].substr(1) << endl;
				fileMM << arrTerm[2].substr(1) << endl;


			}
		}

		prePos = globalPos + strSent.length();
	}


	file.close();
	fileNP.close();
	fileMM.close();
}

void func::ChooseRelName(string fileRel, string fileOut, std::map<string,string>& relOnto)
{
	std::ifstream in(fileRel.c_str(),std::ios_base::in);
	
	std::ofstream out(fileOut.c_str(), std::ios_base::out);
	string strLine;
	while(getline(in,strLine))
	{
		if(strLine.find("Sentence")==string::npos)
		{
			vector<string> arrTerm = common::split(strLine,"\t");
			string relName = arrTerm[0];
			vector<string> arrName = common::split(relName+";",";");
			vector<string> temp;
			string res = "";
			for(int i=0; i< arrName.size(); i++)
			{
			//	//if(arrName[i].find("_by")==string::npos && arrName[i].find("has_")==string::npos)
			//	{
			//		temp.push_back(arrName[i]);
					res += arrName[i] + " ";
			//	}
			}
			//bool arr
			for(int i=0; i< arrName.size()-1; i++)
			{
				string type1 = arrName[i];
				for(int j=i+1; j < arrName.size(); j++)
				{
					string type2 = arrName[j];
					string combine = type1 + "-" + type2;
					std::map<string,string>::iterator it = relOnto.find(combine);
					if(it!=relOnto.end())
					{
						if(it ->second == "isa")
						{
								size_t pos = res.find(type1);
								if(pos==string::npos)
									continue;
								res = res.replace(pos,type1.length() + 1, "");
						}
					}
					else 
					{
						combine = type2 + "-" + type1;
						it = relOnto.find(combine);
						if(it!=relOnto.end())
						{
							if(it ->second == "isa")
							{
								size_t pos = res.find(type2);
								if(pos==string::npos)
									continue;
								res = res.replace(pos,type2.length() + 1, "");
							}
						}
					}
				}
			}
			//cout
			strLine = strLine.replace(0,arrTerm[0].length(),res);
		}
		out << strLine << endl;
	}

	in.close();
	out.close();

}

void func::sortEntityFiles(string fileIn, string fileOut) {
	//this function is used to sort entities in *.ann files by brat based on their text span
	std::fstream entFile(fileIn.c_str(), std::fstream::in);
	if(!entFile) {
		cerr << "Error when opening " << fileIn << endl;
		return;
	}
	vector<string> arrLine; // = new vector<string>();
	vector<int> textSpan; // = new vector<int>();

	string strLine;
	while(getline(entFile,strLine)) {
		if(strLine[0] == '#'){
			arrLine.push_back(strLine);
			continue;
		}
		vector<string> arrStr = common::split(strLine, "\t");
		if(arrStr.size() < 3) continue;

		istringstream entIs(arrStr[1].c_str());
		int entStart, entEnd;
		string category;
		entIs >> category >> entStart >> entEnd;
		if(textSpan.size()==0) {
			textSpan.push_back(entStart);
			arrLine.push_back(strLine);
		} else {
			int lastEntPos = textSpan[textSpan.size()-1];
			if(entStart >= lastEntPos) {
				textSpan.push_back(entStart);
				arrLine.push_back(strLine);
			} else {
				//find the position that create the disorder
				int i=0;
				while(i < textSpan.size()) {
					lastEntPos = textSpan[i];
					if(entStart < lastEntPos) break;
					i++;
				}
				//move every element behind that back to one position
				if(i < textSpan.size()) {
					int movePos = textSpan[textSpan.size()-1];
					string moveLine = arrLine[textSpan.size()-1];
					for(int j = textSpan.size() - 1; j > i;  j--) {
						textSpan[j] = textSpan[j-1];
						arrLine[j] = arrLine[j-1];
					}
					textSpan[i] = entStart;
					arrLine[i] = strLine;
					textSpan.push_back(movePos);
					arrLine.push_back(moveLine);
				} else {
					cerr << "Check " << strLine << endl;
				}


			}

		}

	}
	entFile.close();

	std::ofstream resFile(fileOut.c_str(), std::fstream::out);
	if(!resFile) {
		cerr << "Error when opening " << fileOut << endl;
		return;
	}

	for(int i=0; i< arrLine.size(); i++) {
		resFile << arrLine[i] << endl;
	}

	resFile.close();
}

void func::extractOccurrence(const string& txtName, const string& entName,
		const string& enjuName,
		const string& dirOut,
		statistics& stat)
{
	cerr << txtName << endl;
	//this function is used to process biodiversity documents
	std::wifstream wif(txtName.c_str());
	//wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	wif.imbue(std::locale("en_US.utf8"));
	std::wstringstream wss;
	wss << wif.rdbuf();

	wstring strText =  wss.str();


	/*fstream is(txtName.c_str(), std::fstream::in);
	if(!is)
	{
		cerr << "Error when opening " << txtName << endl;
		return;
	}*/

	//the order of entities isn't ascending --> should read all entities first ...
	//pre-process to sort entities in the files based on their text span

	sortEntityFiles(entName, "temp.txt");
	int pos = entName.rfind("/");
	string relFileName = entName.substr(pos + 1, entName.length() - pos - 1);

	//fstream entFile(entName.c_str(), std::fstream::in);
	std::fstream entFile("temp.txt", std::fstream::in);
	if(!entFile) {
		cerr << "Error when opening " << entName << endl;
		return;
	}

	std::ifstream enjuFile(enjuName.c_str(), std::ios_base::in | std::ios_base::binary);
	//fstream enjuF("test.txt");

	if(!enjuFile) {
		cerr << "Error when opening file " << enjuName << endl;
		return;
	}
	boost::iostreams::filtering_istream enjuF;
	enjuF.push(boost::iostreams::gzip_decompressor());
	enjuF.push(enjuFile);


	relFileName = dirOut + "/" + relFileName;
	//copy the annotated entities to the resulting file
	boost::filesystem::path source = entName.c_str();
	boost::filesystem::path des = relFileName.c_str();
	if (exists (des))
	    remove (des);
	boost::filesystem::copy_file(source, des);//, copy_option::fail_if_exists);

	std::ofstream fileOut(relFileName.c_str(), std::ofstream::app);

	if(!fileOut) {
		cerr << "Error when opening file " << relFileName << endl;
		return;
	}


	string strLine, strEnt, cate;
	//int countPred = 0, countUnk=0;
	long int count= 0;
	int countRel = 0;
	//int posTok=0;

	getline(enjuF,strLine);
	while(strLine.find("parse_status=\"success\"")==string::npos
			&& strLine.find("parse_status=\"fragmental parse\"")==string::npos)
	{
		getline(enjuF,strLine);
		if(strLine == "")
			return;
		if(strLine.find("sentence id=")!=string::npos)
		{
			//fileOut << "Sentence" << stat.totalSent << endl;
			stat.totalSent++;
		}
	}


	getline(entFile, strEnt);
	while(strEnt[0] == '#'){
		getline(entFile,strEnt);
	}


	while(strLine.length() > 1)
	{
		if(strLine.length() < 2) { 	break; }
		Sentence sent;
		sent.arrRelA.clear();
		//posTok = 0;

		while(strLine.find("parse_status=\"success\"")==string::npos
			&& strLine.find("parse_status=\"fragmental parse\"")==string::npos)
		{
			//skip empty line
			if(strLine.find("parse_status=\"empty line\"")!= string::npos) {
				getline(enjuF,strLine);
			}
			getline(enjuF,strLine);
			if(strLine == "")
				return;
			if(strLine.find("sentence id=")!=string::npos)
			{
				//fileOut << "Sentence" << stat.totalSent << endl;
				stat.totalSent++;

			}
		}


		cate = sent.parse(strLine);
		//loop on enju file for sentences
		while(strLine.length() > 1){
			Term term;
			term.parse(strLine);
			term.check = false;
			term.netag = "O";
			if(term.start == term.end && term.start==0) break;
			if(term.type == "tok")
			{
				sent.arrTok.push_back(term);
			}
			else
			{
				sent.arrTerm.push_back(term);
			}
			cate = term.type;
			getline(enjuF, strLine);
			//cerr << strLine << "\n";
		}

		stat.totalSent++;
		/*if(stat.totalSent == 23) {
			cerr << "Debug" ;
		}*/
//		int sGene, eGene;
		Term preEnt;
		do{ // loop on entity file
			if(strEnt[0] == '#') continue;
			Term ent;
			//int sGene, eGene;
			bool res = ent.parseBiodivEnt(strEnt,sent.start, sent.end);			//--> for all

			if(res)
			{
				if(ent.netag!="NONE")
				{
					sent.arrEnt.push_back(ent);
					preEnt = ent;
				}
			}
			else { break; }

		}while(getline(entFile, strEnt));


		std::sort(sent.arrEnt.begin(),sent.arrEnt.end());


		if(sent.arrEnt.size() > 1)
		{
			cout << "***" << "Sentence" << stat.totalSent << ": ";
			wcout << strText.substr(sent.start, sent.end - sent.start) << endl;
			sent.CheckRule(strText,stat);
			sent.PrintOccurence(strText,fileOut, countRel);


		}
		getline(enjuF,strLine);

	} //while(cate == "sentence");


	cerr << endl;

	/*re = tp * 1.0 / (tp+fn); pre = tp * 1.0 /(tp+fp); fscore = 2*re*pre/(re+pre);
	cout << "True positive:" << tp << "; false positive:" << fp << "; false negative:" << fn << endl;
	cout << "Precision=" <<pre <<"; "<<"Recall=" << re << "; F-score=" << fscore << endl;

	fscore = countCover * 1.0 / (tp + fn);
	cout << "Covered relations:" << countCover << "(" << fscore << ")" << endl;*/


	entFile.close();
	enjuFile.close();
	fileOut.close();
	//fEnt.close();

}


void func::ProcessFiles3(const string& txtName, const string& entName, const string& enjuName, const string& strFileOut, 
	std::map<string,string>& semNet, std::map<string,string>& metaRel, statistics& stat)
{
	//this function is used to process the whole MEDLINE with metamap output after filtering 3
	//ofstream fEnt("ent.txt", std::ios_base::out);
	std::fstream is(txtName.c_str(), std::fstream::in);
	if(!is)
	{
		cerr << "Error when opening " << txtName << endl;
		return;
	}
		
	std::ifstream entFile(entName.c_str(), std::ios_base::in | std::ios_base::binary);
	if(!entFile)
	{
		cerr << "Error when opening " << entName<< endl;
		return;
	}
	boost::iostreams::filtering_istream entF;
	entF.push(boost::iostreams::gzip_decompressor());
	entF.push(entFile);

	//	fstream enjuF("medline12n0684.enju-so");
	
	//fstream enjuF(temp.c_str(),std::fstream::in);
	std::ifstream enjuFile(enjuName.c_str(), std::ios_base::in | std::ios_base::binary);
	//fstream enjuF("test.txt");

	if(!enjuFile) {
		cerr << "Error when opening file" << enjuName << endl;	
		return;
	}
	boost::iostreams::filtering_istream enjuF;
	enjuF.push(boost::iostreams::gzip_decompressor());
	enjuF.push(enjuFile);

	std::ofstream fileOut(strFileOut.c_str(),std::ofstream::out);

	if(!fileOut) {
		cerr << "Error when opening file" << strFileOut << endl;	
		return;
	}


	is.seekg (0, is.end);
	size_t length = is.tellg();
	is.seekg (0, is.beg);

	char * buffer = new char [length];
	is.read(buffer,length);

	string strText(buffer);
	length = strText.length();
	string strPat = "";

	string strLine, strEnt, cate;
	//int countPred = 0, countUnk=0;
	long int count= 0;
	//int posTok=0;

	getline(enjuF,strLine);	
	while(strLine.find("parse_status=\"success\"")==string::npos 
			&& strLine.find("parse_status=\"fragmental parse\"")==string::npos)
	{
		getline(enjuF,strLine);
		if(strLine == "")
			return;
		if(strLine.find("sentence id=")!=string::npos)
		{
			//fileOut << "Sentence" << stat.totalSent << endl;
			stat.totalSent++;
		}
	}
	
	getline(entF, strEnt);
	
	//ofstream out;
	do
	{	
		if(strLine.length() < 2) { 	break; }
	
		Sentence sent;
		sent.arrRelA.clear();
		//posTok = 0;
		
		while(strLine.find("parse_status=\"success\"")==string::npos 
			&& strLine.find("parse_status=\"fragmental parse\"")==string::npos)
		{
			getline(enjuF,strLine);
			if(strLine == "")
				return;
			if(strLine.find("sentence id=")!=string::npos)
			{
				//fileOut << "Sentence" << stat.totalSent << endl;
				stat.totalSent++;

			}
		}
	

		cate = sent.parse(strLine);

		do{ //loop on enju file
			getline(enjuF, strLine);
			
			if(strLine.length() < 1) break;
			Term term;
			term.parse(strLine);
			term.check = false;
			term.netag = "O";
			if(term.start == term.end && term.start==0) break;
			if(term.type == "tok")
			{
				sent.arrTok.push_back(term);
			}
			else
			{
				sent.arrTerm.push_back(term);
			}
			
			cate = term.type;
	
		}while(cate != "sentence");
		
//		int sGene, eGene;	
		Term preEnt;
		do{ // loop on entity file
			Term ent;
			int sGene, eGene;		
			do{				// this loop is used to skip entities that don't belong to any sentences in Enju output
				istringstream geneIs(strEnt.c_str());
				geneIs >> sGene >> eGene;
			} while (sGene < sent.start && getline(entF,strEnt));
			
			//bool res = ent.parseEnt(strEnt,sent.start, sent.end, 1);			//--> for gene
			//bool res = ent.parseGene(strEnt,sent.start,sent.end, true);			// --> for golden entity of PPI and DDI
			//bool res = ent.parseEnt(strEnt,sent.start, sent.end, 2);			//--> for drug
			bool res = ent.parseEnt(strEnt,sent.start, sent.end);			//--> for all
			
			if(res)
			{		
				if(ent.type!="NONE")
				{
					sent.arrEnt.push_back(ent);				
					preEnt = ent;
				}
			}
			else { break; }
			
		}while(getline(entF, strEnt));

		if(count++ % 100 ==0)
		{
			cerr << ".";
		}
		//cout << "***** Sentence " << sent.id << "\n";	
	
		std::sort(sent.arrEnt.begin(),sent.arrEnt.end());
		cout << "***" << "Sentence" << stat.totalSent << ": " <<strText.substr(sent.start, sent.end - sent.start) << endl;
		//sent.CheckRule(strText,stat); //temporarily close for UTF-8
		sent.PrintRel(strText,fileOut);
		stat.totalSent++;
	

	} while(cate == "sentence");
	
	
	cerr << endl;
		
	/*re = tp * 1.0 / (tp+fn); pre = tp * 1.0 /(tp+fp); fscore = 2*re*pre/(re+pre);
	cout << "True positive:" << tp << "; false positive:" << fp << "; false negative:" << fn << endl;
	cout << "Precision=" <<pre <<"; "<<"Recall=" << re << "; F-score=" << fscore << endl;

	fscore = countCover * 1.0 / (tp + fn);
	cout << "Covered relations:" << countCover << "(" << fscore << ")" << endl;*/


	entFile.close();	
	enjuFile.close();
	fileOut.close();
	//fEnt.close();
	
}

void func::createLinkLDAInput(string fileIn, map<string,string>& relHash, int& count)
{
	/*string test = "test \"testing\"";
	test = common::normalizeStr2(test);*/	
	
	std::ifstream in(fileIn.c_str(), std::ios_base::in);
	string strLine;
	while(getline(in,strLine))
	{
		//use#analyzed_by;assessed_for_effect_by;measured_by;	ABE	orch	glucose	lbpr
		count ++;
		vector<string> arrTerm = common::split(strLine+ "\t" , "\t");
		vector<string> arrRel = common::split(arrTerm[0] + ";", ";");
		for(int i=0; i< arrRel.size(); i++)
		{
			size_t pos = arrRel[i].find("  ");
			if(pos!=string::npos)
				arrRel[i] = arrRel[i].replace(pos,2," ");
			string strNormal = arrRel[i];
			int len = arrRel[i].length();
			while(len > 0 && arrRel[i][len-1] == ' ')
			{
				strNormal = strNormal.replace(len-1,1,"");
				len = strNormal.length();
			}
			arrRel[i] = strNormal;

			if(arrRel[i].length() < 1)
				continue;
			if(relHash.find(arrRel[i])==relHash.end())
			{
				relHash.insert(std::pair<string,string>(arrRel[i],arrTerm[1] + "\t" + arrTerm[2]));
			}
			else
			{
				relHash[arrRel[i]] += "#" + arrTerm[1] + "\t" + arrTerm[2];
			}
		}
		
	
	}



	

	in.close();
	//out.close();
}

void func::readSemRep(string fileIn, string fileOut)
{
	std::ifstream in(fileIn.c_str(), std::ios_base::in);
	std::ofstream out(fileOut.c_str(), std::ios_base::out);
	string strLine;
	int index = 0;
	while(getline(in,strLine))
	{
		vector<string> arrTerm = common::split(strLine + "|","|");
		if(strLine.find("|text|")!=string::npos)
		{
			if(arrTerm.size()!=7)
				continue;
			out << "Sentence" << index <<":" << arrTerm[6] << endl;
			index++;
		}
		else if(strLine.find("|relation|")!=string::npos)
		{
			if(arrTerm.size()!=41)
				continue;
			//has_part	103	neurons	C0027882	cell	150	transferrin	C1442762	gngm
			out << arrTerm[21] << "\t" << arrTerm[22] << "\t" << arrTerm[19] << "\t" << arrTerm[14] << "\t" <<arrTerm[8] <<"\t" << arrTerm[11] << "\t"
				<< arrTerm[39] << "\t" << arrTerm[34] << "\t" << arrTerm[28] << "\t" << arrTerm[31] << endl;
		}

	}

	in.close();
	out.close();
}
