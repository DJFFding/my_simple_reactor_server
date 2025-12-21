#ifndef _Acceptor_h_
#define _Acceptor_h_
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"

class Acceptor
{
public:
    Acceptor(EventLoop* loop,const char* ip,uint16_t port);
    ~Acceptor();
    void new_connection();
private:
    EventLoop* _loop=nullptr;
    Socket* _serverSock=nullptr;
    Channel* _acceptChannel=nullptr;
};
#endif