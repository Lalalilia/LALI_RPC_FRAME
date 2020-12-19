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
	pid_t tid_{}; 
	std::string name_; 
	
	static void* ThreadRunfunc(void* obj); 
};



#endif