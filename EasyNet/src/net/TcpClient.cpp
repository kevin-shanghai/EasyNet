#include "EasyNet/include/net/TcpClient.h"
#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/TcpConnector.h"
#include "EasyNet/include/net/TcpConnection.h"
#include "EasyNet/include/net/ICallBackDefine.h"


using namespace base;

namespace Net
{
    TcpClient::TcpClient(EventLoop*loop, const InternetAddress& serverAddr, const std::string& nameArg)
        :loop_(loop),
        connector_(new Connector(loop,serverAddr)),
        name_(nameArg),
        connectionCallback_(defaultConnectionCallback),
        messageCallback_(defaultMessageCallback),
        retry_(false),
        connect_(true),
        nextConnId_(1)
    {
        connector_->setNewConnectionCallback(Bind(&TcpClient::newConnection, this, Placeholders _1));
        std::cout << "TcpClient::TcpClient [" << name_ << "]";
    }

    TcpClient::~TcpClient()
    {
        std::cout << "TcpClient::~TcpClient [" << name_ << "]";
        TcpConnectionPtr conn;
        bool unique = false;
        {
            LockGuard lck(&mutex_);
            unique = connection_.unique();
            conn = connection_;
        }
        if (conn)
        {
          
        }
        else
        {
            connector_->stop();
            //FIXME: HACK
            //loop_->runAfter(1, boost::bind(&detail::removeConnector, connector_));
        }
    }
    void TcpClient::connect()
    {
        // FIXME: check state
        std::cout << "TcpClient::connect[" << name_ << "] - connecting to "
            << connector_->serverAddress().GetIpAndPort();
        connect_ = true;
        connector_->start();
    }

    void TcpClient::disconnect()
    {
        connect_ = false;
        {
            LockGuard lck(&mutex_);
            if (connection_)
            {
                connection_->ShutDownWriteForHalfClose();
            }
        }
    }
    void TcpClient::stop()
    {
        connect_ = false;
        connector_->stop();
    }

     EventLoop* TcpClient::getLoop() const { return loop_; }

    void TcpClient::newConnection(int sockfd)
    {
        loop_->AssertInLoopThreadOrDie();
        sockaddr_in_t peer = SocketsApi::GetPeerAddr(sockfd);
        InternetAddress peerAddr(peer);
        char buf[32];
        snprintf(buf, sizeof buf, ":%s#%d", peerAddr.GetIpAndPort().c_str(), nextConnId_);
        ++nextConnId_;
        std::string connName = name_ + buf;

        sockaddr_in_t local = SocketsApi::GetLocalAddr(sockfd);
        InternetAddress localAddr(local);

        TcpConnectionPtr conn(new TcpConnection(loop_,
            connName,
            sockfd,
            localAddr,
            peerAddr));

        conn->SetTcpConnectionCallBack(connectionCallback_);
        conn->SetMessageCallBack(messageCallback_);
        
        conn->SetCloseCallback(
            Bind(&TcpClient::removeConnection, this, Placeholders _1)); 
        {
            LockGuard lk(&mutex_);
            connection_ = conn;
        }
        conn->ConnectionEstablished();
    }

    void TcpClient::removeConnection(const TcpConnectionPtr& conn)
    {
        loop_->AssertInLoopThreadOrDie();
        assert(loop_ == conn->GetConnectionEventLoop());
        {
            LockGuard lk(&mutex_);
            assert(connection_ == conn);
            connection_.reset();
        }

        Functor f = Bind(&TcpConnection::ConnectionDestroyed, conn);
        loop_->QueueInLoop(f);
        if (retry_ && connect_)
        {
            std::cout << "TcpClient::connect[" << name_ << "] - Reconnecting to "
                << connector_->serverAddress().GetIpAndPort();
            connector_->restart();
        }
    }
}
