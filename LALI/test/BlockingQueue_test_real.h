

#include <iostream>
#include "Thread.h"
#include "Exception.h"
#include <cstdio>
#include "CurrentThread.h"
#include "ThreadLocal.h"
#include "unistd.h"
#include "Mutex.h"
#include "Condition.h"
#include "CountDownLatch.h"
#include "BlockingQueue.h"
#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <stdio.h>
#include <algorithm>


using  namespace  std;

int n=0;
ThreadLocal<int> object;

Mutex Amutex;

Condition cond_(Amutex);

// 生产者消费者模型；
class test{

public:
    test(int numThreads)
            :Latch_(numThreads),
             Threads_(numThreads){
        for(int i=0;i<numThreads;i++){
            char name[40];
            snprintf(name,sizeof name,"thread %d",i);
            Threads_.push_back(new Thread(boost::bind(&test::ThreadFun, this),std::string(name)));
        }
        for_each(Threads_.begin(),Threads_.end(),boost::bind(&Thread::start,_1));
    }

    void put(int times){
        cout << "wait" << endl;
        sleep(1);
        Latch_.wait();
        cout << "all threads stared"<< endl;
        for(int i=0;i<times;i++){
            char name[30];
            snprintf(name,sizeof name,"THE number %d",i);
            queue_.put(name);
        }
    }

    void joinAll(){

        for(size_t i=0;i<Threads_.size();i++){
            queue_.put("stop");
        }
        for_each(Threads_.begin(),Threads_.end(),boost::bind(&Thread::join,_1));
    }


    void ThreadFun(){
        Latch_.countDown();
        bool running = true;
        while(running){
            std::string n(queue_.take());
            std::cout << " thread name: " << CurrentThread::getThreadName()
            << " thread tid: "<< CurrentThread::tid()<< " wrok " << n << std::endl;
            running = (n != "stop");
        }
    }
private:

    BlockingQueue<std::string> queue_;
    boost::ptr_vector<Thread> Threads_;
    CountDownLatch Latch_;

};
//
int main(){
    test t(20);
    t.put(20);
    t.joinAll();
}
