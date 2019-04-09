#ifndef RPC_SERVER_H__
#define RPC_SERVER_H__

#include "EasyNet/include/net/InternetAddress.h"
#include "EasyNet/include/net/TcpServer.h"
#include "EasyNet/include/net/ICallBackDefine.h"


namespace google
{
    namespace protobuf
    {
        class Service;
    }
}


namespace Net
{
    class EventLoop;
    class RpcServer
    {
    public:
        RpcServer(EventLoop* loop, const InternetAddress& addr);

        void RegisterService(::google::protobuf::Service*);
        void Start();

    private:
        void onConnection(const TcpConnectionPtr& conn);

    private:
        TcpServer server_;
        std::map<std::string, ::google::protobuf::Service*> serviceMap_;

    };
}


#endif