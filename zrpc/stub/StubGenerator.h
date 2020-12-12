//
// Created by lalalilia
//

#ifndef ZILIPROJECT_STUBGENERATOR_H
#define ZILIPROJECT_STUBGENERATOR_H

#include <jackson/Value.h>

class StubGenerator{
public:
    explicit
    StubGenerator(json::Value& proto)
    {
        parseProto(proto);
    }
    virtual ~StubGenerator() = default;

public:
    // 纯虚函数交给客户端类和服务器类实现
    virtual std::string getStub() = 0;
    virtual std::string getStubClassName() = 0;

protected:
    // 存储json文件中到返回值函数、无返回值通知
    // 利用服务信息结构体存储多个函数和通知
    struct RpcReturn
    {
        RpcReturn(const std::string& name_,
                  json::Value& params_,
                  json::Value& returns_):
                name(name_),
                params(params_),
                returns(returns_)
        {}

        std::string name;
        mutable json::Value params;
        mutable json::Value returns;
    };

    struct RpcNotify
    {
        RpcNotify(const std::string& name_,
                  json::Value& params_):
                name(name_),
                params(params_)
        {}

        std::string name;
        mutable json::Value params;
    };

    struct ServiceInfo
    {
        std::string name;
        std::vector<RpcReturn> rpcReturn;
        std::vector<RpcNotify> rpcNotify;
    };

    ServiceInfo serviceInfo_;

private:
    // 解析json
    void parseProto(json::Value& proto);
    void parseRpc(json::Value& rpc);
    void validateParams(json::Value& params);
    void validateReturns(json::Value& returns);

};

// 置换文本函数，用于生成存根头文件。
inline void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
    while (true) {
        size_t i = str.find(from);
        if (i != std::string::npos) {
            str.replace(i, from.size(), to);
        }
        else return;
    }
}




#endif //ZILIPROJECT_STUBGENERATOR_H
