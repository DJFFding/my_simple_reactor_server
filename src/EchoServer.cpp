#include "EchoServer.h"
#include "Connection.h"

EchoServer::EchoServer(const char *ip, uint16_t port)
    :_tcpServer(ip,port)
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
    
}

void EchoServer::handle_close_connection(Connection *conn)
{
    printf("client fd:%d,addr:(%s:%d) close\n",conn->fd(),conn->ip(),conn->port());
}

void EchoServer::handle_error_connection(Connection *conn)
{

}

void EchoServer::handle_on_message(Connection *conn, std::string message)
{
    printf("recv(eventfd=%d):%s\n",conn->fd(),message.data());
    message = "reply:"+message; //回显业务
    int len = message.size();
    std::string tempBuf;
    tempBuf.append((char*)&len,4);
    tempBuf.append(message,0,len);
    conn->send(tempBuf.data(),tempBuf.size());
}

void EchoServer::handle_send_complete(Connection *conn)
{
    printf("send Completed.\n");
}

void EchoServer::handle_epoll_timeout(EventLoop *loop)
{
    printf("epoll timeou.\n");
}
