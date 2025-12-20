//网络通讯的客户端程序
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>
#include <sys/epoll.h>

int main(int argc,char* argv[])
{
    if (argc!=3){
        printf("usage: ./tcpepoll ip port\n");
        printf("example:./tcpepoll 191.168.150.126 8923\n\n");
        return -1;
    }
    
    //创建服务器用于监听的listenfd
    int listenfd = socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,IPPROTO_TCP);
    if (listenfd<0){
        perror("socket() failed");
        return -1;
    }
    
    //设置listenfd的属性
    int opt=1;
    socklen_t opt_len= sizeof(opt);
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,opt_len);
    setsockopt(listenfd,IPPROTO_TCP,TCP_NODELAY,&opt,opt_len);
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEPORT,&opt,opt_len);

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));
    if (bind(listenfd,(sockaddr*)&servaddr,sizeof(servaddr))<0)
    {
        perror("bind() failed");
        close(listenfd);
        return -1;
    }
    
    if (listen(listenfd,SOMAXCONN)!=0)
    {
        perror("listen() failed");
        close(listenfd);
        return -1;
    }
    int epollfd = epoll_create(1);

    epoll_event ev;
    ev.data.fd = listenfd;
    ev.events = EPOLLIN;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&ev);
    epoll_event evs[10];
    while (true){
        int nEvents = epoll_wait(epollfd,evs,10,-1);
        if (nEvents<0) {
            perror("epoll_wait() failed");
            break;
        }
        //超时
        if (nEvents==0){
            printf("epoll_wait() timeout.\n");
            continue;
        }
        
        //如果nEvents>0，表示有事件发生的fd的数量
        for (int i = 0; i < nEvents; i++){
            if (evs[i].data.fd == listenfd){ //如果是listenfd有事件，表示有新的客户端连接
                sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                //从已连接队列中取客户端的fd
                int clientfd = accept4(listenfd,(sockaddr*)&clientaddr,&len,SOCK_NONBLOCK);
                printf("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientfd,inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
                ev.data.fd = clientfd;
                ev.events = EPOLLIN|EPOLLET;
                epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&ev);
            }else{ //如果是客户端连接的fd有事件
                if (evs[i].events&EPOLLRDHUP){
                    printf("EPOLLRDHUP client(eventfd=%d) disconnected.\n",evs[i].data.fd);
                    close(evs[i].data.fd);
                }
                else if(evs[i].events&EPOLLIN){
                    char buffer[1024];
                    while (true){ //由于使用非阻塞IO，一次读取buffer大小数据，直到全部数据读取完毕
                        bzero(&buffer,sizeof(buffer));
                        ssize_t nread = read(evs[i].data.fd,buffer,sizeof(buffer));
                        if (nread>0){
                            //把接收到的报文内容原封不动的发回去
                            printf("recv(eventfd=%d):%s\n",evs[i].data.fd,buffer);
                            send(evs[i].data.fd,buffer,nread,0);
                        }else if(nread==0){ //客户端连接已断开
                            printf("read=0 client(eventfd=%d) disconnected.\n",evs[i].data.fd);
                            close(evs[i].data.fd); //关闭客户端的fd
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
                else if(evs[i].events&EPOLLOUT){

                }else{
                    printf("client(eventfd=%d) error.\n",evs[i].data.fd);
                    close(evs[i].data.fd);
                }
            }
        }
    }
    


    return 0;
}