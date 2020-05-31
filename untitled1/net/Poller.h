//
// Created by lalalilia on 4/29/20.
//

#ifndef UNTITLED1_POLLER_H
#define UNTITLED1_POLLER_H


#include <boost/noncopyable.hpp>
#include <vector>
#include "EventLoop.h"
#include <map>
#include "../base/Timestamp.h"


// 使用 poller 来捕捉事件信号；
// 前向声明一个结构体；
struct epoll_event;
class Channel;

class Poller:boost::noncopyable{

public:
    typedef std::vector<Channel*> ChannelList;

    explicit Poller(EventLoop*);
    ~Poller();

    Timestamp poll(int timeoutMs,ChannelList* activeChannels);
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    const char* operatortoString(int op);
    void update(int operation,Channel* channel);

    void assertInLoopThread(){
        ownerLoop_->assertInLoopThread();
    }

private:

    void fillActiveChannels(int numEvents,
                            ChannelList* activeChannels) const;
    //typedef std::vector<struct pollfd> PollFdList;
    typedef std::map<int,Channel*>ChannelMap;
    typedef std::vector<struct epoll_event> EventList;


    EventLoop* ownerLoop_;
    //PollFdList pollfdList_;
    ChannelMap channels_;

    static const int InitEventListSize=16;
    int epollfd_;
    EventList events_;

};
#endif //UNTITLED1_POLLER_H
