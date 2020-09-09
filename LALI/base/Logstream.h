//
// Created by lalalilia on 4/16/20.
//

#ifndef UNTITLED1_LOGSTREAM_H
#define UNTITLED1_LOGSTREAM_H


#include <string.h>
#include <string>

const int kSmallBuffer = 4000;
const int kLargerBuffer = 4000*1000;


template<int SIZE>
class FixedBuffer{
public:
    FixedBuffer()
            :cur_(data_){

    }
    ~FixedBuffer()= default;

    void append(const char* buf,size_t len){
        if(static_cast<size_t>(avail())>len){
            memcpy(cur_,buf,len);
            cur_+=len;
        }
    }

    size_t avail() const {return static_cast<size_t>(end()-cur_);}

    void add(size_t len){cur_+=len;}

    char* getcur(){return cur_;}

    const char* data() const{return data_;}

    void reset(){cur_ = data_;}

    void bzero(){ memset(data_,0, sizeof(data_));}

    size_t length() const{return static_cast<size_t>(cur_ - data_);}

    std::string toString() const{return std::string(data_);}

private:
    const char* end() const{return data_ + sizeof data_;}
    char data_[SIZE]{};
    char* cur_;
};


class Logstream {

public:
    typedef FixedBuffer<kSmallBuffer> Buffer;

    // 重载操作符；
    Logstream& operator<<(bool v){
        buffer_.append(v?"1":"0",1);
        return *this;
    }
    Logstream& operator<<(short v);
    Logstream& operator<<(unsigned short v);
    Logstream& operator<<(int v);
    Logstream& operator<<(unsigned int v);
    Logstream& operator<<(long v);
    Logstream& operator<<(unsigned long v);
    Logstream& operator<<(long long v);
    Logstream& operator<<(unsigned long long v);

    Logstream& operator<<(float v){
        *this << static_cast<double>(v);
        return *this;
    }

    Logstream& operator<<(double v);
    Logstream& operator<<(const void* v);

    Logstream& operator<<(char v){
        buffer_.append(&v,1);
        return *this;
    }

    Logstream& operator<<(const char* v){
        if(v){
            buffer_.append(v,strlen(v));
        }
        else{
            buffer_.append("(null)",6);
        }
        return *this;
    }
    Logstream& operator<<(const unsigned char* v){
        return operator<<(reinterpret_cast<const char*>(v));
    }
    Logstream& operator<<(const std::string v){
        buffer_.append(v.c_str(),v.size());
        return *this;
    }

    void append(const char* data,int len){buffer_.append(data,len);}

    const Buffer& getbuffer() const{return buffer_;}

    void resetBuffer(){buffer_.reset();}

private:

    template<typename T>
    void formatInteger(T);

    static const int kMaxNumericSize = 32;

    Buffer buffer_;

};

#endif //UNTITLED1_LOGSTREAM_H
