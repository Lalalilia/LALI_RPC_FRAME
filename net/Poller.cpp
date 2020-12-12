//
// Created by lalalilia on 4/29/20.
//


//Poller.cc


#include "Poller.h"
#include "Channel.h"

#include "../base/Logging.h"

#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>


const int New=-1;
const int Deleted=2;
const int Add=1;


Poller::Poller(EventLoop* loop)
        :ownerLoop_(loop),
        epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
        events_(InitEventListSize){
    if(epollfd_<0){
        LOG<<"Poller::Poller Error";
    }
}

Poller::~Poller(){
    ::close(epollfd_);
}

// 把活跃事IO件写入pollfdList_中；
Timestamp Poller::poll(int timeoutMs,ChannelList* activeChannels){

    int numEvents=::epoll_wait(epollfd_,
            &*events_.begin(),
            static_cast<int>(events_.size()),
            timeoutMs);
    int savedErrno = errno;
    Timestamp now(Timestamp::now());
    if(numEvents>0){
        LOG<<numEvents<<" events happened ";
        fillActiveChannels(numEvents,activeChannels);
        if(static_cast<size_t >(numEvents)==events_.size()){
            events_.resize(events_.size()*2);
        }

    }
    else if(numEvents==0){
        LOG<<" nothing happened ";
    }
    else{
        if(savedErrno!=EINTR){
            errno = savedErrno;
            LOG<<" Poller::poll() ";
        }
    }
    return now;

}

// 依托于pollfdList中的活跃事件fd配合ChannelMap来找到对应Channel
// 然后把这个Channel填入activeChannel 中；
void Poller::fillActiveChannels(int numEvents,
                                ChannelList* activeChannels) const{
    for(int i=0;i<numEvents;++i){
        auto* channel= static_cast<Channel*>(events_[i].data.ptr);
        channel->set_currentEvents(events_[i].events);
        activeChannels->push_back(channel);
    }

}


void Poller::updateChannel(Channel* channel){
    assertInLoopThread();
    const int idx=channel->get_index();
    LOG<<" fd= "<<channel->get_fd()<<" events= "<<channel->get_events()<<" index= "<<idx;
    if(idx==New||idx==Deleted){
        int fd=channel->get_fd();
        if(idx==New){
            channels_[fd]=channel;
        }
        channel->set_index(Add);
        update(EPOLL_CTL_ADD,channel);
    }
    else{
        int fd=channel->get_fd();
        if(channel->isNoneEvent()){
            update(EPOLL_CTL_DEL,channel);
            channel->set_index(Deleted);
        }
        else{
            update(EPOLL_CTL_MOD,channel);
        }
    }
}

void Poller::update(int operation, Channel *channel) {
    struct epoll_event event{};
    event.events=channel->get_events();
    event.data.ptr=channel;
    int fd=channel->get_fd();
    if(::epoll_ctl(epollfd_,operation,fd,&event)<0){
        LOG<<" epoll_ctl op = "<< operatortoString(operation)<<" fd = "<<fd;
    }
}

const char* Poller::operatortoString(int op) {
    switch (op){
        case EPOLL_CTL_DEL:
            return "DEL";
        case EPOLL_CTL_ADD:
            return "ADD";
        case EPOLL_CTL_MOD:
            return "MOD";
        default:
            return "Unknown Operation";
    }

}

void Poller::removeChannel(Channel *channel) {
    assertInLoopThread();
    int fd=channel->get_fd();
    int index=channel->get_index();
    channels_.erase(fd);

    if(index==1){
        update(EPOLL_CTL_DEL,channel);
    }
    channel->set_index(New);

}
