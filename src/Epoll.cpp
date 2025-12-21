#include "Epoll.h"
#include "Log.hpp"
using namespace std;

Epoll::Epoll()
{
    if((_epollfd = epoll_create(1))==-1){
        LOG_PERRORE()<<"Epoll epoll_create() failed";
        exit(-1);
    }
}

Epoll::~Epoll()
{
    close(_epollfd);
}

void Epoll::add_fd(int sockfd, uint32_t op)
{
    epoll_event ev;
    ev.data.fd = sockfd;
    ev.events = op;
    if(epoll_ctl(_epollfd,EPOLL_CTL_ADD,sockfd,&ev)==-1){
        LOG_PERRORE()<<"epoll_ctl() failed";
        exit(-1);   
    }
}

vector<epoll_event> Epoll::loop(int timeout)
{
    vector<epoll_event> evs;
    bzero(_events,sizeof(_events));
    int nEvents = epoll_wait(_epollfd,_events,nMaxEvents,timeout);
    if (nEvents<0) {
        perror("epoll_wait() failed");
        exit(-1);
    }
        //超时
    if (nEvents==0){
        printf("epoll_wait() timeout.\n");
        return evs;
    }
    //如果nEvents>0，表示有事件发生的fd的数量
    for (int i = 0; i < nEvents; i++){
        evs.push_back(_events[i]);
    }
    return evs;
}


