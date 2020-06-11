//
// Created by lalalilia on 4/8/20.
//

#ifndef UNTITLED1_CONDITION_H
#define UNTITLED1_CONDITION_H
/// 一个条件变量的类，为了更好的实现线程同步；

#include <boost/noncopyable.hpp>
#include <pthread.h>
#include "Mutex.h"
#include <cerrno>


class Condition : boost::noncopyable{
public:
    explicit Condition(Mutex& mutex)
    :mutex_(mutex)
    {
        pthread_cond_init(&pcond_,NULL);
    }

    ~Condition(){
        pthread_cond_destroy(&pcond_);
    }

    void wait(){
        pthread_cond_wait(&pcond_,mutex_.getMutex());
    }

    void weakup(){
        pthread_cond_signal(&pcond_);
    }

    void weakupAll(){
        pthread_cond_broadcast(&pcond_);
    }

    bool timedWait(int seconds){
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME,&abstime);
        abstime.tv_sec+=seconds;
        return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getMutex(), &abstime);
    }

private:
    Mutex& mutex_;
    pthread_cond_t pcond_{};
};


#endif //UNTITLED1_CONDITION_H
