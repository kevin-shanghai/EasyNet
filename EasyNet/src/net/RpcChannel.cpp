#include "EasyNet/include/net/RpcChannel.h"
#include "EasyNet/include/net/RpcMessage.pb.h"
#include "EasyNet/include/net/TcpConnection.h"
#include "EasyNet/include/base/MutexLock.h"
#include "EasyNet/include/base/Log.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "google/protobuf/stubs/common.h"
#include <assert.h>



using namespace Net;
using namespace base;

static uint64_t id_ = 0;

RpcChannel::RpcChannel() 
                         : mutex_(new MutexLock())
                         , codec_(Bind(&RpcChannel::onRpcMessage, this, Placeholders _1, Placeholders _2))
{

}


RpcChannel::RpcChannel(const TcpConnectionPtr& conn) 
                        : conn_(conn)
                        , mutex_()
                        , codec_(Bind(&RpcChannel::onRpcMessage, this, Placeholders _1, Placeholders _2))
                                                     
{
    
}

RpcChannel::~RpcChannel()
{
    std::cout << "RpcChannel dtor..." << std::endl;
}


void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
    ::google::protobuf::RpcController* controller,
    const ::google::protobuf::Message* request,
    ::google::protobuf::Message* response,
    ::google::protobuf::Closure* done)
{
    controller;
    Rpc::RpcMessage message;
    message.set_type(Rpc::REQUEST);
    //FIXME, id should be atomic//
    int64_t id = id_++;
    message.set_id(id);
    message.set_service(method->service()->name());
    message.set_method(method->name());
    message.set_request(request->SerializeAsString()); 

    UnFinishedRpcCallInfo out(response, done);
    {
        LockGuard lock(mutex_);
        unFinishedRpcCalls_[id] = out;
    }

    codec_.Encode(conn_, message);
}



void RpcChannel::onRpcMessage(const TcpConnectionPtr& conn, const ProtobufMessagePtr& messagePtr)
{
    assert(conn == conn_);
    RpcMessagePtr  messageTmp = Static_Pointer_Cast<Rpc::RpcMessage>(messagePtr);
    Rpc::RpcMessage& message = *messageTmp.get();
    if (message.type() == Rpc::RESPONSE)
    {
        HandleRpcResponse(message);
    }
    else if (message.type() == Rpc::REQUEST)
    {
        HandleRpcRequest(conn, message);   
    }
    else
    {
        LOG_ERROR << "not rpc message, can not handle here.";
    }
}

void RpcChannel::HandleRpcRequest(const TcpConnectionPtr& conn, const Rpc::RpcMessage& message)
{
    conn;
    Rpc::ErrorCode error = Rpc::WRONG_PROTO;
    if (serviceMap_)
    {
        std::map<std::string, google::protobuf::Service*>::const_iterator it = serviceMap_->find(message.service());
        if (it != serviceMap_->end())
        {
            google::protobuf::Service* service = it->second;
            assert(service != NULL);
            const google::protobuf::ServiceDescriptor* desc = service->GetDescriptor();
            const google::protobuf::MethodDescriptor* method
                = desc->FindMethodByName(message.method());
            if (method)
            {
                Scoped_ptr<google::protobuf::Message> request(service->GetRequestPrototype(method).New());
                if (request->ParseFromString(message.request()))
                {
                    google::protobuf::Message* response = service->GetResponsePrototype(method).New();
                    // response is deleted in doneCallback
                    int64_t id = message.id();
                    service->CallMethod(method, NULL, request.get(), response,
                        NewCallback(this, &RpcChannel::DoneCallback, response, id));
                    error = Rpc::NO_RPC_ERROR;
                }
                else
                {
                    error = Rpc::INVALID_REQUEST;
                }
            }
            else
            {
                error = Rpc::NO_METHOD;
            }
        }
        else
        {
            error = Rpc::NO_SERVICE;
        }
    }
    else
    {
        error = Rpc::NO_SERVICE;
    }
    if (error != Rpc::NO_RPC_ERROR)
    {
        Rpc::RpcMessage response;
        response.set_type(Rpc::RESPONSE);
        response.set_id(message.id());
        response.set_error(error);
        codec_.Encode(conn_, response);
    }
}

void RpcChannel::HandleRpcResponse(const Rpc::RpcMessage& message)
{
    int64_t id = message.id();
    assert(message.has_response() || message.has_error());

    UnFinishedRpcCallInfo unFinishedCall;

    {
        LockGuard lock(mutex_);
        std::map<int64_t, UnFinishedRpcCallInfo>::iterator it = unFinishedRpcCalls_.find(id);
        if (it != unFinishedRpcCalls_.end())
        {
            unFinishedCall = it->second;
            unFinishedRpcCalls_.erase(it);
        }
    }

    if (unFinishedCall.response)
    {
        Scoped_ptr<google::protobuf::Message> d(unFinishedCall.response);
        if (message.has_response())
        {
            unFinishedCall.response->ParseFromString(message.response());
        }
        if (unFinishedCall.done)
        {
            unFinishedCall.done->Run();
        }
    }
}

void RpcChannel::DoneCallback(::google::protobuf::Message* response, int64_t id)
{
    Scoped_ptr<google::protobuf::Message> d(response);
    Rpc::RpcMessage message;
    message.set_type(Rpc::RESPONSE);
    message.set_id(id);
    message.set_response(response->SerializeAsString()); 
    codec_.Encode(conn_, message);
}

void RpcChannel::onMessage(const TcpConnectionPtr& conn, ChannelBuffer* buf)
{
    //parse message from buffer, when a single rpc(protobuf) message parsed success,
    // the onrpcMessage function will be called //
    codec_.Decode(conn, buf);
}