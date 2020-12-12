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
        peerAddr_(peerAddr),
        socket_(new Socket(socketfd)),
        highWaterMark_(64*1024*1024)
        {
    setTcpNoDelay(true);
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead,this));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite,this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose,this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError,this));
    socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection(){
    //socketOps::close(sockfd_);
}


void TcpConnection::connectEstablished() {
    loop_->assertInLoopThread();
    setState((Connected));
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed(){
    loop_->assertInLoopThread();
    //assert(state_==Connected);
    setState(Disconnected);
    channel_->disableAll();
    connectionCallback_(shared_from_this());


    loop_->removeChannel(channel_.get());

}

void TcpConnection::handleRead() {
    int saveError;
    //使用buffer来读取socket数据，怎么确保将socket的数据读完呢？一次readv可以读完fd中到数据；
    ssize_t n=inputBuffer_.readFd(socket_->fd(),&saveError);
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
    assert(state_==Connected||state_==Disconnecting);
    channel_->disableAll();
    TcpConnectionPtr guardThis(shared_from_this());
    closeCallback_(guardThis);
}

void TcpConnection::send(const std::string &message) {
    if(state_==Connected){
        if(loop_->isInloopThread()){
            sendInLoop(message,message.size());
        }
        else {
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message,message.size()));
        }
    }
}
void TcpConnection::sendInLoop(const std::string &message,  size_t len) {
    loop_->assertInLoopThread();
    size_t remaining = len;
    ssize_t n=0;
    //确保buffer里没有可读事件，如果存在可读事件则不能写入，因为说明上一次需要发送到数据没有发送完毕，
    // 需要将新数据追加到buffer后面，保证数据发送准确性
    if(!channel_->isWriting()&&outputBuffer_.readableBytes()==0){
        n=::write(socket_->fd(),message.data(),message.size());
        if(n>=0){
            remaining = len - n;
            //低水位回调，在缓冲区为空的时候回调
            if(remaining == 0 && writeCompleteCallback_){
                loop_->queueInLoop(std::bind(writeCompleteCallback_,shared_from_this()));
            }
        }
        else{
            n=0;
            LOG<<"Tcpconnection::sendInLoop";
        }

    }
    //如果write没有写入message的全部数据，就要利用buffer来存储剩余数据
    if(remaining > 0){
        size_t oldLen = outputBuffer_.readableBytes();
        //高水位回调，在发送缓冲区高于设定值到时候回调
        if(oldLen+remaining>highWaterMark_&&oldLen<highWaterMark_&&highWaterMarkCallback_){
            loop_->queueInLoop(std::bind(highWaterMarkCallback_,shared_from_this(),oldLen+remaining));
        }
        outputBuffer_.append(message.data()+n,message.size()-n);
        if(!channel_->isWriting()){
            //注册POLLOUT可写事件，一旦变得可写就立刻发送数据
            channel_->enableWriting();
        }
    }

    if(static_cast<size_t >(n)<message.size()){
        outputBuffer_.append(message.data()+n,message.size()-n);
        if(!channel_->isWriting()){
            //注册POLLOUT可写事件，一旦变得可写就立刻发送数据
            channel_->enableWriting();
        }
    }
}
void TcpConnection::handleWrite() {
    loop_->assertInLoopThread();
    if(channel_->isWriting()){
        ssize_t n=::write(socket_->fd(),
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
              //  LOG<<" will write more data ";
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
        socket_->shutdownWrite();
    }
}

void TcpConnection::handleError() {
    int err = socketOps::getSocketError(socket_->fd());

}

void TcpConnection::setTcpNoDelay(bool on) {
    if(on)socketOps::setTcpNoDelay(socket_->fd());
}

void TcpConnection::forceClose() {
    loop_->queueInLoop(std::bind(&TcpConnection::handleClose,this));
}