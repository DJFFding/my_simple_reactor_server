#include "TcpServer.h"
#include "Socket.h"
#include "Channel.h"
#include "Connection.h"
#include "Acceptor.h"

TcpServer::TcpServer(const char *ip, uint16_t port)
    :_acceptor(new Acceptor(&_loop,ip,port))
{
    _acceptor->set_new_connection_cb(std::bind(&TcpServer::new_connection,this,std::placeholders::_1));
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

void TcpServer::new_connection(int sockClient)
{
    Connection* conn = new Connection(_loop.epObj(),sockClient);
    conn->set_close_callback(std::bind(&TcpServer::close_connection,this,std::placeholders::_1));
    conn->set_error_callback(std::bind(&TcpServer::error_connection,this,std::placeholders::_1));
    printf("new_connection accept client(fd=%d,ip=%s,port=%d) ok.\n",sockClient,conn->ip(),conn->port());
    _conns[sockClient]=conn;
}

void TcpServer::close_connection(Connection *conn)
{
    printf("client(eventfd=%d) disconnected.\n",conn->fd());
    auto iter = _conns.find(conn->fd());
    if (iter!=_conns.end()){
        delete iter->second;
        _conns.erase(iter);
    }
    
}

void TcpServer::error_connection(Connection *conn)
{
    printf("client(eventfd=%d) error.\n",conn->fd());
    auto iter = _conns.find(conn->fd());
    if (iter!=_conns.end()){
        delete iter->second;
        _conns.erase(iter);
    }
}
