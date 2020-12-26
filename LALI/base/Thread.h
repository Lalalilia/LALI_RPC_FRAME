//Created by lalalilia
#ifndef _thread_h
#define _thread_h


#include <pthread.h>
#include <memory>
#include <functional>

class Thread {
public:
    typedef std::function<void()> Threadfunc;
	explicit Thread(const Threadfunc&,const std::string& name = std::string()) ;
    ~Thread() ;
	 bool started() {};
	 void start();
	 void join();
     void runInthread();
private:
	bool started_{};
	Threadfunc func_; 
	pthread_t pid_{}; 
	pid_t tid_{}; // 线程的真实 id
	std::string name_; 
	// 使用静态函数避免 this 指针，而匹配 pthreadcreated 函数中的需要
	static void* ThreadRunfunc(void* obj); 
};



#endif
