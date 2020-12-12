

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

void connectioncallback(const TcpConnectionPtr& conn){

    cout<<" new connection name: "<< conn->name() << " from " << conn->peerAddr().toHostPort() <<endl;

}

void messCallback(const TcpConnectionPtr& conn,
                  Buffer* data,
                  ssize_t len){
    std::string echom(data->retrieveAsString());

    cout << " messages from "<<"[" << conn->name() << " - "<<conn->peerAddr().toHostPort()<<"]"
            <<" len: "<< echom.length() <<" data: "<< echom  <<endl;
    conn->send(echom);
}

// Using English to annotation

int main(int argc,char* argv[]){
    AddressOps addr(9981);
    EventLoop loop;
    TcpServer server(&loop,addr);
    server.setConnectionCallback(connectioncallback);
    server.setMessageCallback(messCallback);
     server.start();
    loop.loop();
}


