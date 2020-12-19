//
// Created by lalalilia 
//

#ifndef UNTITLED1_ASYNCLOGGING_H
#define UNTITLED1_ASYNCLOGGING_H

#include "Logstream.h"
#include "Thread.h"
#include "Mutex.h"
#include "Condition.h"
#include "CountDownLatch.h"
#include <vector>
#include <atomic>


class AsyncLogging{
public:
    explicit AsyncLogging(const char* filename,off_t rollSize = 1024,int flushInterval = 3);

    ~AsyncLogging(){
        if(running_){
            stop();
        }
    }

    void append(const char* msg,int len);

    void start(){
        running_ = true;
        thread_.start();
        latch_.wait();
    }

    void stop(){
        running_ = false;
        cond_.weakup();
        thread_.join();
    }

    //void AsyncAppend();

private:

    void threadFunc();
    typedef FixedBuffer<kLargerBuffer> Buffer;
    typedef std::unique_ptr<Buffer> BufferPtr;
    typedef std::vector<BufferPtr> BufferptrVector;

    const int flushInterval_;
    std::atomic<bool> running_;
    //bool running_;
    const char* filename_;
    Thread thread_;
    Mutex mutex_;
    CountDownLatch latch_;
    Condition cond_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferptrVector buffers_;
    off_t rollSize_ ;
};

#endif //UNTITLED1_ASYNCLOGGING_H
