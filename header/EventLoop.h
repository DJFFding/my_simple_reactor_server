#ifndef _EventLoop_h_
#define _EventLoop_h_
#include "Epoll.h"
#include <functional>
#include <memory>
#include <mutex>

class EventLoop
{
public:
    EventLoop();  
    ~EventLoop();
    void run(); //运行事件循环
    void set_epoll_timeout_callback(std::function<void(EventLoop*)> timeout_cb);
    bool is_in_loop_thread() const;
    void queue_in_loop(std::function<void()> fn);
    void wake_up();
    void handle_wake_up();
    void update_channel(Channel *ch);
    void remove_channel(Channel *ch);
private:
    Epoll _ep;
    std::function<void(EventLoop*)> _epoll_time_out_cb;
    pid_t _thread_id=0;
    std::queue<std::function<void()>> _task_queue;
    std::mutex _task_queue_mutex;
    bool _init_tid=false;
    int _wakeup_fd=-1;
    std::unique_ptr<Channel> _wakeup_channel;
};

#endif