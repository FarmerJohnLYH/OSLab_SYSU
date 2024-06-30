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
const int UControl = 3;//假设使用位U在右起第3位
const int PControl = 2;//假设存在位P在右起第2位

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
	void setBit(int iWhich) {//iWhich这一位置为1
		iControl = (iControl | (1 << iWhich));
	}

	void resetBit(int iWhich) {//iWhich这一位置为0
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
	int iBufferSize;//分配页框数
	int iCurrent;//指针位置
	vector<PageItem*> vBuffer;//驻留集
};

class ClockPolicy :public Replacement {
public:
	ClockPolicy(int _iBufferSize = 3) : Replacement(_iBufferSize) {}
	bool visitPage(PageItem* pi);
};