//
// Created by lalalilia
//

#ifndef ZILIPROJECT_TCPCLIENT_H
#define ZILIPROJECT_TCPCLIENT_H

#include <base/Mutex.h>
#include <net/TcpConnection.h>
#include <string>

class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;
class TcpClient{
public:
    TcpClient(EventLoop* loop,
              const AddressOps& serverAddr,
              const std::string& nameArg);
    ~TcpClient();

    void connect();
    void disconnect();
    void stop();

    TcpConnectionPtr getconnection() const{
        MutexGuard lock(mutex_);
        return connection_;
    }

    EventLoop* getLoop() const { return loop_; }
    void enableRetry(){ retry_ = true;}
    bool getretry(){ return retry_;}

    const std::string& getname(){
        return name_;
    }

    void setConnectionCallback(const ConnectionCallback& cb){
        connectionCallback_ = cb;
    }
    void setMessageCallback(const MessageCallback& cb){
        messageCallback_ = cb;
    }

private:
    void newConnection(int sockfd);
    void removeConnection(const TcpConnectionPtr& conn);

    EventLoop* loop_;
    ConnectorPtr connector_;
    const std::string name_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    bool retry_;
    bool connect_;
    int nextConnId_;
    mutable Mutex mutex_;
    TcpConnectionPtr connection_;
};

#endif //ZILIPROJECT_TCPCLIENT_H
