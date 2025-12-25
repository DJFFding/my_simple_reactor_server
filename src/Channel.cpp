#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Connection.h"
#include "Log.hpp"

Channel::Channel(EventLoop* loop, int fd,bool time_out_event)
    :_loop(loop),_sockfd(fd),_is_time_out_event(time_out_event)
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
    _loop->update_channel(this);
}

void Channel::enableWriting()
{
    _events|=EPOLLOUT;
    _loop->update_channel(this);
}

void Channel::disableReading()
{
     _events&=~EPOLLIN;
    _loop->update_channel(this);
}

void Channel::disableWriting()
{
     _events&=~EPOLLOUT;
    _loop->update_channel(this);
}

void Channel::disableAll()
{
    _events=0;
    _loop->update_channel(this);
}

void Channel::remove()
{
    _loop->remove_channel(this);
    _inEpoll=false;
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

void Channel::handle_event()
{  
    if (revents()&EPOLLRDHUP){
        _closeCallback();
    }
    if(revents()&EPOLLIN){
        _readCallback();
    }
    if(revents()&EPOLLOUT){
        _writeCallback();
    }
    if (!(revents()&EPOLLIN||revents()&EPOLLOUT||events()&EPOLLRDHUP)){
        _errorCallback();
    }
}

void Channel::set_read_cb(std::function<void()> read_cb)
{
    _readCallback=read_cb;
}

void Channel::set_close_cb(std::function<void()> close_cb)
{
    _closeCallback=close_cb;
}

void Channel::set_error_cb(std::function<void()> error_cb)
{
    _errorCallback = error_cb;
}

void Channel::set_write_cb(std::function<void()> write_cb)
{
    _writeCallback = write_cb;
}

bool Channel::isTimeout() const
{
    return _is_time_out_event;
}
