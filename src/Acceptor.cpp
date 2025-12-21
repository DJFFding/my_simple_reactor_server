#include "Acceptor.h"
#include "Connection.h"

Acceptor::Acceptor(EventLoop *loop, const char *ip, uint16_t port)
    :_loop(loop)
{
    _serverSock = new Socket(Socket::create_nonblocking());
    _serverSock->set_reuse_addr(true);
    _serverSock->set_tcp_nodelay(true);
    _serverSock->set_reuse_port(true);
    _serverSock->set_keepalive(true);
    InetAddress servaddr(ip,port);
    _serverSock->bind(servaddr);
    _serverSock->listen();
    _acceptChannel = new Channel(_loop->epObj(),_serverSock->fd());
    _acceptChannel->set_read_cb(std::bind(&Acceptor::new_connection,this));
    _acceptChannel->enableReading();
}

Acceptor::~Acceptor()
{
    delete _serverSock;
    delete _acceptChannel;
}

void Acceptor::new_connection()
{
    InetAddress clientaddr;
    int sockClient=_serverSock->accept(clientaddr);
    Connection* conn = new Connection(_loop->epObj(),sockClient);
    printf("accept client(fd=%d,ip=%s,port=%d) ok.\n",sockClient,clientaddr.ip(),clientaddr.port());
}
