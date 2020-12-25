//
//Created by lalalilia
//

#include "ThreadPool.h"
#include <iostream>

typedef std::function<void()> Task;

ThreadPool::ThreadPool(const std::string name)
        :name_(name),
         mutex_(nullptr),
         pcond_(mutex_),
         running_(false){
}
ThreadPool::~ThreadPool() {
    if(running_){
        stop();
    }
}

void ThreadPool::start(int numsThreads){
    running_ = true;
    threads_.reserve(numsThreads);
    for(int i=0;i<numsThreads;i++){
        char id[30];
        snprintf(id,sizeof id,"thread:%d",i);
        threads_.emplace_back(new Thread(
                std::bind(&ThreadPool::runInqueue,this),std::string(id)));
        threads_[i]->start();
    }
}

void ThreadPool::stop(){
    {
        MutexGuard lock(mutex_);
        running_ = false;
        pcond_.weakupAll();
    }

    for(auto& thread:ThreadPool::threads_){
        thread->join();
    }

}

void ThreadPool::run(const Task& task){
    if(threads_.empty()){
        task();
    }
    else{
        MutexGuard lock(mutex_);
        queue_.push_back(std::move(task));
        pcond_.weakup();
    }
}

Task ThreadPool::take(){

    MutexGuard lock(mutex_);
    while(running_&&queue_.empty()){
        pcond_.wait();
    }
    Task t;
    if(!queue_.empty()){
        t = queue_.front();
        queue_.pop_front();
    }
    return t;
}

void ThreadPool::runInqueue(){
    try{
        while(running_){
            Task t(take());
            if(t){
                t();
            }
        }

    }
    catch(...){
        std::cout << "one error" << std::endl;
    }
}
