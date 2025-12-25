#include "EventLoop.h"
#include "Channel.h"
#include "Log.hpp"
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/timerfd.h>
using namespace std;



//在构造函数中创建Epoll对象
EventLoop::EventLoop(bool mainloop,int alarm_val,int timeout)
    :_wakeup_fd(eventfd(0,EFD_NONBLOCK))
    ,_wakeup_channel(make_unique<Channel>(this,_wakeup_fd))
    ,_alarm_val(alarm_val)
    ,_timeout(timeout)
    ,_timer_fd(create_timer_fd(alarm_val))
    ,_timer_channel(make_unique<Channel>(this,_timer_fd,true))
    ,_mainloop(mainloop)
{
    _wakeup_channel->set_read_cb(std::bind(&EventLoop::handle_wake_up,this));
    _wakeup_channel->enableReading();
    _timer_channel->set_read_cb(std::bind(&EventLoop::alarm_handler,this));
    _timer_channel->enableReading();
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
    vector<Channel*> channels;
    while (!_stop){
        channels = _ep.loop(1000);
        if(_stop)break;
        if(channels.empty()){
            _epoll_time_out_cb(this);
            continue;
        }
        if(channels[0]==(Channel*)-1)
            continue;
        //如果nEvents>0，表示有事件发生的fd的数量
        for (const auto& ch:channels){
            ch->handle_event();
        }
    }
}

void EventLoop::stop()
{
    _stop = true;
    wake_up();
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
    //LOGI()<<"handle_wake_up() thread is"<<syscall(SYS_gettid)<<".";
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

void EventLoop::alarm_handler()
{
    if(_mainloop){
    }
    else{
        std::vector<ConnectionPtr> connPtrs;
        {
            std::lock_guard<std::mutex> lock(_mutex_conns);
            time_t now = time(nullptr);
            for (auto iter=_conns.begin();iter!=_conns.end();){
                if (iter->second->timeout(now,_timeout)){
                    connPtrs.push_back(iter->second);
                    iter = _conns.erase(iter);
                    continue;
                }
                iter++;
            }
        }
        for (auto conn:connPtrs){
            conn->close_callback();
        }
        connPtrs.clear();
    }
     //重新计时
    struct itimerspec timeout;
    bzero(&timeout,sizeof(timeout));
    timeout.it_value.tv_sec = _alarm_val;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(_timer_fd,0,&timeout,nullptr);
}

int EventLoop::create_timer_fd(int sec)
{
    int tfd = timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC);
    struct itimerspec timeout;
    bzero(&timeout,sizeof(timeout));
    timeout.it_value.tv_sec = sec;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(tfd,0,&timeout,nullptr);
    return tfd;
}

void EventLoop::new_connection(ConnectionPtr conn)
{
    std::lock_guard<std::mutex> lock(_mutex_conns);
    _conns[conn->fd()]=conn;
}

void EventLoop::set_remove_conn_cb(std::function<void(int)> remove_conn_cb)
{
    _remove_conn_cb =remove_conn_cb;
}
