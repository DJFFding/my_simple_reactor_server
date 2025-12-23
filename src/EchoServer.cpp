#include "EchoServer.h"
#include "Connection.h"
#include "Log.hpp"
#include <sys/syscall.h>
#include <unistd.h>

EchoServer::EchoServer(const char *ip, uint16_t port,int io_thread_num,int work_thread_num)
    :_tcpServer(ip,port,io_thread_num),_thread_pool(new ThreadPool(work_thread_num,"Work"))
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
    delete _thread_pool;
}

void EchoServer::start()
{
    _tcpServer.start();
}

void EchoServer::handle_new_connection(Connection* conn)
{
    LOGI()<<"new connection. client fd:"<<conn->fd()<<",addr:("<<conn->ip()<<":"<<conn->port()<<") connect";
}

void EchoServer::handle_close_connection(Connection *conn)
{
    LOGI()<<"close connection. client fd:"<<conn->fd()<<",addr:("<<conn->ip()<<":"<<conn->port()<<") close";
}

void EchoServer::handle_error_connection(Connection *conn)
{

}

void EchoServer::handle_on_message(Connection *conn, std::string message)
{
    //把业务添加到线程池的任务队列中
    _thread_pool->addTask(std::bind(&EchoServer::on_message,this,conn,message));
}

void EchoServer::handle_send_complete(Connection *conn)
{
    LOGI()<<"send Completed.";
}

void EchoServer::handle_epoll_timeout(EventLoop *loop)
{
  
}


uint32_t times=0;
void EchoServer::on_message(Connection *conn, std::string message)
{
    LOGI()<<"thread_id:"<<syscall(SYS_gettid)<<"work time:"<<++times;
    message = "reply:"+message; //回显业务
    //拼接报文长度(头部)+报文内容
    conn->send(message.data(),message.size());
}
