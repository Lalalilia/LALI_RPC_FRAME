//
// Created by lalalilia on 4/16/20.
//

#include "Logstream.h"
#include <iostream>
#include <algorithm>


const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

template <typename  T>
size_t convert(char buf[],T value){
    T i = value;
    char* p = buf;

    do{
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    }while(i != 0);

    if(value < 0){
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf,p);
    return p - buf;
}

template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargerBuffer>;

template<typename T>
void Logstream::formatInteger(T v){
    if(buffer_.avail() >= kMaxNumericSize){
        size_t len = convert(buffer_.getcur(),v);
        buffer_.add(len);
    }
}

Logstream& Logstream::operator<<(short v){
    *this << static_cast<int>(v);
    return *this;
}


Logstream& Logstream::operator<<(unsigned short v)
{
    *this << static_cast<unsigned int>(v);
    return *this;
}

Logstream& Logstream::operator<<(int v)
{
    formatInteger(v);
    return *this;
}

Logstream& Logstream::operator<<(unsigned int v)
{
    formatInteger(v);
    return *this;
}

Logstream& Logstream::operator<<(long v)
{
    formatInteger(v);
    return *this;
}

Logstream& Logstream::operator<<(unsigned long v)
{
    formatInteger(v);
    return *this;
}

Logstream& Logstream::operator<<(long long v)
{
    formatInteger(v);
    return *this;
}

Logstream& Logstream::operator<<(unsigned long long v)
{
    formatInteger(v);
    return *this;
}

Logstream& Logstream::operator<<(double v)
{
    if (buffer_.avail() >= kMaxNumericSize)
    {
        int len = snprintf(buffer_.getcur(), kMaxNumericSize, "%.12g", v);
        buffer_.add(len);
    }
    return *this;
}