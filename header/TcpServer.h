#ifndef _TcpServer_h_
#define _TcpServer_h_
#include "EventLoop.h"
#include "Acceptor.h"

//网络服务类 一个监听的fd和很多客户端连接的fd
class TcpServer
{
public:
    TcpServer(const char* ip,uint16_t port);
    ~TcpServer();
    void start();
private:
    EventLoop _loop; //一个TcpServer可能有多个事件循环，现在是单线程，暂且只用到一个
    Acceptor* _acceptor;//一个TcpServer只有一个Acceptor对象
};

#endif