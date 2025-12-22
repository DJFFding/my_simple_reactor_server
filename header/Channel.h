#ifndef _Channel_h_
#define _Channel_h_
#include "Epoll.h"
#include <functional>

class Socket;
class Channel
{
public:
    Channel(Epoll* ep,int fd);
    ~Channel();
    int fd()const;
    void makeETMode(); //采用边缘触发
    void enableReading(); //让epoll_wait()监视_sockfd的读事件
    void makeAddEpoll();
    void set_revents(uint32_t ev);
    bool is_in_epoll()const;
    uint32_t events() const;
    uint32_t revents() const;
    void handle_event();//用于处理epoll_wait返回的事件
    void set_read_cb(std::function<void()> read_cb);
    void set_close_cb(std::function<void()> close_cb);
    void set_error_cb(std::function<void()> error_cb);
private:
    int _sockfd=-1; //Channel拥有的fd，Channel和fd是一对一的关系
    Epoll* _ep=nullptr;
    bool _inEpoll=false; //Channel是否以添加到epoll树上
    uint32_t _events=0; //_sockfd需要监听的事件
    uint32_t _revents=0; //——sockfd已发生的事件
    std::function<void()> _readCallback;
    std::function<void()> _closeCallback;
    std::function<void()> _errorCallback;
};
#endif