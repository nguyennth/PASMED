#include "sentence.h"
#include <ctype.h>

Sentence::Sentence()
{
	start = end = count = 0;
	numOfNPpairs = 0;
	id = "";	
}
Sentence::~Sentence(void)
{
	arrTerm.clear();
	arrEnt.clear();
	arrTok.clear();
	arrRel.clear();
	arrRelA.clear();
	//semNet.clear();
}

string Sentence::parse(string strLine)
{
	string cate;	
	istringstream enjuIs(strLine.c_str());
	enjuIs >> start >> end >> cate;		
	size_t firstTab = strLine.find("id=\""); 
	size_t secondTab = strLine.find('\"', firstTab + 4);
	//cerr << "Use substr: Sentence::parse" << endl;
	id = strLine.substr(firstTab + 4, secondTab - firstTab - 4);		
	
	return cate;
}


vector<Term>::size_type Sentence::lookUpTerm(const vector<Term>& arrT, const string& strId)
{
	for(vector<Term>::size_type i=0; i < arrT.size(); i++)
	{
		Term iTerm = arrT[i];
		if(iTerm.id == strId)
			return i;
	}
	return -1;
}

Term Sentence::lookUpHead(const string& strId)
{
	Term head;
	vector<Term>::size_type start = lookUpTerm(arrTerm, strId);
	if(start == -1)
		return head;
	int count = 1;
	head = arrTerm[start];
	string strHead = head.content;
	size_t pos1 = strHead.find("head=\"");
	size_t pos2 = strHead.find("\"", pos1+6);
	strHead = strHead.substr(pos1+6, pos2 - pos1 - 6);
	while(count < 3 /*|| strHead[0]=='c'*/) //look up the token
	{
		start = lookUpTerm(arrTerm, strHead); 
		if(start == -1)
			return head;
		head = arrTerm[start];
		strHead = head.content;
		pos1 = strHead.find("head=\"");
		pos2 = strHead.find("\"", pos1+6);
		strHead = strHead.substr(pos1+6, pos2 - pos1 - 6);
		count++;
	}
	if(strHead[0]=='c')
	{
		start = lookUpTerm(arrTerm, strHead); 
		return arrTerm[start];
	}
	else
	{
		start = lookUpTerm(arrTok, strHead); 
		return arrTok[start];
	}

	return head;
}
void Sentence::CheckEntity2(const Term& np1, const Term& verb, const Term& np2, const Term& np3)
{
	//this function is used to check coverage 
	int pos1, pos2;
	//for(vector<Relation>::iterator it= arrRel.begin(); it!=arrRel.end(); it++)
	for(size_t i = 0; i<arrRel.size(); i++)
	{
		Relation rel = (Relation)arrRel[i];
		if(rel.check==true)
			continue;
		/*if(rel.arg1 == "entity-0.16")
		{
			rel.arg1 = "entity-0.16";
		}*/
		/*size_t t1 = lookUpTerm(arrEnt,rel.arg1);
		size_t t2 = lookUpTerm(arrEnt,rel.arg2);
		if(t1==-1 || t2==-1)
		{
			if(t1==-1)
				cerr << id << "\t" << rel.arg1 << " doesn't exist!" << endl;
			if(t2==-1)
				cerr << id << "\t" << rel.arg2 << " doesn't exist!" << endl;
			continue;
		}*/
		pos1=0; pos2=0;
		Term e1 = arrEnt[rel.arg1], e2=arrEnt[rel.arg2];
		if(e1.start >= np1.start && e1.end <= np1.end) {pos1 = 1;}
		else if (e1.start >= verb.start && e1.end <= verb.end) {pos1 = 2;}
		else if (e1.start >= np2.start && e1.end <= np2.end) {pos1 = 3;}
		else if (e1.start >= np3.start && e1.end <= np3.end) {pos1 = 4;}
		
		if(pos1 == 0)
			continue;

		if(e2.start >= np1.start && e2.end <= np1.end) {pos2 = 1;} 
				else if (e2.start >= verb.start && e2.end <= verb.end) {pos2 = 2;}
				else if (e2.start >= np2.start && e2.end <= np2.end) {pos2 = 3;}
				else if (e2.start >= np3.start && e2.end <= np3.end) {pos2 = 4;}

		if(pos1!=0 && pos2!=0)//  && pos1!=pos2) //--> check expected covered
		{
			rel.check = true;
			arrRel[i].check = true;
			count++;
			//cout << "Relations:" << rel.arg1 << "\t" << rel.arg2 << endl;
			//	cout << "NP1:"  << np1.content << "\t"
			//		<< "VERB:" << verb.content << "\t"
			//////		<< "COOD:" << strCood << "\t"
			//<<"NP2:" << np2.content << "\t"
			//<< "NP3:" << np3.content << endl;
		}

	}
}
void Sentence::CheckEntity(const Term& np1, const Term& verb, const Term& np2,
		const Term& np3, const wstring& strText )
{
	vector<Relation> arrRel;
	int flag = 0;	
	int pos1 = 0, pos2 = 0;
	//for(vector<Term>::iterator it = arrEnt.begin(); it!=arrEnt.end(); it++)
	for(int i=0; i< arrEnt.size(); i++)
	{
		pos1 = 0;
		Term e1 = arrEnt[i];
		if(e1.start >= np1.start && e1.end <= np1.end) {pos1 = 1;}
		//else if (e1.start >= verb.start && e1.end <= verb.end) {pos1 = 2;}
		else if (e1.start >= np2.start && e1.end <= np2.end) {pos1 = 3;}
		else if (e1.start >= np3.start && e1.end <= np3.end) {pos1 = 4;}
		if(pos1==0)
			continue;
		
		//for(vector<Term>::iterator it1 = it+1; it1!=arrEnt.end(); it1++)
		for(int j=i+1; j< arrEnt.size(); j++)
		{
			pos2 = 0;
			Term e2 = arrEnt[j];
			if(e2.start >= np1.start && e2.end <= np1.end) {pos2 = 1;} 
			//else if (e2.start >= verb.start && e2.end <= verb.end) {pos2 = 2;}
			else if (e2.start >= np2.start && e2.end <= np2.end) {pos2 = 3;}
			else if (e2.start >= np3.start && e2.end <= np3.end) {pos2 = 4;}
			if(pos2!=0 && pos2!=pos1)// && e1.type!=e2.type) --> for ppi and ddi
			//if(pos2!=0 && pos2!=pos1 && e1.type!=e2.type) //--> for disease and gene associations
			{
				flag++;
				Relation rel;
				rel.arg1 = i; rel.arg2 = j; rel.start = start; rel.end = end; 
				if(pos2 == 3 || pos1 == 3)
					rel.name = verb.base;// + "/" + verb.POS;
				else if(pos2==4 || pos1 == 4)
					rel.name = verb.base + " " + np2.base;// + "/" + verb.POS ;
				if(CheckSemantic(rel) && InsertRelation(rel)) {
					arrRelA.push_back(rel);
				//arrRelA.push_back(rel);
				//string typeRel = e1.type + "_" + e2.type;
				//rel.name = typeRel;
					wcout << "Relation\t" << arrEnt[i].start << ":" << strText.substr(arrEnt[i].start, arrEnt[i].end - arrEnt[i].start) << "\t"
						<< arrEnt[j].start << ":" << strText.substr(arrEnt[j].start, arrEnt[j].end - arrEnt[j].start) << endl;
				}

			}
			
		}		

	}

	
}
bool Sentence::isInSemList(string strSem)
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

bool Sentence::CheckMetaRel(Relation& rel, std::map<string,string>& metaRel)
{
	string concept = arrEnt[rel.arg1].conceptID + "-" + arrEnt[rel.arg2].conceptID ;
//	string sem1 = arrEnt[rel.arg1].type + "#" +arrEnt[rel.arg2].type;
	//string sem2 = arrEnt[rel.arg2].type + ",";
	std::map<string,string>::iterator it = metaRel.find(concept);
	if(it!=metaRel.end())
	{
	//	rel.name = it ->second;
		//	rel.name = "NIL";
	//	rel.semType = sem1;
		/*if(CheckSemRel(rel,semNet) ==false) //close this one to use for biodiversity
			return false;*/
		if(InsertRelation(rel)){						
			arrRelA.push_back(rel);
			return true;
		}
		
	}
	return false;
}
bool Sentence::CheckSemRel(Relation& rel, std::map<string,string>& semNet)
{
	bool flag = false;
	string sem1 = arrEnt[rel.arg1].type + ",";
	string sem2 = arrEnt[rel.arg2].type + ",";
	vector<string> arrSem1 = common::split(sem1,",");
	vector<string> arrSem2= common::split(sem2,",");
	rel.name += "#";
	for(vector<string>::iterator i = arrSem1.begin(); i!=arrSem1.end(); i++)
	{
		sem1 = *i;
	/*	if(isInSemList(sem1)==false)
			continue;*/
		for(vector<string>::iterator j= arrSem2.begin(); j!=arrSem2.end(); j++)
		{
			sem2 = *j;
		/*	if(isInSemList(sem2)==false)
				continue;*/
			//Relation newRel = rel;
			//rel.name = "";
			std::map<string,string>::iterator it = semNet.find(sem1+"-"+sem2);
			if(it!=semNet.end())
			{
				rel.name += it->second + ";";
				rel.semType += sem1+"-"+sem2 + ";";
				flag = true;
			}
			else if((it=semNet.find(sem2+ "-" + sem1))!=semNet.end())
			{
				rel.name = it->second;
				rel.semType = sem2+ "-" + sem1;
				flag = true;
			}

			/*if(rel.name!="")
			{
				if(InsertRelation(rel)){				
					arrRelA.push_back(rel);
					cout << rel.start << "\t" << rel.end << "\t" << rel.name << "\t" << rel.semType << "\t" << arrEnt[rel.arg1].type << "=" << strText.substr(arrEnt[rel.arg1].start, arrEnt[rel.arg1].end - arrEnt[rel.arg1].start) 
						<< "\t" << arrEnt[rel.arg2].type << "=" << strText.substr(arrEnt[rel.arg2].start, arrEnt[rel.arg2].end - arrEnt[rel.arg2].start) << "\"" << endl;
				}
		
			}*/
			
		}
	}

	return flag;
}

bool Sentence:: InsertRelation(Relation rel)
{	
	bool flag = false;
	for(unsigned int i=0; i< arrRelA.size(); i++)
	{
		Relation t = arrRelA[i];
		if((t.arg1 == rel.arg1 && t.arg2 == rel.arg2)
			||(t.arg1 == rel.arg2 && t.arg2==rel.arg1))
		{
			/*string e11 = t.name.substr(0,t.name.find("-"));
			string e12 = t.name.substr(t.name.find("-")+1);
			string e21 = rel.name.substr(0,rel.name.find("-"));
			string e22 = rel.name.substr(rel.name.find("-")+1);
			if((e11==e21 && e12==e22) || (e11==e22 && e12==e21))
				return false;*/
			/*if(t.name == rel.name || t.name.find(rel.name)!=string::npos){
				if(rel.semType.find(t.semType)==string::npos)
					arrRelA[i].semType += "|" + rel.semType;
				return false;
			}
			else
			{
				arrRelA[i].name += ";" + rel.name;
				arrRelA[i].semType += ";" + rel.semType;
				return false;
			}*/

			//for occurrences only
			if(t.semType == rel.semType) return false;
		}
	}
	
	return true;
}

