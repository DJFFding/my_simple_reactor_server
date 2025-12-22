#ifndef _EchoServer_h_
#define _EchoServer_h_
#include "TcpServer.h"

class EchoServer
{
public:
    EchoServer(const char* ip,uint16_t port);
    ~EchoServer();
    void start();
    void handle_new_connection(Connection* conn);
    void handle_close_connection(Connection* conn);
    void handle_error_connection(Connection* conn);
    void handle_on_message(Connection* conn,std::string message);
    void handle_send_complete(Connection* conn);
    void handle_epoll_timeout(EventLoop* loop);
private:
    TcpServer _tcpServer;
};
#endif