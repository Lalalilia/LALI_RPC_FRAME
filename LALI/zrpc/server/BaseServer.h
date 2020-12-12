//
// Created by lalalilia
//

#ifndef ZILIPROJECT_BASESERVER_H
#define ZILIPROJECT_BASESERVER_H

#include <jackson/Value.h>
#include <net/TcpServer.h>
#include <zrpc/util.h>


template <typename ProtocolServer>
class BaseServer {
public:
    void setNumThread(size_t n){
        server_.setThreadNums(n);
    }

    void start(){
        server_.start();
    }

protected:
    BaseServer(EventLoop* loop,const AddressOps& listen);

    ~BaseServer() = default;

private:
    // 建立 tcp 链接
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn,Buffer* buffer,ssize_t len);
    void handleMessage(const TcpConnectionPtr& conn,Buffer* buffer);
    void sendResponse(const TcpConnectionPtr& conn,const json::Value& response);

    ProtocolServer& convert();
    const ProtocolServer& convert() const;

protected:

private:
    TcpServer server_;
};




#endif //ZILIPROJECT_BASESERVER_H