void Sentence::CheckEntity2(const Term& np1, const Term& verb, const Term& np2)
{
	//this function is used to check coverage 
	int pos1, pos2;
	for(size_t i=0; i< arrRel.size(); i++)
	{
		Relation rel = (Relation)arrRel[i];
		if(rel.check == true)
			continue;
		/*if(rel.arg1 == "entity-0.16")
		{
			rel.arg1 = "entity-0.16";
		}
*/
		/*size_t t1 = lookUpTerm(arrEnt,rel.arg1);
		size_t t2 = lookUpTerm(arrEnt,rel.arg2);
		if(t1==-1 || t2==-1)
		{B
			if(t1==-1)
				cerr << id << "\t" << rel.arg1 << " doesn't exist!" << endl;
			if(t2==-1)
				cerr << id << "\t" << rel.arg2 << " doesn't exist!" << endl;
			continue;
		}*/
		pos1=0; pos2=0;
		Term e1 = arrEnt[rel.arg1], e2=arrEnt[rel.arg2];
		
		if(e1.start >= np1.start && e1.end <= np1.end) {pos1 = 1;}
		else if (e1.start >= verb.start && e1.end <= verb.end) {pos1=2;}
		else if (e1.start >= np2.start && e1.end <= np2.end) {pos1 = 3;}
		
		if(pos1==0)
			continue;

		if(e2.start >= np1.start && e2.end <= np1.end) {pos2 = 1;} 
				else if (e2.start >= verb.start && e2.end <= verb.end) {pos2=2;}
				else if (e2.start >= np2.start && e2.end <= np2.end) {pos2 = 3;}
		
		if(pos1!=0 && pos2!=0)// && pos1!=pos2) //--> check expected covered --> don't check if pos1 and pos2 are the same
		{
			//rel.check = true;
			arrRel[i].check = true;
			count++;
			//cout << "Relations:" << rel.arg1 << "\t" << rel.arg2 << endl;
			/*cout << "NP1:"  << np1.content << "\t"
					<< "VERB:" << verb.content << "\t"
			////		<< "COOD:" << strCood << "\t"
			<<"NP2:" << np2.content << endl;*/
		
		}

	}
}

/*For biodiversity documents select relations between
 * Taxon, Habitat
 * Taxon, Person
 * Taxon, TempExp
 * Taxon, GeoLoc
 */

bool Sentence::CheckSemantic (Relation& rel) {
	string cat1 = arrEnt[rel.arg1].netag;
	string cat2 = arrEnt[rel.arg2].netag;
	if(cat1 == cat2) return false;

	//case study: Occurrences
	if(cat1 == "Taxon" && (cat2 == "Habitat" || cat2=="TemporalExpression"
			|| cat2 == "GeographicalLocation")) {
		rel.semType = "Occur";
		return true;
	}

	if(cat2 == "Taxon" && (cat1 == "Habitat" || cat1=="TemporalExpression"
			|| cat1 == "GeographicalLocation")) {
		rel.semType = "Occur";
		int temp = rel.arg1;
		rel.arg1 = rel.arg2;
		rel.arg2 = temp;
		return true;
	}

	if(cat1=="Taxon" && cat2 == "Person") {
		rel.semType = "SeenBy";
		return true;
	}

	if(cat2=="Taxon" && cat1 == "Person") {
		rel.semType = "SeenBy";
		int temp = rel.arg1;
		rel.arg1 = rel.arg2;
		rel.arg2 = temp;
		return true;
	}

	return false;
}

void Sentence::CheckEntity(const Term& np1, const Term& verb, const Term& np2, 
	const wstring& strText)
{	
	int flag = 0;	
	int pos1 = 0, pos2 = 0;
	signed int i=0, j=0;
	//for(vector<Term>::iterator it = arrEnt.begin(); it!=arrEnt.end(); it++)
	for(i=0; i< arrEnt.size(); i++)
	{
		pos1 = 0;
		Term e1 = arrEnt[i];
		if(e1.start >= np1.start && e1.end <= np1.end) {pos1 = 1;}
	//	else if (e1.start >= verb.start && e1.end <= verb.end) {pos1=2;}
		else if (e1.start >= np2.start && e1.end <= np2.end) {pos1 = 3;}
		if(pos1!=0)
		{
			//for(vector<Term>::iterator it1 = it+1; it1!=arrEnt.end(); it1++)
			for(j = i+1; j < arrEnt.size(); j++)
			{
				Term e2 = arrEnt[j];
				pos2 = 0;
				if(e2.start >= np1.start && e2.end <= np1.end) {pos2 = 1;} 
		//		else if (e2.start >= verb.start && e2.end <= verb.end) {pos2=2;}
				else if (e2.start >= np2.start && e2.end <= np2.end) {pos2 = 3;}
				
				if(pos2!=0 && pos2!=pos1) // --> for occurrences
			//	if(pos2!=0 && pos2!=pos1 && e1.type!=e2.type) //--> for disease and gene association
				{
					flag++;
					Relation rel;
					rel.arg1 = i; rel.arg2 = j; rel.start = start; rel.end = end;					
					rel.name = verb.base;//+ "/" + verb.POS;
					//arrRel.push_back(rel);
					/*string typeRel = e1.type + "_" + e2.type;
					rel.name = typeRel;*/
					//if(CheckMetaRel(rel)) --> for relations in PubMed
					if(CheckSemantic(rel) && InsertRelation(rel))
					{
						arrRelA.push_back(rel);
						wcout << "Relation\t" << arrEnt[i].start << ":" << strText.substr(arrEnt[i].start, arrEnt[i].end - arrEnt[i].start) << "\t"
						<< arrEnt[j].start << ":" << strText.substr(arrEnt[j].start, arrEnt[j].end - arrEnt[j].start) << endl;
		//				cout << start << "\t" << end << "\t" << typeRel << " entity1=\"" << strText.substr(e1.start, e1.end - e1.start) 
						//	<< "\" entity2=\"" << strText.substr(e2.start, e2.end - e2.start) << "\"" << endl;
						//cerr << "Use substr: Sentence::checkEntity" << endl;
						//cout << e1.type << ":" << strText.substr(e1.start, e1.end - e1.start) << "\t" 
						//	<< e2.type << ":" << strText.substr(e2.start, e2.end - e2.start) << endl;
					}
				}

				
			}
		}		
		
	}

}


