//
// Created by lalalilia
//

#include <zrpc/server/Procedure.h>
#include <exception>


template<>
void Procedure<ProcedureReturnCallback>::validateRequest(json::Value &request) const {
    switch (request.getType()){
        case json::TYPE_OBJECT:
        case json::TYPE_ARRAY:
            if (!validateGeneric(request))
                throw "params name or type mismatch";
            break;
        default:
            throw "params type must be object or array";
    }
}

template <>
void Procedure<ProcedureNotifyCallback>::validateRequest(json::Value &request) const {
    switch (request.getType()){
        case json::TYPE_OBJECT:
        case json::TYPE_ARRAY:
            if (!validateGeneric(request))
                throw "params name or type mismatch";
            break;
        default:
            throw "params type must be object or array";
    }
}

template <typename Func>
bool Procedure<Func>::validateGeneric(json::Value &request) const {
    auto mIter = request.findMember("params");
    if(mIter == request.memberEnd()){
        return params_.empty();
    }

    auto& params = mIter->value;
    if(params.getSize() == 0 || params.getSize() != params_.size()){
        return false;
    }

    switch (params.getType()){
        case json::TYPE_ARRAY:
            for(size_t i = 0; i < params_.size(); i++){
                if(params[i].getType() != params_[i].paramType)
                    return false;
            }
            break;
        case json::TYPE_OBJECT:
            for(auto& temp:params_){
                auto it = params.findMember(temp.paramName);
                if(it == params.memberEnd())
                    return false;
                if(it->value.getType() != temp.paramType)
                    return false;
            }
            break;
        default:
            return false;
    }
    return true;
}

template <>
void Procedure<ProcedureReturnCallback>::invoke(json::Value& request,
                                                const RpcDoneCallback& done)
{
    validateRequest(request);
    callback_(request, done);
}

template <>
void Procedure<ProcedureNotifyCallback>::invoke(json::Value& request)
{
    validateRequest(request);
    callback_(request);
}
