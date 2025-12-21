#ifndef _TcpServer_h_
#define _TcpServer_h_
#include "EventLoop.h"

class TcpServer
{
public:
    TcpServer(const char* ip,uint16_t port);
    ~TcpServer();
    void start();
private:
    EventLoop _loop; //一个TcpServer可能有多个事件循环，现在是单线程，暂且只用到一个
};

#endif