#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <unordered_map>
#include <cstdlib> //to use rand() ; atoi()
#include <cassert>
#include <algorithm>
#include <fstream>

using namespace std;

int MAX_WAIT_NUM = 5; //you can change it to some other value to test

class Process
{
public:
	int id;
	vector<int> vWaitID;

	void randomlySetWaitID(const vector<int>& vIDs)
	{
		vWaitID.resize(0);
		int iTotal = vIDs.size();
		if (0 == iTotal)
			return;
		if (1 == iTotal)
		{
			assert(id == vIDs[0]);
			cout << "no need to wait for itself\n";
			return;
		}

		int iWaitNum = rand() % MAX_WAIT_NUM;
		unordered_map<int, bool> mTemp;

		for (int ii = 0; ii < iWaitNum; ++ii)
		{
			int iTry = vIDs[rand() % iTotal];
			if (id != iTry && mTemp.find(iTry) == mTemp.end())
			{
				vWaitID.push_back(iTry);
				mTemp[iTry] = true;
			}
		}
	}

	void print()
	{
		cout << "ID=" << id;
		if (vWaitID.size() == 0)
		{
			cout << "\tno waitting, let it go\n";
			return;
		}
		cout << "\twait list:";

		for (int ii = 0; ii < vWaitID.size(); ++ii)
			cout << '\t' << vWaitID[ii];
		cout << endl;
	}

	bool checkExist(int iHappendID)
	{
		return find(vWaitID.begin(), vWaitID.end(), iHappendID) != vWaitID.end();
	}
};

void freeList(vector<Process*>& vBlockList)
{
	for (int ii = 0; ii < vBlockList.size(); ++ii)
	{
		if (vBlockList[ii])
			delete vBlockList[ii];
		vBlockList[ii] = NULL;
	}
	vBlockList.resize(0);
}
//get a list of different IDs.
void getAListOfIDs(int iSize, vector<int>& vIDs)
{
	map<int, bool> mIDs;//mIDs record all exist IDs.
	int iCount = 0;
	vIDs.resize(0);
	while (iCount < iSize)
	{
		++iCount;
		//generate a unique id
		int id = -1;
		do
		{
			id = rand();
		} while (mIDs.find(id) != mIDs.end());
		mIDs[id] = 1;
		vIDs.push_back(id);
	}
}

void generateRandomly(vector<Process*>& vBlockList, int iSize)
{
	if (iSize <= 0)
		return;
	//clean the list first
	freeList(vBlockList);

	vector<int> vIDs;
	getAListOfIDs(iSize, vIDs);

	for (int iCount = 0; iCount < iSize; ++iCount)
	{
		Process* newProcess = new Process();
		newProcess->id = vIDs[iCount];
		vBlockList.push_back(newProcess);
		vBlockList[iCount]->randomlySetWaitID(vIDs);
	}
}

//this function help deviding iSize members into iGroup groups.
//and store the seperator of groups in vSeperator, please see the output result.
void balanceGroup(int iSize, int& iGroup, vector<int>& vSeperator)
{
	//iSize = iSize/iGroup * iGroup + iSize % iGroup;
	//set the remaining part(iSize % iGroup) one by one to each group
	int iLeft = iSize % iGroup;
	int iGroupMembers = iSize / iGroup;
	vSeperator.resize(iGroup + 1);
	int iSep = 0;
	vSeperator[0] = iSep;
	for (int ii = 1; ii <= iGroup; ++ii)
	{
		iSep += iGroupMembers;
		if (iLeft > 0)
		{
			++iSep;
			--iLeft;
		}
		vSeperator[ii] = iSep;
	}
	assert(vSeperator[iGroup] == iSize);

	cout << "\n---------------------------------------------------\n";
	//show the groups [start, end)
	//remove empty groups
	for (int ii = 1; ii < iGroup; ++ii)
	{
		if (vSeperator[ii] == iSize)
		{
			cout << "There are " << iGroup - ii << " empty groups,\n";
			cout << "so the number of groups is changed from " << iGroup << " to " << ii << endl;
			iGroup = ii;
			vSeperator.resize(iGroup + 1);
			break;
		}
	}
	for (int ii = 0; ii + 1 < vSeperator.size(); ++ii)
		cout << "Group " << ii << ": start=" << vSeperator[ii] << "\tend=" << vSeperator[ii + 1] << endl;
	cout << "---------------------------------------------------\n";
}

