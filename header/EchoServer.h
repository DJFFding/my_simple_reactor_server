#ifndef _EchoServer_h_
#define _EchoServer_h_
#include "TcpServer.h"
#include "ThreadPool.h"

class EchoServer
{
public:
    EchoServer(const char* ip,uint16_t port,int io_thread_num =3,int work_thread_num = 3);
    ~EchoServer();
    void start();
    void handle_new_connection(Connection* conn);
    void handle_close_connection(Connection* conn);
    void handle_error_connection(Connection* conn);
    void handle_on_message(Connection* conn,std::string message);
    void handle_send_complete(Connection* conn);
    void handle_epoll_timeout(EventLoop* loop);

    void on_message(Connection* conn,std::string message);
private:
    TcpServer _tcpServer;
    ThreadPool* _thread_pool;
};
#endif