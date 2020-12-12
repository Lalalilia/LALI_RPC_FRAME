//
// Created by lalalilia on 5/13/20.
//




#include <errno.h>
#include <sys/uio.h>


#include "Buffer.h"

// 在fd中读文件，先使用buffer内的剩余空间，同时开辟了一段栈空间，若buffer内剩余不够用，
// 就先把数据往栈空间内存，之后append到buffer中去。
//使用LT，一次读完数据，不会丢失数据
// 兼顾链接公平性，不会因为一个链接数据过大而影响其他链接；、
// 节省了一次 ioctl，不必事先知道有多少数据可读而预留空间；
ssize_t Buffer::readFd(int fd, int *savedErrno) {
    char extrabuf[65536]; // 空间直接取自栈上
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base=begin()+writerIndex_;
    vec[0].iov_len=writable;
    vec[1].iov_base=extrabuf;
    vec[1].iov_len=sizeof extrabuf;
    const ssize_t n=readv(fd,vec,2); //一次调用读完数据；输入缓冲区足够大，一次读取后将extrabuf到数据append到buffer后面
    if(n<0){
        *savedErrno=errno;
    }
    else if(static_cast<size_t>(n)<=writable){
        writerIndex_+=n;
    }
    else{
        writerIndex_=buffer_.size();
        append(extrabuf,n-writable);
    }
    return n;
}