class BlockList
{
public:
	vector<Process*> vBlockList;
	ofstream fout;
	int iSize;
	void getList()
	{
		generateRandomly(vBlockList, iSize);
	}

	void showList()
	{
		cout << "\n---------------------------------------------------\n";
		for (int ii = 0; ii < vBlockList.size(); ++ii)
			vBlockList[ii]->print();
		cout << "---------------------------------------------------\n";
	}

	int randomlyPickAnID()
	{
		if (iSize < 1 || vBlockList.size() < iSize)
		{
			cout << "no id is available\n";
			return -1;
		}
		int id = vBlockList[rand() % iSize]->id;
		cout << "\n---------------------------------------------------\n";
		cout << "choose id = " << id << " as the happend id\n";
		cout << "Please ignore the waitIDs inside this happend process,\n";
		cout << "it is just a practice for using multiple threads.\n";
		cout << "---------------------------------------------------\n";
		return id;
	}

	void handle(int start, int end, int iHappendID, int& iCountAffect, int& iCountReady)
	{
		iCountAffect = 0;//左闭右开 
		iCountReady = 0;
		for (int ii = start; ii < end; ++ii)
		{
			bool bExist = vBlockList[ii]->checkExist(iHappendID);
			if (bExist)
			{
				++iCountAffect;
				if (vBlockList[ii]->vWaitID.size() == 1)
					++iCountReady;
			}
		}
	}

	//you can add anything you want to finish the code inside this class
	#define fo(i,x,y) for(int i=(x);i<=(y);++i)
	void run(int iNumOfThreads, int iHappendID)
	{
		int iCountAffect = 0, iCountReady = 0;
		if (iNumOfThreads < 2)
		{
			handle(0, vBlockList.size(), iHappendID, iCountAffect, iCountReady);
		}
		else
		{
			//start writing your code from here
			//lyh
			int n=vBlockList.size();//balanceGroup
				vector<thread> vThreads; 
//				vThreads.clear();
				vector<int> a,b,le,ri; 
//				a.clear(),b.clear(),le.clear(),ri.clear(); 
				fo(ii,0,n-1) a.push_back(0),b.push_back(0),le.push_back(0),ri.push_back(n); 
				//提前准备vector，防止pushback影响指针  
				int m = n/iNumOfThreads;
				for (int ii = 0; ii < iNumOfThreads; ++ii)
				{
					//用le,ri分组 
					if(ii==0) le[ii]=0;else le[ii]=ri[ii-1];
					if(ii!=iNumOfThreads-1) ri[ii]=le[ii]+m; else ri[ii]= n;
					vThreads.push_back(thread(&BlockList::handle,this,le[ii],ri[ii],iHappendID, ref(a[ii]), ref(b[ii])));
				}
				for (int ii = 0; ii < iNumOfThreads; ++ii)
					vThreads[ii].join();
				for (int ii = 0; ii < iNumOfThreads; ++ii)
				{
					iCountAffect+=a[ii];
					iCountReady+=b[ii];
//					printf("%d:%d\n",le[ii],ri[ii]);
				}
			//end of your code
		}
		cout <<"numOfThreads="<<iNumOfThreads<< "\taffected=" << iCountAffect << "\tready=" << iCountReady << endl;
		fout <<"numOfThreads\t"<<iNumOfThreads<< "\taffected\t" << iCountAffect << "\tready\t" << iCountReady << endl;

	}
};

int main(int argc, char** argv)
{
	string myStudentID("21311223");//replace by yours.
	int iNumOfThreads = 4;//线程数 change it to another value for test
	int iBlockSize = 20; //you can set the size of block list for test
	int iHappend = 0; //0 means that randomly choose an id; other value means that choose that value

	//you are not allowed to change any of the following codes.
	int position=4; //the position of the last parameter or the total number of parameters
	if(argc>position)
		iHappend=atoi(argv[position]);
	--position;
	if(argc>position)
		MAX_WAIT_NUM=atoi(argv[position]);
	--position;
	if(argc>position)
		iBlockSize=atoi(argv[position]);
	--position;
	if(argc>position)
		iNumOfThreads=atoi(argv[position]);
	BlockList bl;
	bl.fout.open("../result.xlsx",std::fstream::app);
	bl.fout << myStudentID << '\t';
	bl.iSize=iBlockSize;
	bl.getList();
	bl.showList();
	if(iHappend == 0)
		iHappend = bl.randomlyPickAnID();
	bl.run(iNumOfThreads, iHappend);
	freeList(bl.vBlockList);
	bl.fout.close();
	return 0;
}
