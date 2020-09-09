//
// Created by lalalilia on 4/9/20.
//

#ifndef UNTITLED1_BLOCKINGQUEUE_H
#define UNTITLED1_BLOCKINGQUEUE_H

#include <deque>
#include "Condition.h"
#include <deque>
#include "Mutex.h"

template<typename T>
class BlockingQueue{
public:
    BlockingQueue()
            : mutex_(nullptr),
             cond_(mutex_),
             queue_(){
    }

    ~BlockingQueue(){

    }

    void put(const T& obj){
        MutexGuard lock(mutex_);
        queue_.push_back(obj);
        cond_.weakup();
    }

    T take(){
        MutexGuard lcok(mutex_);
        while(queue_.empty()){
            cond_.wait();
        }
        T currentTask(queue_.front());
        queue_.pop_front();
        return currentTask;
    }

    size_t size() const{
        MutexGuard lock(mutex_);
        return queue_.size();
    }

private:

    mutable Mutex mutex_;
    Condition cond_;
    std::deque<T> queue_;

};

#endif //UNTITLED1_BLOCKINGQUEUE_H
