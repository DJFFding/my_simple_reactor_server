#include "Buffer.h"

Buffer::Buffer()
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
    _buf.append((char*)&size,sizeof(size)); //处理报文长度
    _buf.append(data,size); //处理报文内容
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
