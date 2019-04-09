#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/RpcServer.h"
#include "EasyNet/include/net/FooService.pb.h"


using namespace Net;

class EchoServiceImplement: public FooService
{
public:
    EchoServiceImplement()
    {

    }

    virtual void Foo(::google::protobuf::RpcController* controller,
        const ::FooRequest* request,
        ::FooResponse* response,
        ::google::protobuf::Closure* done)
    {
        request; controller;
        response->set_text("uih rpc test");
        response->set_result(true);
        done->Run();
    }

};


int main_rtp_server_ut()
{
    EventLoop loop;
    InternetAddress serverAddr(2000);
    RpcServer server(&loop, serverAddr);
    EchoServiceImplement echoServiceImpl;
    server.RegisterService(&echoServiceImpl);

    server.Start();
    loop.Loop();
    return 0;

}