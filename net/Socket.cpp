//
// Created by lalalilia on 10/15/20.
//

#include "Socket.h"
#include "SocketOps.h"
#include "AddressOps.h"

Socket::~Socket(){
    socketOps::close(sockfd_);
}

void Socket::bindAddress(const AddressOps& localAddr) {
    socketOps::bind(sockfd_,localAddr.getSockAddr());
}

void Socket::listen() {
    socketOps::listen(sockfd_);
}

int Socket::accept(AddressOps *peerAddr) {
    struct sockaddr_in addr{};
    int connfd = socketOps::accept(sockfd_,&addr);
    if(connfd>0){
        peerAddr->setAddrClient_(addr);
    }
    return connfd;
}

void Socket::shutdownWrite() {
    socketOps::shutdown(sockfd_);
}

void Socket::setReuseAddr(bool on) {
    socketOps::setReuseAddr(sockfd_,on);
}

void Socket::setKeepAlive(bool on) {
    int optval = on?1:0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_KEEPALIVE,&optval, static_cast<socklen_t >(sizeof(optval)));
}