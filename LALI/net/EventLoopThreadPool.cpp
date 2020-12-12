//
// Created by lalalilia on 5/18/20.
//

#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "ThreadEventLoop.h"
#include <stdio.h>


EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseloop)
    :next_(0),
    started_(false),
    baseloop_(baseloop),
    numThreads_(0)
    {

}

EventLoopThreadPool::~EventLoopThreadPool() {

}

void EventLoopThreadPool::start() {
    baseloop_->assertInLoopThread();
    started_=true;
    for(int i=0;i<numThreads_;i++){
        ThreadEventLoop* t = new ThreadEventLoop;
        threads_.push_back(std::unique_ptr<ThreadEventLoop>(t));
        loops_.push_back(t->startLoop());
    }

}

void EventLoopThreadPool::setThreadNums(int nums) {
    numThreads_=nums;
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    baseloop_->assertInLoopThread();
    EventLoop* loop=baseloop_;
    if(!loops_.empty()){
        loop = loops_[next_];
        next_++;
        if((next_)>=loops_.size()){
            next_=0;
        }
    }
    return loop;
}