//
// Created by lalalilia on 4/29/20.
//

#ifndef UNTITLED1_CHANNEL_H
#define UNTITLED1_CHANNEL_H


#include "EventLoop.h"
#include "../base/Logging.h"
#include <functional>

/// 使用通道类 Channel 来处理不同IO事件；


class Poller;
class EventLoop;
class Channel{
public:
    typedef std::function<void()> EventCallback;
    Channel(EventLoop* loop,int fd);
    ~Channel();
    void handleEvent();

    void setReadCallback(const EventCallback& cb){
        readCallback_ = cb;
    }
    void setWriteCallback(const EventCallback& cb){
        writeCallback_ = cb;
    }
    void setErrorCallback(const EventCallback& cb){
        errorCallback_ = cb;
    }
    void setCloseCallback(const EventCallback& cb){
        closeCallback_ = cb;
    }

    int get_fd()const{return fd_;}
    int get_events()const{return events_;}
    void set_currentEvents(int revt){currentEvents_=revt;}
    bool isNoneEvent()const{return events_==NoneEventFlag;}


    void enableReading(){events_ |= ReadEventFlag;update();}
    void enableWriting(){events_ |= WriteEventFlag;update();}
    void disableWritinf(){events_ &= ~WriteEventFlag;update();}
    bool isWriting() const{return events_&WriteEventFlag;}
    void disableAll(){events_|NoneEventFlag;update();}

    int get_index(){return index_;}
    void set_index(int idx){index_=idx;}
    EventLoop* ownerLoop(){return loop_;}

    void remove();
private:

    void update();
    static const int NoneEventFlag;
    static const int ReadEventFlag;
    static const int WriteEventFlag;

    EventLoop* loop_;
    const int fd_;
    int events_;
    int currentEvents_;
    int index_; //???

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;

    bool EventHanding_;
};


#endif //UNTITLED1_CHANNEL_H
