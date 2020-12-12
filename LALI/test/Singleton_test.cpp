

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
#include "ThreadPool.h"
#include "Singleton.h"

using  namespace  std;

class test{
public:
    test(){
        cout << "good" << endl;
    }
    ~test(){}
    void setName(const string& name){
        name_ = name;
    }
    const string& getName() const{
        return name_;
    }
private:
    string name_;
};

void threadFun(){
    cout << " tid: " << CurrentThread::tid() << " name: " << CurrentThread::getThreadName() << endl;
    cout << " test name thread: " << Singleton<test>::instance().getName() << endl;
    Singleton<test>::instance().setName(string("onle one changed"));
    cout << "thread's test address: " << &Singleton<test>::instance() << endl;
}

int main() {
    Singleton<test>::instance().setName(string("only one"));
    cout << "main's test address: " << &Singleton<test>::instance() << endl;
    Thread t1(threadFun,"thread 1");
    t1.start();
    t1.join();
    cout << " tid: " << CurrentThread::tid() << " name: " << CurrentThread::getThreadName() << endl;
    cout << " test name: " << Singleton<test>::instance().getName() << endl;
}


