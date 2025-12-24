#include "Timestamp.h"

Timestamp::Timestamp()
{
    _epoch_seconds = time(NULL);
}

Timestamp::Timestamp(int64_t epoch_seconds)
    :_epoch_seconds(epoch_seconds)
{
}

Timestamp Timestamp::now()
{
    return Timestamp(); //返回当前时间
}

std::string Timestamp::to_string() const
{
   char buf[512]={0};
   tm* tm_time = localtime(&_epoch_seconds);
   snprintf(buf,512,"%04d-%02d-%02d %02d:%02d:%02d",
    tm_time->tm_year+1900,
    tm_time->tm_mon+1,
    tm_time->tm_mday,
    tm_time->tm_hour,
    tm_time->tm_min,
    tm_time->tm_sec);
    return buf;
}

time_t Timestamp::toInt() const
{
    return _epoch_seconds;
}
