#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"

Channel::Channel(Epoll *ep, int fd,bool isListen)
    :_ep(ep),_sockfd(fd),_isListen(isListen)
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

bool Channel::isListen() const
{
    return _isListen;
}

void Channel::handle_event(Socket* pServeSock)
{
    if (isListen()){ //如果是listenfd有事件，表示有新的客户端连接
        InetAddress clientaddr;
        int sockClient=pServeSock->accept(clientaddr);
        Channel* clientChannel = new Channel(_ep,sockClient);
        clientChannel->makeETMode();
        clientChannel->enableReading();
        printf("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientChannel->fd(),clientaddr.ip(),clientaddr.port());
    }else{ //如果是客户端连接的fd有事件
        if (revents()&EPOLLRDHUP){
            printf("EPOLLRDHUP client(eventfd=%d) disconnected.\n",fd());
            close(fd());
        }
        if(revents()&EPOLLIN){
            char buffer[1024];
            while (true){ //由于使用非阻塞IO，一次读取buffer大小数据，直到全部数据读取完毕
                bzero(&buffer,sizeof(buffer));
                ssize_t nread = read(fd(),buffer,sizeof(buffer));
                if (nread>0){
                    //把接收到的报文内容原封不动的发回去
                    printf("recv(eventfd=%d):%s\n",fd(),buffer);
                    send(fd(),buffer,nread,0);
                }else if(nread==0){ //客户端连接已断开
                    printf("read=0 client(eventfd=%d) disconnected.\n",fd());
                    close(fd()); //关闭客户端的fd
                }
                else if (errno==EINTR){ //读取数据的时候被信号中断，继续读取
                
                }
                else if (errno == EAGAIN||errno==EWOULDBLOCK){//全部数据已读取完毕
                
                }
            }
        }
        if(revents()&EPOLLOUT){
            
        }
        if (!(revents()&EPOLLIN||revents()&EPOLLOUT)){
            printf("client(eventfd=%d) error.\n",fd());
            close(fd());
        }
    }

}
