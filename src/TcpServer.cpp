#include "TcpServer.h"
#include "Socket.h"
#include "Channel.h"

TcpServer::TcpServer(const char *ip, uint16_t port)
    :_acceptor(new Acceptor(&_loop,ip,port))
{

}

TcpServer::~TcpServer()
{
    delete _acceptor;
}

void TcpServer::start()
{
    _loop.run();
}
