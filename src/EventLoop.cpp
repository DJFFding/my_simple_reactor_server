#include "EventLoop.h"
#include "Channel.h"
#include "Log.hpp"
#include <unistd.h>
#include <sys/syscall.h>
using namespace std;



//在构造函数中创建Epoll对象
EventLoop::EventLoop()
{

}

EventLoop::~EventLoop()
{
   
}

void EventLoop::run()
{
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

Epoll *EventLoop::epObj() 
{
    return &_ep;
}

void EventLoop::set_epoll_timeout_callback(std::function<void(EventLoop *)> timeout_cb)
{
    _epoll_time_out_cb = timeout_cb;
}
