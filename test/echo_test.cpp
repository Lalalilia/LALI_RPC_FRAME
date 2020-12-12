

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
EventLoop *gloop;

//typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
void connectionCallback(const TcpConnectionPtr& tcpConnection){
    if(tcpConnection->connected()){
        cout << " a new connection from" << tcpConnection->peerAddr().toHostPort() << " name: "
             << tcpConnection->name() << endl;
    }
    else{
        cout << " a new connection from" << tcpConnection->peerAddr().toHostPort() << " name: "
             << tcpConnection->name() << " disconnected " << endl;
    }
}

//typedef std::function<void (const TcpConnectionPtr&,
//                            Buffer* data,
//                            ssize_t len)> MessageCallback;
void messagesCallback(const TcpConnectionPtr& tcpConnection,
                            Buffer* data,
                            ssize_t len){
    cout << " from " << tcpConnection->peerAddr().toHostPort() << " name: " << tcpConnection->name()
    << " len: "<< len << " message: " << data->peek();
    tcpConnection->send(data->retrieveAsString());
}


int main(int argc,char* argv[]){
    EventLoop loop;
    gloop=&loop;
    AddressOps port(9981);
    TcpServer server(gloop,port);
    server.setThreadNums(10);
    server.setConnectionCallback(connectionCallback);
    server.setMessageCallback(messagesCallback);
    server.start();
    gloop->loop();
}


