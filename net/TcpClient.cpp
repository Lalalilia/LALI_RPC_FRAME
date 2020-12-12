//
// Created by lalalilia
//


#include <net/TcpClient.h>
#include <base/Logging.h>
#include <net/Connector.h>
#include <net/EventLoop.h>
#include <net/SocketOps.h>
#include <stdio.h>
#include <functional>
#include <net/Callback.h>

using  std::placeholders::_1;

TcpClient::TcpClient(EventLoop *loop,
        const AddressOps &serverAddr,
        const std::string &nameArg)
    :loop_(loop),
    connector_(new Connector(loop,serverAddr)),
    name_(nameArg),
    connectionCallback_(),
    messageCallback_(),
    retry_(false),
    connect_(true),
    nextConnId_(1),
    mutex_(nullptr)
{
    connector_->setNewConnectionCallback(
            std::bind(&TcpClient::newConnection,this,_1));
}

void removeConnectInLoop(EventLoop* loop,const TcpConnectionPtr& conn){
    loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed,conn));
}

TcpClient::~TcpClient() {
    TcpConnectionPtr conn;
    bool unique = false;
    {
        MutexGuard lock(mutex_);
        unique = connection_.unique();
        conn = connection_;
    }
    if(conn){
        CloseCallback cb = std::bind(&removeConnectInLoop,loop_,_1);
        loop_->runInLoop(
                std::bind(&TcpConnection::setCloseCallback,conn,cb));
        if(unique){
            conn->forceClose();
        }
    }
    else{
        connector_->stop();
    }
}

void TcpClient::connect() {
    connect_ = true;
    connector_->start();
}

void TcpClient::disconnect() {
    connect_ = false;
    {
        MutexGuard lock(mutex_);
        if(connection_){
            connection_->shutdown();
        }
    }
}

void TcpClient::stop()
{
    connect_ = false;
    connector_->stop();
}

void TcpClient::newConnection(int sockfd) {
    loop_->assertInLoopThread();
    AddressOps peerAddr(socketOps::getPeerAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof buf,":%s#%d",
            peerAddr.toHostPort().c_str(),nextConnId_);
    nextConnId_++;
    string connName = name_ + buf;
    AddressOps localAddr(socketOps::getLocalAddr(sockfd));
    TcpConnectionPtr conn( new TcpConnection(loop_,
                                             connName,
                                             sockfd,
                                             localAddr,
                                             peerAddr));

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(
            std::bind(&TcpClient::removeConnection,this,_1)
            );
    {
        MutexGuard lock(mutex_);
        connection_ = conn;
    }
    conn->connectEstablished();

}

void TcpClient::removeConnection(const TcpConnectionPtr &conn) {
    loop_->assertInLoopThread();
    {
        MutexGuard lock(mutex_);
        connection_.reset();
    }

    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed,conn));
    if(retry_ && connect_){
        connector_->restart();
    }

}