#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/RpcServer.h"
#include "EasyNet/include/net/FooService.pb.h"
#include "EasyNet/include/net/InternetAddress.h"
#include "EasyNet/include/net/RpcChannel.h"
#include "EasyNet/include/net/TcpConnection.h"
#include "EasyNet/include/net/TcpClient.h"


using namespace Net;


class RpcClient
{
public:
    RpcClient(EventLoop* loop, const InternetAddress& serverAddr)
        : loop_(loop),
        client_(loop, serverAddr, "RpcClient"),
        channel_(new RpcChannel()),
        stub_(channel_.get())
    {
        client_.setConnectionCallback(
            Bind(&RpcClient::onConnection, this, Placeholders _1));
        client_.setMessageCallback(
            Bind(&RpcChannel::onMessage, channel_.get(), Placeholders _1, Placeholders _2));
        client_.EnableRetry();
    }

    void connect()
    {
        client_.connect();
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        if (conn->IsConnected())
        {
            channel_->SetConnection(conn);

            FooRequest request;
            request.set_text("001010");
            FooResponse* response = new FooResponse();

            stub_.Foo(NULL, &request, response, NewCallback(this, &RpcClient::RpcFinished, response));
        }
    }

    void RpcFinished(FooResponse* resp)
    {
        std::cout << "Rpc call finished..." << resp->DebugString().c_str();


    }

    EventLoop* loop_;
    TcpClient client_;
    Shared_ptr<RpcChannel> channel_;
    FooService::Stub stub_;
};

int main_rpc_client_ut()
{

    EventLoop loop;
    InternetAddress serverAddr("10.8.61.53", 2000);

    RpcClient rpcClient(&loop, serverAddr);
    rpcClient.connect();
    loop.Loop();

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
