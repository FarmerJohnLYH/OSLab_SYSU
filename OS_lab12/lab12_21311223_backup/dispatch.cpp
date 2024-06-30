#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <unordered_map>
#include <cassert>
#include <queue>
using namespace std;
typedef char ID_TYPE;
typedef int TimeStamp_TYPE;
typedef int HowLong_TYPE;
#define NO_PROCESS ' '
#define MAX_TimeStamp 100

ofstream fout;

class Process {
public:
	Process() : done(0), id(NO_PROCESS) {}
	Process(ID_TYPE _id, TimeStamp_TYPE _arrive, HowLong_TYPE _Ts) :id(_id), arrive(_arrive), Ts(_Ts), done(0) {}

	ID_TYPE id;
	int arrive;
	HowLong_TYPE Ts;

	HowLong_TYPE done;//how much has been done. 0<= done <= Ts.
	TimeStamp_TYPE tSelected;//when it is selected to run.
};

class LoadProcess {
public:
	TimeStamp_TYPE tLast;
	ifstream fin;
	Process* pProcess;

	LoadProcess() : tLast(-1), pProcess(NULL) {
		fin.open("../process.txt");
	}

	//if there exists a process comes at tCurrent timestamp, then load it in pResult, and return true.
	//otherwise, return false.
	bool load(TimeStamp_TYPE tCurrent, Process& pResult)
	{
		if (tCurrent <= tLast) {
			cout << "Error: loading a process happended before last loading" << endl;
			return false;
		}
		if (pProcess == NULL)
		{
			if (fin.eof())
				return false;
			fin >> pBuffer.id >> pBuffer.arrive >> pBuffer.Ts;
			pProcess = &pBuffer;
		}
		assert(pProcess);
		if (pProcess->arrive == tCurrent)
		{
			//find the process
			pResult = *pProcess;
			pProcess = NULL;
			tLast = tCurrent;
			return true;
		}
		else if (pProcess->arrive > tCurrent)
			return false; // the process haven't came.
		else
		{
			cout << "Error: miss one process" << endl;
			return false;
		}
	}
private:
	Process pBuffer;
};

class Dispatch {
	private:
		TimeStamp_TYPE tMax;
		TimeStamp_TYPE tCurrent;
		Process proCurrent;
		Process proSelect;
		LoadProcess lp;
		vector<ID_TYPE> vRecords;

		void handleNext()
		{
			bool bSelected = select(proSelect);
			if (bSelected)
			{
				proSelect.tSelected = tCurrent;
				proCurrent = proSelect;
				vRecords[tCurrent] = proCurrent.id;
				cout << "select Process " << proCurrent.id << endl;
			}
			else
			{
				cout << "cpu is free from timestamp:" << tCurrent << endl;
				proCurrent.id = NO_PROCESS;
				vRecords[tCurrent] = NO_PROCESS;
			}
		}
		void next() {
			cout << "At timestamp " << tCurrent << ", ";

			Process proNew;
			//push the new come process at tCurrent into the ready queue.
			if (lp.load(tCurrent, proNew))
			{
				insert(proNew,true);
				cout << "Process " << proNew.id << " comes, ";
			}

			if (proCurrent.id == NO_PROCESS)
			{
				cout << "cpu is free currently,";
				
				//handle the next process
				handleNext();
			}
			else {
				//stop the current running process if it should be stopped, push to the ready queue if needed.
				if (goOnRunning(proCurrent))
				{
					cout << "Process " << proCurrent.id << " goes on running" << endl;
					vRecords[tCurrent] = proCurrent.id;
					return;
				}

				//check if the current process finished		
				proCurrent.done += tCurrent - proCurrent.tSelected;
				bool bFinished = (proCurrent.id == NO_PROCESS || proCurrent.done == proCurrent.Ts);
				bool bNoWaitting = nowaitting();
				
				if (!bFinished)
				{
					if (!bNoWaitting)
					{
						//if no finish and exist waitting process, put it back to the ready queue.
						insert(proCurrent, false);
					}
					else
					{
						//no finish, no one waitting
						proCurrent.tSelected = tCurrent;
						vRecords[tCurrent] = proCurrent.id;
						cout << "Process " << proCurrent.id << " should stop but no one waitting so continue" << endl;
						return;
					}
				}
				//if yes, consider to select another
				//handle the next process
				handleNext();
			}		
		}

	protected:
		Dispatch() :tCurrent(0), tMax(20) { vRecords.resize(tMax+1); }
		TimeStamp_TYPE getCurrentTimeStamp() { return tCurrent; }

		string strMethod;
		virtual void setName() = 0;

		virtual void insert(Process proNew, bool isNewProcess) = 0;
		virtual bool nowaitting() = 0;
		virtual bool select(Process& proSelcet) = 0; //if select one process, return true; otherwise return false.
		virtual bool goOnRunning(Process proCurrent) = 0;//should go on running then return true; otherwise return false.

