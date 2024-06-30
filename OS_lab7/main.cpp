#include "banker.h"

int main(int argc, char** argv) {
	int iNumOfProcess = 4;
	int iNumOfResource = 3;
	string dataFolder("data1");
	//g++ -o run main.cpp safe.cpp banker.h 
	//./run 4 3 data2
	//g++ -o run main.cpp -I./dir 
	//g++ -o run main.cpp safe.cpp -I./dir 
	int position = 3;
	if(argc>position)
		dataFolder=(string)argv[position];
	--position;
	if(argc>position)
		iNumOfResource=atoi(argv[position]);
	--position;
	if(argc>position)
		iNumOfProcess=atoi(argv[position]);


	State myState(iNumOfProcess, iNumOfResource);
	myState.setStudentID();
	myState.dataFolder=dataFolder;
	cout<<myState.studentID<<'\t'<<dataFolder<<'\t';
	myState.test1();
	myState.test2();
	myState.test3();
	cout<<endl;
	return 0;
}
