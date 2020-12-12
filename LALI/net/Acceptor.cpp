//
// Created by lalalilia on 5/8/20.
//

#include "Acceptor.h"
#include "SocketOps.h"


Acceptor::Acceptor(EventLoop* loop, const AddressOps& listenAddr)
    :loop_(loop),
    listenning_(false),
    acceptSocketfd_(socketOps::creatNonBlocking()),
    acceptChannel_(loop,acceptSocketfd_)
    {
    socketOps::setReuseAddr(acceptSocketfd_,true);
    socketOps::bind(acceptSocketfd_,listenAddr.getSockAddr());
    acceptChannel_.setReadCallback(
            std::bind(&Acceptor::handleRead,this));
}


int Acceptor::accept(AddressOps *peeraddr) {
    struct sockaddr_in addr{};
    int connfd = socketOps::accept(acceptSocketfd_,&addr);
    if(connfd>0){
        peeraddr->setAddrClient_(addr);
    }
    return connfd;
}

void Acceptor::listen() {
    loop_->assertInLoopThread();
    listenning_ = true;
    socketOps::listen(acceptSocketfd_);
    acceptChannel_.enableReading();
}


void Acceptor::handleRead(){
    loop_->assertInLoopThread();
    AddressOps peerAddr(0);
    int connfd = Acceptor::accept(&peerAddr);
    if(connfd>0){
        if(NewConnectionCallback_){
            NewConnectionCallback_(connfd,peerAddr);
        }
        else{
            socketOps::close(connfd);
        }
    }
}