//
// Created by lalalilia on 5/13/20.
//

#ifndef UNTITLED1_BUFFER_H
#define UNTITLED1_BUFFER_H

#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>
#include <assert.h>

/*
 *  |prependspace  |  readablespace    |   writablespace    |
 *  0       readableIndex       writableIndex           size（）
 *
 *  CheapPrepend = 8;
 *  InitialSize = 1024
 *
 */

class Buffer{
public:
    static const size_t CheapPrepend = 8;
    static const size_t InitialSize = 1024;
    Buffer()
    :buffer_(CheapPrepend+InitialSize),
     readerIndex_(CheapPrepend),
     writerIndex_(CheapPrepend){

    }
    void swap(Buffer& rhs){
        buffer_.swap(rhs.buffer_);
        std::swap(readerIndex_,rhs.readerIndex_);
        std::swap(writerIndex_,rhs.writerIndex_);

    }
    // 空间段容量返回；
    size_t readableBytes() const{return writerIndex_-readerIndex_;}
    size_t writableBytes() const{return buffer_.size()-writerIndex_;}
    size_t prependableBytes() const{return readerIndex_;}

    const char* peek() const{return begin()+readerIndex_;}

    // 为解析http请求
    const char* findCRLF() const{
        std::string scrlf("\r\n");
        const char* crlf=std::search(peek(),beginWrite(),scrlf.begin(),scrlf.end());
        return crlf==beginWrite()?NULL:crlf;
    }

    const char* findCRLF(const char* start) const{
        assert(peek()<=start);
        assert(start<=beginWrite());
        std::string scrlf("\r\n");
        const char* crlf=std::search(start,beginWrite(),scrlf.begin(),scrlf.end());
        return crlf==beginWrite()?NULL:crlf;
    }

    // 读取完毕后，移动reader指针，取回读取的空间
    void retrieve(size_t len){
        if(len < readableBytes())
            readerIndex_+=len;
        else
            retrieveAll();
    }
    void retrieveUntil(const char* end){
        retrieve(end-peek());
    }
    void retrieveAll(){
        readerIndex_=CheapPrepend;
        writerIndex_=CheapPrepend;
    }
    std::string retrieveAsString(){
        std::string str(peek(),readableBytes());
        retrieveAll();
        return str;
    }

    // 在buffer后面追加字符；
    void append(const char* data,size_t len){
        ensureWritableBytes(len);
        std::copy(data,data+len,beginWrite());
        hasWritten(len);
    }
    void append(const void* data,size_t len){
        append(static_cast<const char*>(data),len);
    }
    void append(const std::string& str){
        append(str.data(),str.length());
    }
    void ensureWritableBytes(size_t len){
        if(writableBytes()<len){
            makeSpace(len);
        }
    }

    // 写部分函数
    char* beginWrite(){return begin()+writerIndex_;}
    const char* beginWrite() const{return begin()+writerIndex_;}
    void hasWritten(size_t len){writerIndex_+=len;}

    // 在buffer头部追加一些信息。
    void prepend(const void* data,size_t len){
        readerIndex_-=len;
        const char* d= static_cast<const char*>(data);
        std::copy(d,d+len,begin()+readerIndex_);
    }
    // 把一些空余空间填满，避免空间碎片。
    void shrink(size_t reserve){
        std::vector<char> buf(CheapPrepend+readableBytes()+reserve);
        std::copy(peek(),peek()+readableBytes(),buf.begin()+CheapPrepend);
        buf.swap(buffer_);
    }

    ssize_t readFd(int fd,int* savedErrno);

private:
    // 返回buffer的头部
    char* begin(){return &*buffer_.begin();}
    const char* begin() const{return &*buffer_.begin();}

    // 制造空间，目的是为了回收多次读写造成的内存碎片。
    void makeSpace(size_t len){
        // 如果检测到头部+可写部分空间不够写，就要对buffer进行扩容；
        if(writableBytes()+prependableBytes()<len+CheapPrepend){
            buffer_.resize(writerIndex_+len);
        }
        // 头部+可写空间足够，说明有内存碎片，则重整一下buffer；
        else{
            size_t readbale=readableBytes();
            std::copy(begin()+readerIndex_,
                      begin()+writerIndex_,
                      begin()+CheapPrepend);
            readerIndex_=CheapPrepend;
            writerIndex_=readerIndex_+readbale;
        }
    }

    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;

};
#endif //UNTITLED1_BUFFER_H
