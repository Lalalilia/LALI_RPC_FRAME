//
// Created by lalalilia
//

#include <test/ArithmeticServiceStub.h>
#include <base/ThreadPool.h>

class ArithmeticService: public ArithmeticServiceStub<ArithmeticService>
{
public:
    explicit
    ArithmeticService(RpcServer& server):
            ArithmeticServiceStub(server)
    {
        pool_.start(0);
    }

    void Add(double lhs, double rhs, const UserDoneCallback& cb)
    {
        pool_.run([=](){
            cb(json::Value(lhs + rhs));
        });
    }

    void Sub(double lhs, double rhs, const UserDoneCallback& cb)
    {
        pool_.run([=](){
            cb(json::Value(lhs - rhs));
        });
    }

private:
    ThreadPool pool_;

};

int main()
{
    EventLoop loop;
    AddressOps addr(9877);

    RpcServer rpcServer(&loop, addr);
    ArithmeticService service(rpcServer);

    rpcServer.start();
    loop.loop();
}