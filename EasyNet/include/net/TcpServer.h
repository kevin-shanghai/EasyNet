#ifndef TCP_SERVER_H__
#define TCP_SERVER_H__

#include "EasyNet/include/net/ICallBackDefine.h"
#include "EasyNet/include/base/Noncopyable.h"
#include "EasyNet/include/net/InternetAddress.h"
#include <map>

#ifdef EXPORT_NETBASElIGHT
#define CSW_EXPORT_OWNER
#endif

namespace Net
{
    class Acceptor;
    class EventLoop;
    class EventLoopThreadGroup;
    class EXPORT_NETBASElIGHT TcpServer : Noncopyable
    {
    public:
        typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;
        typedef Function<void(EventLoop*)> ThreadInitCallback;
        TcpServer(EventLoop* loop,
            const InternetAddress& listenAddr,
            const std::string& nameArg);

        ~TcpServer();  

        const std::string& hostport() const { return hostport_; }
        const std::string& name() const { return name_; }
        EventLoop* GetLoop() const { return loop_; }


        void setThreadNum(int numThreads);
        void setThreadInitCallback(const ThreadInitCallback& cb) { threadInitCallback_ = cb; }
        EventLoopThreadGroup* threadPool() { return threadGroup_; }

        void start();

        void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }

        void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }

        void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }

    private:
        void newConnection(int sockfd, const InternetAddress& peerAddr);

        void RemoveConnection(const TcpConnectionPtr& conn);

        void RemoveConnectionInLoop(const TcpConnectionPtr& conn);

        

    private:

        EventLoop* loop_;  
        const std::string hostport_;
        const std::string name_;
        Scoped_ptr<Acceptor> acceptor_; 
        EventLoopThreadGroup* threadGroup_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        ThreadInitCallback threadInitCallback_;
        bool started_;
        // always in loop thread
        int nextConnId_;
        ConnectionMap connections_;
    };
}
#endif