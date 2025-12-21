#include "Connection.h"

Connection::Connection(Epoll* ep, int clientSock)
    :_clientSock(clientSock),_ep(ep)
{
    _clientChannel = new Channel(_ep,_clientSock.fd());
    _clientChannel->makeETMode();
    _clientChannel->enableReading();
    _clientChannel->set_read_cb(std::bind(&Channel::onMessage,_clientChannel));
}

Connection::~Connection()
{
    delete _clientChannel;
}
