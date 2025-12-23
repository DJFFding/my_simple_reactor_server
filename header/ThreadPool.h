#ifndef _ThreadPool_h_
#define _ThreadPool_h_
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <vector>
#include <queue>


class ThreadPool
{
public:
    //在构造函数中将启动threadnum个线程
    explicit ThreadPool(int thread_num,const std::string&thread_type);
    //把任务添加到队列中
    void addTask(std::function<void()> task);
    //在析构函数中将停止线程
    ~ThreadPool();

private:
    std::vector<std::thread> _threads; //线程池中的线程
    std::queue<std::function<void()>> _taskqueue; //任务队列
    std::mutex _mutex; //任务队列同步的互斥锁
    std::condition_variable _conditional; //任务队列同步的条件变量
    std::atomic_bool _stop; //在析构函数找那个，把_stop的值设为true，全部的线程将退出
    std::atomic_int _wait;
    std::string _thread_type;  //IO WORKS
};

#endif