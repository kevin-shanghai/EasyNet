#ifndef CONNECTOR_H__
#define CONNECTOR_H__
#include "EasyNet/include/net/InternetAddress.h"
#include "EasyNet/include/base/Noncopyable.h"
#include "EasyNet/include/base/PlatformDefine.h"
#include <assert.h>


class MutexLock;
namespace Net
{
    class EventLoop;
    class Channel;
    class Connector
    {
    public:
        typedef std::function<void(int sockfd)> NewConnectionCallback;
        Connector(EventLoop* loop, const InternetAddress& serverAddr);
        ~Connector();

        void setNewConnectionCallback(const NewConnectionCallback& cb) { newConnectionCallback_ = cb; }

        void start();
        void restart();
        void stop();

        const InternetAddress& serverAddress() const { return serverAddr_; }
    private:
        enum States {kDisconnected,kConnecting,kConnected};
        static const int kMaxRetryDelayMs = 30 * 1000;
        static const int kInitRetryDelayMs = 500;

        void setState(States s) { state_ = s; }

        void startInLoop();
        void stopInLoop();
        void connect();
        void connecting(int sockfd);

        void handleWrite();
        void handleError(); 

        void retry(int sockfd);

        int removeAndResetChannel();
        void resetChannel();

        EventLoop* loop_;
        InternetAddress serverAddr_;
        bool connect_;
        States state_;
        Shared_ptr<Channel> channel_;
        NewConnectionCallback newConnectionCallback_;
        int retryDelayMs_;
    };
}

#endif