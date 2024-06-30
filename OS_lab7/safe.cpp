#include "banker.h"
void State::setStudentID()
{
	studentID="21311223";//replace it with your real student id
}

#define fo(i,x,y) for(int i=(x);i<=(y);++i)
bool State::safe()
{
	bool possible=true;
	//lyh
	vector<int> work=available;
	vector<bool> finish(numOfProcess,0);
	// fo(i,0,numOfResource-1)
	// 	printf("%d ",work[i]);
	// 	printf("\n");

	fo(count,0,numOfProcess-1)//枚举至多numOfProcess次
	{
		bool found=false;

		fo(i,0,numOfProcess-1) 
		{
			if (!finish[i]) 
			{
				bool flag=false;
				fo(j,0,numOfResource-1) 
					if (claim[i][j]-alloc[i][j]>work[j])// available[j]
					{
						flag=true;//不可用
						break;
					}
				if (!flag) 
				{
					fo(j,0,numOfResource-1)
						work[j]+=alloc[i][j];//释放空间
					finish[i] = true;
					found = true;
					break;
				}
			}
		}

		if (!found) //
		{
			possible = false;
			// printf("unsafe：%d\n",count);
			
			// fo(i,0,numOfResource-1)
			// 	printf("%d ",work[i]);
			// 	printf("\n");
			break;
		}
	}

	return possible;
}
