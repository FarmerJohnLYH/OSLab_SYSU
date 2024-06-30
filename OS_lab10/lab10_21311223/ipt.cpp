#include "../ipt.h"
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
	判定是否在内存，是通过P控制位。
	页表项的类PageItem里有一个控制位的变量ControlBits iControl，以及函数inMemory()。
	关于控制位，我们用一个整数的二进制值表示，默认值是0。我们关注的P位位于右起第2位(PControl=2)。例如 5 的二进制 101，右起第0位是1，第1位是0，第2位1(表示在内存里)；8的二进制1000，右起第0，1，2位都是0，第3位1。
	当内存中某一页被换出时，需要把P位变成0，参考release函数。
	同样的，当有一页被换入insert后，要把P位变成1。
	*/
	assert(lookup(iPageNo, iPID) == INVALID); 
	ADDRESS curr = hash(iPageNo, iPID); // 计算该页表项的地址
	ADDRESS prev = INVALID; // 记录最后一个页表
	while (curr != INVALID) { 
		// 遍历倒排页表
		if (!vTable[curr].inMemory()) { 
			vTable[curr].set(iPageNo, iPID); 
			// void set(int _iPageNo, int _iPID, int _iControl)
			return curr; // 返回该页表项的地址
		}
		prev = curr;
		curr = vTable[curr].link;
	}
	
	if (prev != INVALID) { 
		ADDRESS newPage = prev + 4; 
		if (newPage >= vTable.size()) 
			return INVALID;
		// 新建一个页表项
		vTable[prev].link = newPage;
		vTable[newPage].set(iPageNo, iPID); 
		return newPage; // 返回新建的页表项的地址
	}
	return INVALID; // 如果没有空闲的页表项，则返回INVALID
}