//
//Created by lalalilia
//

#ifndef UNTITLED1_MUTEX_H
#define UNTITLED1_MUTEX_H



#include <pthread.h>
#include "CurrentThread.h"
#include "assert.h"

// save auto
class Mutex {
public:

    Mutex(Mutex *pMutex)
            :holder_(0){
        int a = pthread_mutex_init(&mutex_,NULL);
        assert(a==0);(void) a;
    }

    ~Mutex(){
        assert(holder_==0);
        int a = pthread_mutex_destroy(&mutex_);
        assert(a==0);(void) a;
    }

    void lock(){
        pthread_mutex_lock(&mutex_);
        holder_ = CurrentThread::tid();
    }

    void unlock(){
        pthread_mutex_unlock(&mutex_);
        holder_ = 0;
    }

    bool isLockbyThisThread(){
        return holder_==CurrentThread::tid();
    }

    void assertLocked(){
        assert(isLockbyThisThread());
    }
    pthread_mutex_t* getThreadmutex_(){
        return &mutex_;
    }

    pthread_mutex_t* getMutex(){
        return &mutex_;
    }
private:

    pthread_mutex_t mutex_;
    pid_t holder_;
};

class MutexGuard
{
public:
    explicit MutexGuard(Mutex& mutex)
            :mutex_(mutex){
        mutex_.lock();
    }
    ~MutexGuard(){
        mutex_.unlock();
    }
private:
    Mutex& mutex_;
};


#endif //UNTITLED1_MUTEX_H
