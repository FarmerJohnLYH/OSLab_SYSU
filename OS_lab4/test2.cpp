#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
using namespace std;



class myClass {
public:

	void print_block(int c) {

		cout << "PrintTimes=" << iPrintTimes << endl;
		for (int i = 0; i < iPrintTimes; ++i) { cout << c; usleep(1);}
		//sleep for 1 microsecond
		cout << '\n';

	}
	void setPrintTimes(int times)
	{
		iPrintTimes = times;
	}
private:
	int iPrintTimes;
};
#define fo(i,x,y) for(int i=(x);i<=(y);++i)
void tryThreads()
{
	const int m=100;
	myClass class0[m];
	fo(i,0,m-1)
		class0[i].setPrintTimes(i*5+3);
	thread th0[m];
	fo(i,0,m-1) //for(int i=
		th0[i]= thread(&myClass::print_block, &class0[i], i);

	fo(i,0,m-1)
		th0[i].join();
//	th1.join();
//	th2.join();
}

int main()
{
	tryThreads();
	return 0;
}
