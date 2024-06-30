#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <cassert>

using namespace std;
const int INVALID = -1;
typedef int ADDRESS;
const int PControl = 2;//

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
	
	void setBit(int iWhich) {
		iControl = (iControl | (1 << iWhich));
	}

	void resetBit(int iWhich) {
		iControl = (iControl & (~(1 << iWhich)));
	}
private:
	int iControl;
};

class PageItem {
public:
	PageItem() :iControl(0), link(INVALID) {}
	ADDRESS link;

	void set(int _iPageNo, int _iPID)
	{
		iPageNo = _iPageNo;
		iPID = _iPID;
		iControl.set(0);
		iControl.setBit(PControl);
	}

	void set(int _iPageNo, int _iPID, int _iControl)
	{
		iPageNo = _iPageNo;
		iPID = _iPID;
		iControl.set(_iControl);
	}

	bool hit(int _iPageNo, int _iPID) const
	{
		return iPageNo == _iPageNo && iPID == _iPID;
	}

	bool inMemory() const
	{
		return iControl.getBit(PControl);
	}

	void reset(int iWhich)
	{
		iControl.resetBit(iWhich);
	}
private:

	int iPageNo;
	int iPID;
	ControlBits iControl;
};

class InvertedPageTable {
public:
	vector<PageItem> vTable;
	string sid;
	void setSID();

	void initial(int _m) {
		m = _m;
		vTable.resize(1 << m);
	}

	ADDRESS lookup(int iPageNo, int iPID) const;

	ADDRESS insert(int iPageNo, int iPID);

	bool release(ADDRESS curr)
	{
		if (INVALID == curr)
			return false;
		vTable[curr].reset(PControl);
		return true;
	}
	bool release(int iPageNo, int iPID)
	{
		ADDRESS curr = lookup(iPageNo, iPID);
		return release(curr);
	}

private:

	int m;

	ADDRESS hash(int iPageNo, int iPID) const {
		return (iPageNo + (iPID << 2)) & ((1 << m) - 1);
	}

	ADDRESS conflict(ADDRESS curr) const {
		return ((curr + ((1 << m) >> 2))) & ((1 << m) - 1);
	}
};

void InvertedPageTable::setSID()
{
	sid = "21311223";//replace it with your student id
}

ADDRESS InvertedPageTable::lookup(int iPageNo, int iPID) const
{
	/*
	补充在倒排页表里查询页框号的函数lookup(页号,进程ID)，返回所在页框号。
	hash(页号,进程ID) 可返回一个可能的页框号，
	如果该页框对应的正好是所查询的，并且控制位的P位是1（即在内存），
	那么返回该页框。否则，需要去链指针link指示的页框继续查找...
	若最终没找到，则返回INVALID。
	*/
	ADDRESS curr = hash(iPageNo, iPID);
	while (curr != INVALID) {
		if (vTable[curr].hit(iPageNo, iPID)) {
			if (vTable[curr].inMemory()) 
				return curr;
			
		}
		curr = vTable[curr].link;
	}
	return INVALID;
}

ADDRESS InvertedPageTable::insert(int iPageNo, int iPID)
{
	/*
	从hash(页号,进程ID) 开始考虑，
	若该页框未被占用，则可做为插入页框。
	若已被占用，则发生冲突，应根据具体情况选择页框...
	
	若最终无法插入内存，则返回INVALID。
	注意，冲突时有时需要设置link的值。

	*/
	assert(lookup(iPageNo, iPID) == INVALID); 
	ADDRESS curr = hash(iPageNo, iPID); // 计算该页表项的地址
	ADDRESS prev = INVALID,firs = curr; // 记录最后一个页表

	if (curr >= vTable.size()) return INVALID;
	
	while (curr != INVALID) { 
		// 遍历倒排页表
		if (!vTable[curr].inMemory()) { 
			vTable[curr].set(iPageNo, iPID); 
			// void set(int _iPageNo, int _iPID, int _iControl)
			return curr; // 返回该页表项的地址
		}
		prev = curr;
		curr = vTable[curr].link;
		if(curr == firs) return INVALID;//内存用完了
	}
	// if(curr==firs) return INVALID;
	
	if (prev != INVALID) { 
		ADDRESS newPage = conflict(prev); 
		if (newPage >= vTable.size()) 
			return INVALID;
		// 新建一个页表项
		vTable[prev].link = newPage;
		vTable[newPage].set(iPageNo, iPID); 
		return newPage; // 返回新建的页表项的地址
	}
	return INVALID; // 如果没有空闲的页表项，则返回INVALID
}
