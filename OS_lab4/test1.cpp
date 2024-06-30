#include <iostream>
#include <thread>
#include <vector>
using namespace std;


const int g_iPrintTimes = 10;

void print_block(int n, int c) {

	for (int i = 0; i < n; ++i) { cout << c; }
	cout << '\n';

}

void tryTwoThreads()
{
	thread th0(print_block, g_iPrintTimes, 0);
	thread th1(print_block, g_iPrintTimes, 1);

	th0.join();
	th1.join();
}

//wrong example to use multiple threads
void tryMoreThreads0(int iNumOfThreads = 2)
{
	for (int ii = 0; ii < iNumOfThreads; ++ii)
	{
		thread thii(print_block, g_iPrintTimes, ii);
		thii.join();
	}
}

//correct example
void tryMoreThreads1(int iNumOfThreads = 2)
{
	vector<thread> vThreads;
	for (int ii = 0; ii < iNumOfThreads; ++ii)
	{
		vThreads.push_back(thread(print_block, g_iPrintTimes, ii));
	}
	for (int ii = 0; ii < iNumOfThreads; ++ii)
		vThreads[ii].join();
}

int main()
{
	int iNumOfThreads = 4;
	cout << "Wrong example, threads run one by one in this case:\n";
	tryMoreThreads0(iNumOfThreads);

	cout << "\nWe need to create all threads first, then call the join()\n";
	tryMoreThreads1(iNumOfThreads);
	return 0;
}
