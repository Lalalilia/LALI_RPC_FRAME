//
// Created by lalalilia
//

#include "net/EventLoop.h"
#include "base/Thread.h"
#include "net/ThreadEventLoop.h"
#include "net/AddressOps.h"
#include "net/HttpServer.h"
#include "net/HttpRequst.h"
#include "net/HttpRespond.h"

using namespace std;
void outputEmpty(const char* msg,int len){

}

void onRequest(const HttpRequest& request,
               HttpRespond* respond){
    if(request.getPath() == "/"){
        respond->setStatus(HttpRespond::r200Succeed);
        respond->setStatusMessage(" Succeed ");
        respond->addHeader("Server","ZILI");
        respond->setBody("<html><head><title>This is title</title></head>"
                         "<body><h1>hello</h1> "
                         "</body></html>");
    }
    else {
        respond->setStatus(HttpRespond::r404Notfound);
        respond->setStatusMessage("Not Found");
        respond->setCloseConnection(true);
    }

}

int main(int argc,char* argv[]){
    Logger::setOutput(outputEmpty); //设置 log 输出为空
    EventLoop ploop;
    AddressOps addr(8001);
    HttpServer server(&ploop,addr);
    server.setThreadNums(3);
    server.setHttpCallbak(onRequest);
    server.start();
    ploop.loop();
}
