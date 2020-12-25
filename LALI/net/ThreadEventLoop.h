//
//Created by lalalilia
//

#ifndef UNTITLED1_THREADEVENTLOOP_H
#define UNTITLED1_THREADEVENTLOOP_H

#include "../base/Thread.h"
#include "../base/Mutex.h"
#include "../base/Condition.h"
#include "EventLoop.h"

class ThreadEventLoop{
public:
    ThreadEventLoop();
    ~ThreadEventLoop();

    EventLoop* startLoop();

private:

    void threadFunc();
    EventLoop* loop_;
    Thread thread_;
    Mutex mutex_;
    Condition cond_;


};


#endif //UNTITLED1_THREADEVENTLOOP_H
