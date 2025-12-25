#include "Epoll.h"
#include "Log.hpp"
#include "Channel.h"
#include <sys/syscall.h>
#include <unistd.h>
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

void Epoll::update_channel(Channel *ch)
{
    epoll_event ev;
    ev.data.ptr = ch;
    ev.events = ch->events();
    if(ch->is_in_epoll()){
        if(epoll_ctl(_epollfd,EPOLL_CTL_MOD,ch->fd(),&ev)==-1){
            LOG_PERRORE()<<"epoll_ctl() failed";
            exit(-1);   
        }
    }else{
        if(epoll_ctl(_epollfd,EPOLL_CTL_ADD,ch->fd(),&ev)==-1){
            LOG_PERRORE()<<"epoll_ctl() failed";
            exit(-1);   
        }
        ch->makeAddEpoll();
    }
}

void Epoll::remove_channel(Channel *ch)
{
    if(ch->is_in_epoll()){
        if(epoll_ctl(_epollfd,EPOLL_CTL_DEL,ch->fd(),nullptr)==-1){
            LOG_PERRORE()<<"epoll_ctl() failed";
            exit(-1);   
        }
        LOGI()<<"remove_channel";
    }
}

void Epoll::add_event(int fd,void *ptr, uint32_t op)
{
    epoll_event ev;
    ev.data.ptr=ptr;
    ev.events = op;
    if(epoll_ctl(_epollfd,EPOLL_CTL_ADD,fd,&ev)==-1){
        LOG_PERRORE()<<"epoll_ctl() failed";
        exit(-1);   
    }
}

vector<Channel*> Epoll::loop(int timeout)
{
    vector<Channel*> channels;
    vector<Channel*> timeout_channels;
    bzero(_events,sizeof(_events));
    int nEvents = epoll_wait(_epollfd,_events,nMaxEvents,timeout);
    if (nEvents<0) {
        if (errno==EINTR){
            channels.push_back((Channel*)-1);
            return channels;            
        }
        perror("epoll_wait() failed");
        exit(-1);
    }
    //超时
    if (nEvents==0){
        return channels;
    }
    //如果nEvents>0，表示有事件发生的fd的数量
    for (int i = 0; i < nEvents; i++){
        Channel* ch =(Channel*)_events[i].data.ptr;
        ch->set_revents(_events[i].events);
        if (ch->isTimeout()){
            timeout_channels.push_back(ch);
        }else{
            channels.push_back(ch);
        }
    }
    for (auto time_ch:timeout_channels){
        channels.push_back(time_ch);
    }
    return channels;
}
