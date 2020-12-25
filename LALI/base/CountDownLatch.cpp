//
//Created by lalalilia
//
#include "CountDownLatch.h"
#include "Mutex.h"

CountDownLatch::CountDownLatch(int count)
        : mutex_(nullptr),
         cond_(mutex_),
         count_(count){
}

CountDownLatch::~CountDownLatch(){
}

void CountDownLatch::wait(){
    MutexGuard lock(mutex_);
    while(count_>0){
        cond_.wait();
    }
}

void CountDownLatch::countDown(){
    MutexGuard lock(mutex_);
    count_--;
    if(count_==0) {
        cond_.weakupAll();
    }
}



int CountDownLatch::getcount() const{
    MutexGuard lock(mutex_);
    return count_;
}


