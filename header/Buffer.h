#ifndef _Buffer_h_
#define _Buffer_h_
#include <string>
#include <iostream>
#include <stdint.h>

class Buffer
{
public:
    Buffer();
    ~Buffer();

    void append(const char*data,size_t size); //把数据追加到_buf中
    void append_with_head(const char*data,uint32_t size);
    size_t size() const; //返回_buf的大小
    const char* data(); //返回_buf的首地址
    void clear(); //清空_buf
    void erase(size_t pos,size_t nBytes);

private:
    std::string _buf; //用于存放数据
};

#endif