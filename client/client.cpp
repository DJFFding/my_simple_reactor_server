//网络通讯的客户端程序
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <string>

int main(int argc, char * argv[])
{
    if(argc!=3){
        printf("usage:./client ip port\n");
        printf("example:./client 127.0.0.1 4396\n\n");
        return -1;
    }
    int sockfd;
    struct sockaddr_in servaddr;
    char buf[1024];
    if ((sockfd = socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,0))<0) {
        printf("socket() failed/\n");
        return -1;
    }
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    printf("sockfd=%d\n",sockfd);
    if (connect(sockfd,(sockaddr*)&servaddr,sizeof(servaddr))!=0){
        if (errno!=EINPROGRESS){
            perror("connect");
            printf("connect(%s:%s) failed.\n",argv[1],argv[2]);
            close(sockfd);
            return -1;
        }
    }
    fd_set write_fd;
    int max_fd = sockfd;
    FD_ZERO(&write_fd);
    FD_SET(sockfd,&write_fd);
    select(max_fd+1,nullptr,&write_fd,nullptr,nullptr);
    printf("connect ok.\n");

    for (int i = 0; i < 100; i++){
        //从命令行输入内容
        memset(buf,0,sizeof(buf));
        sprintf(buf,"这是第%d个超级女生。",i);

        char temp_buf[1024]; //临时的buffer，报文头部+报文内容
        bzero(temp_buf,sizeof(temp_buf));
        int len =strlen(buf);
        memcpy(temp_buf,&len,4);
        memcpy(temp_buf+4,buf,len);

        if (send(sockfd,temp_buf,len+4,0)<=0){
            printf("write() failed.\n");
            close(sockfd);
            return -1;
        }
    }
     std::string buffer;
     bzero(buf,sizeof(buf));
     while (true){
        while (true){
            int nLen = recv(sockfd,buf,sizeof(buf),0);
            if (nLen<0){
                if (errno==EINTR)continue;
                else if (errno==EAGAIN||errno==EWOULDBLOCK){
                    break;
                }
                perror("recv() failed.");
                close(sockfd);
                return -1;
            }
            else if (nLen==0){
                break;
            }
            buffer.append(buf,nLen);
        }
       
        while(true){
            int len=0;
            if(buffer.size()<4){
                break;
            }
            memcpy(&len,buffer.data(),4);
            if (buffer.size()<len+4){
                break;
            }
            buffer.erase(0,4);
            std::string message(buffer,0,len);
            printf("recv:%s\n",message.data());
            buffer.erase(0,len);
        }
     }
     return 0;    
}