#include "Acceptor.h"
#include "Connection.h"

Acceptor::Acceptor(EventLoop* loop, const char *ip, uint16_t port)
    :_loop(loop)
    ,_serverSock(Socket::create_nonblocking())
    ,_acceptChannel(_loop,_serverSock.fd())
{
    _serverSock.set_reuse_addr(true);
    _serverSock.set_tcp_nodelay(true);
    _serverSock.set_reuse_port(true);
    _serverSock.set_keepalive(true);
    InetAddress servaddr(ip,port);
    _serverSock.bind(servaddr);
    _serverSock.listen();
    _acceptChannel.set_read_cb(std::bind(&Acceptor::new_connection,this));
    _acceptChannel.enableReading();
}

Acceptor::~Acceptor()
{
}

void Acceptor::new_connection()
{
    InetAddress clientaddr;
    int sockClient=_serverSock.accept(clientaddr);
    _new_connection_cb(sockClient,clientaddr);
}

void Acceptor::set_new_connection_cb(std::function<void(int,const InetAddress&)> fn)
{
    _new_connection_cb=fn;
}
