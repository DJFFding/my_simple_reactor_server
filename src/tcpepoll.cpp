//网络通讯的客户端程序
#include "InetAddress.h"
#include "Log.hpp"
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"
#include "EventLoop.h"
#include "EchoServer.h"
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
#include <signal.h>
using namespace std;

EchoServer * echoServer=nullptr;

void Stop(int sig) // 信号2和15的处理函数，功能是停止服务程序
{
    LOGI()<<"sig="<<sig;
    LOGI()<<"EchoServer已停止.";
    delete echoServer;
}

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
    signal(SIGINT,Stop); //信号2,Ctrl+C发出的信号
    signal(SIGTERM,Stop); //信号15，系统kill或killall命令默认发送对的信号

    echoServer = new EchoServer(argv[1],atoi(argv[2]));
    echoServer->start();
    return 0;
}