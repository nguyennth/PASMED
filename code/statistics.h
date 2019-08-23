#include <iostream>

using namespace std;

class statistics
{
public:
	int truepos, falsepos, falseneg;
	int countCover;
	int totalSent;
	int rule1, rule1_2, rule1_3, rule1_4, rule2, rule2_1, rule3, rule4;
	
public:
	//void calculateFScore();
	void print();
	statistics(void);
	~statistics(void);
};