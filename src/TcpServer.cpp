#include "TcpServer.h"
#include "Socket.h"
#include "Channel.h"
#include "Connection.h"
#include "Acceptor.h"

TcpServer::TcpServer(const char *ip, uint16_t port)
    :_acceptor(new Acceptor(&_loop,ip,port))
{
    _acceptor->set_new_connection_cb(std::bind(&TcpServer::new_connection,this,std::placeholders::_1,std::placeholders::_2));
    _loop.set_epoll_timeout_callback(std::bind(&TcpServer::epoll_timeout,this,std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    delete _acceptor;
    for (auto& conn:_conns){
        delete conn.second;
    }
    _conns.clear();
}

void TcpServer::start()
{
    _loop.run();
}

void TcpServer::new_connection(int sockClient,const InetAddress& addr)
{
    Connection* conn = new Connection(_loop.epObj(),sockClient);
    conn->set_ip_port(addr.ip(),addr.port());
    conn->set_close_callback(std::bind(&TcpServer::close_connection,this,std::placeholders::_1));
    conn->set_error_callback(std::bind(&TcpServer::error_connection,this,std::placeholders::_1));
    conn->set_send_complete_callback(std::bind(&TcpServer::send_complete,this,std::placeholders::_1));
    conn->set_on_message_callback(std::bind(&TcpServer::on_message,this,std::placeholders::_1,std::placeholders::_2));
    _conns[sockClient]=conn;
    if (_new_conncetion_cb){
         _new_conncetion_cb(conn);
    }
}

void TcpServer::close_connection(Connection *conn)
{
    if (_close_connection_cb){
      _close_connection_cb(conn);
    }
    auto iter = _conns.find(conn->fd());
    if (iter!=_conns.end()){
        delete iter->second;
        _conns.erase(iter);
    }
}

void TcpServer::error_connection(Connection *conn)
{
    if (_error_connection_cb){
        _error_connection_cb(conn);
    }
    auto iter = _conns.find(conn->fd());
    if (iter!=_conns.end()){
        delete iter->second;
        _conns.erase(iter);
    }
}

void TcpServer::on_message(Connection *conn, std::string message)
{
    if (_on_message_cb){
        _on_message_cb(conn,message);
    }
}

//数据发送完成后
void TcpServer::send_complete(Connection *conn)
{
    if(_send_completion_cb){
        _send_completion_cb(conn);
    }
}

//epoll_wait()超时
void TcpServer::epoll_timeout(EventLoop *loop)
{
    if (_epoll_timeout_cb) {
         _epoll_timeout_cb(loop);
    }
}

void TcpServer::set_new_conncetion_cb(std::function<void(Connection*)> fn)
{
    _new_conncetion_cb = fn;
}

void TcpServer::set_close_connection_cb(std::function<void(Connection *)> fn)
{
    _close_connection_cb = fn;
}

void TcpServer::set_error_connection_cb(std::function<void(Connection *)> fn)
{
    _error_connection_cb = fn;
}

void TcpServer::set_on_message_cb(std::function<void(Connection *, std::string message)> fn)
{
    _on_message_cb = fn;
}

void TcpServer::set_send_completion_cb(std::function<void(Connection *)> fn)
{
    _send_completion_cb =fn;
}

void TcpServer::set_epoll_timeout_cb(std::function<void(EventLoop *)> fn)
{
    _epoll_timeout_cb=fn;
}
