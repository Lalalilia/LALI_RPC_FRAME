//
// Created by lalalilia on 11/30/20.
//



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
#include "net/HttpServer.h"
#include "net/HttpRequst.h"
#include "net/HttpRespond.h"

using namespace std;

void onRequest(const HttpRequest& request,
               HttpRespond* respond){
    if(request.getPath() == "/"){
        respond->setStatus(HttpRespond::r200Succeed);
        respond->setStatusMessage(" Succeed ");
        respond->addHeader("Server","ZILI");
        respond->setBody("<html><head><title>This is title</title></head>"
                         "<body><h1>Congradulation!</h1> "
                         "</body></html>");
    }
    else {
        respond->setStatus(HttpRespond::r404Notfound);
        respond->setStatusMessage("Not Found");
        respond->setCloseConnection(true);
    }

}

int main(int argc,char* argv[]){
    EventLoop ploop;
    AddressOps addr(8001);
    HttpServer server(&ploop,addr);
    server.setThreadNums(2);
    server.setHttpCallbak(onRequest);
    server.start();
    ploop.loop();
}
