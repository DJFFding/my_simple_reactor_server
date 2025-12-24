#include "EventLoop.h"
#include "Channel.h"
#include "Log.hpp"
#include <unistd.h>
#include <sys/syscall.h>
using namespace std;



//在构造函数中创建Epoll对象
EventLoop::EventLoop()
    :_wakeup_fd(eventfd(0,EFD_NONBLOCK))
    ,_wakeup_channel(new Channel(this,_wakeup_fd))
{
    _wakeup_channel->set_read_cb(std::bind(&EventLoop::handle_wake_up,this));
    _wakeup_channel->enableReading();
}

EventLoop::~EventLoop()
{
   
}

void EventLoop::run()
{
    if(!_init_tid){
        _thread_id = syscall(SYS_gettid);
        _init_tid=true;
    }
    LOGI()<<"before eventloop run() thread is"<<syscall(SYS_gettid);
    vector<Channel*> channels;
    while (true){
        channels = _ep.loop(1000);
        if(channels.empty()){
            _epoll_time_out_cb(this);
        }
        //如果nEvents>0，表示有事件发生的fd的数量
        for (const auto& ch:channels){
            ch->handle_event();
        }
    }
}

void EventLoop::set_epoll_timeout_callback(std::function<void(EventLoop *)> timeout_cb)
{
    _epoll_time_out_cb = timeout_cb;
}

bool EventLoop::is_in_loop_thread() const
{
    if (!_init_tid){
        LOGW()<<"_thread_id 还未初始化";
        return false;
    }
    return _thread_id == syscall(SYS_gettid);
}

void EventLoop::queue_in_loop(std::function<void()> fn)
{
    {
        std::lock_guard<std::mutex> lock(_task_queue_mutex);
        _task_queue.push(fn);
    }
    wake_up();
}

void EventLoop::wake_up()
{
    uint64_t val =1;
    write(_wakeup_fd,&val,sizeof(uint64_t));
}

//事件循环被event_fd唤醒后执行的函数
void EventLoop::handle_wake_up()
{
    LOGI()<<"handle_wake_up() thread is"<<syscall(SYS_gettid)<<".";
    uint64_t val;
    read(_wakeup_fd,&val,sizeof(uint64_t));
    std::function<void()> fn;
    std::lock_guard<std::mutex> lock(_task_queue_mutex);
    while (!_task_queue.empty()){
        fn = std::move(_task_queue.front());
        _task_queue.pop();
        fn();
    }
}

void EventLoop::update_channel(Channel *ch)
{
    _ep.update_channel(ch);
}

void EventLoop::remove_channel(Channel *ch)
{
    _ep.remove_channel(ch);
}