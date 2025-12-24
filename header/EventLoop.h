#ifndef _EventLoop_h_
#define _EventLoop_h_
#include "Epoll.h"
#include <functional>

class EventLoop
{
public:
    EventLoop();  
    ~EventLoop();
    void run(); //运行事件循环
    Epoll* epObj();
    void set_epoll_timeout_callback(std::function<void(EventLoop*)> timeout_cb);
private:
    Epoll _ep;
    std::function<void(EventLoop*)> _epoll_time_out_cb;
};

#endif