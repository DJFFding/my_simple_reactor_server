//网络通讯的客户端程序
#include "InetAddress.h"
#include "Log.hpp"
#include "Socket.h"
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
    Log::SetLogWriterFunc([](const LogData& d) {
		std::cout << Log::ToString(d) << std::endl;
	});
    Socket serv_sock(Socket::create_nonblocking());
    serv_sock.set_reuse_addr(true);
    serv_sock.set_tcp_nodelay(true);
    serv_sock.set_reuse_port(true);
    serv_sock.set_keepalive(true);
    InetAddress servaddr(argv[1],atoi(argv[2]));
    serv_sock.bind(servaddr);
    serv_sock.listen();

    int epollfd = epoll_create(1);

    epoll_event ev;
    ev.data.fd = serv_sock.fd();
    ev.events = EPOLLIN;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,serv_sock.fd(),&ev);
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
            if (evs[i].data.fd == serv_sock.fd()){ //如果是listenfd有事件，表示有新的客户端连接
                InetAddress clientaddr;
                Socket* clientSock = new Socket(serv_sock.accept(clientaddr));
                printf("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientSock->fd(),clientaddr.ip(),clientaddr.port());
                ev.data.fd = clientSock->fd();
                ev.events = EPOLLIN|EPOLLET;
                epoll_ctl(epollfd,EPOLL_CTL_ADD,clientSock->fd(),&ev);
                continue;
            }else{ //如果是客户端连接的fd有事件
                if (evs[i].events&EPOLLRDHUP){
                    printf("EPOLLRDHUP client(eventfd=%d) disconnected.\n",evs[i].data.fd);
                    close(evs[i].data.fd);
                    continue;
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