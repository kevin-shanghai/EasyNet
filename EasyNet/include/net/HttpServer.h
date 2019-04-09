#ifndef HTTP_SERVER_H__
#define HTTP_SERVER_H__
#include "EasyNet/include/net/TcpServer.h"
#include "EasyNet/include/net/HttpRequest.h"



namespace Net
{
    class TcpServer;
    class HttpRequest;
    class HttpResponse;
    class HttpParser;
    // This class used for describe a simple http server , we can use this to display 
    // the process info or monitor the running status of the workstation with web browser
    class HttpServer 
    {
    public:
        typedef Function<void (const HttpRequest&, HttpResponse*)> HttpCallback;

        HttpServer(EventLoop* loop,
            const InternetAddress& listenAddr,
            const std::string& name="HttpServer");

        ~HttpServer();  

        EventLoop* getLoop() const { return server_.GetLoop(); }

        void setHttpCallback(const HttpCallback& cb)
        {
            httpCallback_ = cb;
        }

        void setThreadNum(int numThreads)
        {
            server_.setThreadNum(numThreads);
        }

        void start();

    private:
        void onConnection(const TcpConnectionPtr& conn);
        void onMessage(const TcpConnectionPtr& conn,
            ChannelBuffer* buf);
        void onRequest(const TcpConnectionPtr& conn, HttpRequest& req);

        TcpServer server_;
        HttpCallback httpCallback_;
    };
}

#endif