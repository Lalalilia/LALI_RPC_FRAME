

#include <iostream>
#include "net/EventLoop.h"
#include "net/Channel.h"
#include <sys/timerfd.h>
#include "base/Thread.h"
#include "net/ThreadEventLoop.h"
#include "net/Acceptor.h"
#include "net/AddressOps.h"
#include "net/SocketOps.h"
#include "net/TcpServer.h"

using namespace std;
EventLoop* pLoop;
EventLoop* pLoop2;
void good(){
    cout<<"good"<<endl;
}
void hello(){
    cout<< "hello" <<endl;
    pLoop2->queueInLoop(good);
}
// Using English to annotation

int main(int argc,char* argv[]){

    ThreadEventLoop loop1,loop2;
    pLoop=loop1.startLoop();
    pLoop2=loop2.startLoop();
    pLoop->runInLoop(hello);
}

