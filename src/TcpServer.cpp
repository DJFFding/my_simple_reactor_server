#include "TcpServer.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"



TcpServer::TcpServer(const char *ip, uint16_t port,int thread_num)
    :_main_loop(std::make_unique<EventLoop>(true))
    ,_acceptor(_main_loop.get(),ip,port)
    ,_thread_num(thread_num)
    ,_thread_pool(_thread_num,"IO")
{
    _acceptor.set_new_connection_cb(std::bind(&TcpServer::new_connection,this,std::placeholders::_1,std::placeholders::_2));
    _main_loop->set_epoll_timeout_callback(std::bind(&TcpServer::epoll_timeout,this,std::placeholders::_1));
    //创建从事件循环
    _sub_loops.reserve(_thread_num);
    for (int i = 0; i < _thread_num; i++){
        _sub_loops.emplace_back(new EventLoop(false,5,10));
        _sub_loops[i]->set_epoll_timeout_callback(std::bind(&TcpServer::epoll_timeout,this,std::placeholders::_1));
        _thread_pool.addTask([this,i](){
            _sub_loops[i]->run();
        });
    }
    
}

TcpServer::~TcpServer()
{
    _conns.clear();
    _sub_loops.clear();
}

void TcpServer::start()
{
    _main_loop->run();
}

//处理新客户端连接请求的成员函数
void TcpServer::new_connection(int sockClient,const InetAddress& addr)
{
    ConnectionPtr conn = std::make_shared<Connection>(_sub_loops[sockClient%_thread_num].get(),sockClient);
    conn->set_ip_port(addr.ip(),addr.port());
    conn->set_close_callback(std::bind(&TcpServer::close_connection,this,std::placeholders::_1));
    conn->set_error_callback(std::bind(&TcpServer::error_connection,this,std::placeholders::_1));
    conn->set_send_complete_callback(std::bind(&TcpServer::send_complete,this,std::placeholders::_1));
    conn->set_on_message_callback(std::bind(&TcpServer::on_message,this,std::placeholders::_1,std::placeholders::_2));
    {
        std::lock_guard<std::mutex> lock(_mutex_conns);
        _conns[sockClient]=conn;
    }
    conn->enableReading();
    _sub_loops[sockClient%_thread_num]->set_remove_conn_cb(std::bind(&TcpServer::remove_conn,this,std::placeholders::_1));
    _sub_loops[sockClient%_thread_num]->new_connection(conn);
    if (_new_conncetion_cb){
        _new_conncetion_cb(conn);
    }
    
}

void TcpServer::close_connection(ConnectionPtr conn)
{
    if (_close_connection_cb){
        _close_connection_cb(conn);
    }
    remove_conn(conn->fd());
}

void TcpServer::error_connection(ConnectionPtr conn)
{
    if (_error_connection_cb){
        _error_connection_cb(conn);
    }
    remove_conn(conn->fd());
}

void TcpServer::on_message(ConnectionPtr conn, std::string message)
{
    if (_on_message_cb){
        _on_message_cb(conn,message);
    }
}

//数据发送完成后
void TcpServer::send_complete(ConnectionPtr conn)
{
    if(_send_completion_cb){
        _send_completion_cb(conn);
    }
}

//epoll_wait()超时
void TcpServer::epoll_timeout(EventLoop*loop)
{
    if (_epoll_timeout_cb) {
         _epoll_timeout_cb(loop);
    }
}

void TcpServer::remove_conn(int fd)
{
    {
        std::lock_guard<std::mutex> lock(_mutex_conns);
        auto iter = _conns.find(fd);
        if (iter!=_conns.end()){
            _conns.erase(iter);
        }
    }
}

void TcpServer::set_new_conncetion_cb(std::function<void(ConnectionPtr)> fn)
{
    _new_conncetion_cb = fn;
}

void TcpServer::set_close_connection_cb(std::function<void(ConnectionPtr)> fn)
{
    _close_connection_cb = fn;
}

void TcpServer::set_error_connection_cb(std::function<void(ConnectionPtr)> fn)
{
    _error_connection_cb = fn;
}

void TcpServer::set_on_message_cb(std::function<void(ConnectionPtr, std::string message)> fn)
{
    _on_message_cb = fn;
}

void TcpServer::set_send_completion_cb(std::function<void(ConnectionPtr)> fn)
{
    _send_completion_cb =fn;
}

void TcpServer::set_epoll_timeout_cb(std::function<void(EventLoop*)> fn)
{
    _epoll_timeout_cb=fn;
}
