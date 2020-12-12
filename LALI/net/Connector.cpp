//
// Created by lalalilia
//

#include <net/Connector.h>
#include <base/Logging.h>
#include <net/EventLoop.h>
#include <net/SocketOps.h>
#include <net/Channel.h>

#include <memory>
#include <cerrno>



Connector::Connector(EventLoop *loop, const AddressOps &serverAddr)
    :loop_(loop),
    serverAddr_(serverAddr),
    retryDelatMs_(KInitRetryDelayMs),
    connect_(false){

}

void Connector::start() {
    connect_ = true;
    loop_->runInLoop(std::bind(&Connector::startInLoop,this));
}

void Connector::stop() {
    loop_->queueInLoop(std::bind(&Connector::stopInLoop,this));

}

void Connector::startInLoop() {
    loop_->assertInLoopThread();
    if(connect_){
        connect();
    }
}

void Connector::stopInLoop() {
    loop_->assertInLoopThread();
    int sockfd = removeAndResetChannel();
    retry(sockfd);
}

void Connector::connect() {
    int sockfd = socketOps::creatNonBlocking();
    int ret = ::connect(sockfd,
            reinterpret_cast<const struct sockaddr*> (&serverAddr_.getSockAddr()),
            sizeof(serverAddr_.getSockAddr()));
    int Errno = (ret == 0) ? 0 : errno;
    connecting(sockfd);
}

void Connector::connecting(int sockfd) {
    assert(!channel_);
    channel_ = std::make_unique<Channel>(loop_,sockfd);
    channel_->setWriteCallback(
            std::bind(&Connector::handleWrite,this));
    channel_->enableWriting();
}

void Connector::handleWrite() {
    int sockfd = removeAndResetChannel();
    int err = socketOps::getSocketError(sockfd);
    if(err){
        LOG << "Connector::handleWrite  = "
            << err << " " << strerror(err);
        retry(sockfd);
    }
    // todo selfConnect;
    else{
        if(connect_){
            newConnectionCallback_(sockfd);
        }
        else {
            socketOps::close(sockfd);
        }
    }

}

int Connector::removeAndResetChannel() {
    channel_->disableAll();
    channel_->remove();
    int sockfd = channel_->get_fd();
    loop_->queueInLoop(std::bind(&Connector::resetChannel,this));
    return sockfd;

}

void Connector::resetChannel() {channel_.reset();}

void Connector::retry(int sockfd) {
    socketOps::close(sockfd);
    if(connect_){
        loop_->runAfter(std::bind(&Connector::startInLoop,this),
                retryDelatMs_/1000.0);
        retryDelatMs_ = std::min(retryDelatMs_*2,Connector::kMaxRetryDelayMs);
    }
    else
        LOG << " do not connect ";

}

void Connector::restart() {
    loop_->assertInLoopThread();
    retryDelatMs_ = KInitRetryDelayMs;
    connect_ = true;
    startInLoop();
}

Connector::~Connector() {
    assert(!channel_);
}