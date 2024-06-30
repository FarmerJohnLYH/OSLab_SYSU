#include <iostream>
#include <vector>

class DiskScheduler {
public:
    DiskScheduler(std::vector<int> requests, int initialPosition=0) : initialPosition_(initialPosition), requests_(std::move(requests)) {}

    virtual void execute() = 0;  // 纯虚函数，子类需实现具体的调度算法

    void printResult() const {
        std::cout << "Order of execution: ";
        for (int track : orderOfExecution_) {
            std::cout << track << " ";
        }
        std::cout << "\nTotal head movements: " << totalHeadMovements_ << std::endl;
    }

protected:
    int initialPosition_;                 // 磁头初始位置
    std::vector<int> requests_;           // 磁道请求序列
    std::vector<int> orderOfExecution_;   // 执行顺序
    int totalHeadMovements_{0};           // 总磁头移动数
};

class FIFODiskScheduler : public DiskScheduler {
public:
    FIFODiskScheduler(std::vector<int> requests) : DiskScheduler(std::move(requests)) {}

    void execute() override {
        currentHead_ = initialPosition_;
        for (int track : requests_) {
            orderOfExecution_.push_back(track);
            totalHeadMovements_ += std::abs(track - currentHead_);
            currentHead_ = track;
        }
    }

private:
    int currentHead_{0};  // 当前磁头位置
};

class PRIDiskScheduler : public DiskScheduler {
public:
    PRIDiskScheduler(std::vector<int> requests, std::vector<int> priority, int initialPosition=0) : DiskScheduler(std::move(requests), initialPosition), priority_(std::move(priority)) {}
    #define PI std::pair<int,int>
    #define mp std::make_pair
    void execute() override {
        std::vector<PI> priorityRequests;
        for (int i = 0; i < requests_.size(); i++) {
            priorityRequests.push_back(mp(priority_[i],requests_[i]));
        }
        std::sort(priorityRequests.begin(), priorityRequests.end(), std::greater<PI>());
        currentHead_ = initialPosition_;
        for (PI track : priorityRequests) {
            orderOfExecution_.push_back(track.second);
            totalHeadMovements_ += std::abs(track.second - currentHead_);
            currentHead_ = track.second;
        }
    }
private:
    int currentHead_{0};  // 当前磁头位置
    std::vector<int> priority_;  // 优先级
};
class LIFODiskScheduler : public DiskScheduler {
public:
    LIFODiskScheduler(std::vector<int> requests,int initialPosition=0) : DiskScheduler(std::move(requests), initialPosition) {}
    void execute() override {
        currentHead_ = initialPosition_;
        for (int i = requests_.size() - 1; i >= 0; i--) {
            orderOfExecution_.push_back(requests_[i]);
            totalHeadMovements_ += std::abs(requests_[i] - currentHead_);
            currentHead_ = requests_[i];
        }
    }
private:
    int currentHead_{0};  // 当前磁头位置
};
class SSTFDiskScheduler : public DiskScheduler {
public:
    SSTFDiskScheduler(std::vector<int> requests, int initialPosition = 0) : DiskScheduler(std::move(requests), initialPosition) {}

    void execute() override {
        currentHead_ = initialPosition_;
        std::vector<int> remainingRequests = requests_;
        while (!remainingRequests.empty()) {
            int closestTrack = findClosestTrack(remainingRequests);
            orderOfExecution_.push_back(closestTrack);
            totalHeadMovements_ += std::abs(closestTrack - currentHead_);
            currentHead_ = closestTrack;
            remainingRequests.erase(std::find(remainingRequests.begin(), remainingRequests.end(), closestTrack));
        }
    }

private:
    int currentHead_{ 0 };  // 当前磁头位置

    int findClosestTrack(const std::vector<int>& remainingRequests) {
        int closestTrack = remainingRequests[0];
        int minDistance = std::abs(remainingRequests[0] - currentHead_);
        for (int i = 1; i < remainingRequests.size(); i++) {
            int distance = std::abs(remainingRequests[i] - currentHead_);
            if (distance < minDistance) {
                minDistance = distance;
                closestTrack = remainingRequests[i];
            }
        }
        return closestTrack;
    }
};


class SCANDiskScheduler : public DiskScheduler {
public:
    SCANDiskScheduler(std::vector<int> requests, int initialPosition = 0) : DiskScheduler(std::move(requests), initialPosition) {}
    void execute() override {
        currentHead_ = initialPosition_;
        std::vector<int> remainingRequests = requests_;
        std::sort(remainingRequests.begin(), remainingRequests.end());

        // 寻找当前磁头位置在请求序列中的位置
        auto it = std::lower_bound(remainingRequests.begin(), remainingRequests.end(), currentHead_);
        int currentIndex = std::distance(remainingRequests.begin(), it);
        // totalHeadMovements_ += std::distance();
        // std::distance 返回两个迭代器之间的距离


        // 移动磁头到最后一个请求
        for (int i = currentIndex; i < remainingRequests.size(); i++) {
            orderOfExecution_.push_back(remainingRequests[i]);
            totalHeadMovements_ += std::abs(remainingRequests[i] - currentHead_);
            currentHead_ = remainingRequests[i];
        }

        // 移动磁头到最前一个请求。
        // 本题中，请求被一次性给出，因此返回扫描时，必然前往remainingRequests[currentIndex - 1]。
        for (int i = currentIndex - 1; i >= 0; i--) {
            orderOfExecution_.push_back(remainingRequests[i]);
            totalHeadMovements_ += std::abs(remainingRequests[i] - currentHead_);
            currentHead_ = remainingRequests[i];
        }
    }

private:
    int currentHead_{ 0 };  // 当前磁头位置
};

class CSCANDiskScheduler : public DiskScheduler {
public:
    CSCANDiskScheduler(std::vector<int> requests, int initialPosition = 0) : DiskScheduler(std::move(requests), initialPosition) {}

    void execute() override {
        currentHead_ = initialPosition_;
        std::vector<int> remainingRequests = requests_;
        std::sort(remainingRequests.begin(), remainingRequests.end());

        // 寻找当前磁头位置在请求序列中的位置
        auto it = std::lower_bound(remainingRequests.begin(), remainingRequests.end(), currentHead_);
        int currentIndex = std::distance(remainingRequests.begin(), it);

        // 移动磁头到最后一个请求
        for (int i = currentIndex; i < remainingRequests.size(); i++) {
            orderOfExecution_.push_back(remainingRequests[i]);
            totalHeadMovements_ += std::abs(remainingRequests[i] - currentHead_);
            currentHead_ = remainingRequests[i];
        }

        // 移动磁头到最前一个请求。
        for (int i = 0; i < currentIndex; i++) {
            orderOfExecution_.push_back(remainingRequests[i]);
            totalHeadMovements_ += std::abs(remainingRequests[i] - currentHead_);
            currentHead_ = remainingRequests[i];
        }
    }

private:
    int currentHead_{ 0 };  // Current head position
};
//g++ main.cpp -std=c++11 -o main && ./main
// 待实现的类：
// CSCANDiskScheduler 单向扫描算法

