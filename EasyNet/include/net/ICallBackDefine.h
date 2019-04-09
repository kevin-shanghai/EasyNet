#ifndef ICALL_BACK_TYPE_H__
#define ICALL_BACK_TYPE_H__

#include "EasyNet/include/base/PlatformDefine.h"
#include "EasyNet/include/base/TimeStamp.h"
#include "google/protobuf/message.h"
namespace Net
{
    class ChannelBuffer;
    class TcpConnection;
    class InternetAddress;
    typedef Shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef Shared_ptr<google::protobuf::Message> ProtobufMessagePtr;

    typedef Function<void()> TimerCallback;
    typedef Function<void (const TcpConnectionPtr&)> ConnectionCallback;
    typedef Function<void (const TcpConnectionPtr&)> CloseCallback;
    typedef Function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
    typedef Function<void (const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;
    typedef Function<void (const TcpConnectionPtr&, ChannelBuffer*)> MessageCallback;
    typedef Function<void (int sockfd, const InternetAddress&)> NewConnectionCallback;
    typedef Function<void()> EventCallback;
    typedef Function<void()> Functor;

    void defaultConnectionCallback(const TcpConnectionPtr& conn);
    void defaultMessageCallback(const TcpConnectionPtr& conn, ChannelBuffer* buffer);
}

#endif