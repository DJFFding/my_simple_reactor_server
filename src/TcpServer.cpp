#include "TcpServer.h"
#include "Socket.h"
#include "Channel.h"

TcpServer::TcpServer(const char *ip, uint16_t port)
{
    Socket *pServSock = new Socket(Socket::create_nonblocking());
    pServSock->set_reuse_addr(true);
    pServSock->set_tcp_nodelay(true);
    pServSock->set_reuse_port(true);
    pServSock->set_keepalive(true);
    InetAddress servaddr(ip,port);
    pServSock->bind(servaddr);
    pServSock->listen();
    Channel* servChannel = new Channel(_loop.epObj(),pServSock->fd());
    servChannel->set_read_cb(std::bind(&Channel::new_connection,servChannel,pServSock));
    servChannel->enableReading();
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
    _loop.run();
}
