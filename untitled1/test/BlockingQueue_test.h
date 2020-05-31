



#include <vector>
// 生产者消费者模型；
class test{

public:

	test(int numThreads)
	:Latch_(numThreads),
	Threads_(numThreads){
		for(int i=0;i<numThreads;i++){
			char name[30];
			snprintf(name,sizeof name,"thread %d",i);
			Threads_.push_back(Thread(boost::bind(&ThreadFun,this),name));
		}
		for_each(Threads_.begin(),Threads_.end(),Thread::start());
	}

	void put(int times){

		Latch_.wait();
		for(int i=0;i<times;i++){
			char name[30];
			snprintf(name,sizeof name,"THE number %d",i);
			queue_put(name);
		}
	}

	void joinAll(){
		Latch_.wait();
		for(size_t i=0;i<Threads_.size();i++){
			queue_.push_back("stop");
		}
		for_each(Threads_.begin(),Threads_.end(),Thread::join());
	}


	void ThreadFun(){
		bool running = true;
		Latch_.countDown();
		while(running){
			std::string n(queue_.take());
			std::cout << n << std::endl;
			running = (n != "stop");
		}
	}


private:

	BlockingQueue<std::string> queue_;
	boost::ptr_vector<Thread> Threads_;
	CountDownLatch Latch_;

}


