#ifndef _Buffer_h_
#define _Buffer_h_
#include <string>
#include <iostream>
#include <stdint.h>

class Buffer
{
public:
    Buffer(uint16_t sep=1);
    ~Buffer();

    void append(const char*data,size_t size); //把数据追加到_buf中
    void append_with_head(const char*data,uint32_t size);
    size_t size() const; //返回_buf的大小
    const char* data(); //返回_buf的首地址
    void clear(); //清空_buf
    void erase(size_t pos,size_t nBytes);
    bool pick_message(std::string& ss);

private:
    std::string _buf; //用于存放数据
    const uint16_t _sep; //报文的分隔符：0-无分隔符(固定长度、视频会议)；1-四字节的报头；2-"\r\n\r\n"分隔符(http协议)
};

#endif