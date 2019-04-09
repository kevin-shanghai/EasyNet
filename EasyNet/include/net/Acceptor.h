#ifndef ACCEPTOR_H__
#define ACCEPTOR_H__
#include "EasyNet/include/base/PlatformDefine.h"
#include "EasyNet/include/base/Noncopyable.h"
#include "EasyNet/include/net/Channel.h"
#include "EasyNet/include/net/Socket.h"
#include "EasyNet/include/net/ICallBackDefine.h"


namespace Net
{
    class EventLoop;
    class InternetAddress;

    class Acceptor : private Noncopyable
    {
    public:
        Acceptor(EventLoop* loop, const InternetAddress& listenAddr);
        ~Acceptor();

        void setNewConnectionCallback(const NewConnectionCallback& cb)
        { newConnectionCallback_ = cb; }

        bool listenning() const { return listenning_; }
        void Listen();

    private:
        void HandleRead();


    private:
        EventLoop* loop_;
        Socket acceptSocket_;
        Channel acceptChannel_;
        NewConnectionCallback newConnectionCallback_;
        bool listenning_;
    };
}

#endif