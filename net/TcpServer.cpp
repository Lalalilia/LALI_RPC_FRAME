//
// Created by lalalilia on 5/10/20.
//

#include "TcpServer.h"
#include "stdio.h"
#include "TcpConnection.h"

#include <memory>


TcpServer::TcpServer(EventLoop * loop, const AddressOps & InetAddr)
    :loop_(loop),
    name_(InetAddr.toHostPort()),
    started_(false),
    acceptor_(new Acceptor(loop,InetAddr)),
    connId_(1),
    threadPool_(new EventLoopThreadPool(loop)){
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection,this,
                                         std::placeholders::_1,std::placeholders::_2));
}

TcpServer::~TcpServer() {

}

void TcpServer::start() {
    if(!started_){
        started_ = true;
    }
    threadPool_->start();
    if(!acceptor_->listenning()){
        loop_->runInLoop(
                std::bind(&Acceptor::listen,acceptor_.get()));
    }

}

void TcpServer::newConnection(int socketfd, const AddressOps & peerAddr) {
    char buf[32];
    snprintf(buf,sizeof buf,"#%d",connId_);
    ++connId_;
    std::string connName = name_+buf;

    LOG<<" New Tcpconnection -- ["<<connName<<"]"<<" from ["<<peerAddr.toHostPort()<<"] ";
    AddressOps localAddr = peerAddr.getSockAddr();
    EventLoop* ioLoop = threadPool_->getNextLoop();
    TcpConnectionPtr conn(new TcpConnection(ioLoop,connName,socketfd,localAddr,peerAddr));
    connection_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::destroyConnection,this, std::placeholders::_1));
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished,conn));

}

void TcpServer::destroyConnection(const TcpConnectionPtr& conn){
    //LOG<<" delete Tcpconnection -- ["<<conn->name()<<"]";
    loop_->runInLoop(std::bind(&TcpServer::destroyConnectionInLoop,this,conn));
}

void TcpServer::destroyConnectionInLoop(const TcpConnectionPtr &conn) {
    loop_->assertInLoopThread();
    LOG<<" remove Tcpconnection -- ["<<conn->name()<<"]";
    connection_.erase(conn->name());
    EventLoop* ioLoop=conn->getLoop();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed,conn));
}

void TcpServer::setThreadNums(int nums) {
    assert(0<=nums);
    threadPool_->setThreadNums(nums);
}


