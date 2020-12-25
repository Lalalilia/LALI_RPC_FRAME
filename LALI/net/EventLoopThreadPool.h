//
//Created by lalalilia
//

#ifndef UNTITLED1_EVENTLOOPTHREADPOOL_H
#define UNTITLED1_EVENTLOOPTHREADPOOL_H

#include <vector>
#include <memory>


class EventLoop;
class ThreadEventLoop;

class EventLoopThreadPool{
private:
    EventLoop* baseloop_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<EventLoop*> loops_;
    std::vector<std::unique_ptr<ThreadEventLoop>> threads_;

public:
    explicit EventLoopThreadPool(EventLoop*);
    ~EventLoopThreadPool();
    void setThreadNums(int nums);
    void start();
    EventLoop* getNextLoop();

};

#endif //UNTITLED1_EVENTLOOPTHREADPOOL_H
