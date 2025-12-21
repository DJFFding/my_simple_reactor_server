#ifndef _Connection_h_
#define _Connection_h_
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"

class Connection
{
public:
    Connection(Epoll* ep,int clientSock);
    ~Connection();
    int fd();
    const char* ip();
    uint16_t port();
    void close_callback();  //tcp连接断开的回调函数
    void error_callback(); //tcp连接错误的回调函数
private:
    Epoll* _ep=nullptr;
    Socket _clientSock;
    Channel* _clientChannel=nullptr;
};
#endif