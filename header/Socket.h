#ifndef _Socket_h_
#define _Socket_h_
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "InetAddress.h"

class Socket
{
public:
    Socket(int fd);
    ~Socket();
    int fd()const;
    const char* ip()const;
    uint16_t port()const;
    void set_ip_port(const char*ip,uint16_t port);
    void set_reuse_addr(bool on);
    void set_reuse_port(bool on);
    void set_tcp_nodelay(bool on);
    void set_keepalive(bool on);
    void bind(const InetAddress& servAddr);
    void listen(int nMaxConn=SOMAXCONN);
    int accept(InetAddress&clientAddr);
    static int create_nonblocking();
private:
    const int _sockfd; //Socket持有的fd，在构造函数中传进来
    std::string _ip;
    uint16_t _port;
};



#endif