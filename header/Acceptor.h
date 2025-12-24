#ifndef _Acceptor_h_
#define _Acceptor_h_
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include <memory>
class Acceptor
{
public:
    Acceptor(EventLoop* loop,const char* ip,uint16_t port);
    ~Acceptor();
    void new_connection();
    void set_new_connection_cb(std::function<void(int,const InetAddress&)>);
private:
    EventLoop* _loop;
    Socket _serverSock;
    Channel _acceptChannel;
    std::function<void(int,const InetAddress&)> _new_connection_cb;
};
#endif