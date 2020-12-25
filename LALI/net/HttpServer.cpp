//
//Created by lalalilia
//

#include "HttpRespond.h"
#include "HttpContext.h"
#include "HttpRequst.h"
#include "HttpServer.h"
#include "../base/Timestamp.h"

static void default_HttpCallback(const HttpRequest& request,
                           HttpRespond* respond){
    respond->setStatus(HttpRespond::r404Notfound);
    respond->setStatusMessage("NotFound");
    respond->setCloseConnection(true);
}

void HttpServer::onConnection(const TcpConnectionPtr &conn) {
    if(conn->connected()){

    }
}

void HttpServer::onMessage(const TcpConnectionPtr & conn, Buffer *data, ssize_t len) {
    HttpContext context;
    if(!context.parseRequst(data,Timestamp::now())){
        conn->send(" 400 Bad Request ");
        conn->shutdown();
    }
    if(context.parseAll()){
        onRequest(conn, context.getRequest());
        context.reset();
    }


}

void HttpServer::onRequest(const TcpConnectionPtr &conn, const HttpRequest &request) {
    const std::string& connection = request.getHeader("Connection");
    bool close = connection=="close"||
            (request.getVersion()==HttpRequest::Http10&&connection!="Keep-Alive");
    HttpRespond respond(close);
    httpCallback_(request,&respond);
    Buffer buf;
    respond.appendToBuffer(&buf);
    std::string message=buf.retrieveAsString();
    conn->send(message);
    if(respond.closeConnection())conn->shutdown();
}


void HttpServer::start() {
    server_.start();
}

using namespace std::placeholders;

HttpServer::HttpServer(EventLoop * loop, AddressOps &address)
    :server_(loop,address),
    httpCallback_(default_HttpCallback){
    server_.setConnectionCallback(std::bind(&HttpServer::onConnection,this,_1));
    server_.setMessageCallback(std::bind(&HttpServer::onMessage,this,_1,_2,_3));
}
