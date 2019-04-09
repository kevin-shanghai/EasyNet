#ifndef TCPCONNECTION_H__
#define TCPCONNECTION_H__

#include <string>
#include "EasyNet/include/base/Noncopyable.h"
#include "EasyNet/include/base/MutexLock.h"
#include "EasyNet/include/base/StringPiece.h"

#include "EasyNet/include/net/ICallBackDefine.h"
#include "EasyNet/include/net/InternetAddress.h"
#include "EasyNet/include/net/Socket.h"
#include "EasyNet/include/net/Channel.h"
#include "EasyNet/include/net/ChannelBuffer.h"
#include "EasyNet/include/net/Codec.h"


#ifdef EXPORT_NETBASElIGHT
#define CSW_EXPORT_OWNER
#endif


namespace Net
{
    class EventLoop;
    class HttpParser;
    class EXPORT_NETBASElIGHT TcpConnection  : private Noncopyable
        , public enable_shared_from_this<TcpConnection>
    {
    public:
        TcpConnection(EventLoop* loop,
            const std::string& name,
            int sockfd,
            const InternetAddress& localAddr,
            const InternetAddress& peerAddr);

        ~TcpConnection();

        EventLoop* GetConnectionEventLoop() const { return  loop_; }
        std::string GetConnectionRawName() const { return rawName_; }
        void SetConnectionName(std::string& strName) { name_ = strName; }
        std::string GetConnectionName() const {return name_; }

        bool IsConnected() const { return connectionState_ == kConnected; }

        void ConnectionEstablished();
        void ConnectionDestroyed();

        const InternetAddress& GetLocalAddress() const { return localAddress_; }
        const InternetAddress& GetPeerAddress() const { return peerAddress_; }

        void Send(const StringPiece& message);
        void Send(ChannelBuffer& message);  
        void ShutDownWriteForHalfClose();

        void SetTcpConnectionCallBack(ConnectionCallback cb) { connectionCallback_ = cb;}
        void SetMessageCallBack(MessageCallback cb) { messageCallback_ = cb;}
        void SetCloseCallback(const CloseCallback& cb) { closeCallback_ = cb; }

        void SetHttpParser(const Shared_ptr<HttpParser>& httpParser) { httpParser_ = httpParser;}
        Shared_ptr<HttpParser> GetHttpParser() const { return httpParser_; }


        void Shutdown();

    private:
        enum ConnectionState { kDisconnected, kConnecting, kConnected, kDisconnecting };
        void SetState(ConnectionState s) { connectionState_ = s; }
        const char* StateToString() const;

        void SendStringPieceInLoop(const StringPiece& message) ;
        void SendDataInLoop(const void* message, size_t len);
        void ShutdownInLoop();

        void HandleRead();
        void HandleWrite();
        void HandleClose();
        void HandleError();


    private:
        TcpConnection(const TcpConnection&);
        TcpConnection& operator= (const TcpConnection&);

    private:

        const InternetAddress& localAddress_;
        const InternetAddress& peerAddress_;
        Socket* socket_;
        EventLoop* loop_;
        Shared_ptr<HttpParser> httpParser_;
        ConnectionState connectionState_;
        const std::string rawName_;
        std::string name_;
        Scoped_ptr<Channel> connectionChannelPtr_;
        ChannelBuffer inputBuf_;
        ChannelBuffer outputBuf_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        HighWaterMarkCallback highWaterMarkCallback_;
        CloseCallback closeCallback_;
        base::MutexLock* mutex_;
    };
}


#endif
