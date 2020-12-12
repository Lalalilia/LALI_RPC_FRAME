//
// Created by lalalilia
//


#include <base/Logging.h>
#include <net/EventLoop.h>
#include <net/TcpClient.h>
#include <iostream>
using namespace std;


TcpClient* g_client;

void timeout()
{
    LOG << "timeout";

    g_client->stop();
}

void onConnection(const TcpConnectionPtr& conn){
    cout<<" new connection name: "<< conn->name() << " from " << conn->peerAddr().toHostPort() <<endl;
}

int main(int argc, char* argv[])
{
    EventLoop loop;
    AddressOps serverAddr("127.0.0.1", 9981); // no such server
    TcpClient client(&loop, serverAddr, "TcpClient");
    g_client = &client;
    //loop.runAfter(timeout,0.0);
    //loop.runAfter(std::bind(&EventLoop::quit, &loop),1.0 );
    client.connect();
    client.setConnectionCallback(onConnection);
    loop.loop();
}

