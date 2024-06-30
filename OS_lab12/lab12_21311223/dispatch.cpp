#include "../dispatch.h"
// #include "dispatch.h"



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
	queue<Process> readyQueue;
	void setName() {
		strMethod = "SPN";
	}
	bool nowaitting() {
		return readyQueue.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		if (readyQueue.empty()) {
			readyQueue.push(proNew);
			return;
		}
		queue<Process> tempQueue;
		while (!readyQueue.empty() && readyQueue.front().Ts > proNew.Ts) {
			tempQueue.push(readyQueue.front());
			readyQueue.pop();
		}
		readyQueue.push(proNew);
		while (!tempQueue.empty()) {
			readyQueue.push(tempQueue.front());
			tempQueue.pop();
		}
	}
	bool select(Process& proSelcet) {
		if (readyQueue.empty()) {
			return false;
		}
		proSelcet = readyQueue.front();
		readyQueue.pop();
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

	queue<Process> readyQueue;
	void setName() {
		strMethod = "SRT";
	}
	bool nowaitting() {
		return readyQueue.empty();
	}
	void insert(Process proNew, bool isNewProcess) {
		if (readyQueue.empty()) {
			readyQueue.push(proNew);
			return;
		}
		queue<Process> tempQueue;
		while (!readyQueue.empty() && readyQueue.front().Ts-readyQueue.front().done > proNew.Ts) {
			tempQueue.push(readyQueue.front());
			readyQueue.pop();
		}
		readyQueue.push(proNew);
		while (!tempQueue.empty()) {
			readyQueue.push(tempQueue.front());
			tempQueue.pop();
		}
	}
	bool select(Process& proSelcet) {
		if (readyQueue.empty()) {
			return false;
		}
		proSelcet = readyQueue.front();
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
		if ((proCurrent.Ts - (proCurrent.done + (getCurrentTimeStamp() - proCurrent.tSelected))) <= readyQueue.front().Ts - readyQueue.front().done) //当前进程剩余时间小于队列中最短进程的时间
		{
			return true;
		}
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
		return 1 + ((double)getCurrentTimeStamp() - (double)pro.arrive) / (double)pro.Ts; // 1 +(等待时间 / 服务时间) 
	}

	void update()// 更新 因时间戳改变 导致的响应比改变 
	{
		int bz = 0;
		Process proSelcet = readyQueue[0];
		int n = readyQueue.size();
		//遍历 readyQueue
		for(int i=1;i<=n-1;++i)
		{
			if (responseRatio(readyQueue[i]) > responseRatio(proSelcet)) {// 选择响应比最高的进程
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

		// for (int i=0;i<readyQueue.size();++i)
		// 	printf("%d,%c:%.1lf;",i,readyQueue[i].id,responseRatio(readyQueue[i]));

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
	FCFS a;
	a.run();

	int q = 1;
	RR b;
	b.setQ(q);
	b.run();

	RR c;
	q = 4;
	c.setQ(q);
	c.run();

	SPN d;
	d.run();

	SRT e;
	e.run();

	HRRN f;
	f.run();
}

//please use this main function when submitting.
int main() {
	fout.open("result.txt");
	test();
	fout.close();
	return 0;
}