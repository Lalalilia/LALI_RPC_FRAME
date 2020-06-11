//
// Created by lalalilia on 5/13/20.
//




#include <errno.h>
#include <sys/uio.h>


#include "Buffer.h"

// 在fd中读文件，先使用buffer内的剩余空间，同时开辟了一段栈空间，若buffer内剩余不够用，
// 就先把数据往栈空间内存，之后append到buffer中去。
ssize_t Buffer::readFd(int fd, int *savedErrno) {
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base=begin()+writerIndex_;
    vec[0].iov_len=writable;
    vec[1].iov_base=extrabuf;
    vec[1].iov_len=sizeof extrabuf;
    const ssize_t n=readv(fd,vec,2);
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