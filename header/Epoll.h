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

class Epoll
{
public:
    Epoll();
    ~Epoll();
    void add_fd(int fd,uint32_t op);
    std::vector<epoll_event> loop(int timeout=-1); //运行epoll_wait,等待事件的发生
private:
    static constexpr int nMaxEvents=100;
    int _epollfd =-1;
    epoll_event _events[nMaxEvents];
};

#endif