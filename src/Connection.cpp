#include "Connection.h"
#include "Log.hpp"

Connection::Connection(EventLoop* loop, int clientSock)
    :_clientSock(clientSock),_loop(loop),_disconnect(false)
{
    _clientChannel = std::make_unique<Channel>(_loop,_clientSock.fd());
    _clientChannel->makeETMode();
    _clientChannel->set_read_cb(std::bind(&Connection::onMessage,this));
    _clientChannel->set_close_cb(std::bind(&Connection::close_callback,this));
    _clientChannel->set_error_cb(std::bind(&Connection::error_callback,this));
    _clientChannel->set_write_cb(std::bind(&Connection::write_callback,this));
    _clientChannel->enableReading();
}

Connection::~Connection()
{
    
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
    _disconnect=true;
    _clientChannel->remove();
    _close_cb(shared_from_this());
}

void Connection::error_callback()
{
    _disconnect=true;
    _clientChannel->remove();
    _error_cb(shared_from_this());
}

void Connection::set_close_callback(std::function<void(ConnectionPtr)> close_cb)
{
    _close_cb=close_cb;
}

void Connection::set_error_callback(std::function<void(ConnectionPtr)> error_cb)
{
    _error_cb = error_cb;
}

void Connection::set_send_complete_callback(std::function<void(ConnectionPtr)> send_ccb)
{
    _send_complete_cb = send_ccb;
}

void Connection::set_on_message_callback(std::function<void(ConnectionPtr, std::string)> on_mcb)
{
    _on_message_cb = on_mcb;
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
            //将收到的数据放到接收缓冲区
            _input_buffer.append(buffer,nread);
        }else if(nread==0){ //客户端连接已断开
            close_callback();
            break;
        }
        else if (errno==EINTR){ //读取数据的时候被信号中断，继续读取
            continue;
        }
        else if (errno == EAGAIN||errno==EWOULDBLOCK){//全部数据已读取完毕
            while (true){
                int32_t len;
                if (_input_buffer.size()<sizeof(len))break;
                memcpy(&len,_input_buffer.data(),sizeof(len));
                if (_input_buffer.size()<len+sizeof(len))break;
                std::string message(_input_buffer.data()+sizeof(len),len);
                _input_buffer.erase(0,len+sizeof(len));
                 //在这里，将经过若干步骤的运算
                _on_message_cb(shared_from_this(),message);
            }
            break;
        }
    }
}

void Connection::send(const char *data, size_t size)
{
    if (_disconnect){
        LOGI()<<"客户端连接已断开,send直接返回.";
        return;
    }
    if(_loop->is_in_loop_thread()) {//判断当前线程是否为IO线程
        //如果当前线程是IO线程，直接执行发送数据的操作
        send_in_loop(data,size);
    }else{
        //如果当前线程不是IO线程，把发送数据的操作交给IO线程去执行
        _loop->queue_in_loop(std::bind(&Connection::send_in_loop,this,data,size));
    }
}

void Connection::send_in_loop(const char *data, size_t size)
{
    _output_buffer.append_with_head(data,size);
    //注册写事件
    _clientChannel->enableWriting();
}

void Connection::write_callback()
{
    while (true){
        int written = ::send(fd(),_output_buffer.data(),_output_buffer.size(),0);
        if (written>0){
            _output_buffer.erase(0,written);
            if (_output_buffer.size()==0){
                _clientChannel->disableWriting();
                _send_complete_cb(shared_from_this());
                break;
            }
        }else if(written==0){ //客户端连接已断开
            close_callback();
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
