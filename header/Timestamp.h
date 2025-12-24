#ifndef _Timestamp_h_
#define _Timestamp_h_
#include <iostream>
#include <string>

//时间戳
class Timestamp
{
public:
    Timestamp();
    Timestamp(int64_t epoch_seconds);
    static Timestamp now();
    std::string to_string()const;

    time_t toInt() const;
private:
    time_t _epoch_seconds; //1970 until now
};
#endif