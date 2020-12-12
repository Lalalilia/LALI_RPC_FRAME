//
// Created by lalalilia
//

#ifndef ZILIPROJECT_CONNECTOR_H
#define ZILIPROJECT_CONNECTOR_H

#include <net/AddressOps.h>
#include <functional>
#include <memory>

class Channel;
class EventLoop;

class Connector
    :public std::enable_shared_from_this<Connector>{
public:
    typedef std::function<void(int sockfd)> NewConnectionCallback;
    Connector(EventLoop* loop,const AddressOps& serverAddr);
    ~Connector() ;
    void setNewConnectionCallback(const NewConnectionCallback& callback){
        newConnectionCallback_ = callback;
    }
    const AddressOps& serverAddress() const{return serverAddr_;}

    void start();
    void stop();
    void restart();

private:
    static const int KInitRetryDelayMs = 500;
    const int kMaxRetryDelayMs = 30*1000;

    void startInLoop();
    void stopInLoop();
    void connect();
    void connecting(int sockfd);
    void handleWrite();
    void retry(int sockfd);
    int removeAndResetChannel();
    void resetChannel();

private:
    EventLoop* loop_;
    bool connect_;
    std::unique_ptr<Channel> channel_;
    NewConnectionCallback newConnectionCallback_;
    AddressOps serverAddr_;
    int retryDelatMs_;
};



#endif //ZILIPROJECT_CONNECTOR_H
