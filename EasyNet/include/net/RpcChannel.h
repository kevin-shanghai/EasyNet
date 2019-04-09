#ifndef RPC_CHANNEL_H__
#define RPC_CHANNEL_H__

#include "EasyNet/include/net/ProtobufFrameCodec.h"
#include "google/protobuf/service.h"
#include <map>

namespace google 
{
    namespace protobuf 
    {
        class Descriptor;            
        class ServiceDescriptor;     
        class MethodDescriptor;      
        class Message;               
        class Closure;
        class RpcController;
        class Service;

    }
}  

namespace base
{
    class MutexLock;
}


namespace Rpc
{
    class RpcMessage;
}



namespace Net
{
    class ChannelBuffer;
    class RpcChannel : public google::protobuf::RpcChannel
    {
    public:
        typedef Shared_ptr<Rpc::RpcMessage> RpcMessagePtr;
        RpcChannel();
        RpcChannel(const TcpConnectionPtr& conn);
        virtual ~RpcChannel();

        void CallMethod(const ::google::protobuf::MethodDescriptor* method,
            ::google::protobuf::RpcController* controller,
            const ::google::protobuf::Message* request,
            ::google::protobuf::Message* response,
            ::google::protobuf::Closure* done);


        void onRpcMessage(const TcpConnectionPtr& conn, const ProtobufMessagePtr& messagePtr);

        void DoneCallback(::google::protobuf::Message* response, int64_t id);

        void onMessage(const TcpConnectionPtr& conn, ChannelBuffer* buf);

        void SetServicesMap(const std::map<std::string, ::google::protobuf::Service*>* services)
        {
            serviceMap_ = services;
        }

        void SetConnection(const TcpConnectionPtr& conn)
        {
            conn_ = conn;
        }

    private:
        void HandleRpcRequest(const TcpConnectionPtr& conn, const Rpc::RpcMessage& message);

        void HandleRpcResponse(const Rpc::RpcMessage& message);



    private:
        struct UnFinishedRpcCallInfo
        {
            UnFinishedRpcCallInfo(::google::protobuf::Message* response, ::google::protobuf::Closure* done) 
                                    : response (response)
                                    , done (done)
            {

            }

            UnFinishedRpcCallInfo() : response(nullptr)
                                    , done(nullptr)
            {

            }
            ::google::protobuf::Message* response;
            ::google::protobuf::Closure* done;
        };


    private:
        const std::map<std::string, ::google::protobuf::Service*>* serviceMap_;
        ProtobufFrameCodec codec_;
        TcpConnectionPtr conn_;
        std::map<int64_t, UnFinishedRpcCallInfo> unFinishedRpcCalls_;
        base::MutexLock* mutex_;

        
    };

}


#endif