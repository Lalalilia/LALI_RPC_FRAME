//
// Created by lalalilia
//

#ifndef ZILIPROJECT_RPCSERVER_H
#define ZILIPROJECT_RPCSERVER_H


#include <zrpc/server/BaseServer.h>
#include <zrpc/server/RpcService.h>


class RpcServer: public BaseServer<RpcServer>{
public:
    RpcServer(EventLoop* loop,const AddressOps& listen):
        BaseServer(loop,listen)
    {}

    ~RpcServer() = default;

    void addService(std::string_view serviceName,RpcService* service);

    void handleRequest(const std::string& json,
                        const RpcDoneCallback& done);

private:
    void handleSingleRequest(json::Value& request,
                             const RpcDoneCallback& done);
    void handleSingleNotify(json::Value& request);
    void handleBatchRequests(json::Value& requests,
                             const RpcDoneCallback& done);
    void validateRequest(json::Value& request);
    void validateNotify(json::Value& request);

private:
    typedef std::unique_ptr<RpcService> RpcServerPtr;
    typedef std::unordered_map<std::string_view ,RpcServerPtr> ServiceList;

    ServiceList serviceList_;
};

#endif //ZILIPROJECT_RPCSERVER_H
