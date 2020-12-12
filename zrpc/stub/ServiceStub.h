//
// Created by lalalilia
//

#ifndef ZILIPROJECT_SERVICESTUB_H
#define ZILIPROJECT_SERVICESTUB_H

#include <zrpc/stub/StubGenerator.h>

class ServiceStubGenerator:public StubGenerator{

public:
    explicit ServiceStubGenerator(json::Value& proto):
        StubGenerator(proto){

    }
    // 获取存根头文件主题
    std::string getStub() override ;
    // 获取存根类名
    std::string getStubClassName() override ;

private:
    // 获取文件名
    std::string getMacroName();
    // 获取用户类名
    std::string getUserClassName();

    // 获取存根程序与服务器链接
    std::string getStubProcedureBindings();
    // 获取存根程序定义
    std::string getStubProcedureDefinitions();

    // 获取通知函数服务器链接
    std::string getStubNotifyBindings();
    // 获取通知函数定义
    std::string getStubNotifyDefinitions();

    // 获取存根函数名
    template <typename Rpc>
    std::string getStubGenericName(const Rpc& r);
    // 获取函数参数
    template <typename Rpc>
    std::string getGenericParams(const Rpc& r);
    // 获取函数调用参数
    template <typename Rpc>
    std::string getGenericArgs(const Rpc& r);
    // 获取json中数组
    template <typename Rpc>
    std::string getParamsFromJsonArray(const Rpc& r);
    // 获取json中对象
    template <typename Rpc>
    std::string getParamsFromJsonObject(const Rpc& r);

};


#endif //ZILIPROJECT_SERVICESTUB_H
