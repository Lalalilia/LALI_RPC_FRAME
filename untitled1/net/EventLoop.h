//
// Created by lalalilia on 4/29/20.
//

#ifndef UNTITLED1_EVENTLOOP_H
#define UNTITLED1_EVENTLOOP_H

#include <boost/noncopyable.hpp>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include "../base/CurrentThread.h"
#include "TImerId.h"
#include "Callback.h"
#include "../base/Timestamp.h"
#include "../base/Mutex.h"

/*
ont loop per thread  保证每个线程只有一个EventLoop对象
*/

class Poller;
class Channel;
class TimerQueue;

class EventLoop:boost::noncopyable{
public:
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();

    // 负责事件的执行
    void loop();
    // 断言是否在循环线程内
    void assertInLoopThread(){
        if(!isInloopThread()){
            abortNotInLoopThread();
        }
    }
    // 判断是否处于循环线程内
    bool isInloopThread() const{
        return threadId_ == CurrentThread::tid();
    }

    void quit();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    TimerId runTimer(const TimerCallback&,const Timestamp&);
    TimerId runAfter(const TimerCallback&, double );
    TimerId runRepeat(const TimerCallback&, double);

    static EventLoop* getEventLoopOfCurrentThread();

    void doPendingFunctor();
    void runInLoop(const Functor&);
    void queueInLoop(const Functor&);

private:
    // 不在循环线程内退出
    void abortNotInLoopThread();
    bool looping_;
    bool quit_;
    const pid_t threadId_;

    typedef std::vector<Channel*> ChannelList;
    boost::scoped_ptr<Poller> poller_;
    ChannelList activeChannels_;
    TimerQueue* TimerQueue_;

    Mutex mutex_;
    bool callPendingFunctor_;
    std::vector<Functor> pendingFunctors_;
    int weakupFd_;
    boost::scoped_ptr<Channel> weakupChannel_;

    void weakup();
    void handleRead();


};

#endif //UNTITLED1_EVENTLOOP_H