bool Sentence::CheckRule3(vector<Term>::iterator it, const Term& termNP1, const wstring& strText,
	statistics& stat)
{	
	bool check1= false, check2 = false;
	Term prep, termNP2, cood, tok = *it, verb; //tok is the first main verb
	size_t pos, pos2;
	string argId;	

	
	for(vector<Term>::iterator cit = arrTok.begin(); cit!= arrTok.end(); cit++)
	{
		cood = *cit;
//		if(cood.start<= tok.start && cood.end <= tok.end)
		if(cood.end <= tok.start || cood.start <= tok.start)
			continue;
		//find coordination that has two arguments, one of them must be verb1 (tok)
		size_t posCood = cood.content.find("pred=\"coord_arg12\"");
		if(posCood==string::npos)
			continue;
		pos = cood.content.find("arg1=\"");		
		pos2 = cood.content.find("\"", pos + 6);
		argId = cood.content.substr(pos + 6, pos2 - pos - 6);
		if(argId == "unk")
		{
			continue;	
		}
		size_t vp = lookUpTerm(arrTerm, argId);
		if(vp == -1)
		{
			cerr<< "CheckRule3:Check source code or corpus!!!\n";
			return false;
		}		
		verb = (Term) arrTerm[vp];
		if(verb.content.find("cat=\"VP\"")==string::npos && verb.content.find("cat=\"VX\"")==string::npos) //-->this is not a verb or a verb phrase
			continue;
		//find head of this verb
		pos = verb.content.find("head=\"");		
		pos2 = verb.content.find("\"", pos + 6);
		argId = verb.content.substr(pos + 6, pos2 - pos - 6);
		if(argId[0]=='c')
		{
				vp = lookUpTerm(arrTerm, argId);
				verb = arrTerm[vp];
		}
		else {
			vp = lookUpTerm(arrTok, argId);
			verb = arrTok[vp];
		}

		wstring strVerb = strText.substr(tok.start, tok.end - tok.start);
		wstring strVerbPhrase = strText.substr(verb.start, verb.end - verb.start);
		
		if(strVerbPhrase.find(strVerb)==string::npos) //this verb or verb phrase is not verb1
			continue;
		
		//find arg2
		pos = cood.content.find("arg2=\"");		
		pos2 = cood.content.find("\"", pos + 6);
		argId = cood.content.substr(pos + 6, pos2 - pos - 6);
		if(argId == "unk")
		{
			continue;	
		}
		vp = lookUpTerm(arrTerm, argId);
		if(vp == -1)
		{
			cerr<< "CheckRule3:Check source code or corpus!!!\n";
			return false;
		}		
		verb = (Term) arrTerm[vp];
		if(verb.content.find("cat=\"VP\"")==string::npos && verb.content.find("cat=\"VX\"")==string::npos) //-->this is not a verb or a verb phrase
			continue;
		//find head of this verb
		pos = verb.content.find("head=\"");		
		pos2 = verb.content.find("\"", pos + 6);
		argId = verb.content.substr(pos + 6, pos2 - pos - 6);
		if(argId == "unk")
		{
			continue;	
		}
		if(argId[0]=='c')
			vp = lookUpTerm(arrTerm, argId);
		else vp = lookUpTerm(arrTok,argId);

		if(vp == -1)
		{
			cerr<< "CheckRule3:Check source code or corpus!!!\n";
			return false;
		}		
		if(argId[0]=='c')
			verb = (Term) arrTerm[vp];
		else verb = (Term)arrTok[vp];

		//find prep modifies this verb and an NP

		for(vector<Term>::iterator jt = it + 1; jt != arrTok.end(); jt++)
		{
			prep = *jt;
			if(prep.content.find("prep_arg12")==string::npos)
				continue;
			//find two arguments of this prep

			pos = prep.content.find("arg1=\"");		
			pos2 = prep.content.find("\"", pos + 6);
			argId = prep.content.substr(pos + 6, pos2 - pos - 6);
			if(argId == "unk")
			{
				continue;	
			}
			vp = lookUpTerm(arrTerm, argId);
			if(vp == -1)
			{
				cerr<< "CheckRule3:Check source code or corpus!!!\n";
				return false;
			}		
			const Term& verb2 = (Term) arrTerm[vp];
			if(verb2.content.find("cat=\"VP\"")==string::npos && verb2.content.find("cat=\"VX\"")==string::npos) //-->this is not a verb or a verb phrase
				continue;
			if(verb2.content.find("xcat=\"COOD\"")==string::npos) //prep must modify COOD
				continue;
			//find head of this verb
			pos = verb2.content.find("head=\"");		
			pos2 = verb2.content.find("\"", pos + 6);
			argId = verb2.content.substr(pos + 6, pos2 - pos - 6);
			if(argId == "unk")
			{
				continue;	
			}
			if(argId[0]=='c')
				vp = lookUpTerm(arrTerm, argId);
			else 
				vp = lookUpTerm(arrTok,argId);

			if(vp == -1)
			{
				cerr<< "CheckRule3:Check source code or corpus!!!\n";
				return false;
			}		
			Term head;
			if(argId[0]=='c')
				head = (Term) arrTerm[vp];
			else head = (Term) arrTok[vp];

			strVerb = strText.substr(verb.start, verb.end - verb.start);
			strVerbPhrase = strText.substr(head.start, head.end - head.start);

		if(strVerbPhrase.find(strVerb)==string::npos && strVerb.find(strVerbPhrase)==string::npos) //this verb or verb phrase is not verb2
			//if(strVerb.find(strVerbPhrase)==string::npos) //this verb or verb phrase is not verb2
				continue;
		
			//find arg2
			pos = prep.content.find("arg2=\"");		
			pos2 = prep.content.find("\"", pos + 6);
			argId = prep.content.substr(pos + 6, pos2 - pos - 6);
			if(argId == "unk")
			{
				continue;	
			}
			size_t np = lookUpTerm(arrTerm, argId);
			if(np == -1)
			{
				cerr<< "CheckRule3:Check source code or corpus!!!\n";
				return false;
			}		
			termNP2 = (Term) arrTerm[np];
			if(termNP2.content.find("cat=\"NP\"")==string::npos && termNP2.content.find("cat=\"NX\"")==string::npos) //-->this is not a noun or a noun phrase
				continue;
			else
			{
				wstring strNP1 = strText.substr(termNP1.start,termNP1.end - termNP1.start);
				wstring strNP2 = strText.substr(termNP2.start, termNP2.end - termNP2.start);

				//strNP1 += L" ";
				//strNP2 += L" ";

				//cout << "pattern 3:" << endl;
				//cerr << "Use substr: Sentence::checkRule1_5" << endl;
				/*cout << "NP1:"  << strText.substr(termNP1.start,termNP1.end - termNP1.start) << "\t"
					<< "VERB:" << strText.substr(tok.start,tok.end - tok.start) << "\t"
					<< "COOD:" << strText.substr(cood.start, cood.end - cood.start) + " " + strText.substr(verb2.start, verb2.end - verb2.start) << "\t"
					<<"Prep:" << strText.substr(prep.start,prep.end - prep.start) << "\t"
					<< "NP2:" << strText.substr(termNP2.start, termNP2.end - termNP2.start) << endl;*/
				if(common::splitUTF8(strNP1 + L" ",L" ").size() <= LIMIT_LENGTH &&
						common::splitUTF8(strNP2 + L" ", L" ").size() <= LIMIT_LENGTH )
				{
					cout << "pattern 3:" << endl;
					//cerr << "Use substr: Sentence::checkRule1_5" << endl;
					wcout << "NP1:"  << strNP1 << "\t"
						<< "VERB:" << strText.substr(tok.start,tok.end - tok.start) << "\t"
						<< "COOD:" << strText.substr(cood.start, cood.end - cood.start) << " " << strText.substr(verb2.start, verb2.end - verb2.start)
						<< "\t" <<"Prep:" << strText.substr(prep.start,prep.end - prep.start) << "\t"
						<< "NP2:" << strNP2 << endl;

					CheckEntity(termNP1,tok,termNP2,verb2,strText);

					stat.rule3 ++;
				
				}

				return true;
			}

		}
	}
	return false;

}
bool Sentence::CheckRule2(vector<Term>::iterator it, const Term& termNP1,
		const wstring& strText,	statistics& stat)
{
	//find PREP modifies VERB
	
	bool check1= false, check2 = false;
	Term prep, termNP2, tok = *it;
	size_t pos, pos2;
	string argId;
	for(vector<Term>::iterator nit = it+1; nit!= arrTok.end(); nit++)
	{
		prep = *nit;
		size_t posPrep = prep.content.find("pred=\"prep_arg12");
		if(posPrep==string::npos) //not PREP
		{			
				continue;			
		}
		check1 = false; check2=false;
		//check if this PREP modifies VERB and a NP
		pos = prep.content.find("pred=\"");
		pos2 = prep.content.find("\"", pos + 6);
		int flag = 0;
		while(prep.content[pos2 - 1] >= '1' && prep.content[pos2 - 1] <= '9')
		{				
			//cerr << "Use substr: Sentence::checkRule3 _1" << endl;
			pos = prep.content.find("arg"+ prep.content.substr(pos2-1,1)+"=\"");
			
			string::size_type pos3 = prep.content.find("\"", pos + 6);
			argId = prep.content.substr(pos + 6, pos3 - pos - 6);
			if(argId == "unk")
			{
				pos2--;
				continue;

			}
			size_t np = lookUpTerm(arrTerm, argId);
			if(np == -1)
			{
				cerr<< "CheckRule2:Check source code or corpus!!!\n";
				return false;
			}
			Term temp = (Term) arrTerm[np];
			if(temp.content.find("cat=\"NX\"")!=string::npos || temp.content.find("cat=\"NP\"")!=string::npos) //this is a NP 
			{				
			//	flag ++;
				check1 = true;
				termNP2 = temp;
			}
			else //if(temp.content.find(tok.id)!=string::npos) //this is a VERB or verb phrase which has a head of VERB
			{

				wstring strTemp = strText.substr(temp.start, temp.end - temp.start);
				wstring strVerb = strText.substr(tok.start, tok.end - tok.start);
				if(strTemp.find(strVerb)!=string::npos){check2 = true;} //this is a VERB or verb phrase which has a head of VERB
				//	flag ++;
				
			} 

			pos2--;

			if(check1 && check2)
			{

				break; // this sentence satifies rule 3 ; this break out of while loop only!!!
			}




		}
		//check if termNP1, tok, termNP2 contain entities?
		if(check1 && check2 )
		{
			wstring strNP2 = strText.substr(termNP2.start, termNP2.end - termNP2.start) ;
			wstring strNP1 = strText.substr(termNP1.start, termNP1.end - termNP1.start) ;

			strNP2 += L" ";
			strNP1 += L" ";
			//
			//cout << "pattern 2:" << endl;
			////cerr << "Use substr: Sentence::checkRule3 _2" << endl;
			//cout << "NP1:"  << strText.substr(termNP1.start,termNP1.end - termNP1.start) << "\t"
			//	<< "VERB:" << strText.substr(tok.start,tok.end - tok.start) << "\t"
			//	<<"Prep:" << strText.substr(prep.start,prep.end - prep.start) << "\t"
			//	<< "NP2:" <<strNP2 << endl;
			if(common::splitUTF8(strNP1,L" ").size() <= LIMIT_LENGTH
					&& common::splitUTF8(strNP2,L" ").size() <= LIMIT_LENGTH )
			{
				cout << "pattern 2:" << endl;
				//cerr << "Use substr: Sentence::checkRule3 _2" << endl;
				wcout << "NP1:"  << strNP1<< "\t"
					<< "VERB:" << strText.substr(tok.start,tok.end - tok.start) << "\t"
					<<"Prep:" << strText.substr(prep.start,prep.end - prep.start) << "\t"
					<< "NP2:" <<strNP2 << endl;
				Term temp = tok;
				temp.base += " "  + prep.base;
				//tok.base += " " + prep.base;
				CheckEntity(termNP1,temp,termNP2,strText);
		
				stat.rule2++;
				break;
			}
			
		//	stat.countCover += count;
			return true;
			/*if(arrRel.size() >0)
			{	
				cout << "rule 3." << endl;			
				return true;
			}*/
		}
	}
	return false;

}
bool Sentence::CheckRule1_3(vector<Term>::iterator it, Term termNP1, Term termPP, 
	const wstring& strText,statistics& stat)
{
	Term tok = *it; //--> main verb
	Term temp;
	/*if(tok.content.find("passive_verb")==string::npos)
		return false;*/
	if(termNP1.start > termPP.start)
	{
		temp.start = termPP.start; temp.end = termPP.end;
		termPP.start = termNP1.start; termPP.end = termNP1.end;
		termNP1.start = temp.start; termNP1.end = temp.end;
	}

	wstring strPP = strText.substr(termPP.start, termPP.end - termPP.start);
	
	if(strPP.find(L"by")!=0)
		return false;

	wstring strNP1 = strText.substr(termNP1.start, termNP1.end - termNP1.start);
	

	if(common::splitUTF8(strNP1 + L" ",L" ").size() <= LIMIT_LENGTH
			&& common::splitUTF8(strPP+ L" ",L" ").size() <= LIMIT_LENGTH )
	{
		cout << "pattern 1.3:" << endl;
		wcout << "NP1:" << strNP1 << "\t"
			<< "VERB: " << strText.substr(tok.start, tok.end - tok.start) << "\t"
			<< strPP << "\t" << endl;
	
		CheckEntity(termNP1, tok,termPP, strText);
		stat.rule1_3 ++;
	}

	//CheckEntity2(termNP1, tok, termPP);
	

	return true;
}

