#include "statistics.h"

statistics::statistics(void)
{
	truepos=falsepos=falseneg=countCover=0;
	totalSent = rule1 = rule2 = rule3 = rule4 = 0;
	rule1_2=rule1_3=rule1_4=rule2_1 = 0;
}
statistics::~statistics(void)
{

}

void statistics::print()
{
	double pre = truepos* 1.0/(truepos+falsepos), re = truepos * 1.0/(truepos+falseneg), fscore = 2*re*pre/(re+pre);
	double exp_pre = countCover*1.0/(truepos+falsepos), exp_re = countCover*1.0/(truepos+falseneg), exp_fscore = 2*exp_re*exp_pre/(exp_re+exp_pre);
	
	
	//cerr << "Expected Precision: " << exp_pre << endl;
	//cerr << "Expected F-score: " << exp_fscore << endl;
	cerr << "Cover:" << countCover << endl;
	cerr << "Expected Recall: " << exp_re << endl;
	cerr << "True pos:" << truepos << ", false pos:" << falsepos << ", false neg:" << falseneg << endl;
	cerr << "Total: Recall = " << re << endl;
	cerr << "Precision = " << pre << endl;
	cerr << "F-score = " <<fscore <<endl;
	
	//cout << "Expected Recall: " << exp_re << endl;
	cout << "Expected Recall: " << exp_re << endl;
	cout << "Expected Precision: " << exp_pre << endl;
	//cout << "Expected F-score: " << exp_fscore << endl;
	cout << "True pos:" << truepos << ", false pos:" << falsepos << ", false neg:" << falseneg << endl;
	cout << "Total: Recall = " << re << endl;
	cout << "Precision = " << pre << endl;
	cout << "F-score = " <<fscore <<endl;

	cout << "Total Sentence:" << totalSent << endl;
	cout << "Rule 1:" << rule1 << endl;
	cout << "Rule 1.2:" << rule1_2 << endl;
	cout << "Rule 1.3:" << rule1_3 << endl;
	cout << "Rule 1.4:" << rule1_4 << endl;
	cout << "Rule 2:" << rule2 << endl;
	cout << "Rule 2.1:" << rule2_1 << endl;
	cout << "Rule 3:" << rule3 << endl;
	cout << "Rule 4:" <<rule4 <<endl;
}