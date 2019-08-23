#include "term.h"


Term::Term(void)
{
	
	start = end = 0;
	content = id = "";
	type = -1; check = false;
	netag = "O";
}
void Term::parse(string strLine)
{	
	istringstream enjuIs2(strLine.c_str());
	enjuIs2 >> start >> end >> type;	
	//secondTab = strLine.find(' ', firstTab	+ 1);

	size_t firstTab = strLine.find("id=\""); 
	size_t secondTab = strLine.find('\"', firstTab + 4);
	//cerr << "Use substr: Term::parse" << endl;
	id = strLine.substr(firstTab + 4, secondTab - firstTab - 4);
	
	//234	248	cons id="c0.17" cat="VP" xcat="" head="c0.18" sem_head="c0.18" schema="head_mod" lex_head="t0.7"
	content = strLine.substr(secondTab + 2);
	//tok id="t0.9" cat="P" pos="IN" base="in" lexentry="V[&lt;P&gt;NP.acc]" pred="prep_arg12" type="verb_mod" arg1="c0.20" arg2="c0.23"
	if(type=="tok")
	{
		firstTab = strLine.find("pos=\"");
		secondTab = strLine.find("\"", firstTab + 6);
		POS = strLine.substr(firstTab+5, secondTab - firstTab - 5);
		firstTab = strLine.find("base=\"");
		secondTab = strLine.find("\"", firstTab + 7);
		base = strLine.substr(firstTab+6, secondTab - firstTab - 6);
	}


}

bool Term::parseGene(string strGene, int sStart, int sEnd, bool gene)
{
	//188	203	Entity id="AIMed.d0.s1.e0"
	int sGene, eGene;
	istringstream geneIs(strGene.c_str());
	geneIs >> sGene >> eGene;
	if(sGene >= sStart && eGene <= sEnd )
	{
		start = sGene; end = eGene + 1; //gene.setType(3);
		size_t firstTab = strGene.find(" id=\""); 
		size_t secondTab = strGene.find('\"', firstTab + 5);
		//cerr << "Use substr: Term::parseEnt" << endl;
		id = strGene.substr(firstTab + 5, secondTab - firstTab - 5);
		
		firstTab = strGene.find(" type=\""); secondTab = strGene.find("\"", firstTab + 7);
		if(gene)
			type = "gene";
		else
			type = strGene.substr(firstTab+7, secondTab - firstTab - 7);

		if(strGene.length() > secondTab + 2)
		{
			content = strGene.substr(secondTab + 2);
		}
		return true;
	}
	return false;
}

bool Term::parseBiodivEnt(string strGene, int sStart, int sEnd) {
	vector<string> arrStr = common::split(strGene, "\t");
	if(arrStr.size() < 3) return false;
	id = arrStr[0];

	istringstream entIs(arrStr[1].c_str());
	int entStart, entEnd;
	string category;
	entIs >> category >> entStart >> entEnd;
	netag = category;

	if(entStart >= sStart && entEnd <= sEnd ){
		start = entStart; end = entEnd;
		return true;
	}


	return false;
}

bool Term::parseEnt(string strGene, int sStart, int sEnd) //type = 1 --> gene, type = 2 --> drug, type = 3 --> all
{
	//13	22	aapp id="0" cytokines
	int sGene, eGene, score;
	string eType, eId, eConcept;
	istringstream geneIs(strGene.c_str());	
	geneIs >> sGene >> eGene;
	vector<string> arrStr = common::split(strGene,"\t");
	if(arrStr.size() < 5)
		return false;
	eType = arrStr[3]; eConcept = arrStr[4]; 
	//>>eType >> eId >> eConcept; // --> for entities after all filter
	//geneIs >> sGene >> eGene; //--> for entities after filtering DIC
	//geneIs >> sGene >> eGene >> eConcept >> score >> eType; // --> for entities not filtered
	
	if(sGene >= sStart && eGene <= sEnd )
	{
		start = sGene; end = eGene; //gene.setType(3);
		size_t firstTab = strGene.find(" id=\""); 
		size_t secondTab = strGene.find('\"', firstTab + 5);
		//cerr << "Use substr: Term::parseEnt" << endl;
		//id = strGene.substr(firstTab + 5, secondTab - firstTab - 5);
		//id = eId; //--> for entities after filtering
		type = eType;
		
		/*if(strGene.length() > secondTab + 2)
		{
			content = strGene.substr(secondTab + 2);
		}*/

		size_t lastTab = strGene.rfind("\t");
		
		conceptID = eConcept;
		//content = strGene.substr(lastTab + 1);
		content = arrStr[2];
		
		//vector<string> arrTerm = common::split(strGene + "\t", "\t"); // --> for entities after filtering DIC
		//eType = arrTerm[3];
		//content = arrTerm[2];		
		//type = eType;
		//conceptID = arrTerm[4];

		//if(kind == 1 && eType.find("aapp")==string::npos && eType.find("amas")==string::npos  && eType.find("gngm")==string::npos && eType.find("enzy")==string::npos && eType.find("nnon")==string::npos
		//	&& eType.find("nusq")==string::npos)// && eType.find("celf")==string::npos  && eType.find("horm")==string::npos)
		//{
		//	type = "NONE";		
		//}
		//else if (kind == 2 && eType.find("antb")==string::npos && eType.find("horm")==string::npos && eType.find("hops")==string::npos && eType.find("imft")==string::npos && 
		//	eType.find("nsba")==string::npos && eType.find("orch")==string::npos && eType.find("phsu")==string::npos)
		//{
		//	type = "NONE";
		//
		//}
		//

		return true;
	}
	
	
	return false;
}

