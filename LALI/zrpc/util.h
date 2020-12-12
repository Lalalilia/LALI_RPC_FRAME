//
// Created by lalalilia
//

#ifndef ZILIPROJECT_UTIL_H
#define ZILIPROJECT_UTIL_H

#include <string>
#include <string_view>
#include <jackson/Value.h>
#include <functional>
#include <jackson/Document.h>
#include <iostream>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;


typedef std::function<void(json::Value response)> RpcDoneCallback;

class UserDoneCallback{
public:
    UserDoneCallback(json::Value& request,
                     const RpcDoneCallback& callback)
                     :request_(request),
                     callback_(callback)
    {}

    void operator()(json::Value&& result) const{
        json::Value response(json::TYPE_OBJECT);
        response.addMember("jsonrpc", "2.0");
        response.addMember("id", request_["id"]);
        response.addMember("result", result);
        callback_(response);
    }

private:
    mutable json::Value request_;
    RpcDoneCallback callback_;
};

#endif //ZILIPROJECT_UTIL_H
