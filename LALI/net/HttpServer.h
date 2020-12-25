//
//Created by lalalilia
//

#ifndef UNTITLED1_HTTPSERVER_H
#define UNTITLED1_HTTPSERVER_H


#include "TcpServer.h"
#include <functional>

class HttpRequest;
class HttpRespond;
class HttpServer{
public:

    typedef std::function<void(const HttpRequest& request,
     HttpRespond* respond)> HttpCallback;

    HttpServer(EventLoop*,AddressOps& address);
    void setHttpCallbak(const HttpCallback& cb){
        httpCallback_=cb;
    }
    void setThreadNums(int nums){
        server_.setThreadNums(nums);
    }
    void start();

private:
    void onConnection(const TcpConnectionPtr&);
    void onMessage(const TcpConnectionPtr&,
                   Buffer* data,
                   ssize_t len);

    void onRequest(const TcpConnectionPtr&,const HttpRequest&);
    TcpServer server_;
    HttpCallback httpCallback_;



};





#endif //UNTITLED1_HTTPSERVER_H
