#include "InetAddress.h"

InetAddress::InetAddress(const char *ip, uint16_t port)
{
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = inet_addr(ip);
    _addr.sin_port = htons(port);
}

InetAddress::InetAddress(const sockaddr_in &addr)
    :_addr(addr)
{

}

const char *InetAddress::ip() const
{
    return inet_ntoa(_addr.sin_addr);
}

uint16_t InetAddress::port() const
{
    return ntohs(_addr.sin_port);
}

const sockaddr *InetAddress::addr() const
{
    return (sockaddr*)&_addr;
}