bool Sentence::CheckRule1_4(vector<Term>::iterator it, vector<Term>::size_type t1, 
	vector<Term>::size_type t2, const wstring& strText,statistics& stat)
{
	Term tok = *it;
	int tokStart = tok.start, tokEnd = tok.end;
	Term termNP1 = arrTerm[t1], termNP2 = arrTerm[t2], prep, termNP3;
	//Term prep, phrase;
	int count = 0;
	//check if termNP2 is an ADJP that ADJP <-- Prep --> NP
	//get head of adj phrase (termNP2)
	size_t pos1, pos2;
	pos1 = termNP2.content.find("head=\"");
	pos2 = termNP2.content.find("\"", pos1+6);
	string headId = termNP2.content.substr(pos1+6, pos2-pos1-6);
	size_t hp; 
	Term head;
	if(headId[0]=='c')
	{
		hp = lookUpTerm(arrTerm,headId);
		head = arrTerm[hp];
	}
	else{
			hp = lookUpTerm(arrTok, headId);
			head = arrTok[hp];
	}
	
	
	for(vector<Term>::size_type i=0; i < arrTok.size(); i++)
	{		
		prep = arrTok[i];
		if(prep.start < head.start)
			continue;
		if(prep.content.find("prep_arg12")==string::npos)
			continue;
		//find arg1
		pos1 = prep.content.find("arg1=\"");		
		pos2 = prep.content.find("\"", pos1 + 6);
		headId = prep.content.substr(pos1 + 6, pos2 - pos1 - 6);
		if(headId == "unk")
		{
			continue;	
		}
		hp = lookUpTerm(arrTerm, headId);		
		Term adj = (Term) arrTerm[hp];
		if(adj.content.find("cat=\"ADJP\"")==string::npos && adj.content.find("cat=\"ADJ\"")==string::npos) //-->this is not an adj or an adj phrase
				continue;
		wstring adj1 = strText.substr(head.start, head.end - head.start);
		wstring adj2 = strText.substr(adj.start, adj.end - adj.start);
		if(adj1.find(adj2)==string::npos)
			continue;
		
		//find arg2
		pos1 = prep.content.find("arg2=\"");		
		pos2 = prep.content.find("\"", pos1 + 6);
		headId = prep.content.substr(pos1 + 6, pos2 - pos1 - 6);
		if(headId == "unk")
		{
			continue;	
		}
		hp = lookUpTerm(arrTerm, headId);		
		termNP3 = arrTerm[hp];
		if(termNP3.content.find("cat=\"NX\"")==string::npos && termNP3.content.find("cat=\"NP\"")==string::npos)
			continue;
		else
		{
			wstring strNP1 = strText.substr(termNP1.start,termNP1.end - termNP1.start);
			wstring strNP3 = strText.substr(termNP3.start, termNP3.end - termNP3.start) ;
			if(common::splitUTF8(strNP1+L" ",L" ").size() <= LIMIT_LENGTH
					&& common::splitUTF8(strNP3+L" ",L" ").size() <= LIMIT_LENGTH )
			{
				cout << "pattern 1.4:" << endl;
				//cerr << "Use substr: Sentence::checkRule2 _1" << endl;
				wcout << "NP1:"  << strNP1 << "\t"
					<< "be \t"
					<< "ADJP:" << strText.substr(adj.start, adj.end - adj.start) << "\t"
					<< "PREP:" << strText.substr(prep.start, prep.end - prep.start) << "\t" 
					<< "NP2:" << strNP3 << "\t" <<endl; 
				
				//tok.base = "be " + strText.substr(adj.start, adj.end - adj.start) + " " + prep.base;
				tok.base = "be ADJ " + prep.base;
				CheckEntity(termNP1,tok,termNP3,strText);
				
				stat.rule1_4 ++;
			}
			

		//	CheckEntity2(termNP1,tok,termNP3);
			
			return true;
		}
	}		
	
	return false;
}
bool Sentence::CheckRule1(vector<Term>::iterator it, const Term& termNP1, Term& termNP2, 
	const wstring& strText, statistics& stat)
{
	
	Term tok = *it;
	//find PREP modifies VERB
	bool check1= false, check2 = false, check=false;
	vector<Relation> arrRel;
	Term prep, termNP3;
	//Term tok = *it;
	size_t pos, pos2;
	string argId;
	for(vector<Term>::iterator nit = it+1; nit!= arrTok.end(); nit++)
	{
		prep = *nit;
		size_t posPrep = prep.content.find("pred=\"prep_arg12");
		if(posPrep==string::npos || prep.check == true) //not PREP
		{			
				continue;			
		}			
		check1 = false; check2=false;
		//check if this PREP modifies VERB and a NP
		pos = prep.content.find("pred=\"");
		pos2 = prep.content.find("\"", pos + 6);
		int flag = 0;
		while(prep.content[pos2 - 1] >= '1' && prep.content[pos2 - 1] <= '9')
		{				
			//cerr << "Use substr: Sentence::checkRule2 _1" << endl;
			pos = prep.content.find("arg"+ prep.content.substr(pos2-1,1)+"=\"");
			
			string::size_type pos3 = prep.content.find("\"", pos + 6);
			argId = prep.content.substr(pos + 6, pos3 - pos - 6);
			if(argId == "unk")
			{
				pos2--;
				continue;

			}
			size_t np = lookUpTerm(arrTerm, argId);
			if(np == -1)
			{
				cerr<< "CheckRule1: Check source code or corpus!!!\n";
				return false;
			}
			Term temp = (Term) arrTerm[np];
			if(temp.content.find("cat=\"NX\"")!=string::npos || temp.content.find("cat=\"NP\"")!=string::npos) //this is a NP 
			{				
			//	flag ++;
				check1 = true;
				termNP3 = temp;
			}
			else 
			{
				wstring strTemp = strText.substr(temp.start, temp.end - temp.start);
				wstring strVerb = strText.substr(tok.start, tok.end - tok.start);
				wstring strNP2 = strText.substr(termNP2.start, termNP2.end - termNP2.start);
				//if(temp.content.find(tok.id)!=string::npos //this prep modifies a VERB or verb phrase that has a head of VERB --> rule 2.2
				//|| temp.content.find(termNP2.id)!=string::npos)  // this prep modifies NP2 --> rule 2.1
				if(strTemp.find(strVerb)!=string::npos || strTemp.find(strNP2)!=string::npos){check2 = true;}
			//	flag ++;
			//	check2 = true;
			}

			pos2--;

			if(check1 && check2)
			{
				check = true;
				break; // this sentence satifies rule 1 ; this break out of while loop only!!!
			}




		}
		////check if termNP1, tok, termNP2 contain entities?
		
			
		if(check1 && check2 )
		{
			wstring strNP1 = strText.substr(termNP1.start,termNP1.end - termNP1.start) ;
			wstring strNP2 = strText.substr(termNP2.start,termNP2.end - termNP2.start) ;
			wstring strNP3 = strText.substr(termNP3.start, termNP3.end - termNP3.start);
			
			//cout << "pattern 1.2:" << endl;
			//////////cerr << "Use substr: Sentence::checkRule2 _1" << endl;
			//cout << "NP1:"  << strText.substr(termNP1.start,termNP1.end - termNP1.start) << "\t"
			//	<< "VERB:" << strText.substr(tok.start,tok.end - tok.start) << "\t"
			//	<< "NP2:" << strNP2
			//  << "Prep:" << strText.substr(prep.start,prep.end - prep.start) << "\t"
			//	<< "NP3:" << strText.substr(termNP3.start, termNP3.end - termNP3.start) << endl;						
			if(common::splitUTF8(strNP1+L" ",L" ").size() <= LIMIT_LENGTH
					&& common::splitUTF8(strNP3+L" ",L" ").size() <= LIMIT_LENGTH)
			{
				cout << "pattern 1.2:" << endl;
				////////cerr << "Use substr: Sentence::checkRule2 _1" << endl;
				wcout << L"NP1:"  << strNP1 << L"\t"
					<< L"VERB:" << strText.substr(tok.start,tok.end - tok.start) << L"\t"
					<< L"NP2:" << strNP2 << L"\t"
					<< L"Prep:" << strText.substr(prep.start,prep.end - prep.start) << L"\t"
					<< L"NP3:" << strNP3 << endl;
				
				//termNP2.base = termNP2.base + " " + prep.base;
				CheckEntity(termNP1,tok,termNP2,termNP3,strText);
				//break;
			}

			//stat.rule2 ++;
		//	CheckEntity2(termNP1,tok,termNP3);
		//	CheckEntity(termNP1,tok,termNP3,strText);
			//	stat.countCover += count;
				//return true;
				//cout << "rule 2." << endl;
			
			//return true;
		}		
	}
	
	if(check==false)
	{
		wstring strNP2 = strText.substr(termNP2.start, termNP2.end - termNP2.start) ;
		wstring strNP1 = strText.substr(termNP1.start,termNP1.end - termNP1.start) ;

		/*cout << "pattern 1:" << endl;
		cout << "NP1:"  << strText.substr(termNP1.start,termNP1.end - termNP1.start) << "\t"
		<< "VERB:" << strText.substr(tok.start,tok.end - tok.start) << "\t"
		<< "NP2:" << strNP2 << endl; */

		if(common::splitUTF8(strNP1 + L" ",L" ").size() <= LIMIT_LENGTH
				&& common::splitUTF8(strNP2 + L" ",L" ").size() <= LIMIT_LENGTH )
		{			
			cout << "pattern 1:" << endl;
			////cerr << "Use substr: Sentence::checkRule2 _1" << endl;
			wcout << "NP1:"  << strNP1 << "\t"
					<< "VERB:" << strText.substr(tok.start,tok.end - tok.start) << "\t"
				<< "NP2:" << strNP2 << endl; 
			CheckEntity(termNP1,tok,termNP2,strText);
		}
		stat.rule1 ++;		
		
	}
	
	return true;

}
bool Sentence::CheckTransitive(vector<Term>::iterator it,
		const wstring& strText, statistics& stat)
{
	Term tok = *it;
	size_t pos = tok.content.find("arg1=\""); 
	//cerr << "Use substr: Sentence::checkRule" << endl;
	size_t pos3 = tok.content.find("\"", pos + 6);
	string argId = tok.content.substr(pos + 6, pos3 - pos - 6);
	if(argId == "unk")			
	{
		cerr << "CheckTransitive: Check source code!!" << endl;
		return false; 
	}
	vector<Term>::size_type t1 = lookUpTerm(arrTerm, argId);
	if(t1 == -1)
	{
		cerr<< "CheckTransitive: Check source code or corpus!!!\n";
		return false;
	}
	Term termNP1 = (Term) arrTerm[t1]; //NP 

	//because arguments are not given in order ...
	int count = 0;
	if(termNP1.content.find("cat=\"NX\"")!=string::npos || termNP1.content.find("cat=\"NP\"")!=string::npos) //VERB doens't modify NP
	{
		count ++; 
	}
	else if(termNP1.content.find("cat=\"ADJP\"")!=string::npos)
	{
		count +=4;
	}		
	else if(termNP1.content.find("cat=\"PP\"")!=string::npos)
	{
		count+=3;
	}
	else return false;
	
	//check argument 2
	pos = tok.content.find("arg2=\""); 
	pos3 = tok.content.find("\"", pos + 6);
	//cerr << "Use substr: Sentence::checkRule" << endl;
	argId = tok.content.substr(pos + 6, pos3 - pos - 6);
	if(argId == "unk")			
	{
		cerr << "CheckTransitive: Check source code!!" << endl;
		return false; 
	}
	vector<Term>::size_type t2 = lookUpTerm(arrTerm, argId);
	if(t2 == -1)
	{
		cerr<< "CheckTransitive: Check source code or corpus!!!\n";
		return false;
	}
	Term termNP2 = (Term) arrTerm[t2]; //NP 

	if(termNP2.content.find("cat=\"NX\"")!=string::npos || termNP2.content.find("cat=\"NP\"")!=string::npos )
		count++;			
	else if(termNP2.content.find("cat=\"ADJP\"")!=string::npos)
		count +=4;
	else if(termNP2.content.find("cat=\"PP\"")!=string::npos)
		count +=3;
	else return false;
	
	
	if(count==2)
		return CheckRule1(it,termNP1,termNP2, strText,stat); //check for rule 1
	else if (count==5)
		return CheckRule1_4(it,t1,t2,strText,stat);
	else if(count==4)
		return CheckRule1_3(it,termNP1, termNP2,strText,stat);
	
	return false;
}
bool Sentence::CheckIntransitive(vector<Term>::iterator it, const wstring& strText,statistics& stat)
{
	Term tok = *it;
	size_t pos = tok.content.find("arg1=\""); 
	size_t pos3 = tok.content.find("\"", pos + 6);
	string argId = tok.content.substr(pos + 6, pos3 - pos - 6);
	if(argId == "unk")						
	{
		if((pos = tok.content.find("arg2=\""))!=string::npos)
		{
			pos3 = tok.content.find("\"", pos + 6);
			argId = tok.content.substr(pos + 6, pos3 - pos - 6);
			if(argId=="unk")				
				return false; 
		}
	
	}
	vector<Term>::size_type t = lookUpTerm(arrTerm, argId);
	if(t == -1)
	{
		cerr<< "CheckIntransitive: Check source code or corpus!!!\n";
		return false;
	}
	Term termNP1 = (Term) arrTerm[t]; //NP 
	if(termNP1.content.find("cat=\"NX\"")!=string::npos || termNP1.content.find("cat=\"NP\"")!=string::npos) //VERB modifies NP
	{
		//check rule 2 first
		if(CheckRule2(it,termNP1,strText,stat)==false)
		{			
			//check for rule 3, find a COOR modifies VERB
			return CheckRule3(it,termNP1,strText, stat);
		}
		else return true;
	}
	return false;
}

bool Sentence::CheckRule(const wstring& strText, statistics& stat)
{
	//detect verb --> check if it modifies NP

	for(vector<Term>::iterator it = arrTok.begin(); it!= arrTok.end(); it++)
	{
		Term curTok = *it;
		/*size_t posArg = curTok.content.find("verb_arg");
		if(posArg == string::npos)
		{
			posArg = curTok.content.find("verb_mod_arg");
			if(posArg==string::npos) 			continue;
		}*/
		if(curTok.content.find("verb_arg")==string::npos && curTok.content.find("verb_mod_arg")==string::npos && curTok.content.find("prep_arg")==string::npos)
			continue;
		if(curTok.content.find("pred=\"verb_arg12\"")!=string::npos || curTok.content.find("pred=\"verb_mod_arg12\"")!=string::npos) //transitive verb
		{
			if(curTok.content.find("unk")==string::npos)
			{
				if(CheckTransitive(it, strText, stat)==false) continue;
				else it->check = true;
			}
			else
			{
				if(CheckIntransitive(it,strText, stat)==false)
					continue;
				else
				{
				//	cout << "Unknown Arg" << endl;
					stat.rule2_1++;
					it->check = true;
				}
			}
		}
		else if(curTok.content.find("pred=\"verb_arg1\"")!=string::npos && curTok.content.find("unk")==string::npos) // instransitive verb
		{
			if(CheckIntransitive(it,strText,stat)==false) continue;
			else it->check = true;
		}
		else if(curTok.content.find("pred=\"prep_arg12\"")!=string::npos && curTok.content.find("unk")==string::npos) // prep
		{
			if(CheckRule4(it,strText,stat)==false) continue;
			else it->check = true;
		}
	}

	

	return false;
}

