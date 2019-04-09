#include "EasyNet/include/net/RpcServer.h"
#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/TcpConnection.h"
#include "EasyNet/include/net/RpcChannel.h"
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"


namespace Net
{
    RpcServer::RpcServer(EventLoop* loop, const InternetAddress& addr)
        : server_(loop, addr, "rpc_server")
    {
        server_.setConnectionCallback(
            Bind(&RpcServer::onConnection, this, Placeholders _1));
    }


    void RpcServer::RegisterService(::google::protobuf::Service* service)
    {
        const std::string& name = service->GetDescriptor()->name();
        assert (serviceMap_.find(name) == serviceMap_.end());
        serviceMap_[name] = service;
    }

    void RpcServer::Start() 
    {
        server_.start();
    }

    void RpcServer::onConnection(const TcpConnectionPtr& conn)
    {
        std::cout << "RpcServer - " << conn->GetPeerAddress().GetIpAndPort() << " -> "
            << conn->GetLocalAddress().GetIpAndPort() << " is "
            << (conn->IsConnected() ? "UP" : "DOWN");
        if (conn->IsConnected())
        {
            Shared_ptr<RpcChannel> channel(new RpcChannel(conn));
            channel->SetServicesMap(&serviceMap_);
            conn->SetMessageCallBack(Bind(&RpcChannel::onMessage, channel , Placeholders _1, Placeholders _2));

        }
        else
        {
            
        }
    }
}