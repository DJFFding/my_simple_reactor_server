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
    if ((sockfd = socket(AF_INET,SOCK_STREAM,0))<0) {
        printf("socket() failed/\n");
        return -1;
    }
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    printf("sockfd=%d\n",sockfd);
    if (connect(sockfd,(sockaddr*)&servaddr,sizeof(servaddr))!=0){
        perror("connect");
        printf("connect(%s:%s) failed.\n",argv[1],argv[2]);
        close(sockfd);
        return -1;
    }
    printf("connect ok.\n");

    for (int i = 0; i < 200000; i++){
        //从命令行输入内容
        memset(buf,0,sizeof(buf));
        printf("please input:");
        scanf("%s",buf);
        if (send(sockfd,buf,strlen(buf)+1,0)<=0){
            printf("write() failed.\n");
            close(sockfd);
            return -1;
        }
        bzero(buf,sizeof(buf));
        if (recv(sockfd,buf,sizeof(buf),0)<=0){//接收服务端的回应
            printf("read() failed.\n");
            close(sockfd);
            return -1;
        }
        printf("recv:%s\n",buf);
    }
    return 0;    
}