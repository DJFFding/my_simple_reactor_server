#include "EventLoop.h"
#include "Channel.h"
using namespace std;



//在构造函数中创建Epoll对象
EventLoop::EventLoop()
    :_ep(new Epoll())
{

}

EventLoop::~EventLoop()
{
    delete _ep;
}

void EventLoop::run()
{
    vector<Channel*> channels;
    while (true){
        channels = _ep->loop();
        //如果nEvents>0，表示有事件发生的fd的数量
        for (const auto& ch:channels){
            ch->handle_event();
        }
    }
}

Epoll *EventLoop::epObj() 
{
    return _ep;
}