bool Sentence::CheckRule4(vector<Term>::iterator it, const wstring& strText,
		statistics& stat)
{
	Term prep = *it; //--> prep
	//find two NPs modified by prep
	bool check1= false, check2 = false, check=false;
	vector<Relation> arrRel;
	Term termNP1, termNP2;
	size_t pos = prep.content.find("pred=\"prep_arg12");
	if(pos==string::npos) //not PREP
	{			
		return false;			
	}
	size_t pos1 = prep.content.find("arg1=\"");		
	size_t pos2 = prep.content.find("\"", pos1 + 6);
	string argId = prep.content.substr(pos1 + 6, pos2 - pos1 - 6);
	pos = lookUpTerm(arrTerm,argId);
	if(pos==-1)
		return false;
	termNP1 = arrTerm[pos];
	if(termNP1.content.find("cat=\"NP\"")==string::npos && termNP1.content.find("cat=\"NX\"")==string::npos ) //not a NP
		return false;
	pos1 = prep.content.find("arg2=\"");		
	pos2 = prep.content.find("\"", pos1 + 6);
	argId = prep.content.substr(pos1 + 6, pos2 - pos1 - 6);
	pos = lookUpTerm(arrTerm,argId);
	if(pos==-1)
		return false;
	termNP2 = arrTerm[pos];
	if(termNP2.content.find("cat=\"NP\"")==string::npos && termNP2.content.find("cat=\"NX\"")==string::npos ) //not a NP
		return false;
	
	wstring strNP1 = strText.substr(termNP1.start,termNP1.end - termNP1.start);
	wstring strNP2 = strText.substr(termNP2.start, termNP2.end - termNP2.start);

	if(common::splitUTF8(strNP1+L" ",L" ").size() <= LIMIT_LENGTH &&
			common::splitUTF8(strNP2+L" ",L" ").size() <= LIMIT_LENGTH)
	{
		cout << "pattern 4:" << endl;
	//cerr << "Use substr: Sentence::checkRule2 _1" << endl;
		wcout << L"NP1:"  << strNP1 << L"\t"
				<< L"PREP:" << strText.substr(prep.start, prep.end - prep.start) << L"\t"
				<< L"NP2:" << strNP2 << L"\t" <<endl;
		
	//CheckEntity2(termNP1,prep,termNP2);
		CheckEntity(termNP1,prep,termNP2,strText);
	
		stat.rule4 ++;
	}

	
	return true;
}
void Sentence::Evaluate(statistics& stat)
{
	//arrRel: relations extracted by ee tools or human
	//arrRelA: relations extracted by patterns
	//evaluate precision, recall and f-score of arrRel and arrRelA
	int countPos = 0;
	if(arrRel.size()==0)
	{
		stat.falsepos += arrRelA.size();
		return;
	}
	if(arrRelA.size()==0)
	{
		stat.falseneg += arrRel.size();
		return;
	}
	for(vector<Relation>::iterator it = arrRelA.begin(); it!=arrRelA.end(); it++)
	{
		Relation relI = *it;
		for(vector<Relation>::iterator jt=arrRel.begin(); jt!=arrRel.end(); jt++)
		{
			Relation relJ=*jt;
			if((relI.arg1==relJ.arg1 && relI.arg2==relJ.arg2)||
				(relI.arg1==relJ.arg2 && relI.arg2 == relJ.arg1))
			{
				countPos++;
			}
		}
	}

	//if(countPos != count)
	//{
	//	cout << "Check count!" << endl;
	//	cerr << id << " Check count!" << endl;
	//}

	stat.truepos += countPos;
	stat.falsepos += arrRelA.size() - countPos;
	stat.falseneg += arrRel.size() - countPos;
	stat.countCover += count;
}

void Sentence::PrintOccurence(const wstring& strText, std::ofstream& out, int& index)
{
	if(arrRelA.size()==0)
		cout << "No relations" << endl;

	for(int i=0; i< arrRelA.size(); i++)
	{
		Relation rel = arrRelA[i];
		out << "R" << index+1 << "\t" << rel.semType
				<< " Arg1:" << arrEnt[rel.arg1].id << " Arg2:" << arrEnt[rel.arg2].id
				<< endl;
		index++;
		/*out << rel.name
			<< "\t" << arrEnt[rel.arg1].id
			<< "\t" << arrEnt[rel.arg1].netag
			<< "\t" << arrEnt[rel.arg2].id
			<< "\t" << arrEnt[rel.arg2].netag
			<< endl;*/

	}
}


void Sentence::PrintRel(const string& strText, std::ofstream& out)
{
	if(arrRelA.size()==0)
		cout << "No relations" << endl;
	for(int i=0; i< arrRelA.size(); i++)
	{
		Relation rel = arrRelA[i];
		
		out << rel.name
		 	<< "\t" << arrEnt[rel.arg1].start - rel.start << "\t"
			<< strText.substr(arrEnt[rel.arg1].start, arrEnt[rel.arg1].end - arrEnt[rel.arg1].start) << "\t"
		//	<< arrEnt[rel.arg1].conceptID
			<< "\t" << arrEnt[rel.arg1].type << "\t"
			<< arrEnt[rel.arg2].start - rel.start << "\t" 
			<< strText.substr(arrEnt[rel.arg2].start, arrEnt[rel.arg2].end - arrEnt[rel.arg2].start) << "\t"
		//	<< arrEnt[rel.arg2].conceptID
			<< "\t" << arrEnt[rel.arg2].type << endl;

		/*out << rel.name << "\t"
			<< strText.substr(arrEnt[rel.arg1].start, arrEnt[rel.arg1].end - arrEnt[rel.arg1].start) << "\t"
		//	<< arrEnt[rel.arg1].type << "\t"
			<< strText.substr(arrEnt[rel.arg2].start, arrEnt[rel.arg2].end - arrEnt[rel.arg2].start) 
			//	<< "\t" << arrEnt[rel.arg2].type 
			<< endl;*/

		/* //print to console
		cout << rel.name << "\t" << arrEnt[rel.arg1].start - rel.start << "\t" 
			<< strText.substr(arrEnt[rel.arg1].start, arrEnt[rel.arg1].end - arrEnt[rel.arg1].start) << "\t"
			<< arrEnt[rel.arg1].conceptID << "\t" << arrEnt[rel.arg1].type << "\t"
			<< arrEnt[rel.arg2].start - rel.start << "\t" 
			<< strText.substr(arrEnt[rel.arg2].start, arrEnt[rel.arg2].end - arrEnt[rel.arg2].start) << "\t"
			<< arrEnt[rel.arg2].conceptID << "\t" << arrEnt[rel.arg2].type << endl;*/
		//out << rel.start << "\t" << rel.end << "\t" /*<< rel.name << "\t" */<< rel.name << "\t" << arrEnt[rel.arg1].conceptID 
		//	<< "\t" << arrEnt[rel.arg2].conceptID << "\t" << rel.semType << "\t" << strText.substr(arrEnt[rel.arg1].start, arrEnt[rel.arg1].end - arrEnt[rel.arg1].start) 
		//	<< "\t"  /*<< "=" */<< strText.substr(arrEnt[rel.arg2].start, arrEnt[rel.arg2].end - arrEnt[rel.arg2].start) << endl;
		////cout << rel.start << "\t" << rel.end << "\t" /*<< rel.name << "\t" */<< rel.name << "\t" << arrEnt[rel.arg1].conceptID 
		//	<< "\t" << arrEnt[rel.arg2].conceptID << "\t" << rel.semType << "\t" << strText.substr(arrEnt[rel.arg1].start, arrEnt[rel.arg1].end - arrEnt[rel.arg1].start) 
		//	<< "\t"  /*<< "=" */<< strText.substr(arrEnt[rel.arg2].start, arrEnt[rel.arg2].end - arrEnt[rel.arg2].start) << endl;
	}
}
void Sentence::AssignNETag()
{
	int iTok=0, iEnt=0;

	if(arrEnt.size()==0)
		return;

	while(iEnt < arrEnt.size())
	{
		/*if(arrEnt[iEnt].start == 10079)
			iEnt = iEnt;*/
		while(iTok < arrTok.size() && arrEnt[iEnt].start > arrTok[iTok].start && arrEnt[iEnt].end > arrTok[iTok].end)
			iTok++;
		
		//ent.start = tok.start
		if(iTok < arrTok.size() && arrEnt[iEnt].start == arrTok[iTok].start) 
		{
			//if(arrEnt[iEnt].end==arrTok[iTok].end || arrEnt[iEnt].end > arrTok[iTok].end)
				arrTok[iTok].netag = "B-ENT";
			iTok ++;
			while(iTok < arrTok.size() && arrEnt[iEnt].end >= arrTok[iTok].end)
			{
				
				arrTok[iTok].netag = "I-ENT";				
				iTok++;
			}
		}
		
		
		iEnt++;
	}
}
void Sentence::PrintEnt(const string& strText, std::ofstream& out)
{
	for(int i=0; i< arrTok.size(); i++)
	{
		out << strText.substr(arrTok[i].start, arrTok[i].end - arrTok[i].start) << " " << arrTok[i].netag << endl;
	}
	out << endl;
}


bool Sentence::eCheckRule(const string& strText, statistics& stat, io::filtering_ostream& os)
{
	for(vector<Term>::iterator it = arrTok.begin(); it!= arrTok.end(); it++)
	{
		Term curTok = *it;
		/*size_t posArg = curTok.content.find("verb_arg");
		if(posArg == string::npos)
		{
			posArg = curTok.content.find("verb_mod_arg");
			if(posArg==string::npos) 			continue;
		}*/
		if(curTok.content.find("verb_arg")==string::npos && curTok.content.find("verb_mod_arg")==string::npos && curTok.content.find("prep_arg")==string::npos)
			continue;
		if(curTok.content.find("pred=\"verb_arg12\"")!=string::npos || curTok.content.find("pred=\"verb_mod_arg12\"")!=string::npos) //transitive verb
		{
			if(curTok.content.find("unk")==string::npos)
			{
				if(eCheckTransitive(it, strText, stat,os)==false) continue;
				else it->check = true;
			}
			else
			{
				if(eCheckIntransitive(it,strText, stat,os)==false)
					continue;
				else
				{
				//	cout << "Unknown Arg" << endl;
					stat.rule2_1++;
					it->check = true;
				}
			}
		}
		else if(curTok.content.find("pred=\"verb_arg1\"")!=string::npos && curTok.content.find("unk")==string::npos) // instransitive verb
		{
			if(eCheckIntransitive(it,strText,stat,os)==false) continue;
			else it->check = true;
		}
		else if(curTok.content.find("pred=\"prep_arg12\"")!=string::npos && curTok.content.find("unk")==string::npos) // prep
		{
			if(eCheckRule4(it,strText,stat,os)==false) continue;
			else it->check = true;
		}
	}
	return false;
}

