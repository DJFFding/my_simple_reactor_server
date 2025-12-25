#include "Buffer.h"
#include <string.h>
Buffer::Buffer(uint16_t sep)
    :_sep(sep)
{
}

Buffer::~Buffer()
{
}

void Buffer::append(const char *data, size_t size)
{
    _buf.append(data,size);    
}

//把数据追加到_buf中，附加报文头部
void Buffer::append_with_head(const char *data,uint32_t size)
{
    if (_sep==0){ //表示没有分隔符
        _buf.append(data,size); //处理报文内容
    }else if(_sep==1){
        _buf.append((char*)&size,sizeof(size)); //处理报文长度
        _buf.append(data,size); //处理报文内容
    }else if(_sep==2){
        _buf.append(data,size); //处理报文内容
        _buf.append("\r\n\r\n");
    }
    
}

size_t Buffer::size() const
{
    return _buf.size();
}

const char *Buffer::data()
{
    return _buf.c_str();
}

void Buffer::clear()
{
    _buf.clear();
}

void Buffer::erase(size_t pos, size_t nBytes)
{
    _buf.erase(pos,nBytes);
}

bool Buffer::pick_message(std::string &ss)
{
    if (_buf.empty())return false;
    if (_sep==0){
        ss =_buf;
        _buf.clear();
    }else if(_sep==1){
        int32_t len;
        if (size()<sizeof(len))return false;
        memcpy(&len,data(),sizeof(len));
        if (size()<len+sizeof(len))return false;
        ss = std::string(data()+sizeof(len),len);
        erase(0,len+sizeof(len));
    }else if(_sep==2){
        return false; //TODO 以后实现
    }
    return true;
}
