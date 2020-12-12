//
// Created by lalalilia on 5/8/20.
//

#include "Acceptor.h"
#include "SocketOps.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

Acceptor::Acceptor(EventLoop* loop, const AddressOps& listenAddr)
    :loop_(loop),
    listenning_(false),
    acceptSocket_(socketOps::creatNonBlocking()),
    idleFd_(::open(("/dev/null"),O_RDONLY | O_CLOEXEC)),
    acceptChannel_(loop,acceptSocket_.fd())
    {

    acceptSocket_.setReuseAddr(true);
    acceptSocket_.bindAddress(listenAddr);

    //socketOps::setReuseAddr(acceptSocketfd_,true);
   // socketOps::bind(acceptSocketfd_,listenAddr.getSockAddr());
    acceptChannel_.setReadCallback(
            std::bind(&Acceptor::handleRead,this));
}


int Acceptor::accept(AddressOps *peeraddr) {
    return acceptSocket_.accept(peeraddr);
}

void Acceptor::listen() {
    loop_->assertInLoopThread();
    listenning_ = true;
    acceptSocket_.listen();
    //socketOps::listen(acceptSocketfd_);
    acceptChannel_.enableReading();
}


void Acceptor::handleRead(){
    loop_->assertInLoopThread();
    AddressOps peerAddr(0);
    int connfd = acceptSocket_.accept(&peerAddr);
    if(connfd>=0){
        if(NewConnectionCallback_){
            NewConnectionCallback_(connfd,peerAddr);
        }
        else{
            socketOps::close(connfd);
        }
    }
    else
    {
        LOG << "in Acceptor::handleRead" << errno;
        if (errno == EMFILE)
        {
            ::close(idleFd_);
            idleFd_ = ::accept(acceptSocket_.fd(), NULL, NULL);
            ::close(idleFd_);
            idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }

}