bool Sentence::eCheckIntransitive(vector<Term>::iterator it, const string& strText, statistics& stat,io::filtering_ostream& os)
{
	Term tok = *it;
	size_t pos = tok.content.find("arg1=\""); 
	size_t pos3 = tok.content.find("\"", pos + 6);
	string argId = tok.content.substr(pos + 6, pos3 - pos - 6);
	if(argId == "unk")						
	{
		if((pos = tok.content.find("arg2=\""))!=string::npos)
		{
			pos3 = tok.content.find("\"", pos + 6);
			argId = tok.content.substr(pos + 6, pos3 - pos - 6);
			if(argId=="unk")				
				return false; 
		}
	
	}
	vector<Term>::size_type t = lookUpTerm(arrTerm, argId);
	if(t == -1)
	{
		cerr<< "CheckIntransitive: Check source code or corpus!!!\n";
		return false;
	}
	Term termNP1 = (Term) arrTerm[t]; //NP 
	if(termNP1.content.find("cat=\"NX\"")!=string::npos || termNP1.content.find("cat=\"NP\"")!=string::npos) //VERB modifies NP
	{
		//check rule 2 first
		if(eCheckRule2(it,termNP1,strText,stat,os)==false)
		{			
			//check for rule 3, find a COOR modifies VERB
			return eCheckRule3(it,termNP1,strText,stat,os);				
		}
		else return true;
	}
	return false;
}
bool Sentence::eCheckTransitive(vector<Term>::iterator it, const string& strText, statistics& stat, io::filtering_ostream& os)
{
	Term tok = *it;
	size_t pos = tok.content.find("arg1=\""); 
	//cerr << "Use substr: Sentence::checkRule" << endl;
	size_t pos3 = tok.content.find("\"", pos + 6);
	string argId = tok.content.substr(pos + 6, pos3 - pos - 6);
	if(argId == "unk")			
	{
		cerr << "CheckTransitive: Check source code!!" << endl;
		return false; 
	}
	vector<Term>::size_type t1 = lookUpTerm(arrTerm, argId);
	if(t1 == -1)
	{
		cerr<< "CheckTransitive: Check source code or corpus!!!\n";
		return false;
	}
	Term termNP1 = (Term) arrTerm[t1]; //NP 

	//because arguments are not given in order ...
	int count = 0;
	if(termNP1.content.find("cat=\"NX\"")!=string::npos || termNP1.content.find("cat=\"NP\"")!=string::npos) //VERB doens't modify NP
	{
		count ++; 
	}
	else if(termNP1.content.find("cat=\"ADJP\"")!=string::npos)
	{
		count +=4;
	}		
	else if(termNP1.content.find("cat=\"PP\"")!=string::npos)
	{
		count+=3;
	}
	else return false;
	
	//check argument 2
	pos = tok.content.find("arg2=\""); 
	pos3 = tok.content.find("\"", pos + 6);
	//cerr << "Use substr: Sentence::checkRule" << endl;
	argId = tok.content.substr(pos + 6, pos3 - pos - 6);
	if(argId == "unk")			
	{
		cerr << "CheckTransitive: Check source code!!" << endl;
		return false; 
	}
	vector<Term>::size_type t2 = lookUpTerm(arrTerm, argId);
	if(t2 == -1)
	{
		cerr<< "CheckTransitive: Check source code or corpus!!!\n";
		return false;
	}
	Term termNP2 = (Term) arrTerm[t2]; //NP 

	if(termNP2.content.find("cat=\"NX\"")!=string::npos || termNP2.content.find("cat=\"NP\"")!=string::npos )
		count++;			
	else if(termNP2.content.find("cat=\"ADJP\"")!=string::npos)
		count +=4;
	else if(termNP2.content.find("cat=\"PP\"")!=string::npos)
		count +=3;
	else return false;
	
	
	if(count==2)
		return eCheckRule1(it,termNP1,termNP2, strText,stat, os); //check for rule 1
	else if (count==5)
		return eCheckRule1_4(it,t1,t2,strText,stat,os);
	else if(count==4)
		return eCheckRule1_3(it,termNP1, termNP2,strText,stat,os);
	
	return false;
}

bool Sentence::eCheckRule1(vector<Term>::iterator it, const Term& termNP1, const Term& termNP2, const string& strText, statistics& stat, io::filtering_ostream& os)
{
	Term tok = *it;
	//find PREP modifies VERB
	bool check1= false, check2 = false, check=false;
	vector<Relation> arrRel;
	Term prep, termNP3, newNP1=termNP1, newNP2=termNP2;
	//Term tok = *it;
	size_t pos, pos2;
	string argId;
	for(vector<Term>::iterator nit = it+1; nit!= arrTok.end(); nit++)
	{
		prep = *nit;
		size_t posPrep = prep.content.find("pred=\"prep_arg12");
		if(posPrep==string::npos || prep.check == true) //not PREP
		{			
				continue;			
		}			
		check1 = false; check2=false;
		//check if this PREP modifies VERB and a NP
		pos = prep.content.find("pred=\"");
		pos2 = prep.content.find("\"", pos + 6);
		int flag = 0;
		while(prep.content[pos2 - 1] >= '1' && prep.content[pos2 - 1] <= '9')
		{				
			//cerr << "Use substr: Sentence::checkRule2 _1" << endl;
			pos = prep.content.find("arg"+ prep.content.substr(pos2-1,1)+"=\"");
			
			string::size_type pos3 = prep.content.find("\"", pos + 6);
			argId = prep.content.substr(pos + 6, pos3 - pos - 6);
			if(argId == "unk")
			{
				pos2--;
				continue;

			}
			size_t np = lookUpTerm(arrTerm, argId);
			if(np == -1)
			{
				cerr<< "CheckRule1: Check source code or corpus!!!\n";
				return false;
			}
			Term temp = (Term) arrTerm[np];
			if(temp.content.find("cat=\"NX\"")!=string::npos || temp.content.find("cat=\"NP\"")!=string::npos) //this is a NP 
			{				
			//	flag ++;
				check1 = true;
				termNP3 = temp;
			}
			else 
			{
				string strTemp = strText.substr(temp.start, temp.end - temp.start);
				string strVerb = strText.substr(tok.start, tok.end - tok.start);
				string strNP2 = strText.substr(termNP2.start, termNP2.end - termNP2.start);
				//if(temp.content.find(tok.id)!=string::npos //this prep modifies a VERB or verb phrase that has a head of VERB --> rule 2.2
				//|| temp.content.find(termNP2.id)!=string::npos)  // this prep modifies NP2 --> rule 2.1
				if(strTemp.find(strVerb)!=string::npos || strTemp.find(strNP2)!=string::npos){check2 = true;}
			//	flag ++;
			//	check2 = true;
			}

			pos2--;

			if(check1 && check2)
			{
				check = true;
				break; // this sentence satifies rule 1 ; this break out of while loop only!!!
			}
		}
		////check if termNP1, tok, termNP2 contain entities?
		
			
		if(check1 && check2 )
		{
			string strNP1 = strText.substr(termNP1.start,termNP1.end - termNP1.start) ;
			string strNP2 = strText.substr(termNP2.start,termNP2.end - termNP2.start) ;
			string strNP3 = strText.substr(termNP3.start, termNP3.end - termNP3.start);
			
			//cout << "pattern 1.2:" << endl;
			//////////cerr << "Use substr: Sentence::checkRule2 _1" << endl;
			//cout << "NP1:"  << strText.substr(termNP1.start,termNP1.end - termNP1.start) << "\t"
			//	<< "VERB:" << strText.substr(tok.start,tok.end - tok.start) << "\t"
			//	<< "NP2:" << strNP2
			//  << "Prep:" << strText.substr(prep.start,prep.end - prep.start) << "\t"
			//	<< "NP3:" << strText.substr(termNP3.start, termNP3.end - termNP3.start) << endl;				
			newNP2 = termNP3;
			/*if(common::split(strNP1+" "," ").size() > LIMIT_LENGTH)
			{
				newNP1 = lookUpHead(termNP1.id);
				strNP1 = strText.substr(newNP1.start,newNP1.end - newNP1.start) ;
			}
			if(common::split(strNP3+" "," ").size() > LIMIT_LENGTH )
			{
				newNP2 = lookUpHead(termNP3.id);
				strNP3 = strText.substr(newNP2.start,newNP2.end - newNP2.start) ;				
			}*/
			if(common::split(strNP1+" "," ").size() <= LIMIT_LENGTH && common::split(strNP3+" "," ").size() <= LIMIT_LENGTH )
			{
				cout << strNP1 << endl << strNP3 << endl;
				os << newNP1.start << "\t" << newNP1.end << "\t" << strNP1 << endl
					//<< "rel\t" << tok.start << "\t" << prep.end << "\t" << strText.substr(tok.start,tok.end - tok.start) + " " + strNP2 + " " + strText.substr(prep.start,prep.end - prep.start) */<< endl
					<< "rel\t" << tok.start <<"\t" << tok.end << "\t" << tok.base << endl 
					<< newNP2.start << "\t" << newNP2.end << "\t" << strNP3 << endl;

				

				cout <<strNP1 << endl << strNP2 << endl;
				/*os << strNP1 << "\t"
					<< "rel:" << strText.substr(tok.start, tok.end - tok.start) << "\t"
					<< strNP2 << endl;*/

				os << newNP1.start << "\t" << newNP1.end << "\t" << strNP1 << endl 
					//<< "rel\t" << tok.start << "\t" << tok.end << "\t" << strText.substr(tok.start, tok.end - tok.start) << endl
					<< "rel\t" << tok.start << "\t" << tok.end << "\t" << tok.base << endl
					<< termNP2.start << "\t" << termNP2.end << "\t" << strNP2 << endl;
			
				numOfNPpairs+= 2;
			}

		}		
	}
	
	if(check==false)
	{
		newNP2 = termNP2;
		string strNP2 = strText.substr(termNP2.start, termNP2.end - termNP2.start) ;
		string strNP1 = strText.substr(termNP1.start,termNP1.end - termNP1.start) ;

		/*cout << "pattern 1:" << endl;
		cout << "NP1:"  << strText.substr(termNP1.start,termNP1.end - termNP1.start) << "\t"
		<< "VERB:" << strText.substr(tok.start,tok.end - tok.start) << "\t"
		<< "NP2:" << strNP2 << endl; */
		/*if(common::split(strNP1+" "," ").size() > LIMIT_LENGTH)
		{			
			newNP1 = lookUpHead(termNP1.id);
			strNP1 = strText.substr(newNP1.start,newNP1.end - newNP1.start) ;

		}
		if(common::split(strNP2+" "," ").size() > LIMIT_LENGTH )
		{			
			newNP2 = lookUpHead(termNP2.id);;
			
			strNP2 = strText.substr(newNP2.start,newNP2.end - newNP2.start) ;
		}*/
		
		if(common::split(strNP1+" "," ").size() <= LIMIT_LENGTH && common::split(strNP2+" "," ").size() <= LIMIT_LENGTH )
		{
			cout << strNP1 << endl 	<< strNP2 << endl;
			
			os << newNP1.start << "\t" << newNP1.end << "\t" << strNP1 << endl
				//<< "rel\t" << tok.start << "\t" << tok.end << "\t" << strText.substr(tok.start,tok.end - tok.start) << endl
				<< "rel\t" << tok.start << "\t" << tok.end << "\t" << tok.base << endl
				<< newNP2.start << "\t" << newNP2.end << "\t" << strNP2 << endl;

			numOfNPpairs ++;
			/*os << strNP1 << endl
				<< "rel:" << strText.substr(tok.start,tok.end - tok.start) << "\t"
				<< strNP2 << endl;*/
		}
		stat.rule1 ++;
	}
	return true;
}

