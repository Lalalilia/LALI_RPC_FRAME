//
// Created by lalalilia
//

#ifndef ZILIPROJECT_RPCSERVICE_H
#define ZILIPROJECT_RPCSERVICE_H

#include <zrpc/server/Procedure.h>

class RpcService{
public:
    void addProcedureReturn(std::string_view methodName, ProcedureReturn* ptr){
        assert(procedureReturnList_.find(methodName) == procedureReturnList_.end());
        procedureReturnList_.emplace(methodName,ptr);
    }

    void addProcedureNotify(std::string_view methodName, ProcedureNotify* ptr){
        assert(procedureNotifyList_.find(methodName) == procedureNotifyList_.end());
        procedureNotifyList_.emplace(methodName,ptr);
    }

    void callProcedureReturn(std::string_view methodName,
                             json::Value& request,
                             const RpcDoneCallback& done);
    void callProcedureNotify(std::string_view methodName,
                             json::Value& request);

private:
    // 程序类指针
    typedef std::unique_ptr<ProcedureReturn> ProcedureReturnPtr;
    typedef std::unique_ptr<ProcedureNotify> ProcedureNotifyPtr;

    // 程序类指针和程序名映射表
    typedef std::unordered_map<std::string_view ,ProcedureReturnPtr> ProcedureReturnList;
    typedef std::unordered_map<std::string_view ,ProcedureNotifyPtr> ProcedureNotifyList;

    ProcedureReturnList procedureReturnList_;
    ProcedureNotifyList procedureNotifyList_;

};


#endif //ZILIPROJECT_RPCSERVER_H
