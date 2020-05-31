

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
#include "BoundBlockingQueue.h"
#include "ThreadLocalSingleton.h"

using  namespace  std;
int n=0;
ThreadLocal<int> object;

Mutex Amutex;

Condition cond_(Amutex);

class test{
public:

    test(){
        cout << " create " << CurrentThread::tid() << " name " << name_ << endl;
    }
    ~test(){
        cout << " destory " << CurrentThread::tid() << " name " << name_ << endl;
    }

    const string& getName() const{
        return name_;
    }

    void setName(const string& n){
        name_ = n;
    }

private:
    string name_;
};


void threadFun(const char* name){

    cout << " tid " << CurrentThread::tid() << " singlename: " << ThreadLocalSingleton<test>::instance().getName() << endl;

    ThreadLocalSingleton<test>::instance().setName(string(name));

    cout << " tid " << CurrentThread::tid() << " singlename: " << ThreadLocalSingleton<test>::instance().getName() << endl;

}
//

int main() {
    ThreadLocalSingleton<test>::instance().setName("main");
    cout << " tid " << CurrentThread::tid() << " singlename " << ThreadLocalSingleton<test>::instance().getName() << endl;
    Thread t1(std::bind(&threadFun,"thread 1"), "thread 1");
    Thread t2(std::bind(&threadFun,"thread 2"), "thread 2");
    t1.start();
    t2.start();
    cout << " tid " << CurrentThread::tid() << " singlename " << ThreadLocalSingleton<test>::instance().getName() << endl;
    t1.join();
    t2.join();
    pthread_exit(0);
}
