//
// Created by lalalilia
//

#ifndef ZILIPROJECT_RPCCLIENT_H
#define ZILIPROJECT_RPCCLIENT_H

#include <net/TcpClient.h>
#include <jackson/Value.h>
#include <zrpc/util.h>

typedef std::function<void(json::Value&,
        bool isError,bool isTimeout)>ResponseCallback;

class RpcClient{
public:
    RpcClient(EventLoop* loop,const AddressOps& serverAddress):
        id_(0),
        client_(loop,serverAddress,"client"){
        client_.setMessageCallback(std::bind(
                &RpcClient::onMessage,this,_1,_2,_3));
    }
    void start(){client_.connect();}

    void setConnectionCallback(const ConnectionCallback& cb){
        client_.setConnectionCallback(cb);
    }

    // 函数调用
    void sendCall(const TcpConnectionPtr& conn,json::Value& call,
            const ResponseCallback& cb);

    // 通知
    void sendNotify(const TcpConnectionPtr& conn,json::Value& notify);


private:
    // 消息回调函数
    void onMessage(const TcpConnectionPtr& conn,Buffer* buffer,ssize_t len);
    // 回调消息控制函数
    void handleMessage(Buffer* buffer);
    // 控制回复填充函数
    void handleResponse(std::string& json);
    // 单个回复填充函数
    void handleSingleResponse(json::Value& response);
    // 验证回复的消息的有效性
    void validateResponse(json::Value& response);
    // 请求发送函数
    void sendRequest(const TcpConnectionPtr& conn,json::Value& request);

private:
    TcpClient client_;
    typedef std::unordered_map<int64_t,ResponseCallback> Callbacks;
    int64_t id_;
    Callbacks callbacks_;

};


#endif //ZILIPROJECT_RPCCLIENT_H
