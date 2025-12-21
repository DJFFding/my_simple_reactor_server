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
    void set_close_callback(std::function<void(Connection*)> close_cb);
    void set_error_callback(std::function<void(Connection*)> error_cb);
private:
    Epoll* _ep=nullptr;
    Socket _clientSock;
    Channel* _clientChannel=nullptr;
    std::function<void(Connection*)> _close_cb;
    std::function<void(Connection*)> _error_cb;
};
#endif