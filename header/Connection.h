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
private:
    Epoll* _ep=nullptr;
    Socket _clientSock;
    Channel* _clientChannel=nullptr;
};
#endif