//
// Created by lalalilia
//

#ifndef ZILIPROJECT_CLIENTSTUB_H
#define ZILIPROJECT_CLIENTSTUB_H

#include <zrpc/stub/StubGenerator.h>

class ClientStub: public StubGenerator{
public:
    explicit
    ClientStub(json::Value& content):
        StubGenerator(content)
    {}
    // 获取存根头文件主体
    std::string getStub() override ;
    // 获取存根类名
    std::string getStubClassName() override ;

private:
    // 获取文件名
    std::string getMacroName();

    // 获取程序定义
    std::string getProcedureDefinitions();
    // 获取通知程序定义
    std::string getNotifyDefinitions();

    // 获取程序参数
    template <typename Rpc>
    std::string getGenericArgs(const Rpc& r,bool append);

    // 获取程序成员
    template <typename Rpc>
    std::string getGenericParamMembers(const Rpc& r);


};

#endif //ZILIPROJECT_CLIENTSTUB_H
