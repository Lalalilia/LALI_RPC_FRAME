//
// Created by lalalilia on 4/12/20.
//

#ifndef UNTITLED1_THREADPOOL_H
#define UNTITLED1_THREADPOOL_H


#include "Thread.h"
#include "Condition.h"
#include <string>
#include <deque>
#include <vector>

class ThreadPool{
public:
    ThreadPool(const std::string name = std::string());
    ~ThreadPool();
    typedef std::function<void()> Task;
    // 创建线程
    void start(int numsThreads);
    void run(const Task& task);
    void stop();
private:
    void runInqueue();
    Task take();
    Condition pcond_;
    mutable Mutex mutex_;
    std::vector<std::unique_ptr<Thread>> threads_; // 线程池数组，存储线程
    std::deque<Task> queue_; // 任务队列
    std::string name_;
    bool running_;
};

#endif //UNTITLED1_THREADPOOL_H
