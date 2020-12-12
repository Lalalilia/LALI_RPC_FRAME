//
// Created by lalalilia
//

#include <zrpc/server/Rpcserver.h>
#include <mutex>
#include <base/Exception.h>

// 是否有函数参数
bool hasParams(const json::Value& request){
    return request.findMember("params") != request.memberEnd();
}

// 是否是通知
bool isNotify(const json::Value& request){
    return request.findMember("id") == request.memberEnd();
}

// 检查json中到文件类型是否符合要求
template <json::ValueType dst, json::ValueType... rest>
void checkValueType(json::ValueType type){
    if(type == dst)
        return;;

    if constexpr (sizeof...(rest) > 0)
        checkValueType<rest...>(type);
    else
        throw Exception("bad type ");
}

// 寻找json中的值
template <json::ValueType... types>
json::Value& findValue(json::Value& request,const char* key){
    static_assert(sizeof...(types) > 0, "expect at least one type");
    auto it = request.findMember(key);
    if (it == request.memberEnd())
        throw Exception("missing at least one filed");
    checkValueType<types...>(it->value.getType());
    return it->value;
}

// 线程安全的批处理回复消息辅助类
class ThreadSafeBatchResponse{

public:
    explicit
    ThreadSafeBatchResponse(const RpcDoneCallback& done):
        data_(std::make_shared<ThreadSafeData>(done))
    {}
    void addResponse(json::Value response) const {
        std::unique_lock lock(data_->mutex);
        data_->responses.addValue(response);
    }

private:
    struct ThreadSafeData{
        explicit
        ThreadSafeData(const RpcDoneCallback done_):
            responses(json::TYPE_ARRAY),
            done(done_){}

        ~ThreadSafeData(){
            done(responses);
        }
        std::mutex mutex;
        json::Value responses;
        RpcDoneCallback done;
    };

    typedef std::shared_ptr<ThreadSafeData> DataPtr;
    DataPtr data_;
};

void RpcServer::addService(std::string_view serviceName, RpcService *service) {
    assert(serviceList_.find(serviceName) == serviceList_.end());
    serviceList_.emplace(serviceName,service);
}


void RpcServer::handleRequest(const std::string &json, const RpcDoneCallback &done) {
    json::Document request;
    json::ParseError err = request.parse(json);
    if(err != json::PARSE_OK)
        throw Exception("parse failed");
    switch (request.getType()){
        case json::TYPE_OBJECT:
            if(isNotify(request))
                handleSingleNotify(request);
            else
                handleSingleRequest(request,done);
            break;
        case json::TYPE_ARRAY:
            handleBatchRequests(request,done);
            break;
        default:
           throw Exception("request should be json object or array");
    }
}


void RpcServer::handleBatchRequests(json::Value &requests, const RpcDoneCallback &done) {
    size_t num = requests.getSize();
    if(num == 0)
        throw Exception("batch request is empty");

    ThreadSafeBatchResponse responses(done);

    for(int i=0;i<num;i++){
        auto& request = requests[i];
        if(!request.isObject())
            throw Exception("request json is bad");

        if(isNotify(request))
            handleSingleNotify(request);
        else{
            handleSingleRequest(request,[=](json::Value response){
                responses.addResponse(response);
            });
        }
    }

}




void RpcServer::handleSingleRequest(json::Value &request, const RpcDoneCallback &done) {

    validateRequest(request);
    // serviceName.methodName
    auto& id = request["id"];
    auto methodName = request["method"].getStringView();
    auto pos = methodName.find(".");
    if(pos == std::string_view::npos)
        throw Exception("missing service Name in method");
    auto serviceName = methodName.substr(0,pos);
    auto it = serviceList_.find(serviceName);
    if(it == serviceList_.end())
        throw Exception("serviceName not found");

    methodName.remove_prefix(pos + 1);
    if(methodName.length() == 0)
        throw Exception("missing method name in method");

    auto& service = it->second;
    service->callProcedureReturn(methodName,request,done);

}

void RpcServer::handleSingleNotify(json::Value &request) {
    validateNotify(request);
    auto methodName = request["method"].getStringView();
    auto pos = methodName.find('.');
    if(pos == std::string_view::npos)
        throw Exception("missing service Name in method");
    auto serviceName = methodName.substr(0,pos);
    auto it = serviceList_.find(serviceName);
    if(it == serviceList_.end())
        throw Exception("serviceName not found");

    methodName.remove_prefix(pos + 1);
    if(methodName.length() == 0)
        throw Exception("missing method name in method");

    auto& service = it->second;
    service->callProcedureNotify(methodName, request);

}





// 验证请求json
void RpcServer::validateRequest(json::Value &request) {
    auto& id = findValue<
            json::TYPE_STRING,
            json::TYPE_INT32,
            json::TYPE_INT64>(request,"id");

    auto& version = findValue<json::TYPE_STRING>(request,"jsonrpc");
    if(version.getStringView() != "2.0")
        throw Exception("jsonrpc version is unknown");

    auto& method = findValue<json::TYPE_STRING>(request,"method");
    if(method.getStringView() == "rpc.")
        throw Exception("method name is internal used");
    // jsonrpc, method, id, params
    size_t nMembers = 3u + hasParams(request);

    if(request.getSize() != nMembers)
        throw Exception("unexpected field");
}

// 验证通知json
void RpcServer::validateNotify(json::Value &request) {

    auto& id = findValue<
            json::TYPE_STRING,
            json::TYPE_INT32,
            json::TYPE_INT64>(request,"id");

    auto& version = findValue<json::TYPE_STRING>(request,"jsonrpc");
    if(version.getStringView() != "2.0")
        throw Exception("jsonrpc version is unknown");

    auto& method = findValue<json::TYPE_STRING>(request,"method");
    if(method.getStringView() == "rpc.")
        throw Exception("method name is internal used");

    // jsonrpc, method, params, no id
    size_t nMembers = 2u + hasParams(request);

    if(request.getSize() != nMembers)
        throw Exception("unexpected field");

}