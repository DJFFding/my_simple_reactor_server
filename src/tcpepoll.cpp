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
using namespace std;

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
    EchoServer server(argv[1],atoi(argv[2]));
    server.start();
    return 0;
}