#ifndef _Epoll_h_
#define _Epoll_h_
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>
#include <queue>
#include <sys/eventfd.h>


class Channel;
class Epoll
{
public:
    Epoll();
    ~Epoll();
    void add_fd(int fd,uint32_t op);
    void update_channel(Channel* ch); //把channel添加/更新到红黑树上
    void remove_channel(Channel* ch);
    void add_event(int fd,void* ptr,uint32_t op);
    std::vector<Channel*> loop(int timeout=-1);
private:
    static constexpr int nMaxEvents=100;
    int _epollfd =-1;
    epoll_event _events[nMaxEvents];
};

#endif