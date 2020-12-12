//
// Created by lalalilia
//

#ifndef ZILIPROJECT_PROCEDURE_H
#define ZILIPROJECT_PROCEDURE_H

#include <jackson/Value.h>
#include <functional>
#include <zrpc/util.h>

typedef std::function<void(json::Value&, const RpcDoneCallback&)> ProcedureReturnCallback;
typedef std::function<void(json::Value&)> ProcedureNotifyCallback;

template <typename Func>
class Procedure{
public:
    template <typename ... ParamNameAndTypes>
    explicit Procedure(Func&& callback,ParamNameAndTypes&&... nameAndTypes):
        callback_(std::forward<Func>(callback)){
        constexpr int n = sizeof...(nameAndTypes);
        if constexpr (n > 0)
            initProcedure(nameAndTypes...);
    }

        // 调用程序通知和函数
    void invoke(json::Value& request);
    void invoke(json::Value& request,const RpcDoneCallback& done);


private:
    template <typename Name,typename ... ParaNameAndTypes>
    void initProcedure(Name paramName,json::ValueType paramType,ParaNameAndTypes&& ... nameAndTypes){
        params_.emplace_back(paramName, paramType);
        if constexpr (sizeof...(ParaNameAndTypes) > 0)
            initProcedure(nameAndTypes...);
    }

    template<typename Name, typename Type, typename... ParamNameAndTypes>
    void initProcedure(Name paramName, Type parmType, ParamNameAndTypes &&... nameAndTypes)
    {
        static_assert(std::is_same_v<Type, json::ValueType>, "param type must be json::ValueType");
    }

    // 有效性判断
    void validateRequest(json::Value& request) const;
    bool validateGeneric(json::Value& request) const;
private:
    // 程序结构体 存储函数名，函数类型
    struct Param{
        Param(std::string_view paramName_,json::ValueType paramType_):
            paramName(paramName_),
            paramType(paramType_){

        }
        std::string_view paramName;
        json::ValueType paramType;
    };
private:
    // 存储回调和程序函数
    Func callback_;
    std::vector<Param> params_;

};

typedef Procedure<ProcedureReturnCallback> ProcedureReturn;
typedef Procedure<ProcedureNotifyCallback> ProcedureNotify;

#endif //ZILIPROJECT_PROCEDURE_H