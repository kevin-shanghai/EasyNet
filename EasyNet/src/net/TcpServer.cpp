#include "EasyNet/include/net/TcpServer.h"
#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/TcpConnection.h"
#include "EasyNet/include/net/Socket.h"
#include "EasyNet/include/net/SocketApiWrapper.h"
#include "EasyNet/include/net/LengthFieldFrameCodec.h"
#include "EasyNet/include/net/Acceptor.h"
#include "EasyNet/include/net/InternetAddress.h"
#include "EasyNet/include/net/EventLoopThreadGroup.h"
#include <sstream>
#include <iostream>




namespace Net
{
    TcpServer::TcpServer(EventLoop* loop,
        const InternetAddress& listenAddr,
        const string& nameArg)
        : loop_(loop)
        , hostport_(listenAddr.GetIpAndPort())
        , name_(nameArg)
        , acceptor_(new Acceptor(loop, listenAddr))
        , threadGroup_(new EventLoopThreadGroup(loop))
        , connectionCallback_(defaultConnectionCallback)
        , messageCallback_(defaultMessageCallback)
        , nextConnId_(1)
        , started_(false)
    {
        acceptor_->setNewConnectionCallback(
            Bind(&TcpServer::newConnection, this, Placeholders _1, Placeholders _2));

    }

    TcpServer::~TcpServer()
    {
        loop_->AssertInLoopThreadOrDie();
        //CLOG_INFO("TcpServer::~TcpServer [" << name_ << "] destructing");
        ConnectionMap::iterator it = connections_.begin();
        for ( ; it != connections_.end(); ++it)
        {
            TcpConnectionPtr conn = it->second;
            it->second.reset();
            EventLoop::Functor functor = Bind(&TcpConnection::ConnectionDestroyed, conn);
            conn->GetConnectionEventLoop()->RunInLoop(functor);
            conn.reset();
        }
    }

    void TcpServer::setThreadNum(int numThreads)
    {
        assert(0 <= numThreads);
        threadGroup_->SetThreadNum(numThreads);
    }

    void TcpServer::start()
    {
        if (!started_)
        {
            threadGroup_->Start(threadInitCallback_);
            assert(!acceptor_->listenning());
            
            EventLoop::Functor functor = Bind(&Acceptor::Listen, acceptor_.get());
            loop_->RunInLoop(functor);
            started_ = true;
        }
    }

    void TcpServer::newConnection(int sockfd, const InternetAddress& peerAddr)
    {
        loop_->AssertInLoopThreadOrDie();
        EventLoop* ioLoop = threadGroup_->GetNextLoop();
        char buf[32];
        snprintf(buf, sizeof buf, ":%s#%d", hostport_.c_str(), nextConnId_);
        ++nextConnId_;
        string connName = name_ + buf;

        //CLOG_INFO("TcpServer::newConnection [" << name_
        //    << "] - new connection [" << connName
        //    << "] from " << peerAddr.GetIpAndPort());
        
        sockaddr_in_t local = SocketsApi::GetLocalAddr(sockfd);
        InternetAddress localAddr(local);

        TcpConnectionPtr conn(new TcpConnection(ioLoop,
            connName,
            sockfd,
            localAddr,
            peerAddr));
        connections_[connName] = conn;

        conn->SetTcpConnectionCallBack(connectionCallback_);
        conn->SetMessageCallBack(messageCallback_);
        conn->SetCloseCallback(Bind(&TcpServer::RemoveConnection, this, Placeholders _1)); 

        EventLoop::Functor functor = Bind(&TcpConnection::ConnectionEstablished, conn);        
        ioLoop->RunInLoop(functor);
    }

    void TcpServer::RemoveConnection(const TcpConnectionPtr& conn)
    {
        EventLoop::Functor functor = Bind(&TcpServer::RemoveConnectionInLoop, this, conn);
        loop_->RunInLoop(functor);
    }

    void TcpServer::RemoveConnectionInLoop(const TcpConnectionPtr& conn)
    {
        loop_->AssertInLoopThreadOrDie();
        std::string connRawName = conn->GetConnectionRawName();
        std::cout << "TcpServer::removeConnectionInLoop [" << name_
            << "] - connection " << connRawName << std::endl;
        size_t n = connections_.erase(connRawName);
        assert(n == 1);
        EventLoop* ioLoop = conn->GetConnectionEventLoop();
        EventLoop::Functor functor = Bind(&TcpConnection::ConnectionDestroyed, conn);
        ioLoop->QueueInLoop(functor);
    }


}
