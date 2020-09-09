//
// Created by lalalilia on 4/8/20.
//

#ifndef UNTITLED1_COUNTDOWNLATCH_H
#define UNTITLED1_COUNTDOWNLATCH_H

/// 闭锁类，为实现线程同步；

#include "Mutex.h"
#include "Condition.h"

class CountDownLatch {

public:
    CountDownLatch(int count);

    ~CountDownLatch();

    void countDown();

    void wait();

    int getcount() const;

private:
    mutable Mutex mutex_;
    int count_;
    Condition cond_;

};



#endif //UNTITLED1_COUNTDOWNLATCH_H
