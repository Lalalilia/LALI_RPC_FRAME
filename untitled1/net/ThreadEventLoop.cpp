//
// Created by lalalilia on 5/5/20.
//

#include "ThreadEventLoop.h"


ThreadEventLoop::ThreadEventLoop()
    :mutex_(nullptr),
    cond_(mutex_),
    loop_(),
    thread_(std::bind(&ThreadEventLoop::threadFunc,this)){

}

ThreadEventLoop::~ThreadEventLoop() {
    loop_->quit();
    thread_.join();
}

EventLoop* ThreadEventLoop::startLoop() {

    assert(!thread_.started());
    thread_.start();

    {
        MutexGuard lock(mutex_);
        while(loop_==nullptr){
            cond_.wait();
        }
    }

    return loop_;

}

void ThreadEventLoop::threadFunc() {

    EventLoop loop;

    {
        MutexGuard lock(mutex_);
        loop_ = &loop;
        cond_.weakup();
    }

    loop_->loop();
}