bool Term:: parseEnt2(string strIn, const string strText, const vector<Term>& arrTok, int& pos, std::map<string,int>& entDic, io::filtering_ostream& flog,
	int& total, int& filter)
{
	//this function is used to filter ent by POS
	if(strIn=="")
		return false;
	strIn = strIn + "\t";
	vector<string> arrItem = common::split(strIn,"\t");
	
	//311	316	C1292734	1000	ftcn	treat	Treatment intent	yes	no	'MTH','SNOMEDCT','NCI','CHV'
	//start, end, conceptID, score, sem, txtWord, preferred name, head, overMatch, sourceInfo
	string strEnt = arrItem[5], POS="", normalizeContent="", base="";
	conceptID = arrItem[2];
	type = arrItem[4];
	content = arrItem[5];

	if(strEnt[strEnt.length()-1]!=' ')
		strEnt += " ";
	start = atoi(arrItem[0].c_str());
	end = atoi(arrItem[1].c_str());
	if(end < arrTok[0].start || start > arrTok[arrTok.size()-1].end)
		return false;
	
	vector<string> arrEntTok = common::split(strEnt," ");
	pos = 0;
	cout << content << endl;

	/*if(content.find("cyclic nucleotide phosphodiesterase")!=string::npos)
	{
		content = content;
	}*/
	int tStart = start;
	//4 cases: left, right, middle, and correct
	for(int i=0; i < arrEntTok.size(); i++)
	{
		string strI = arrEntTok[i];
		while(pos < arrTok.size() && arrTok[pos].start < start && arrTok[pos].end < start)
			pos++;
		if(tStart >= arrTok[pos].start /*&& (arrTok[pos].end <= start + strI.length() || )*/)	//check the left boundary
		{
			while(pos < arrTok.size())
			{
				size_t pos1 = arrTok[pos].content.find("base=\"");
				if(pos1==string::npos)
				{
					pos ++;
					continue;
				}
				size_t pos2 = arrTok[pos].content.find("\"", pos1+6);
				base = arrTok[pos].content.substr(pos1+6, pos2 - pos1 - 6);
				string temp = strText.substr(arrTok[pos].start, arrTok[pos].end - arrTok[pos].start);
				if(strI.find(temp)!=string::npos || temp.find(strI)!=string::npos) //check substring
					break;
				pos ++;
			}
			//pos--;
			if(pos < arrTok.size())
			{
				POS += arrTok[pos].POS + " ";
				normalizeContent += base + " ";
				pos++;
			}
			//tStart += strI.length() + 1 ; // 1 = space
			if(pos < arrTok.size())
				tStart = arrTok[pos].start;
			else
				tStart += strI.length() + 1 ; // 1 = space
		}
	}
	

	

		if(POS.find("NN")==string::npos && POS.find("NNP")==string::npos && POS.find("NNS")==string::npos)
		{
			return true;
		}

	/*	if(common::tolowerStr(content)=="increase")	
			content = content;*/
		
		char strStart[20];
		sprintf(strStart,"%d", start);
		char strEnd[20];
		sprintf(strEnd, "%d", end);
		string strOut = string(strStart )+ "\t" + string(strEnd);
		normalizeContent = common::normalizeStr(normalizeContent);
		strOut += "\t" + content + "\t" + type + "\t" + conceptID + "\t" + normalizeContent + "\t" + arrItem[arrItem.size()-1] + "\n" ;
		flog.write(strOut.c_str(), strOut.length());

		//content = common::tolowerStr(content);
		if(entDic.find(normalizeContent)==entDic.end())
		{
			entDic.insert(std::pair<string,int>(normalizeContent, 1));
		}
		else 
		{
			entDic[normalizeContent] ++;
		}

		
		
		filter++;
		
		return true;
	//}

	//return false;
}
Term::~Term(void)
{
}

Relation::Relation(void)
{
	check = false;
	semType = name = "";
}

bool Relation::parse(string strLine, int sSent, int eSent)
{
	//5660	5794	GDA entity1="entity-0.1" entity2="disease0.22"
	istringstream is(strLine.c_str());
	is >> start >> end;
	if(start >=sSent && end <= eSent)
	{
		size_t sPos, ePos;
		//cerr << "Use substr: Relation::parse" << endl;
		sPos = strLine.find("entity1=\""); ePos = strLine.find("\"", sPos + 9);
//		arg1 = strLine.substr(sPos + 9, ePos - sPos - 9 );
		sPos = strLine.find("entity2=\""); ePos = strLine.find("\"", sPos + 9);
	//	arg2 = strLine.substr(sPos + 9, ePos - sPos - 9 );
		return true;
	}
	else { return false;}
}

