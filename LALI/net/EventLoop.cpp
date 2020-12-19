//
// Created by lalalilia on 4/29/20.
//


// EventLoop.cpp

#include "EventLoop.h"
#include "../base/CurrentThread.h"
#include "../base/Logging.h"
#include "Channel.h"
#include "Poller.h"
#include <string>
#include "TimerQueue.h"
#include <sys/eventfd.h>
#include "signal.h"
#include <unistd.h>

__thread EventLoop* t_loopInThisThread = nullptr;

 /// ignore sigpipe to avoid server drop;
class IgnoreSigPipe{
public:
    IgnoreSigPipe(){
        ::signal(SIGPIPE,SIG_IGN);
    }
};
IgnoreSigPipe initObj;

static int creatEventFd(){
    int evtfd = ::eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);
    if(evtfd < 0){
        LOG<<"Failed to create Eventfd";
        abort();
    }
    return evtfd;
}

void EventLoop::weakup() {
    uint64_t one = 1;
    ssize_t n = ::write(weakupFd_,&one,sizeof one);
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t  n = ::read(weakupFd_,&one,sizeof one);
}

void EventLoop::runInLoop(const Functor& cb) {
    if(isInloopThread()){
        cb();
    }
    else{
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const Functor& cb) {
    {
        MutexGuard lock(mutex_);
        pendingFunctors_.push_back(cb);
    }

    if(!isInloopThread()||callPendingFunctor_){
        weakup();
    }
}

void EventLoop::doPendingFunctor() {
    std::vector<Functor> functors;
    callPendingFunctor_ = true;
    {
        MutexGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for(const Functor& functor:functors){
        functor();
    }

    callPendingFunctor_ = false;

}

EventLoop::EventLoop()
        :looping_(false),
         threadId_(CurrentThread::tid()),
         quit_(false),
         poller_(new Poller(this)),
         TimerQueue_(new TimerQueue(this)),
         callPendingFunctor_(false),
         weakupFd_(creatEventFd()),
         weakupChannel_(new Channel(this,weakupFd_)),
         mutex_(nullptr){
    LOG<<" EventLoop created "<< reinterpret_cast<const char *>(this)
      <<" in thread "<< threadId_;
    if(t_loopInThisThread){
        LOG<<" A EventLoop "<< reinterpret_cast<const char*> (t_loopInThisThread)
           <<" exists in the thread: "<<threadId_;
    }
    else{
        t_loopInThisThread = this;
    }
    weakupChannel_->setReadCallback(std::bind(&EventLoop::handleRead,this));
    weakupChannel_->enableReading();
}

EventLoop::~EventLoop(){
    assert(!looping_);
    t_loopInThisThread = nullptr;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread(){
    return t_loopInThisThread;
}

void EventLoop::quit(){
    quit_ = true;
}

void EventLoop::updateChannel(Channel* channel){
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);

}

void EventLoop::removeChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->removeChannel(channel);
}


void EventLoop::loop(){

    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;
    // 不断执行，使用poll获取事件，填满loop向量组不断执行，同时执行完一次poll事件后，执行pengding事件；
    while(!quit_){
        activeChannels_.clear();
        poller_->poll(10000,&activeChannels_);
        for(auto & activeChannel : activeChannels_){
            activeChannel->handleEvent();
        }
        doPendingFunctor();
    }

   LOG<<" EventLoop "<<reinterpret_cast<const char *>(this)<<" stop looping ";
    looping_ = false;

}

void EventLoop::abortNotInLoopThread() {
   LOG << " EventLoop::abortNotInLoopThread - EventLoop "<< reinterpret_cast<const char *>(this)
    << " threadId is "<<threadId_
    << " current threadid is " << CurrentThread::tid();
}

TimerId EventLoop::runTimer(const TimerCallback& cb,const Timestamp& delayTime) {
    return TimerQueue_->addTimer(cb,delayTime,0.0);
}

TimerId EventLoop::runAfter(const TimerCallback& cb, double delayTime) {
    Timestamp time(addTime(Timestamp::now(),delayTime));
    return runTimer(cb,time);
}

TimerId EventLoop::runRepeat(const TimerCallback& cb, double interval) {
    Timestamp time(addTime(Timestamp::now(),interval));
    return TimerQueue_->addTimer(cb, time,interval);
}

