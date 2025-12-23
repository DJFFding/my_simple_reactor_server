#ifndef _TcpServer_h_
#define _TcpServer_h_
#include "EventLoop.h"
#include "Acceptor.h"
#include <map>
class Connection;
class ThreadPool;
//网络服务类 一个监听的fd和很多客户端连接的fd
class TcpServer
{
public:
    TcpServer(const char* ip,uint16_t port,int thread_num=3);
    ~TcpServer();
    void start();
    void new_connection(int sockClient,const InetAddress& addr);
    void close_connection(Connection* conn);
    void error_connection(Connection* conn);
    void on_message(Connection* conn,std::string message);
    void send_complete(Connection* conn);
    void epoll_timeout(EventLoop* loop);

    void set_new_conncetion_cb(std::function<void(Connection*)> fn);
    void set_close_connection_cb(std::function<void(Connection*)> fn);
    void set_error_connection_cb(std::function<void(Connection*)> fn);
    void set_on_message_cb(std::function<void(Connection*,std::string message)> fn);
    void set_send_completion_cb(std::function<void(Connection*)> fn);
    void set_epoll_timeout_cb(std::function<void(EventLoop*)> fn);
private:
    EventLoop* _main_loop; //一个TcpServer可能有多个事件循环，主事件循环
    std::vector<EventLoop*> _sub_loops; //存放从事件循环
    ThreadPool* _thread_pool;
    int _thread_num;
    Acceptor* _acceptor;//一个TcpServer只有一个Acceptor对象
    std::map<int,Connection*> _conns;
    std::function<void(Connection*)> _new_conncetion_cb;
    std::function<void(Connection*)> _close_connection_cb;
    std::function<void(Connection*)> _error_connection_cb;
    std::function<void(Connection*,std::string message)> _on_message_cb;
    std::function<void(Connection*)> _send_completion_cb;
    std::function<void(EventLoop*)> _epoll_timeout_cb;
};

#endif