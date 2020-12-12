//
// Created by lalalilia
//

#include "TimerQueue.h"
#include <sys/timerfd.h>
#include <unistd.h>


int createTimerfd(){
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                   TFD_NONBLOCK|TFD_CLOEXEC);
    if(timerfd<0){
        LOG << "Failed in timerfd_create";
    }
    return timerfd;
}

struct timespec howMuchTimeFromNow(Timestamp when){
    int64_t microseconds = when.microSecondsSinceEpoch()-
                           Timestamp::now().microSecondsSinceEpoch();
    if(microseconds<100){
        microseconds = 100;
    }
    struct timespec ts{};
    ts.tv_sec = static_cast<time_t>(
            microseconds/Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<time_t>(
            (microseconds%Timestamp::kMicroSecondsPerSecond)*1000);

    return ts;
}


void readTimerfd(int timerfd,Timestamp now){
    uint64_t howmany;
    ssize_t n = ::read(timerfd,&howmany,sizeof howmany);
    LOG << " TimerQueue::handleRead() "<<howmany<<" at "<< now.toString();
}

void resetTimerfd(int timerfd,Timestamp expiration){
    struct itimerspec newValue{};
    struct itimerspec oldValue{};
    bzero(&newValue,sizeof newValue);
    bzero(&oldValue,sizeof oldValue);
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd,0,&newValue,&oldValue);
    if(ret){
        LOG << " timerfd_settime() ";
    }
}


TimerQueue::TimerQueue(EventLoop* loop)
        :loop_(loop),
         timerfd_(createTimerfd()),
         timerfdChannel_(loop,timerfd_),
         timers_(){
    timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead,this));
    timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue(){
    ::close(timerfd_);
    for(auto& it:timers_){
        delete it.second;
    }
}


TimerId TimerQueue::addTimer(const TimerCallback&cb,
                             Timestamp when,
                             double interval){
    Timer* timer = new Timer(cb,when,interval);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop,this,timer));
    return TimerId(timer);

}

void TimerQueue::addTimerInLoop(Timer* timer) {
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);

    if(earliestChanged){
        resetTimerfd(timerfd_,timer->expiration());
    }
}

bool TimerQueue::insert(Timer* timer){
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if(it == timers_.end() || when < it->first){
        earliestChanged = true;
    }
    std::pair<TimerList::iterator,bool>result =
            timers_.insert(std::make_pair(when,timer));

    assert(result.second);
    return earliestChanged;
}

void TimerQueue::handleRead(){
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_,now);

    std::vector<Entry> expried = getExpired(now);

    for(auto & it : expried){
        it.second->run();
    }

    reset(expried,now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now){
    std::vector<Entry> expried;
    Entry sentry = std::make_pair(now,reinterpret_cast<Timer*>(UINTPTR_MAX));
    auto it = timers_.lower_bound(sentry);
    assert(it == timers_.end() || now < it->first);
    std::copy(timers_.begin(),it,back_inserter(expried));
    timers_.erase(timers_.begin(),it);
    return expried;
}

void TimerQueue::reset(const std::vector<Entry>& expried,
                       Timestamp now){
    Timestamp nextExpire;

    for(const auto & it : expried){
        if(it.second->repeat()){
            it.second->restart(now);
            insert(it.second);
        }
        else{
            delete it.second;
        }

    }

    if(!timers_.empty()){
        nextExpire = timers_.begin()->second->expiration();
    }

    if(nextExpire.valid()){
        resetTimerfd(timerfd_,nextExpire);
    }
}