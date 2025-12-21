#ifndef _InetAddress_h_
#define _InetAddress_h_
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <strings.h>

//socket的地址协议类
class InetAddress
{
public:
    InetAddress();
    InetAddress(const char* ip,uint16_t port); //如果是监听的fd，用这个函数
    InetAddress(const sockaddr_in& addr);
    const char* ip()const; //返回字符串表示的地址
    uint16_t port()const;
    const sockaddr* addr()const; //返回成员_addr的地址
    InetAddress& operator=(const sockaddr_in& addr);
private:
    sockaddr_in _addr;
};

#endif