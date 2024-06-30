#include "replacement.h"
#include <random>
#include <chrono>
bool SHOW_DETAILS = true;
vector<int> generateAVisitOrder(int pages, int iTotalVisits)
{
	vector<int> vVisitOrder(iTotalVisits);
	for (int ii = 0; ii < vVisitOrder.size(); ++ii)
		vVisitOrder[ii] = rand() % pages;
	return vVisitOrder;
}

/*
vector<int> generateAVisitOrder(int pages, int iTotalVisits)
{
	vector<int> vVisitOrder(iTotalVisits);
	int mean=pages/2;
	int standard=4;
	normal_distribution<double> gaussian(mean, standard);
	unsigned seed = chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed);
	for (int ii = 0; ii < vVisitOrder.size(); ++ii)
	{
		int temp = (int)gaussian(generator);
		if (temp < 0)
			temp = 0;
		if (temp >= pages)
			temp = pages - 1;
		vVisitOrder[ii] = temp;
	}
	return vVisitOrder;
}
*/

vector<PageItem> generateAProcess(int pages)
{
	vector<PageItem> aProcess;
	for (int iPageID = 0; iPageID < pages; ++iPageID)
		aProcess.push_back(PageItem(iPageID));
	return aProcess;
}

double oneTest(int iBufferSize = 3,int pages = 10, int iTotalVisits=20) {
	vector<PageItem> aProcess = generateAProcess(pages);
	vector<int> vVisitOrder = generateAVisitOrder(pages, iTotalVisits);
	int count = 0;//count the number of page falut
	if(SHOW_DETAILS)
		cout << "initial\ttotal pages="<<pages << "\tbuffer:\t" <<string(iBufferSize,'\t') << "current=0" << endl;

	ClockPolicy cp(iBufferSize);
	for (vector<int>::iterator iter = vVisitOrder.begin(); iter != vVisitOrder.end(); ++iter)
	{
		PageItem* pVisitPage = &(aProcess[*iter]);
		bool bPageFault = cp.visitPage(pVisitPage);
		if (bPageFault)
			++count;
		if (SHOW_DETAILS)
		{
			cout << "visit=" << *iter << '\t' << (bPageFault ? "page fault" : "in memory") << '\t';
			cp.show();
		}
	}
	return count * 1.0 / iTotalVisits;
}

void test()
{
	int repeats = 10;
	int pages = 10, iTotalVisits = 1000, iBufferSize = 1;
	printf("fix pages=%d, iTotalVisits=%d, see how the page fault frequency changes accord to buffersize\n", pages, iTotalVisits);
	printf("(buffersize, frequency):");
	for (iBufferSize = 1; iBufferSize <= pages; ++iBufferSize)
	{
		double sum = 0;
		for (int ii = 0; ii < repeats; ++ii)
		{
			double frequency = oneTest(iBufferSize, pages, iTotalVisits);
			sum += frequency;
		}
		printf("(%d, %.1f)\t", iBufferSize, sum / repeats);
	}
	cout << endl << endl;
}

int main() {
	SHOW_DETAILS = true;
	double dPageFaultFrequency = oneTest();
	cout << "page fault frequency: " << dPageFaultFrequency << endl << endl;
	SHOW_DETAILS = false;
	test();
	return 0;
}