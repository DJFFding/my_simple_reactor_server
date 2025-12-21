#ifndef _EventLoop_h_
#define _EventLoop_h_
#include "Epoll.h"

class EventLoop
{
public:
    EventLoop();  
    ~EventLoop();
    void run(); //运行事件循环
    Epoll* epObj();
private:
    Epoll* _ep;
};

#endif