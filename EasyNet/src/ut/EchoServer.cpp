#include "EasyNet/include/net/TcpServer.h"
#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/TcpConnection.h"
#include <iostream>


using namespace Net;
class EchoServer
{
public:
    EchoServer(EventLoop* loop, const InternetAddress& listenAddr)
        : loop_(loop),
        server_(loop, listenAddr, "EchoServer")
    {
        server_.setConnectionCallback(
            Bind(&EchoServer::onConnection, this, Placeholders _1));
        server_.setMessageCallback(
            Bind(&EchoServer::onMessage, this, Placeholders _1, Placeholders _2));
    }

    void start()
    {
        server_.start();
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        std::cout << conn->GetPeerAddress().GetIpAndPort() << " -> "
            << conn->GetLocalAddress().GetIpAndPort() << " is "
            << (conn->IsConnected() ? "UP" : "DOWN");

        const std::string msg = "hello " + conn->GetPeerAddress().GetIpAndPort();
        conn->Send(msg);
    }

    void onMessage(const TcpConnectionPtr& conn, ChannelBuffer* buf)
    {
        string msg(buf->ReadAllAsString());
        std::cout << conn->GetConnectionName() << " recv msg:" << msg << std::endl;
        if (msg == "exit\n")
        {
            conn->Send("bye\n");
            conn->ShutDownWriteForHalfClose();
        }
        if (msg == "quit\n")
        {
           
        }
        conn->Send(msg);
    }

    EventLoop* loop_;
    TcpServer server_;
};

int main_EchoServer()
{
    EventLoop loop;

    EventLoop loop1;

    InternetAddress listenAddr(2000);

    EchoServer server(&loop, listenAddr);

    server.start();

    loop.Loop();

    return 0;
}

