//
// Created by lalalilia on 5/10/20.
//

#ifndef UNTITLED1_TCPCONNECTION_H
#define UNTITLED1_TCPCONNECTION_H


#include <string>
#include "AddressOps.h"
#include "Callback.h"
#include <memory>
#include "Buffer.h"


class Channel;
class EventLoop;

class TcpConnection:public std::enable_shared_from_this<TcpConnection>{
public:

    TcpConnection(EventLoop* loop,
                  const std::string& name,
                  int socketfd,
                  const AddressOps& localAddr,
                  const AddressOps& peerAddr);

    ~TcpConnection();

    EventLoop* getLoop() const{return loop_;}
    const std::string& name() const{return name_;}
    const AddressOps& localAddr() const{return localAddr_;}
    const AddressOps& peerAddr() const{return peerAddr_;}


    void setConnectionCallback(const ConnectionCallback& cb){
        connectionCallback_ = cb;
    }
    void setMessageCallback(const MessageCallback& cb){
        messageCallback_ = cb;
    }
    void setCloseCallback(const CloseCallback& cb){
        closeCallback_ = cb;
    }

    bool connected() const{return state_==Connected;}

    void connectEstablished();
    void connectDestroyed();

    void send(const std::string& message);
    void sendInLoop(const std::string& message);
    void shutdown();
    void shutdownInLoop();
    void setTcpNoDelay(bool on);
private:
    enum State{Connecting,Connected,Disconnecting,Disconnected};

    void setState(State s){state_ = s;}

    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    State state_;
    EventLoop* loop_;
    std::string name_;
    std::unique_ptr<Channel> channel_;
    int sockfd_;

    AddressOps localAddr_;
    AddressOps peerAddr_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;

    Buffer inputBuffer_;
    Buffer outputBuffer_;
};






#endif //UNTITLED1_TCPCONNECTION_H
