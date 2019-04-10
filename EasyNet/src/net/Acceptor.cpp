#include "EasyNet/include/net/Acceptor.h"
#include "EasyNet/include/net/InternetAddress.h"
#include "EasyNet/include/net/EventLoop.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>   
#include <iostream>
#include <assert.h>


namespace Net
{
    Acceptor::Acceptor(EventLoop* loop, const InternetAddress& listenAddr)
        : loop_(loop),
        acceptSocket_(SocketsApi::CreateNonBlockingSocket()),
        acceptChannel_(loop, acceptSocket_.GetFd()),
        listenning_(false)
    {
        acceptSocket_.SetReuseAddr(true);
        acceptSocket_.SetReusePort(false);
        acceptSocket_.BindAddress(listenAddr);
        acceptChannel_.SetReadCallback(
            Bind(&Acceptor::HandleRead, this));
    }

    Acceptor::~Acceptor()
    {
        acceptChannel_.DisableAll();
        acceptChannel_.Remove();
    }

    void Acceptor::Listen()
    {
        loop_->AssertInLoopThreadOrDie();
        listenning_ = true;
        acceptSocket_.Listen();
        acceptChannel_.EnableReading();
    }

    void Acceptor::HandleRead()
    {
        loop_->AssertInLoopThreadOrDie();
        InternetAddress peerAddr;
        int connfd = acceptSocket_.Accept(peerAddr);
        if (connfd >= 0)
        {
            if (newConnectionCallback_)
            {
                newConnectionCallback_(connfd, peerAddr);
            }
            else 
            {
                SocketsApi::Close(connfd);
            }
        }
        else
        {
        }
    }

}

