//
// Created by lalalilia on 5/6/20.
//

#include "SocketOps.h"
#include <fcntl.h>
#include "../base/Logging.h"
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>


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
        LOG<<"sockets::accept error";

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