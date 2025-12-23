#include "EchoServer.h"
#include "Connection.h"
#include "Log.hpp"
#include <sys/syscall.h>
#include <unistd.h>

EchoServer::EchoServer(const char *ip, uint16_t port,int thread_num)
    :_tcpServer(ip,port,thread_num)
{
    _tcpServer.set_new_conncetion_cb(std::bind(&EchoServer::handle_new_connection,this,std::placeholders::_1));
    _tcpServer.set_close_connection_cb(std::bind(&EchoServer::handle_close_connection,this,std::placeholders::_1));
    _tcpServer.set_error_connection_cb(std::bind(&EchoServer::handle_error_connection,this,std::placeholders::_1));
    _tcpServer.set_on_message_cb(std::bind(&EchoServer::handle_on_message,this,std::placeholders::_1,std::placeholders::_2));
    _tcpServer.set_send_completion_cb(std::bind(&EchoServer::handle_send_complete,this,std::placeholders::_1));
    _tcpServer.set_epoll_timeout_cb(std::bind(&EchoServer::handle_epoll_timeout,this,std::placeholders::_1));
}

EchoServer::~EchoServer()
{
}

void EchoServer::start()
{
    _tcpServer.start();
}

void EchoServer::handle_new_connection(Connection* conn)
{
    LOGI()<<"new connection. thread is "<<syscall(SYS_gettid);
}

void EchoServer::handle_close_connection(Connection *conn)
{
    LOGI()<<"client fd:"<<conn->fd()<<",addr:("<<conn->ip()<<":"<<conn->port()<<") close\n";
}

void EchoServer::handle_error_connection(Connection *conn)
{

}

void EchoServer::handle_on_message(Connection *conn, std::string message)
{
    LOGI()<<"handle a message. thread is "<<syscall(SYS_gettid);
    message = "reply:"+message; //回显业务
    //拼接报文长度(头部)+报文内容
    conn->send(message.data(),message.size());
}

void EchoServer::handle_send_complete(Connection *conn)
{
    printf("send Completed.\n");
}

void EchoServer::handle_epoll_timeout(EventLoop *loop)
{
  
}
