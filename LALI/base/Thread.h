#ifndef _thread_h
//Created by lalalilia
#define _thread_h
/*
	首先想想写一个线程类需要干什么好吧；
	首先得有线程的创建把，这个是必须的；
	然后，我们得有这个线程的等待把，合情合理；就是那个join；
	① 第一个这个线程类的成员变量要搞啥呢：
	线程id 这个必须得有吧，
	线程的状态得有吧；
	线程有了，你得让其执行工作把，所以得有一个接受回调函数的变量类型把；
	暂时就先这些；
	至于那个原子整数感觉没啥必要，之后再看；
	② 成员函数要搞啥呢；
	首先一个，核心的 构造函数，这个构造函数必须得初始化所有的变量，然后创建一个线程，对；
	然后没那么重要的析构函数，这个析构函数得确保这个线程合理的消失，不能内存泄漏对把；
	然后得写一点访问它的状态的函数，以便用户知道这个线程的一些状态；
	③ 关于当前线程；
	在muduo中呢，就用了一个命名空间来访问这个当前线程，这个还是挺有必要的；

*/

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
	bool started_{}; // 开始状态；
	Threadfunc func_; // 需要一个那个回调函数来当作线程执行的任务；
	pthread_t pid_{}; // 线程 id
	pid_t tid_{}; // 当前 true id
	std::string name_; //当前这个线程名字;
	//为什么一定得要个静态函数呢，因为普通的类成员函数的话就会自带一个 this 指针参数，而静态函数没有
	//所以当类成员函数作为 pthread_created 函数的参数的时候就会因为参数匹配补上，而报错；
	//那为什么静态函数就没有this指针呢？
	static void* ThreadRunfunc(void* obj); 
};



#endif