		void showRecords()
		{
			cout << "The overall running:" << endl;
			for (int ii = 0; ii < vRecords.size(); ++ii)
				cout << vRecords[ii];
			cout << "reach max runtime"<<endl;
			// 最大运行时间 记录 
			fout << strMethod << ":";
			for (int ii = 0; ii < vRecords.size(); ++ii)
				fout << vRecords[ii];
			fout << endl;
		}

	public:
		
		void resetMaxTimeStamp(TimeStamp_TYPE _tMax) {
			tMax = _tMax;
			vRecords.resize(tMax+1);
		}
		void run()
		{
			setName();
			while (tCurrent < tMax) {
				next();
				++tCurrent;
			}
			showRecords();
		}
};
//dispatch.h ends here 

class FCFS : public Dispatch {
private:
// nowaitting ： 是否没有进程在等待调度运行。
// insert ： 把一个进程proNew插入到等待调度的进程的数据结构中，isNewProcess是说该进程是刚来的，还是运行态返回就绪态（有些调度算法可能需要这个参数）。
// select：在所有等待调度的进程中，按照当前的调度算法，应该选择哪一个运行
// goOnRunning: 在当前时刻，是否继续执行该进程。利用函数getCurrentTimeStamp()可得到当前时刻。

	queue<Process> readyQueue;
	void setName() {
		strMethod = "FCFS";
	}
	bool nowaitting() {
		return readyQueue.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		readyQueue.push(proNew);
	}
	bool select(Process& proSelcet) {
		if (readyQueue.empty()) {
			return false;
		}
		proSelcet = readyQueue.front();
		readyQueue.pop();
		return true;
	}
	bool goOnRunning(Process proCurrent){
		
		bool bFinished = ((proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected)) == proCurrent.Ts);
		if (bFinished)
		{
			return false;
		}
		return true;
	}
};

class RR : public Dispatch {
public:
	void setQ(int _q)
	{
		q = _q;
	}
private:
	// RR(q) 是一种循环调度算法，它将时间划分为固定大小的块，
	//每个块称为一个时间片。每个任务在完成自己的时间片后，
	//都会被移至就绪队列的末尾。当所有任务都完成他们的时间片后，
	//他们将再次从头开始。这种方法的优点是公平性，但缺点是可能会浪费一些CPU时间。
	int q;
	queue<Process> readyQueue;

	void setName() {
		strMethod = "RR"+to_string(q);
	}
	bool nowaitting() {
		return readyQueue.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		readyQueue.push(proNew);
	}
	bool select(Process& proSelcet) {
		if (readyQueue.empty()) {
			return false;
		}
		proSelcet = readyQueue.front();
		readyQueue.pop();
		return true;
	}
	//public:
	//  done;//how much has been done. 0<= done <= Ts.
	//  tSelected;//when it is selected to run.
	//  id;
	//  arrive;
	//  Ts;
	bool goOnRunning(Process proCurrent) {
		bool bFinished = ((proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected)) == proCurrent.Ts);
		//是否完成
		if (bFinished) {
			return false;
		}
		// if (proCurrent.done % q == 0 && proCurrent.done != 0) 
		if ((getCurrentTimeStamp() - proCurrent.tSelected) < q)
		{
			return true;
		}else return false;
	}
};

class SPN : public Dispatch {
private:
	deque<Process> readyQueue;
	void setName() {
		strMethod = "SPN";
	}
	bool nowaitting() {
		return readyQueue.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		if (readyQueue.empty()) {
			readyQueue.push_back(proNew);
			return;
		}
		deque<Process> tempQueue;
		//遍历队列
		while (!readyQueue.empty() && readyQueue.back().Ts > proNew.Ts) {
			tempQueue.push_front(readyQueue.back());
			readyQueue.pop_back();
		}

		readyQueue.push_back(proNew);

		while (!tempQueue.empty()) {
			readyQueue.push_back(tempQueue.front());
			tempQueue.pop_front();
		}
	}
	bool select(Process& proSelcet) {
		if (readyQueue.empty()) {
			return false;
		}
		proSelcet = readyQueue.front();
		readyQueue.pop_front();
		return true;
	}
	bool goOnRunning(Process proCurrent) {
		bool bFinished = ((proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected)) == proCurrent.Ts);
		if (bFinished) {
			return false;
		}
		return true;
	}
};

class SRT : public Dispatch {
private:

	struct CompareProcess {
		bool operator()(const Process& a, const Process& b) const {
			return ((a.Ts - a.done > b.Ts - b.done) || ((a.Ts - a.done == b.Ts - b.done) && (a.arrive > b.arrive)));
		}
	};
	priority_queue<Process, vector<Process>, CompareProcess> readyQueue;
	// priority_queue<Process, vector<Process>> readyQueue;

