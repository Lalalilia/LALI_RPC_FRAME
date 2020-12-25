//
//Created by lalalilia
// 

#ifndef UNTITLED1_TCPSERVER_H
#define UNTITLED1_TCPSERVER_H

#include <string>
#include <memory>
#include "Acceptor.h"
#include <map>
#include "TcpConnection.h"
#include "EventLoopThreadPool.h"


class EventLoop;
class TcpServer{
public:
    TcpServer(EventLoop*,const AddressOps&);
    ~TcpServer();

    void start();
    void setMessageCallback(const MessageCallback& cb) {
        messageCallback_ = cb;
    }

    void setConnectionCallback(const ConnectionCallback& cb) {
        connectionCallback_ = cb;
    }
    void setThreadNums(int nums);

    EventLoop* getLoop() const{
        return loop_;
    }

private:
    void newConnection(int,const AddressOps&);
    void destroyConnection(const TcpConnectionPtr& conn);
    void destroyConnectionInLoop(const TcpConnectionPtr& conn);
    EventLoop* loop_;
    std::string name_;
    bool started_;
    std::unique_ptr<Acceptor> acceptor_;
    typedef std::map<std::string,TcpConnectionPtr> ConnectionMap;
    int connId_;

    MessageCallback messageCallback_;
    ConnectionCallback connectionCallback_;
    ConnectionMap connection_;

    std::unique_ptr<EventLoopThreadPool> threadPool_;
};




#endif //UNTITLED1_TCPSERVER_H
