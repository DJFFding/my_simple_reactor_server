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
