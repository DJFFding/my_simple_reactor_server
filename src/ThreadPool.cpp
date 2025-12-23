#include "ThreadPool.h"
#include "Log.hpp"
#include <sys/syscall.h>
#include <unistd.h>
using namespace std;

ThreadPool::ThreadPool(int thread_num,const std::string&thread_type)
    :_stop(false),_wait(0),_thread_type(thread_type)
{
    
    //启动thread_num哥线程，每个线程阻塞在条件变量上
    for (int i = 0; i < thread_num; i++){
        _threads.emplace_back([this](){
            LOGI()<<"create"<<_thread_type<<"thread ("<<syscall(SYS_gettid)<<")";
            while (!_stop){
                function<void()> task; //存放出队的元素
                {
                    unique_lock<mutex> lock(_mutex);
                    //等待条件变量
                    if (--_wait<0){
                        _conditional.wait(lock,[this]{
                            return (_stop==true)||!_taskqueue.empty();
                        });
                    }
                    //在线程池停止之前，如果队列中还有任务，执行完再退出
                    if (_stop&&_taskqueue.empty()){
                        return;
                    }
                    task = move(_taskqueue.front());
                    _taskqueue.pop();                    
                }   
                task(); //执行任务        
            }
        });
    }

}

void ThreadPool::addTask(std::function<void()> task)
{
    {
        lock_guard<mutex> lock(_mutex);
        _taskqueue.push(task);
    }
    if (++_wait<=0){
        _conditional.notify_one(); //唤醒一个线程
    }
}

ThreadPool::~ThreadPool()
{
    _stop = true;
    _conditional.notify_all();
    for (auto& th:_threads)
        th.join();
}
