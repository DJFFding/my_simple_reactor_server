//网络通讯的客户端程序
#include "InetAddress.h"
#include "Log.hpp"
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"
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
    Socket serv_sock(Socket::create_nonblocking());
    serv_sock.set_reuse_addr(true);
    serv_sock.set_tcp_nodelay(true);
    serv_sock.set_reuse_port(true);
    serv_sock.set_keepalive(true);
    InetAddress servaddr(argv[1],atoi(argv[2]));
    serv_sock.bind(servaddr);
    serv_sock.listen();
    Epoll ep;
    Channel* servChannel = new Channel(&ep,serv_sock.fd(),true);
    servChannel->enableReading();
    vector<Channel*> channels;
    while (true){
        channels = ep.loop();
        //如果nEvents>0，表示有事件发生的fd的数量
        for (const auto& ch:channels){
            ch->handle_event(&serv_sock);
        }
    }
    return 0;
}