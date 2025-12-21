#include "Socket.h"
#include "Log.hpp"

Socket::Socket(int fd)
    :_sockfd(fd)
{
}

Socket::~Socket()
{
    close(_sockfd);
}

int Socket::fd() const
{
    return _sockfd;
}

void Socket::set_reuse_addr(bool on)
{
    int opt= on?1:0;
    socklen_t opt_len= sizeof(opt);
    setsockopt(_sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,opt_len);
}

void Socket::set_reuse_port(bool on)
{
    int opt= on?1:0;
    socklen_t opt_len= sizeof(opt);
    setsockopt(_sockfd,SOL_SOCKET,SO_REUSEPORT,&opt,opt_len);
}

void Socket::set_tcp_nodelay(bool on)
{
    int opt= on?1:0;
    socklen_t opt_len= sizeof(opt);
    setsockopt(_sockfd,IPPROTO_TCP,TCP_NODELAY,&opt,opt_len);
}

void Socket::set_keepalive(bool on)
{
    int opt= on?1:0;
    socklen_t opt_len= sizeof(opt);
    setsockopt(_sockfd,SOL_SOCKET,SO_KEEPALIVE,&opt,opt_len);
}

void Socket::bind(const InetAddress &servAddr)
{
    if (::bind(_sockfd,servAddr.addr(),sizeof(sockaddr))<0){
        perror("bind() failed");
        close(_sockfd);
        exit(-1);
    }
}

void Socket::listen(int nMaxConn)
{
    if (::listen(_sockfd,nMaxConn)!=0){
        perror("listen() failed");
        close(_sockfd);
        exit(-1);
    }
}

int Socket::accept(InetAddress &clientAddr)
{
    sockaddr_in peeraddr;
    socklen_t len = sizeof(peeraddr);
    int clientfd = accept4(_sockfd,(sockaddr*)&peeraddr,&len,SOCK_NONBLOCK);
    clientAddr=peeraddr;
    return clientfd;
}

int Socket::create_nonblocking()
{
    int listenfd = socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,IPPROTO_TCP);
    if (listenfd<0){
        LOG_PERRORE()<<"listen socket create error";
        exit(-1);
    }
    return listenfd;
}
