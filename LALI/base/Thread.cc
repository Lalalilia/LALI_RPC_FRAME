
#include "Thread.h"
#include <unistd.h>
#include <linux/unistd.h>
#include <pthread.h>
#include "stdio.h"
#include "iostream"
#include "Exception.h"
#include <iostream>
#include "CurrentThread.h"
#include <sys/syscall.h>

using namespace std;

namespace CurrentThread {
    // 初始化工作
    __thread int t_cachedTid = 0;
    __thread char t_stringTid[32];
    __thread const char *t_threadName = "unknown";
}

void CurrentThread::CacheTid() {
    // 获取线程 tid
    t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
    snprintf(t_stringTid, sizeof t_stringTid, "%5d ", t_cachedTid);
}

namespace detail {
    void afterFork() {
        CurrentThread::t_cachedTid = 0;
        CurrentThread::t_threadName = "main";
        CurrentThread::CacheTid();
    }

    class CurrentThreadInit {
    public:
        CurrentThreadInit() {
            cout << "oneThreadhasbeenCreate" << endl;
            CurrentThread::t_threadName = "main";
            CurrentThread::CacheTid();
        }
    };

    CurrentThreadInit init;
}

Thread::~Thread(){
    std::cout<<"thread destroyed"<<std::endl;
}

void Thread::start() {
    //cout << CurrentThread::getThreadName() << "start" << endl;
	started_ = true;
	pthread_create(&pid_, NULL, &ThreadRunfunc, this);
}

void Thread::join() {
	pthread_join(pid_, NULL);
}

void* Thread::ThreadRunfunc(void* obj){
	Thread* thread = static_cast<Thread*>(obj);
	thread->runInthread();
	return NULL;
}

void Thread::runInthread() {
    tid_ = CurrentThread::tid();
    CurrentThread::t_threadName = (char*)name_.c_str();
	try{
        func_();
        CurrentThread::t_threadName = "finshed";
	}
	catch (const Exception &ex){
	    cout << "Thread:" << CurrentThread::getThreadName() << "tid=" << CurrentThread::tid() <<endl;
	    cout << "reson:"<<ex.what()<<endl;
	    cout<<"stack trace:"<<ex.stack_trace()<<endl;
	}
	catch(...){
	    cout << "unknown exception" << endl;
	}

}

Thread::Thread(const Thread::Threadfunc& func,const std::string& name)
     : started_(false),
        pid_(0),
        tid_(0),
        func_(func),
        name_(name){
    std::cout<<"thread create"<<std::endl;
}




