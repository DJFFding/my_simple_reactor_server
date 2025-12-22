#ifndef _TcpServer_h_
#define _TcpServer_h_
#include "EventLoop.h"
#include "Acceptor.h"
#include <map>
class Connection;
//网络服务类 一个监听的fd和很多客户端连接的fd
class TcpServer
{
public:
    TcpServer(const char* ip,uint16_t port);
    ~TcpServer();
    void start();
    void new_connection(int sockClient,const InetAddress& addr);
    void close_connection(Connection* conn);
    void error_connection(Connection* conn);
    void on_message(Connection* conn,std::string message);
    void send_complete(Connection* conn);
    void epoll_timeout(EventLoop* loop);
private:
    EventLoop _loop; //一个TcpServer可能有多个事件循环，现在是单线程，暂且只用到一个
    Acceptor* _acceptor;//一个TcpServer只有一个Acceptor对象
    std::map<int,Connection*> _conns;
};

#endif