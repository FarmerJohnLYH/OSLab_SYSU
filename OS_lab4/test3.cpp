#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
using namespace std;

class myClass {
public:

	void print_block(int c) {
		cout << "PrintTimes=" << iPrintTimes << endl;
		for (int i = 0; i < iPrintTimes; ++i) { cout << c;  usleep(1);}//sleep for 1 microsecond
		cout << '\n';

	}
	void setPrintTimes(int times)
	{
		iPrintTimes = times;
	}

	void tryTwoThreads()
	{
		thread th0(&myClass::print_block, this, 0);
		thread th1(&myClass::print_block, this, 1);

		th0.join();
		th1.join();
	}
private:
	int iPrintTimes;
};


int main()
{
	myClass class0;
	class0.setPrintTimes(20);
	class0.tryTwoThreads();
	return 0;
}
