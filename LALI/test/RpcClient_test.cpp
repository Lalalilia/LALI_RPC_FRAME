//
// Created by lalalilia
//


#include <random>
#include <iostream>

#include <test/RpcTestClientStub.h>
#include <net/EventLoop.h>


std::random_device rd; 
std::mt19937 gen(rd());
std::uniform_int_distribution dis(0, 10);


void run(RpcTestClientStub& client)
{
    double lhs = dis(gen);
    double rhs = dis(gen);
    // 用 lambda 表达式作为消息回调函数
    client.Add(lhs, rhs, [=](json::Value response, bool isError, bool timeout) {
        if (!isError) {
            std::cout << lhs << "/" << rhs << "="
                      << response.getDouble() << "\n";
        }
        else if (timeout) {
            std::cout << "timeout\n";
        }
        else {
            std::cout << "response: "
                      << response["message"].getStringView() << ": "
                      << response["data"].getStringView() << "\n";
        }
    });
}

int main()
{
    EventLoop loop;
    AddressOps addr(9877);
    RpcTestClientStub client(&loop, addr);

    client.setConnectionCallback([&](const TcpConnectionPtr& conn) {
        if (conn->disconnected()) {
            loop.quit();
        }
        else {
            // 每2秒进行一次 rpc 请求
            loop.runRepeat([&] {
                run(client);
            },2);
        }
    });
    client.start();
    loop.loop();
}
