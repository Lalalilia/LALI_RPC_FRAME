//
// Created by lalalilia on 4/11/20.
//

#ifndef UNTITLED1_BOUNDBLOCKINGQUEUE_H
#define UNTITLED1_BOUNDBLOCKINGQUEUE_H


#include <deque>
#include "Condition.h"
#include <boost/noncopyable.hpp>
#include <deque>
#include "Mutex.h"


template <typename T>
class BoundedBlockingQueue{

public:
    BoundedBlockingQueue(int size)
            : mutex_(nullptr),
             notFUll_(mutex_),
             notEmpty_(mutex_),
             queue_(),
             size_(size){

    }


    ~BoundedBlockingQueue();

    void put(const T& x){
        MutexGuard lock(mutex_);
        while(queue_.full()){
            notFUll_.wait();
        }
        queue_.push_back(x);
        notEmpty_.weakup();
    }

    T take(){
        MutexGuard lock(mutex_);
        while(queue_.empty){
            notEmpty_.wait();
        }
        T n(queue_.front());
        queue_.pop_front();
        notFUll_.weakup();
        return n;
    }


    bool full(){
        MutexGuard lock(mutex_);
        return size == queue_.size();
    }

    bool empty(){
        MutexGuard lock(mutex_);
        return queue_.size() == 0 ? true : false;
    }

    int size() const{
        MutexGuard lock(mutex_);
        return size;
    }


private:
    Mutex mutex_;
    Condition notFUll_;
    Condition notEmpty_;
    int size_;
    std::deque<T> queue_;
};



#endif //UNTITLED1_BOUNDBLOCKINGQUEUE_H
