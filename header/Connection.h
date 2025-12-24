#ifndef _Connection_h_
#define _Connection_h_
#include <functional>
#include <mutex>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"
#include <memory>
#include <atomic>
class Connection;
using ConnectionPtr = std::shared_ptr<Connection>;

class Connection:public std::enable_shared_from_this<Connection>
{
public:
    Connection(EventLoop* loop,int clientSock);
    ~Connection();
    int fd();
    const char* ip();
    uint16_t port();
    void close_callback();  //tcp连接断开的回调函数
    void error_callback(); //tcp连接错误的回调函数
    void set_close_callback(std::function<void(ConnectionPtr)> close_cb);
    void set_error_callback(std::function<void(ConnectionPtr)> error_cb);
    void set_send_complete_callback(std::function<void(ConnectionPtr)> send_ccb);
    void set_on_message_callback(std::function<void(ConnectionPtr,std::string)> on_mcb);
    void set_ip_port(const char*ip,uint16_t port);
    void onMessage();
    void send(const char* data,size_t size); //发送数据
    void send_in_loop(const char* data,size_t size);//如果是io线程直接调用，如果不是，把这个函数传给io线程执行
    void write_callback();
private:
    EventLoop* _loop=nullptr;
    Socket _clientSock;
    std::unique_ptr<Channel> _clientChannel;
    std::function<void(ConnectionPtr)> _close_cb;
    std::function<void(ConnectionPtr)> _error_cb;
    std::function<void(ConnectionPtr,std::string)> _on_message_cb;
    std::function<void(ConnectionPtr)> _send_complete_cb;
    Buffer _input_buffer; //接收缓冲区
    Buffer _output_buffer; //发送缓冲区
    std::atomic_bool _disconnect;
};
#endif