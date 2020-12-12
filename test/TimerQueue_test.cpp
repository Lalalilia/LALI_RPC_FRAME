

#include <iostream>
#include "net/EventLoop.h"
#include "net/Channel.h"
#include <sys/timerfd.h>

using namespace std;
EventLoop* p_loop;

void once(){
    cout<<"once 2.3"<<endl;
}

void repeat(){
    static int a = 0;
    cout<<"repeat 2.0"<< a++ << endl;
    if(a==10)p_loop->quit();
}
int main(int argc,char* argv[]){
    EventLoop loop;
    p_loop = &loop;

    loop.runAfter(once,2.3);
    loop.runRepeat(repeat,2.0);

    loop.loop();

}


