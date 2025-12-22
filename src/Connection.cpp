#include "Connection.h"

Connection::Connection(Epoll* ep, int clientSock)
    :_clientSock(clientSock),_ep(ep)
{
    _clientChannel = new Channel(_ep,_clientSock.fd());
    _clientChannel->makeETMode();
    _clientChannel->enableReading();
    _clientChannel->set_read_cb(std::bind(&Connection::onMessage,this));
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

void Connection::set_ip_port(const char *ip, uint16_t port)
{
    _clientSock.set_ip_port(ip,port);
}

void Connection::onMessage()
{
    char buffer[1024];
    while (true){ //由于使用非阻塞IO，一次读取buffer大小数据，直到全部数据读取完毕
        bzero(&buffer,sizeof(buffer));
        ssize_t nread = read(fd(),buffer,sizeof(buffer));
        if (nread>0){
            //把接收到的报文内容原封不动的发回去
            //printf("recv(eventfd=%d):%s\n",fd(),buffer);
            //send(fd(),buffer,nread,0);
            _input_buffer.append(buffer,nread);
        }else if(nread==0){ //客户端连接已断开
            close_callback();
            break;
        }
        else if (errno==EINTR){ //读取数据的时候被信号中断，继续读取
            continue;
        }
        else if (errno == EAGAIN||errno==EWOULDBLOCK){//全部数据已读取完毕
            printf("recv(eventfd=%d):%s\n",fd(),_input_buffer.data());
            //在这里，将经过若干步骤的运算
            _output_buffer = _input_buffer;
            _input_buffer.clear();
            send(fd(),_output_buffer.data(),_output_buffer.size(),0);
            break;
        }
    }
}