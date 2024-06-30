#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <semaphore.h>
#include <algorithm>
using namespace std;

int NUM_PHILOSOPHERS = 5;
int NUM_SERVINGS = 20;
const int N=1000;//参数应当小于 1000
sem_t forks[N];
sem_t servings;
mutex print_mutex;

int servings_eaten = 0;
int servings_count[N]; // 存储每个哲学家吃的食物数量的数组

void philosopher(int id) {
    int left_fork = id;
    int right_fork = (id + 1) % NUM_PHILOSOPHERS;

    if (id % 2 == 1) swap(left_fork, right_fork);//"奇数编号的人先访问‘左手叉子’，偶数编号的人先访问‘右手叉子’"

    while (true) {
        // 思考
        // this_thread::sleep_for(chrono::milliseconds(100));

        int rand_time = rand()%100;
        this_thread::sleep_for(chrono::milliseconds(rand_time*17));
        // 等待食物
        sem_wait(&servings);

        // 尝试拿起左边的叉子
        sem_wait(&forks[left_fork]);

        // 尝试拿起右边的叉子
        sem_wait(&forks[right_fork]);

        // 吃饭
        // 睡眠一段随机时间
        if (servings_eaten >= NUM_SERVINGS) {
            break;
        // 如果所有食物都被吃完了，停止吃饭
        }
        servings_eaten++;
        servings_count[id]++; // 增加当前哲学家的食物数量
        {
            lock_guard<mutex> lock(print_mutex);
            cout << "哲学家 " << id << " 正在用餐。已吃的食物数量: " << servings_eaten << endl;
        }

        // 放下叉子
        sem_post(&forks[left_fork]);
        sem_post(&forks[right_fork]);

        // 通知有食物可用
        sem_post(&servings);
    }
}

int main(int argc, char *argv[]) {
    // 初始化叉子
    srand(time(0));//随机数种子


    if (argc > 1) {
        NUM_PHILOSOPHERS = atoi(argv[1]);
    }
    if (argc > 2) {
        NUM_SERVINGS = atoi(argv[2]);
    }
    //输入参数：哲学家数量，食物数量


    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_init(&forks[i], 0, 1);
    }

    // 初始化食物
    sem_init(&servings, 0, NUM_SERVINGS);

    // 创建哲学家线程
    thread philosophers[NUM_PHILOSOPHERS];
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosophers[i] = thread(philosopher, i);
    }

    // 等待哲学家线程完成
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosophers[i].join();
    }

    // 打印每个哲学家吃的食物数量
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        cout << "哲学家 " << i << " 吃了 " << servings_count[i] << " 份食物。" << endl;
    }

    // 清理
    sem_destroy(&servings);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_destroy(&forks[i]);
    }

    return 0;
}