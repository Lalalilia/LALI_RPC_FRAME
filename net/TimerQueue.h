//
// Created by lalalilia on 4/30/20.
//

#ifndef UNTITLED1_TIMERQUEUE_H
#define UNTITLED1_TIMERQUEUE_H



#include "Callback.h"
#include "TImerId.h"
#include "Timer.h"
#include <set>
#include <vector>
#include "Channel.h"


class EventLoop;

class TimerQueue {

public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId addTimer(const TimerCallback& cb,
                     Timestamp when,
                     double interval);

    void addTimerInLoop(Timer*);

private:
    /// 用一个 pair， 一个set，
    typedef std::pair<Timestamp,Timer*> Entry;
    typedef std::set<Entry> TimerList;

    void handleRead();

    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expried,Timestamp now);

    bool insert(Timer* timer);

    EventLoop* loop_;
    const int timerfd_;
    Channel timerfdChannel_;

    TimerList timers_;

};


#endif //UNTITLED1_TIMERQUEUE_H
