#include <iostream>
#include <thread>
#include <vector>
using namespace std;

void modify(int a, int & b)
{
	b = a;
}

void tryTwoThreads()
{
	int a0 = 0, a1 = 1;
	int b0 = 2, b1 = 3;
	thread th0(modify, a0, ref(b0));
	thread th1(modify, a1, ref(b1));

	cout << b0 << '\t' << b1 << endl;

	th0.join();
	th1.join();

	cout << b0 << '\t' << b1 << endl;
}


int main()
{
	tryTwoThreads();
	return 0;
}