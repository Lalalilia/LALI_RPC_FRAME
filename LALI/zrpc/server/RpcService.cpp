//
// Created by lalalilia
//

#include <exception>
#include <zrpc/server/RpcService.h>


void RpcService::callProcedureReturn(std::string_view methodName,
                                     json::Value& request,
                                     const RpcDoneCallback& done)
{
    auto it = procedureReturnList_.find(methodName);
    if (it == procedureReturnList_.end()) {
        throw "method not found";
    }
    it->second->invoke(request, done);
};

void RpcService::callProcedureNotify(std::string_view methodName, json::Value& request)
{
    auto it = procedureNotifyList_.find(methodName);
    if (it == procedureNotifyList_.end()) {
        throw "method not found";
    }
    it->second->invoke(request);
};