Relation::~Relation(void)
{
}

vector<string> common::split(string strIn, string strDelimiter)
{
	vector<string> arrTerm;
	string::size_type pos1 = string::npos;
	string::size_type pos2 = strIn.find(strDelimiter, pos1+1);
	string temp;
	while(pos2!=string::npos)
	{
		temp = strIn.substr(pos1+1, pos2 - pos1 - 1);
		if(temp.length() > 0)
		{
			arrTerm.push_back(temp);
		}
		pos1 = pos2;
		pos2 = strIn.find(strDelimiter, pos1+1);
	}
	temp = strIn.substr(pos1+1);
	if(temp.length()>0)
	{
		if(temp[temp.length()-1] == 13)
		{
			temp = temp.substr(0,temp.length()-1);
		}
		arrTerm.push_back(temp);
	}
	return arrTerm;
	
}

vector<wstring> common::splitUTF8(wstring strIn, wstring strDelimiter)
{
	vector<wstring> arrTerm;
	string::size_type pos1 = string::npos;
	string::size_type pos2 = strIn.find(strDelimiter, pos1+1);
	wstring temp;
	while(pos2!=string::npos)
	{
		temp = strIn.substr(pos1+1, pos2 - pos1 - 1);
		if(temp.length() > 0)
		{
			arrTerm.push_back(temp);
		}
		pos1 = pos2;
		pos2 = strIn.find(strDelimiter, pos1+1);
	}
	temp = strIn.substr(pos1+1);	
	if(temp.length()>0)
	{
		if(temp[temp.length()-1] == 13)
		{
			temp = temp.substr(0,temp.length()-1);
		}
		arrTerm.push_back(temp);
	}
	return arrTerm;
	
}

string common::tolowerStr(string strIn)
{
	size_t pos1 = strIn.find("[");
	if(pos1!=string::npos)
	{
		size_t pos2 = strIn.find("]");
		if(pos2!=string::npos)
		{
			strIn = strIn.replace(pos1,pos2-pos1+1,"");
		}
	}
	pos1 = strIn.find("&#124;");
	if(pos1!=string::npos)
	{
		//strIn = strIn.replace(pos1, 6, "");
		strIn = strIn.replace(pos1, string::npos, "");
	}
	char c;
	string strOut = strIn;
	int i=0;
	while (i<strIn.length())
  {
    c=strIn[i];
    strOut[i] = tolower(c);
    i++;
  }
	return strOut;
}
string common::normalizeStr2(string str)
{
	size_t pos;
	try{
	while((pos=str.find("\""))!=string::npos)
		str = str.replace(pos,1,"&quot;");
	}catch(exception e)
	{
		cerr << e.what() << endl;
	}
	return str;
}
string common::normalizeStr(string str)
{
	string strNormal = str;
	while(strNormal.length() > 0 && strNormal[0] == ' ')
		strNormal = strNormal.replace(0,1,"");
	int len = strNormal.length();
	//delete double spaces
	while(len > 0 && strNormal[len-1] == ' ')
	{
		strNormal = strNormal.replace(len-1,1,"");
		len = strNormal.length();
	}
		
	size_t pos = strNormal.find(" ,");
	while(pos!=string::npos)
	{
		str = strNormal.replace(pos,1,"");
		pos = strNormal.find(" ,", pos + 2);
	}

	pos = strNormal.find(" '");
	while(pos!=string::npos)
	{
		str = strNormal.replace(pos,1,"");
		pos = strNormal.find(" '", pos + 2);
	}

	pos = strNormal.find(" %");
	while(pos!=string::npos)
	{
		str = strNormal.replace(pos,1,"");
		pos = strNormal.find(" %", pos + 2);
	}

	pos = strNormal.find(" .");
	while(pos!=string::npos)
	{
		str = strNormal.replace(pos,1,"");
		pos = strNormal.find(" .", pos + 2);
	}

	pos = strNormal.find(" ;");
	while(pos!=string::npos)
	{
		str = strNormal.replace(pos,1,"");
		pos = strNormal.find(" ;", pos + 2);
	}

	pos = strNormal.find(" :");
	while(pos!=string::npos)
	{
		str = strNormal.replace(pos,1,"");
		pos = strNormal.find(" :", pos + 2);
	}

	pos = strNormal.find(" \"");
	while(pos!=string::npos)
	{
		str = strNormal.replace(pos,1,"&quot;");
		pos = strNormal.find(" \"", pos + 2);
	}

	pos = strNormal.find("\" ");
	while(pos!=string::npos)
	{
		str = strNormal.replace(pos+1,1,"&quot;");
		pos = strNormal.find("\" ", pos + 2);
	}
	return strNormal;

}
