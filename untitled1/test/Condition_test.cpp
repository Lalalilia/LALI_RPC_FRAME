

#include <iostream>
#include "Thread.h"
#include "Exception.h"
#include <cstdio>
#include "CurrentThread.h"
#include "ThreadLocal.h"
#include "unistd.h"
#include "Mutex.h"
#include "Condition.h"

using  namespace  std;

int n=0;
ThreadLocal<int> object;

Mutex Amutex;

Condition cond_(Amutex);

int increment(){
    cond_.wait();
    cout << "the end" << n++ << endl;
}
int incre(){
    while(n<10){
        cout << n++ << endl;
        //sleep(1);
    }
    cond_.weakup();
}

int main(){

    cout << CurrentThread::tid() <<CurrentThread::getThreadName()<<endl;
    Thread t1(increment,string("hello"));
    Thread t2(incre,string("wrold"));
    t1.start();
    t2.start();
    t1.join();
    t2.join();
    pthread_exit(0);


}
