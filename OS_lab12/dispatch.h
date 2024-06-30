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
	HowLong_TYPE Ts; // 总运行时间

	HowLong_TYPE done;//how much has been done. 0<= done <= Ts. 已经完成的运行时间
	TimeStamp_TYPE tSelected;//when it is selected to run. 进程被选中运行的时间
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

/**
 * 选择下一个要运行的进程
 * @param proSelect 选中的进程将被存储在此参数中
 * @return 如果选择成功返回true，否则返回false
 */
class  {
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
	class Dispatch 
	{
	protected:
		TimeStamp_TYPE tCurrent; // 当前时间戳
		TimeStamp_TYPE tMax; // 最大时间戳
		vector<ID_TYPE> vRecords; // 记录每个时间戳下运行的进程ID

	public:
		Dispatch() : tCurrent(0), tMax(20) { vRecords.resize(tMax + 1); } // Dispatch 调度函数，初始化当前时间戳为0，最大时间戳为20，vRecords的大小为tMax+1

		TimeStamp_TYPE getCurrentTimeStamp() { return tCurrent; } // 获取当前时间戳

		//需要写的部分 lyh
		string strMethod; // 调度算法名称
		virtual void setName() = 0; // 设置调度算法名称

		virtual void insert(Process proNew, bool isNewProcess) = 0; // 插入新进程到就绪队列中，isNewProcess表示是否是新进程
		virtual bool nowaitting() = 0; // 判断是否存在等待进程
		virtual bool select(Process& proSelcet) = 0; // 选择下一个要运行的进程，如果选择成功返回true，否则返回false
		virtual bool goOnRunning(Process proCurrent) = 0; // 判断当前进程是否应该继续运行，如果应该返回true，否则返回false

		void showRecords() // 输出运行记录
		{
			cout << "The overall running:" << endl;
			for (int ii = 0; ii < vRecords.size(); ++ii)
				cout << vRecords[ii];
			cout << "reach max runtime" << endl;

			fout << strMethod << ":"; // 输出调度算法名称
			for (int ii = 0; ii < vRecords.size(); ++ii)
				fout << vRecords[ii]; // 输出每个时间戳下运行的进程ID
			fout << endl;
		}
	};

	void showRecords()
	{
		cout << "The overall running:" << endl;
		for (int ii = 0; ii < vRecords.size(); ++ii)
			cout << vRecords[ii];
		cout << "reach max runtime"<<endl;
		
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
		// setName();
		while (tCurrent < tMax) {
			next();
			++tCurrent;
		}
		showRecords();
	}
};