#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Connection.h"

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

void Channel::handle_event()
{  
    if (revents()&EPOLLRDHUP){
        _closeCallback();
    }
    if(revents()&EPOLLIN){
        _readCallback();
    }
    if(revents()&EPOLLOUT){
        
    }
    if (!(revents()&EPOLLIN||revents()&EPOLLOUT||events()&EPOLLRDHUP)){
        _errorCallback();
    }
}

void Channel::onMessage()
{
    char buffer[1024];
    while (true){ //由于使用非阻塞IO，一次读取buffer大小数据，直到全部数据读取完毕
        bzero(&buffer,sizeof(buffer));
        ssize_t nread = read(fd(),buffer,sizeof(buffer));
        if (nread>0){
            //把接收到的报文内容原封不动的发回去
            printf("recv(eventfd=%d):%s\n",fd(),buffer);
            send(fd(),buffer,nread,0);
        }else if(nread==0){ //客户端连接已断开
            _closeCallback();
            break;
        }
        else if (errno==EINTR){ //读取数据的时候被信号中断，继续读取
            continue;
        }
        else if (errno == EAGAIN||errno==EWOULDBLOCK){//全部数据已读取完毕
            break;
        }
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
