#include "../replacement.h"

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