bool Sentence::eCheckRule2(vector<Term>::iterator it, const Term& termNP1, const string& strText, statistics& stat, io::filtering_ostream& os)
{
	//find PREP modifies VERB
	
	bool check1= false, check2 = false;
	Term prep, termNP2, tok = *it;
	
	size_t pos, pos2;
	string argId;
	for(vector<Term>::iterator nit = it+1; nit!= arrTok.end(); nit++)
	{
		prep = *nit;
		size_t posPrep = prep.content.find("pred=\"prep_arg12");
		if(posPrep==string::npos) //not PREP
		{			
				continue;			
		}
		check1 = false; check2=false;
		//check if this PREP modifies VERB and a NP
		pos = prep.content.find("pred=\"");
		pos2 = prep.content.find("\"", pos + 6);
		int flag = 0;
		while(prep.content[pos2 - 1] >= '1' && prep.content[pos2 - 1] <= '9')
		{				
			//cerr << "Use substr: Sentence::checkRule3 _1" << endl;
			pos = prep.content.find("arg"+ prep.content.substr(pos2-1,1)+"=\"");
			
			string::size_type pos3 = prep.content.find("\"", pos + 6);
			argId = prep.content.substr(pos + 6, pos3 - pos - 6);
			if(argId == "unk")
			{
				pos2--;
				continue;

			}
			size_t np = lookUpTerm(arrTerm, argId);
			if(np == -1)
			{
				cerr<< "CheckRule2:Check source code or corpus!!!\n";
				return false;
			}
			Term temp = (Term) arrTerm[np];
			if(temp.content.find("cat=\"NX\"")!=string::npos || temp.content.find("cat=\"NP\"")!=string::npos) //this is a NP 
			{				
			//	flag ++;
				check1 = true;
				termNP2 = temp;
			}
			else //if(temp.content.find(tok.id)!=string::npos) //this is a VERB or verb phrase which has a head of VERB
			{
				string strTemp = strText.substr(temp.start, temp.end - temp.start);
				string strVerb = strText.substr(tok.start, tok.end - tok.start);
				if(strTemp.find(strVerb)!=string::npos){check2 = true;} //this is a VERB or verb phrase which has a head of VERB
			} 

			pos2--;

			if(check1 && check2)
			{
				break; // this sentence satifies rule 3 ; this break out of while loop only!!!
			}
	}
		//check if termNP1, tok, termNP2 contain entities?
		if(check1 && check2 )
		{
			string strNP2 = strText.substr(termNP2.start, termNP2.end - termNP2.start) ;
			string strNP1 = strText.substr(termNP1.start, termNP1.end - termNP1.start) ;
			Term newNP1 = termNP1;
			Term newNP2 = termNP2;
			//
			//cout << "pattern 2:" << endl;
			////cerr << "Use substr: Sentence::checkRule3 _2" << endl;
			//cout << "NP1:"  << strText.substr(termNP1.start,termNP1.end - termNP1.start) << "\t"
			//	<< "VERB:" << strText.substr(tok.start,tok.end - tok.start) << "\t"
			//	<<"Prep:" << strText.substr(prep.start,prep.end - prep.start) << "\t"
			//	<< "NP2:" <<strNP2 << endl;
			/*if(common::split(strNP1+" "," ").size() > LIMIT_LENGTH)
			{
				newNP1 = lookUpHead(termNP1.id);
				strNP1 = strText.substr(newNP1.start,newNP1.end - newNP1.start) ;

			}
			if(common::split(strNP2+" "," ").size() > LIMIT_LENGTH )
			{
				newNP2 = lookUpHead(termNP2.id);
				strNP2 = strText.substr(newNP2.start,newNP2.end - newNP2.start) ;
			}*/
			if(common::split(strNP1+" "," ").size() <= LIMIT_LENGTH && common::split(strNP2+" "," ").size() <= LIMIT_LENGTH )
			{
				cout << strNP1 << endl << strNP2 << endl;
				os << newNP1.start << "\t" << newNP1.end << "\t" << strNP1 << endl
				 //<< "rel\t" << tok.start << "\t" << prep.end << "\t" << strText.substr(tok.start,tok.end - tok.start) + " " + strText.substr(prep.start,prep.end - prep.start) << endl
				 << "rel\t" << tok.start << "\t" << prep.end << "\t" << tok.base + " " + prep.base << endl
			   << newNP2.start << "\t" << newNP2.end << "\t" << strNP2 << endl;

				numOfNPpairs ++;
				/*os << strNP1 << "\t"
				 << "rel:" << strText.substr(tok.start,tok.end - tok.start) + " " + strText.substr(prep.start,prep.end - prep.start)
			   << "\t" << strNP2 << endl;*/
			}
			stat.rule2++;
			return true;
		}
	}
	return false;

}

bool Sentence::eCheckRule3(vector<Term>::iterator it, const Term& termNP1, const string& strText, statistics& stat, io::filtering_ostream& os)
{
	bool check1= false, check2 = false;
	Term prep, termNP2, cood, tok = *it, verb; //tok is the first main verb
	size_t pos, pos2;
	string argId;	

	
	for(vector<Term>::iterator cit = arrTok.begin(); cit!= arrTok.end(); cit++)
	{
		cood = *cit;
//		if(cood.start<= tok.start && cood.end <= tok.end)
		if(cood.end <= tok.start || cood.start <= tok.start)
			continue;
		//find coordination that has two arguments, one of them must be verb1 (tok)
		size_t posCood = cood.content.find("pred=\"coord_arg12\"");
		if(posCood==string::npos)
			continue;
		pos = cood.content.find("arg1=\"");		
		pos2 = cood.content.find("\"", pos + 6);
		argId = cood.content.substr(pos + 6, pos2 - pos - 6);
		if(argId == "unk")
		{
			continue;	
		}
		size_t vp = lookUpTerm(arrTerm, argId);
		if(vp == -1)
		{
			cerr<< "CheckRule3:Check source code or corpus!!!\n";
			return false;
		}		
		verb = (Term) arrTerm[vp];
		if(verb.content.find("cat=\"VP\"")==string::npos && verb.content.find("cat=\"VX\"")==string::npos) //-->this is not a verb or a verb phrase
			continue;
		//find head of this verb
		pos = verb.content.find("head=\"");		
		pos2 = verb.content.find("\"", pos + 6);
		argId = verb.content.substr(pos + 6, pos2 - pos - 6);
		if(argId[0]=='c')
		{
				vp = lookUpTerm(arrTerm, argId);
				verb = arrTerm[vp];
		}
		else {
			vp = lookUpTerm(arrTok, argId);
			verb = arrTok[vp];
		}

		string strVerb = strText.substr(tok.start, tok.end - tok.start);
		string strVerbPhrase = strText.substr(verb.start, verb.end - verb.start);
		
		if(strVerbPhrase.find(strVerb)==string::npos) //this verb or verb phrase is not verb1
			continue;
		
		//find arg2
		pos = cood.content.find("arg2=\"");		
		pos2 = cood.content.find("\"", pos + 6);
		argId = cood.content.substr(pos + 6, pos2 - pos - 6);
		if(argId == "unk")
		{
			continue;	
		}
		vp = lookUpTerm(arrTerm, argId);
		if(vp == -1)
		{
			cerr<< "CheckRule3:Check source code or corpus!!!\n";
			return false;
		}		
		verb = (Term) arrTerm[vp];
		if(verb.content.find("cat=\"VP\"")==string::npos && verb.content.find("cat=\"VX\"")==string::npos) //-->this is not a verb or a verb phrase
			continue;
		//find head of this verb
		pos = verb.content.find("head=\"");		
		pos2 = verb.content.find("\"", pos + 6);
		argId = verb.content.substr(pos + 6, pos2 - pos - 6);
		if(argId == "unk")
		{
			continue;	
		}
		if(argId[0]=='c')
			vp = lookUpTerm(arrTerm, argId);
		else vp = lookUpTerm(arrTok,argId);

		if(vp == -1)
		{
			cerr<< "CheckRule3:Check source code or corpus!!!\n";
			return false;
		}		
		if(argId[0]=='c')
			verb = (Term) arrTerm[vp];
		else verb = (Term)arrTok[vp];

		//find prep modifies this verb and an NP

		for(vector<Term>::iterator jt = it + 1; jt != arrTok.end(); jt++)
		{
			prep = *jt;
			if(prep.content.find("prep_arg12")==string::npos)
				continue;
			//find two arguments of this prep

			pos = prep.content.find("arg1=\"");		
			pos2 = prep.content.find("\"", pos + 6);
			argId = prep.content.substr(pos + 6, pos2 - pos - 6);
			if(argId == "unk")
			{
				continue;	
			}
			vp = lookUpTerm(arrTerm, argId);
			if(vp == -1)
			{
				cerr<< "CheckRule3:Check source code or corpus!!!\n";
				return false;
			}		
			const Term& verb2 = (Term) arrTerm[vp];
			if(verb2.content.find("cat=\"VP\"")==string::npos && verb2.content.find("cat=\"VX\"")==string::npos) //-->this is not a verb or a verb phrase
				continue;
			if(verb2.content.find("xcat=\"COOD\"")==string::npos) //prep must modify COOD
				continue;
			//find head of this verb
			pos = verb2.content.find("head=\"");		
			pos2 = verb2.content.find("\"", pos + 6);
			argId = verb2.content.substr(pos + 6, pos2 - pos - 6);
			if(argId == "unk")
			{
				continue;	
			}
			if(argId[0]=='c')
				vp = lookUpTerm(arrTerm, argId);
			else 
				vp = lookUpTerm(arrTok,argId);

			if(vp == -1)
			{
				cerr<< "CheckRule3:Check source code or corpus!!!\n";
				return false;
			}		
			Term head;
			if(argId[0]=='c')
				head = (Term) arrTerm[vp];
			else head = (Term) arrTok[vp];

			strVerb = strText.substr(verb.start, verb.end - verb.start);
			strVerbPhrase = strText.substr(head.start, head.end - head.start);

		if(strVerbPhrase.find(strVerb)==string::npos && strVerb.find(strVerbPhrase)==string::npos) //this verb or verb phrase is not verb2
			//if(strVerb.find(strVerbPhrase)==string::npos) //this verb or verb phrase is not verb2
				continue;
		
			//find arg2
			pos = prep.content.find("arg2=\"");		
			pos2 = prep.content.find("\"", pos + 6);
			argId = prep.content.substr(pos + 6, pos2 - pos - 6);
			if(argId == "unk")
			{
				continue;	
			}
			size_t np = lookUpTerm(arrTerm, argId);
			if(np == -1)
			{
				cerr<< "CheckRule3:Check source code or corpus!!!\n";
				return false;
			}		
			termNP2 = (Term) arrTerm[np];
			if(termNP2.content.find("cat=\"NP\"")==string::npos && termNP2.content.find("cat=\"NX\"")==string::npos) //-->this is not a noun or a noun phrase
				continue;
			else
			{
				string strNP1 = strText.substr(termNP1.start,termNP1.end - termNP1.start);
				string strNP2 = strText.substr(termNP2.start, termNP2.end - termNP2.start);
				
				//cout << "pattern 3:" << endl;
				//cerr << "Use substr: Sentence::checkRule1_5" << endl;
				/*cout << "NP1:"  << strText.substr(termNP1.start,termNP1.end - termNP1.start) << "\t"
					<< "VERB:" << strText.substr(tok.start,tok.end - tok.start) << "\t"
					<< "COOD:" << strText.substr(cood.start, cood.end - cood.start) + " " + strText.substr(verb2.start, verb2.end - verb2.start) << "\t"
					<<"Prep:" << strText.substr(prep.start,prep.end - prep.start) << "\t"
					<< "NP2:" << strText.substr(termNP2.start, termNP2.end - termNP2.start) << endl;*/
				Term newNP1 = termNP1, newNP2 = termNP2;
				/*if(common::split(strNP1+" "," ").size() > LIMIT_LENGTH)
				{
					newNP1 = lookUpHead(termNP1.id);
					strNP1 = strText.substr(newNP1.start,newNP1.end - newNP1.start) ;

				}
				if(common::split(strNP2+" "," ").size() > LIMIT_LENGTH )
				{
					newNP2 = lookUpHead(termNP2.id);
					strNP2 = strText.substr(newNP2.start,newNP2.end - newNP2.start) ;
				}*/
				if(common::split(strNP1+" "," ").size() <= LIMIT_LENGTH && common::split(strNP2+" "," ").size() <= LIMIT_LENGTH )
				{					
					cout << strNP1 << endl << strNP2 << endl;
					
					os << newNP1.start << "\t" << newNP1.end << "\t" << strNP1 << endl
					//<< "rel\t" << tok.start << "\t" << prep.end << "\t" 
					//	<< strText.substr(tok.start,tok.end - tok.start) + " "
					//	+ strText.substr(cood.start, cood.end - cood.start) + " " 
					//	+ strText.substr(verb2.start, verb2.end - verb2.start) + " "
					//	+ strText.substr(prep.start,prep.end - prep.start) << endl
					<< "rel\t" << tok.start << "\t" << tok.end << "\t" << tok.base << endl
					  << newNP2.start << "\t" << newNP2.end << "\t" << strNP2 << endl;

					numOfNPpairs ++;
					/*os << strNP1 << "\t"
						<< "rel:" << strText.substr(tok.start,tok.end - tok.start) + " "
						+ strText.substr(cood.start, cood.end - cood.start) + " " 
						+ strText.substr(verb2.start, verb2.end - verb2.start) + " "
						+ strText.substr(prep.start,prep.end - prep.start)
					  << "\t" << strNP2 << endl;*/
				}
				stat.rule3 ++;
				
				return true;
			}

		}
	}
	return false;

}

