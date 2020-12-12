//
// Created by lalalilia on 5/7/20.
//

#ifndef UNTITLED1_ACCEPTOR_H
#define UNTITLED1_ACCEPTOR_H

#include "AddressOps.h"
#include "EventLoop.h"
#include "Channel.h"
#include "SocketOps.h"
#include "Socket.h"

class Acceptor{
public:
    typedef std::function<void (int fd,const AddressOps& InetAddress)>
            NewConnectionCallback;

    Acceptor(EventLoop*,const AddressOps&);


    void setNewConnectionCallback(const NewConnectionCallback& cb){
        NewConnectionCallback_ = cb;
    }

    bool listenning() const{return listenning_;}
    void listen();

private:
    int accept(AddressOps* peeraddr);
    void handleRead();
    //int acceptSocketfd_;
    EventLoop* loop_;
    bool listenning_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback NewConnectionCallback_;
    int idleFd_;


};
#endif //UNTITLED1_ACCEPTOR_H
