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
    printf("new_connection accept client(fd=%d,ip=%s,port=%d) ok.\n",sockClient,conn->ip(),conn->port());
    _conns[sockClient]=conn;
}
