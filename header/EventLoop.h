#ifndef _EventLoop_h_
#define _EventLoop_h_
#include "Epoll.h"
#include "Connection.h"
#include <functional>
#include <memory>
#include <mutex>
#include <map>

class Connection;
using ConnectionPtr = std::shared_ptr<Connection>;
class EventLoop
{
public:
    EventLoop(bool mainloop,int alarm_val=30,int timeout=80);  
    ~EventLoop();
    void run(); //运行事件循环
    void stop(); //停止事件循环
    void set_epoll_timeout_callback(std::function<void(EventLoop*)> timeout_cb);
    bool is_in_loop_thread() const;
    void queue_in_loop(std::function<void()> fn);
    void wake_up();
    void handle_wake_up();
    void update_channel(Channel *ch);
    void remove_channel(Channel *ch);
    void alarm_handler();
    void new_connection(ConnectionPtr conn);
    void set_remove_conn_cb(std::function<void(int)> remove_conn_cb);
    static int create_timer_fd(int sec=30);
private:
    Epoll _ep;
    std::function<void(EventLoop*)> _epoll_time_out_cb;
    pid_t _thread_id=0;
    std::queue<std::function<void()>> _task_queue;
    std::mutex _task_queue_mutex;
    bool _init_tid=false;
    int _wakeup_fd=-1;
    std::unique_ptr<Channel> _wakeup_channel;
    int _alarm_val; //闹钟时间间隔
    int _timeout;  //ConnectionPtr超时时间
    int _timer_fd =-1;
    std::unique_ptr<Channel> _timer_channel;
    bool _mainloop;
    std::map<int,ConnectionPtr> _conns;
    std::mutex _mutex_conns;
    std::function<void(int)> _remove_conn_cb;
    bool _stop=false;

};

#endif