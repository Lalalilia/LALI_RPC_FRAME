

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

using  namespace  std;

void print(){
    cout << "tid: "<<CurrentThread::tid() << " name: " << CurrentThread::getThreadName() <<endl;
    sleep(1);
}

void test(int nums){


    ThreadPool pool(string("mainPool"));
    pool.start(5);
    pool.run(print);
    pool.run(print);
    for(int i=0;i<nums;i++){
        pool.run(print);
    }

    CountDownLatch latch(1);
    pool.run(std::bind(&CountDownLatch::countDown,&latch));
    latch.wait();
    pool.stop();

}


int main() {
    test(10);

}
