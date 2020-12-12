

#include <iostream>
#include "net/EventLoop.h"
#include "net/Channel.h"
#include <sys/timerfd.h>

using namespace std;
int fd = 1;
EventLoop* g_loop;

void hello(){
    cout<<" hello "<<endl;
    g_loop->quit();
}

int main(int argc,char* argv[]){

    EventLoop loop;
    g_loop = &loop;
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
    Channel channel(&loop,timerfd);
    channel.setReadCallback(hello);
    channel.enableReading();

    struct itimerspec howlong{};
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd,0,&howlong,NULL);

    loop.loop();
    ::close(timerfd);

}


