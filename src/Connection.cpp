#include "Connection.h"

Connection::Connection(Epoll* ep, int clientSock)
    :_clientSock(clientSock),_ep(ep)
{
    _clientChannel = new Channel(_ep,_clientSock.fd());
    _clientChannel->makeETMode();
    _clientChannel->enableReading();
    _clientChannel->set_read_cb(std::bind(&Channel::onMessage,_clientChannel));
    _clientChannel->set_close_cb(std::bind(&Connection::close_callback,this));
    _clientChannel->set_error_cb(std::bind(&Connection::error_callback,this));
}

Connection::~Connection()
{
    delete _clientChannel;
}

int Connection::fd() 
{
    return _clientSock.fd();
}

const char *Connection::ip() 
{
    return _clientSock.ip();
}

uint16_t Connection::port() 
{
    return _clientSock.port();
}

void Connection::close_callback()
{
    _close_cb(this);
}

void Connection::error_callback()
{
    _error_cb(this);
}

void Connection::set_close_callback(std::function<void(Connection*)> close_cb)
{
    _close_cb=close_cb;
}

void Connection::set_error_callback(std::function<void(Connection*)> error_cb)
{
    _error_cb = error_cb;
}
