//
// Created by lalalilia on 4/29/20.
//

#include "Channel.h"
#include <poll.h>
#include "EventLoop.h"


//Channel.cc

const int Channel::NoneEventFlag = 0;
const int Channel::ReadEventFlag = POLLIN|POLLPRI;
const int Channel::WriteEventFlag = POLLOUT;

Channel::Channel(EventLoop* loop,int fd)
        :loop_(loop),
         fd_(fd),
         events_(0),
         currentEvents_(0),
         index_(-1),
         EventHanding_(false){

}

Channel::~Channel(){
    //assert(!EventHanding_);
}

void Channel::update(){
    loop_->updateChannel(this);
}

// 轮询读写事件，并对读写事件做出反应；
void Channel::handleEvent(){
    EventHanding_ = true;
    if(currentEvents_ & POLLNVAL){
        LOG<<"Channel::handleEvent() POLLNVAL";
    }

    if((currentEvents_&POLLHUP)&&!(currentEvents_&POLLIN)){
        if(closeCallback_)closeCallback_();
    }
    if(currentEvents_ & (POLLERR | POLLNVAL)){
        if(errorCallback_)errorCallback_();
    }
    if(currentEvents_ & (POLLIN | POLLPRI | POLLRDHUP)){
        if(readCallback_)readCallback_();
    }
    if(currentEvents_ & POLLOUT){
        if(writeCallback_)writeCallback_();
    }
    EventHanding_ = false;

}


void Channel::remove() {
    loop_->removeChannel(this);
}