bool Sentence::eCheckRule1_4(vector<Term>::iterator it, vector<Term>::size_type t1, vector<Term>::size_type t2, const string& strText, statistics & stat, io::filtering_ostream& os)
{
	Term tok = *it;
	int tokStart = tok.start, tokEnd = tok.end;
	Term termNP1 = arrTerm[t1], termNP2 = arrTerm[t2], prep, termNP3;
	//Term prep, phrase;
	int count = 0;
	//check if termNP2 is an ADJP that ADJP <-- Prep --> NP
	//get head of adj phrase (termNP2)
	size_t pos1, pos2;
	pos1 = termNP2.content.find("head=\"");
	pos2 = termNP2.content.find("\"", pos1+6);
	string headId = termNP2.content.substr(pos1+6, pos2-pos1-6);
	size_t hp; 
	Term head;
	if(headId[0]=='c')
	{
		hp = lookUpTerm(arrTerm,headId);
		head = arrTerm[hp];
	}
	else{
			hp = lookUpTerm(arrTok, headId);
			head = arrTok[hp];
	}
	
	
	for(vector<Term>::size_type i=0; i < arrTok.size(); i++)
	{		
		prep = arrTok[i];
		if(prep.start < head.start)
			continue;
		if(prep.content.find("prep_arg12")==string::npos)
			continue;
		//find arg1
		pos1 = prep.content.find("arg1=\"");		
		pos2 = prep.content.find("\"", pos1 + 6);
		headId = prep.content.substr(pos1 + 6, pos2 - pos1 - 6);
		if(headId == "unk")
		{
			continue;	
		}
		hp = lookUpTerm(arrTerm, headId);		
		Term adj = (Term) arrTerm[hp];
		if(adj.content.find("cat=\"ADJP\"")==string::npos && adj.content.find("cat=\"ADJ\"")==string::npos) //-->this is not an adj or an adj phrase
				continue;
		string adj1 = strText.substr(head.start, head.end - head.start);
		string adj2 = strText.substr(adj.start, adj.end - adj.start);
		if(adj1.find(adj2)==string::npos)
			continue;
		
		//find arg2
		pos1 = prep.content.find("arg2=\"");		
		pos2 = prep.content.find("\"", pos1 + 6);
		headId = prep.content.substr(pos1 + 6, pos2 - pos1 - 6);
		if(headId == "unk")
		{
			continue;	
		}
		hp = lookUpTerm(arrTerm, headId);		
		termNP3 = arrTerm[hp];
		if(termNP3.content.find("cat=\"NX\"")==string::npos && termNP3.content.find("cat=\"NP\"")==string::npos)
			continue;
		else
		{
			//cout << "pattern 1.4:" << endl;
			//cerr << "Use substr: Sentence::checkRule2 _1" << endl;			
			string strNP1 = strText.substr(termNP1.start,termNP1.end - termNP1.start);
			string strNP3 = strText.substr(termNP3.start, termNP3.end - termNP3.start) ;
			Term newNP1 = termNP1, newNP2 = termNP3;
				/*if(common::split(strNP1+" "," ").size() > LIMIT_LENGTH)
				{
					newNP1 = lookUpHead(termNP1.id);
					strNP1 = strText.substr(newNP1.start,newNP1.end - newNP1.start) ;

				}
				if(common::split(strNP3+" "," ").size() > LIMIT_LENGTH )
				{
					newNP2 = lookUpHead(termNP3.id);
					strNP3 = strText.substr(newNP2.start,newNP2.end - newNP2.start) ;
				}*/
			if(common::split(strNP1+" "," ").size() <= LIMIT_LENGTH && common::split(strNP3+" "," ").size() <= LIMIT_LENGTH )
			{
				cout << strNP1 << endl << strNP3 << endl;
				
				os << newNP1.start << "\t" << newNP1.end << "\t" << strNP1 << endl
					//<< "rel\t" << adj.start << "\t" << prep.end << "\t" << strText.substr(adj.start, adj.end - adj.start) + " " + strText.substr(prep.start, prep.end - prep.start) << endl
					<< "rel\t" << adj.start << "\t" << prep.end << "\t" << "be ADJ " + prep.base << endl
					<< newNP2.start << "\t" << newNP2.end << "\t" << strNP3 << endl;
				numOfNPpairs ++;
				/*os << strNP1 << "\t"
					<< "rel:" << strText.substr(adj.start, adj.end - adj.start) + " " + strText.substr(prep.start, prep.end - prep.start) << "\t"
					<<  strNP3 << endl;*/
			}
			stat.rule1_4 ++;
			return true;
		}
	}		
	
	return false;
}
bool Sentence::eCheckRule1_3(vector<Term>::iterator it, Term termNP1, Term termPP, const string& strText, statistics& stat, io::filtering_ostream& os)
{
	Term tok = *it; //--> main verb
	Term temp;
	/*if(tok.content.find("passive_verb")==string::npos)
		return false;*/
	if(termNP1.start > termPP.start)
	{
		temp.start = termPP.start; temp.end = termPP.end;
		termPP.start = termNP1.start; termPP.end = termNP1.end;
		termNP1.start = temp.start; termNP1.end = temp.end;
	}
	string strPP = strText.substr(termPP.start, termPP.end - termPP.start);
	
	if(strPP.find("by")!=0)
		return false;

	/*cout << "pattern 1.3:" << endl;
	cout << "NP1:" << strText.substr(termNP1.start, termNP1.end - termNP1.start) << "\t" 
		<< "VERB: " << strText.substr(tok.start, tok.end - tok.start) << "\t"
		<< "PP:" << strText.substr(termPP.start, termPP.end - termPP.start) << "\t" << endl;*/
	string strNP1 = strText.substr(termNP1.start, termNP1.end - termNP1.start);
	//string strPP = strText.substr(termPP.start, termPP.end - termPP.start) ;
	Term newNP1 = termNP1, newNP2 = termPP;
	/*if(common::split(strNP1+" "," ").size() > LIMIT_LENGTH)
	{
		newNP1 = lookUpHead(termNP1.id);
		strNP1 = strText.substr(newNP1.start,newNP1.end - newNP1.start) ;

	}
	if(common::split(strPP+" "," ").size() > LIMIT_LENGTH )
	{
		newNP2 = lookUpHead(termPP.id);
		strPP = strText.substr(newNP2.start,newNP2.end - newNP2.start) ;
	}*/
	
	if(common::split(strNP1+" "," ").size() <= LIMIT_LENGTH && common::split(strPP+" "," ").size() <= LIMIT_LENGTH )
	{
		cout << strNP1 << endl << strPP << endl;
		
		os << newNP1.start << "\t" << newNP1.end << "\t" << strNP1 << endl
			//<< "rel\t" << tok.start << "\t" << tok.end << "\t" << strText.substr(tok.start, tok.end - tok.start) << endl
			<< "rel\t" << tok.start << "\t" << tok.end << "\t" << tok.base << endl
			<< newNP2.start << "\t" << newNP2.end << "\t" << strPP << endl;
		numOfNPpairs ++;
	/*	os << strNP1 << "\t"
			<< "rel:" << strText.substr(tok.start, tok.end - tok.start) 
			<< "\t" << strPP << endl;*/
	}

	stat.rule1_3 ++;

	return true;
}


bool Sentence::eCheckRule4(vector<Term>::iterator it, const string& strText, statistics& stat, io::filtering_ostream& os)
{
	Term prep = *it; //--> prep
	//find two NPs modified by prep
	bool check1= false, check2 = false, check=false;
	vector<Relation> arrRel;
	Term termNP1, termNP2;
	size_t pos = prep.content.find("pred=\"prep_arg12");
	if(pos==string::npos) //not PREP
	{			
		return false;			
	}
	size_t pos1 = prep.content.find("arg1=\"");		
	size_t pos2 = prep.content.find("\"", pos1 + 6);
	string argId = prep.content.substr(pos1 + 6, pos2 - pos1 - 6);
	pos = lookUpTerm(arrTerm,argId);
	if(pos==-1)
		return false;
	termNP1 = arrTerm[pos];
	if(termNP1.content.find("cat=\"NP\"")==string::npos && termNP1.content.find("cat=\"NX\"")==string::npos ) //not a NP
		return false;
	pos1 = prep.content.find("arg2=\"");		
	pos2 = prep.content.find("\"", pos1 + 6);
	argId = prep.content.substr(pos1 + 6, pos2 - pos1 - 6);
	pos = lookUpTerm(arrTerm,argId);
	if(pos==-1)
		return false;
	termNP2 = arrTerm[pos];
	if(termNP2.content.find("cat=\"NP\"")==string::npos && termNP2.content.find("cat=\"NX\"")==string::npos ) //not a NP
		return false;
	
	string strNP1 = strText.substr(termNP1.start,termNP1.end - termNP1.start);
	string strNP2 = strText.substr(termNP2.start, termNP2.end - termNP2.start);
	Term newNP1 = termNP1, newNP2 = termNP2;
	//if(common::split(strNP1+" "," ").size() > LIMIT_LENGTH)
	//{
	//	newNP1 = lookUpHead(termNP1.id);
	//	//newNP1 = arrTok[np1];
	//	strNP1 = strText.substr(newNP1.start,newNP1.end - newNP1.start) ;

	//}
	//if(common::split(strNP2+" "," ").size() > LIMIT_LENGTH )
	//{
	//	newNP2 = lookUpHead(termNP2.id);
	//	strNP2 = strText.substr(newNP2.start,newNP2.end - newNP2.start) ;
	//}
	
	if(common::split(strNP1+" "," ").size() <= LIMIT_LENGTH && common::split(strNP2+" "," ").size() <= LIMIT_LENGTH)
	{
		cout << strNP1 << endl << strNP2 << endl;
		os << newNP1.start << "\t" << newNP1.end << "\t" << strNP1  << endl
			<< "rel\t" << prep.start << "\t" << prep.end << "\t" << strText.substr(prep.start, prep.end - prep.start) << endl
			<< newNP2.start << "\t" << newNP2.end << "\t" << strNP2  << endl;

		numOfNPpairs ++;
	}

	//cout << "pattern 4:" << endl;
	////cerr << "Use substr: Sentence::checkRule2 _1" << endl;
	//cout << "NP1:"  << strText.substr(termNP1.start,termNP1.end - termNP1.start) << "\t"		
	//			<< "PREP:" << strText.substr(prep.start, prep.end - prep.start) << "\t" 
	//			<< "NP2:" <<strText.substr(termNP2.start, termNP2.end - termNP2.start) << "\t" <<endl; 
		
	stat.rule4 ++;
	return true;
}
