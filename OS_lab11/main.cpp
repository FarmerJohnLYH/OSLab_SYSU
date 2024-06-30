#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <unordered_map>
#include <cassert>


using namespace std;

const int INVALID = -1;
const int UControl = 3;//����ʹ��λU�������3λ
const int PControl = 2;//�������λP�������2λ

class ControlBits {
public:
	ControlBits() :iControl(0) {}
	ControlBits(int control) { iControl = (INVALID == control ? 0 : control); }

	void set(int _control)
	{
		iControl = _control;
	}

	bool getBit(int iWhich) const {
		return (iControl & (1 << iWhich));
	}
	void setBit(int iWhich) {//iWhich��һλ��Ϊ1
		iControl = (iControl | (1 << iWhich));
	}

	void resetBit(int iWhich) {//iWhich��һλ��Ϊ0
		iControl = (iControl & (~(1 << iWhich)));
	}
private:
	int iControl;
};

class PageItem {
public:
	PageItem(int pageNo = INVALID) :iControl(0), iPageNo(pageNo), iVisitTime(-1) {}
	ControlBits iControl;
	int iPageNo;
	int iVisitTime;
	bool isInvalid()
	{
		return iPageNo == INVALID;
	}
};

class Replacement {
public:
	Replacement(int _iBufferSize = 3):iBufferSize(_iBufferSize), vBuffer(_iBufferSize), iCurrent(0) {}
	virtual bool visitPage(PageItem* pi) = 0;
	void show()
	{
		cout << "buffer:\t";
		for (int ii = 0; ii < iBufferSize; ++ii)
		{
			if (NULL == vBuffer[ii])
			{
				cout << '\t';
				continue;
			}
			cout << vBuffer[ii]->iPageNo;
			if (vBuffer[ii]->iControl.getBit(UControl))
				cout << '*';
			cout << '\t';
		}
		cout << "current=" << iCurrent << '\t';
		cout << endl;
	}
protected:
	int iBufferSize;//����ҳ����
	int iCurrent;//ָ��λ��
	vector<PageItem*> vBuffer;//פ����
};

class ClockPolicy :public Replacement {
public:
	ClockPolicy(int _iBufferSize = 3) : Replacement(_iBufferSize) {}
	bool visitPage(PageItem* pi);
};

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
bool ClockPolicy::visitPage(PageItem* pi)
{
	//UControl 使用位 PControl 在内存中 
	bool bPageFault = false;
	//定义pi->iControl的别名
	ControlBits& iControl = pi->iControl;
	int m_hand = iCurrent % iBufferSize;
	//您需查看控制位P看是否被访问页在内存中，
	//若不在，则应按时钟策略进行置换，置换时应考虑使用位U，我们假设使用位U在右起第UControl位。

	//检查内存位
	if (iControl.getBit(PControl)) 
	{
		bPageFault = false;
		iControl.setBit(UControl);//更新UControl（再次被访问了）
	}
	else {
		//寻找替换页
		bPageFault = true;
		while(true)
		{
			if(vBuffer[m_hand] == nullptr)
			{
				// 如果当前位置为空，则将该页插入到当前位置
				vBuffer[m_hand] = pi;
				pi->iControl.setBit(PControl);//设置PControl在内存中
				pi->iControl.setBit(UControl);//设置UControl使用位
				break;
			}
			//检查UControl位
			if (vBuffer[m_hand]->iControl.getBit(UControl)) {
				vBuffer[m_hand]->iControl.resetBit(UControl);//如果有，UControl位清零
			}
			else {
				PageItem*    &pReplacePage = vBuffer[m_hand];//别名
				// 替换
				
				pReplacePage->iControl.resetBit(PControl);//设置PControl在内存中
				pReplacePage->iControl.resetBit(UControl);
				// pReplacePage->iControl.set(0);
				
				pReplacePage = pi;
				pReplacePage->iControl.setBit(PControl);//设置PControl在内存中
				pReplacePage->iControl.setBit(UControl);//设置UControl使用位
				break;
			}
			m_hand=(m_hand+1)% vBuffer.size();
		}
		iCurrent =(m_hand+1)% vBuffer.size();
	}
	return bPageFault;
}

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
	// return 0;
	test();
	return 0;
}