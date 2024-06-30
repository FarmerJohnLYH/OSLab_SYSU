#include "ipt.h"

void test0(InvertedPageTable& IPT)
{
	cout << "basic test:" << endl;
	int data[][2] = { {1,1},{2,1},{3,1},{5,1} ,{1,2} };
	int iSize = sizeof(data) / sizeof(int) / 2;

	ADDRESS curr;
	for (int ii = 0; ii < iSize; ++ii)
	{
		curr = IPT.insert(data[ii][0], data[ii][1]);
		cout << curr << '\t';
		curr = IPT.lookup(data[ii][0], data[ii][1]);
		cout << curr << endl;
	}
}

void test1(InvertedPageTable& IPT)
{
	cout << "confilict test:" << endl;
	int data[][2] = { {0,0},{0,4},{8,2},{4,3} ,{12,1} };
	int iSize = sizeof(data) / sizeof(int) / 2;

	ADDRESS curr;
	for (int ii = 0; ii < iSize; ++ii)
	{
		curr = IPT.insert(data[ii][0], data[ii][1]);
		cout << curr << '\t';
		curr = IPT.lookup(data[ii][0], data[ii][1]);
		cout << curr << endl;
	}
	// cout << "Error1" << endl;
	IPT.release(data[1][0], data[1][1]);
	cout << "Error2" << endl;
	curr = IPT.lookup(data[1][0], data[1][1]);
	cout << "Error3" << endl;
	cout << curr << endl;
	curr = IPT.insert(data[4][0], data[4][1]);
	cout << curr << '\t';
	curr = IPT.lookup(data[4][0], data[4][1]);
	cout << curr << endl;
}

int main() {
	int m = 4; //页框总数为2^m
	InvertedPageTable IPT;
	IPT.initial(m);
	test0(IPT);
	test1(IPT);
	return 0;
}