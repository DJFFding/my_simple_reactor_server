#include "Channel.h"

Channel::Channel(Epoll *ep, int fd)
    :_ep(ep),_sockfd(fd)
{
}

Channel::~Channel()
{
    //在析构函数中，不要销毁_ep，不关闭_sockfd，这两个东西不属于Channel类
}

int Channel::fd() const
{
    return _sockfd;
}

void Channel::makeETMode()
{
    _events|=EPOLLET;
}

void Channel::enableReading()
{
    _events|=EPOLLIN;
    _ep->update_channel(this);
}

void Channel::makeAddEpoll()
{
    _inEpoll=true;
}

void Channel::set_revents(uint32_t ev)
{
    _revents=ev;
}

bool Channel::is_in_epoll() const
{
    return _inEpoll;
}

uint32_t Channel::events() const
{
    return _events;
}

uint32_t Channel::revents() const
{
    return _revents;
}