	void setName() {
		strMethod = "SRT";
	}

	bool nowaitting() {
		return readyQueue.empty();
	}

	bool cmp(const Process& a, const Process& b) const {
		return (a.Ts - a.done > b.Ts - b.done) || 
		((a.Ts - a.done == b.Ts - b.done) && a.arrive>b.arrive) ;
	}

	void insert(Process proNew, bool isNewProcess) {
		readyQueue.push(proNew);
	}
	bool select(Process& proSelcet) {
		if (readyQueue.empty()) {
			return false;
		}
		proSelcet = readyQueue.top();
		readyQueue.pop();
		return true;
	}
	bool goOnRunning(Process proCurrent) {
		bool bFinished = ((proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected)) == proCurrent.Ts);
		if (bFinished) {
			return false;
		}
		if (readyQueue.empty()) {
			return true;
		}
		if ((proCurrent.Ts - (proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected))) <= readyQueue.top().Ts - readyQueue.top().done) //当前进程剩余时间小于队列中最短进程的时间
		{
			return true;
		}
		// proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected);
		// readyQueue.push(proCurrent);
		return false;
	}
};

class HRRN : public Dispatch {//非抢占
private:
	vector<Process> readyQueue;
	void setName() {
		strMethod = "HRRN";
	}
	
	double responseRatio(Process pro) {
		return 1 + (((double)getCurrentTimeStamp() - (double)pro.arrive) / (double)pro.Ts); // 1 +(等待时间 / 服务时间) 
	}

	void update()// 更新 因时间戳改变 导致的响应比改变 
	{
		int bz = 0;
		Process proSelcet = readyQueue[0];
		int n = readyQueue.size();
		//遍历 readyQueue
		for(int i=1;i<=n-1;++i)
		{
			if (responseRatio(readyQueue[i]) > responseRatio(proSelcet) || (responseRatio(readyQueue[i]) == responseRatio(proSelcet) && readyQueue[i].arrive < proSelcet.arrive)) {// 选择响应比最高的进程
				proSelcet = readyQueue[i];
				bz=i;
			}
		}
		//交换 bz 和开头位置 
		Process temp = readyQueue[0];
		readyQueue[0] = readyQueue[bz];
		readyQueue[bz] = temp;
	}

	bool nowaitting() {
		return readyQueue.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		readyQueue.push_back(proNew);
		update();
	}
	bool select(Process& proSelcet) {
		if (readyQueue.empty()) {
			return false;
		}
		update();
		proSelcet = readyQueue[0];
		readyQueue.erase(readyQueue.begin());
		return true;
	}
	bool goOnRunning(Process proCurrent) {
		bool bFinished = ((proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected)) == proCurrent.Ts);
		if (bFinished) {
			return false;
		}//非抢占
		return true;
	}
};

class Feedback : public Dispatch {
public:
	Feedback(int _n=3) : n(_n),vRQ(_n+1),iLevel(-1),count(0) {	}
	
private:
	int n;
	int iLevel; //the current running process comes from which RQ.
	vector<queue<Process> > vRQ;
	int count; //how many waitting processes.

	void setName() {
		strMethod = "Feedback";
	}

	void insert(Process proNew, bool isNewProcess) {
		if (isNewProcess)
		{
			vRQ[0].push(proNew);
			++count;
		}
		else
		{
			assert(iLevel < n);
			vRQ[iLevel + 1].push(proNew);
			++count;
		}
	}

	bool nowaitting()
	{
		return count == 0;
	}

	bool select(Process& proSelcet) {
		bool bFound = false;
		for (int ii = 0; ii <= n; ++ii)
		{
			if (vRQ[ii].empty())
				continue;
			bFound = true;
			proSelcet = vRQ[ii].front();
			vRQ[ii].pop();
			--count;
			iLevel = ii;
			break;
		}
		return bFound;
	}

	bool goOnRunning(Process proCurrent) {
		bool bFinished = (proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected) == proCurrent.Ts);
		if (bFinished)
		{
			return false;
		}
		
		if (iLevel == n)
			return true;

		HowLong_TYPE duration = getCurrentTimeStamp() - proCurrent.tSelected;
		return (duration < (1 << iLevel));
	}
};

void test()
{
	// Feedback zz;
	// zz.run();

	// FCFS a;
	// a.run();

	// int q = 1;
	// RR b;
	// b.setQ(q);
	// b.run();

	// RR c;
	// q = 4;
	// c.setQ(q);
	// c.run();

	// SPN d;
	// d.run();

	// SRT e;
	// e.run();

	HRRN f;
	f.run();
}

//please use this main function when submitting.
int main() 
{
	fout.open("result.txt");
	test();
	fout.close();
	return 0;
}
