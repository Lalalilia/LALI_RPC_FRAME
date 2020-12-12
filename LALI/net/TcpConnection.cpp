//
// Created by lalalilia on 5/10/20.
//

#include "Channel.h"
#include "TcpConnection.h"
#include "SocketOps.h"
#include <unistd.h>

TcpConnection::TcpConnection(EventLoop *loop,
        const std::string& name,
        int socketfd,
        const AddressOps &localAddr,
        const AddressOps &peerAddr)
        :loop_(loop),
        name_(name),
        channel_(new Channel(loop,socketfd)),
        localAddr_(localAddr),
        peerAddr_(peerAddr)
        {
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead,this));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite,this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose,this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError,this));
}

TcpConnection::~TcpConnection(){

}


void TcpConnection::connectEstablished() {
    loop_->assertInLoopThread();
    setState((Connected));
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed(){
    loop_->assertInLoopThread();
    assert(state_==Connected);
    setState(Disconnected);
    channel_->disableAll();
    connectionCallback_(shared_from_this());

    loop_->removeChannel(channel_.get());

}

void TcpConnection::handleRead() {
    int saveError;
    ssize_t n=inputBuffer_.readFd(channel_->get_fd(),&saveError);
    if(n>0){
        messageCallback_(shared_from_this(),&inputBuffer_,n);
    }
    else if(n==0){
        handleClose();
    }
    else{
        handleError();
    }
}

void TcpConnection::handleClose() {
    loop_->assertInLoopThread();
    assert(state_==Connected);
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::send(const std::string &message) {
    if(state_==Connected){
        if(loop_->isInloopThread()){
            sendInLoop(message);
        }
        else {
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message));
        }
    }
}
void TcpConnection::sendInLoop(const std::string &message) {
    loop_->assertInLoopThread();
    ssize_t n=0;
    if(!channel_->isWriting()&&outputBuffer_.readableBytes()==0){
        n=::write(channel_->get_fd(),message.data(),message.size());
        if(n>=0){
            if(static_cast<size_t >(n)<message.size()){
                LOG<<" will write more data ";
            }
        }
        else{
            n=0;
            LOG<<"Tcpconnection::sendInLoop";
        }

    }

    if(static_cast<size_t >(n)<message.size()){
        outputBuffer_.append(message.data()+n,message.size()-n);
        if(!channel_->isWriting()){
            channel_->enableWriting();
        }
    }
}
void TcpConnection::handleWrite() {
    loop_->assertInLoopThread();
    if(channel_->isWriting()){
        ssize_t n=::write(channel_->get_fd(),
                outputBuffer_.peek(),
                outputBuffer_.readableBytes());
        if(n>0){
            outputBuffer_.retrieve(n);
            if(outputBuffer_.readableBytes()==0){
                channel_->disableWritinf();
                if(state_==Disconnecting) {
                    shutdownInLoop();
                }
            }
            else{
                LOG<<" will write more data ";
            }
        }
        else {
            LOG<<" TcpConnection::handleWrite error ";
        }
    }
    else{
        LOG<<" Connection is down ";
    }


}
void TcpConnection::shutdown() {
    if(state_==Connected){
        setState(Disconnecting);
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop,this));
    }
}
void TcpConnection::shutdownInLoop() {
    loop_->assertInLoopThread();
    if(!channel_->isWriting()){
        socketOps::shutdown(channel_->get_fd());
    }
}

void TcpConnection::handleError() {
    int err = socketOps::getSocketError(channel_->get_fd());

}

void TcpConnection::setTcpNoDelay(bool on) {
    if(on)socketOps::setTcpNoDelay(channel_->get_fd());
}