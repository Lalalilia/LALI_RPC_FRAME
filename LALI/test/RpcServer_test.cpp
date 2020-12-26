//
// Created by lalalilia
//

#include <test/RpcTestServiceStub.h>
#include <base/ThreadPool.h>

class RpcTestService: public RpcTestServiceStub<RpcTestService>
{
public:
    explicit
    RpcTestService(RpcServer& server):
            RpcTestServiceStub(server)
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
    RpcTestService service(rpcServer);

    rpcServer.start();
    loop.loop();
}
