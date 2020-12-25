//
//Created by lalalilia
//

#include "ThreadEventLoop.h"


ThreadEventLoop::ThreadEventLoop()
    :mutex_(nullptr),
    cond_(mutex_),
    loop_(NULL),
    thread_(std::bind(&ThreadEventLoop::threadFunc,this)){

}

ThreadEventLoop::~ThreadEventLoop() {

    if(loop_!=NULL){
        loop_->quit();
        thread_.join();
    }

}

EventLoop* ThreadEventLoop::startLoop() {

    assert(!thread_.started());
    thread_.start();
    EventLoop* loop=NULL;
    {
        MutexGuard lock(mutex_);
        while(loop_==nullptr){
            cond_.wait();
        }
        loop=loop_;
    }
    return loop;
}

void ThreadEventLoop::threadFunc() {

    EventLoop loop;

    {
        MutexGuard lock(mutex_);
        loop_ = &loop;
        cond_.weakup();
    }
    loop.loop();

    MutexGuard lock(mutex_);
    loop_=NULL;

}
