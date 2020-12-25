//
//Created by lalalilia
//

#include "SocketOps.h"
#include <fcntl.h>
#include "../base/Logging.h"
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>


void setNonBlockAndCloseOnExec(int socketfd){
    //non-block
    int flags = ::fcntl(socketfd,F_GETFL,0);
    flags|=O_NONBLOCK;
    int ret = ::fcntl(socketfd,F_SETFL,flags);

    //Close-Exec
    flags = ::fcntl(socketfd,F_GETFD,0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(socketfd,F_SETFD,flags);
}

int socketOps::creatNonBlocking() {
    int sockfd = ::socket(AF_INET,
                     SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,
                     IPPROTO_TCP);
    if(sockfd<0){
        LOG<<"sockets::create error";

    }
    return sockfd;
}

void socketOps::bind(int socketfd, const struct sockaddr_in &addr) {
    int ret = ::bind(socketfd,(const struct sockaddr*)&addr,sizeof addr);
    if(ret<0){
        LOG<<"sockets::bind error";

    }
}


int socketOps::accept(int socketfd, struct sockaddr_in *addr) {
    socklen_t addrlen = sizeof *addr;

    int connfd = ::accept4(socketfd,(struct sockaddr*)addr,
            &addrlen,SOCK_NONBLOCK|SOCK_CLOEXEC);
    if(connfd<0){
        int savedErrno = errno;
        LOG << "Socket::accept error "<<savedErrno;
        switch (savedErrno)
        {
            case EAGAIN:LOG << "Socket::EAGAIN";break;
            case ECONNABORTED:LOG << "Socket::ECONNABORTED";break;
            case EINTR:LOG << "Socket::EINTR";break;
            case EPROTO: LOG << "Socket::EPROTO";break;
            case EPERM:LOG << "Socket::EPERM";break;
            case EMFILE: // per-process lmit of open file desctiptor ???
                // expected errors
                LOG << "Socket::EMFILE";break;
                break;
            case EBADF:LOG << "Socket::EBADF";break;
            case EFAULT:LOG << "Socket::EFAULT";break;
            case EINVAL:LOG << "Socket::EINVAL";break;
            case ENFILE:LOG << "Socket::ENFILE";break;
            case ENOBUFS:LOG << "Socket::ENOBUFS";break;
            case ENOMEM:LOG << "Socket::ENOMEM";break;
            case ENOTSOCK:LOG << "Socket::ENOTSOCK";break;
            case EOPNOTSUPP:
                // unexpected errors
                LOG << "unexpected error of ::accept " << savedErrno;
                break;
            default:
                LOG << "unknown error of ::accept " << savedErrno;
                break;
        }
    }
    return connfd;
}

void socketOps::close(int socketfd) {
    if(::close(socketfd)<0){
        LOG<<"socket::close error";
    }
}

void socketOps::setReuseAddr(int sockfd,bool on) {
    int optval=on?1:0;
    ::setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof optval);
}

void socketOps::listen(int socketfd) {
    int ret = ::listen(socketfd,SOMAXCONN);
    if(ret<0){
        LOG << " socketOps::listen error";
    }
}

void socketOps::toHostPort(char *buf, size_t size, const struct sockaddr_in &addr) {
    char host[INET_ADDRSTRLEN] = "INVALID";
    ::inet_ntop(AF_INET,&addr.sin_addr,host,sizeof host);
    uint16_t port = socketOps::netwrokToHost16(addr.sin_port);
    snprintf(buf,size," %s:%u ",host,port);
}

void socketOps::fromHostPort(const char *ip, __uint16_t port, struct sockaddr_in *addr) {

}

int socketOps::getSocketError(int socketfd) {
    int optval;
    socklen_t optlen = sizeof optval;

    if(::getsockopt(socketfd,SOL_SOCKET,SO_ERROR,&optval,&optlen)<0){
        return errno;
    }
    else{
        return optval;
    }
}

void socketOps::shutdown(int socketfd) {
    if(::shutdown(socketfd,SHUT_WR)<0){
        LOG<<" socketOPs::shutdown error ";
    }
}

void socketOps::setTcpNoDelay(int sockfd) {
    int opt=1;
    ::setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY,
                  &opt,sizeof opt);
}

struct sockaddr_in socketOps::getPeerAddr(int sockfd) {
    struct sockaddr_in peeraddr{};
    memset(&peeraddr,0, sizeof(peeraddr));
    auto addrLen = static_cast<socklen_t >(sizeof peeraddr);
    if(::getpeername(sockfd,(struct sockaddr*)&peeraddr,&addrLen) < 0){
        LOG << "socketOps::getPeerAddr";
    }
    return peeraddr;
}
struct sockaddr_in socketOps::getLocalAddr(int sockfd) {
    struct sockaddr_in localaddr{};
    memset(&localaddr,0, sizeof(localaddr));
    auto addrLen = static_cast<socklen_t >(sizeof localaddr);
    if(::getsockname(sockfd,(struct sockaddr*)&localaddr,&addrLen) < 0){
        LOG << "socketOps::getLocalAddr";
    }
    return localaddr;
}
