#ifndef TCP_CLIENT_H__
#define TCP_CLIENT_H__


#include "EasyNet/include/base/PlatformDefine.h"
#include "EasyNet/include/base/MutexLock.h"
#include "EasyNet/include/net/ICallBackDefine.h"


#ifdef EXPORT_NETBASElIGHT
#define CSW_EXPORT_OWNER
#endif

namespace Net
{
    class Connector;
    class InternetAddress;
    class EventLoop;
    typedef Shared_ptr<Connector> ConnectorPtr;

    class EXPORT_NETBASElIGHT TcpClient 
    {
    public:
        TcpClient(EventLoop*loop,
            const InternetAddress& serverAddr,
            const std::string& nameArg);
        ~TcpClient();

        void connect();
        void disconnect();
        void stop();

        TcpConnectionPtr connection() const
        {
            base::LockGuard lk(&mutex_);
            return connection_;
        }

        EventLoop* getLoop() const;
        bool retry() const { return retry_; }
        void EnableRetry() { retry_ = true; }
        const std::string& name() const { return name_; }

        /// Not thread safe.
        void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
        void setMessageCallback(const MessageCallback& cb) {  messageCallback_ = cb; }
        void setWriteCompleteCallback_(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }
    private:
        /// Not thread safe, but in loop
        void newConnection(int sockfd);
        /// Not thread safe, but in loop
        void removeConnection(const TcpConnectionPtr& conn);

        EventLoop* loop_;
        ConnectorPtr connector_;
        const std::string name_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        bool retry_;
        bool connect_;
        int nextConnId_;
        mutable base::MutexLock mutex_;
        TcpConnectionPtr connection_;

    };//end class TcpClient
}// end namespace net



#endif