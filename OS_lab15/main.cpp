// #include "DiskScheduler.h"
#include "DiskScheduler.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include <algorithm>

// 示例测试函数
// g++ main.cpp -std=c++11 -o main && ./main
int main() {
    std::vector<int> requests = {98, 183, 37, 122, 14, 124, 65, 67};
    std::vector<int> priority = {7, 3, 5, 4, 1, 2, 6, 8};            // 数字越大优先级越高

    // 使用FIFO调度算法进行测试
    DiskScheduler* myScheduler;
    myScheduler =  new FIFODiskScheduler(requests);
    myScheduler->execute();
    myScheduler->printResult();
//    测试代码需取消下方注释
    int initpos = 0;
   myScheduler =  new PRIDiskScheduler(requests, priority,initpos);
   myScheduler->execute();
//    printf("PRIDiskScheduler");
   myScheduler->printResult();

   myScheduler =  new LIFODiskScheduler(requests,initpos);
   myScheduler->execute();
//    printf("LIFODiskScheduler");
   myScheduler->printResult();

   myScheduler =  new SSTFDiskScheduler(requests,initpos);
   myScheduler->execute();
//    printf("SSTFDiskScheduler");
   myScheduler->printResult();

   myScheduler =  new SCANDiskScheduler(requests,initpos);
   myScheduler->execute();
//    printf("SCANDiskScheduler");
   myScheduler->printResult();

   myScheduler =  new CSCANDiskScheduler(requests,initpos);
   myScheduler->execute();
//    printf("CSCANDiskScheduler");
   myScheduler->printResult();

    return 0;
}
