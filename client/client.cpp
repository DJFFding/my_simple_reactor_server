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
     bzero(buf,sizeof(buf));
     int nPos=0;
     while (true){
        int nLen = recv(sockfd,buf+nPos,sizeof(buf)-nPos,0);
        if (nLen<0){
            perror("recv() failed.");
            close(sockfd);
            return -1;
        }
        if (nLen==0){
            break;
        }
        nPos+=nLen;
       
        while(true){
            int len=0;
            memcpy(&len,buf,4);
            if (nPos<len+4){
                break;
            }
            std::string message(buf+4,len);
            printf("recv:%s\n",message.data());
            nPos-=len+4;
            memcpy(buf,buf+len+4,nPos);
        }
     }
     return 0;    
}