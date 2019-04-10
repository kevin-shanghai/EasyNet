#include "EasyNet/include/net/HttpServer.h"
#include "EasyNet/include/net/HttpRequest.h"
#include "EasyNet/include/net/HttpResponse.h"
#include "EasyNet/include/net/HttpParser.h"
#include "EasyNet/include/net/TcpConnection.h"
#include "EasyNet/include/base/Log.h"

using namespace Net;


void defaultHttpCallback(const HttpRequest&, HttpResponse* resp)
{
    resp->setStatusCode(HttpResponse::k404NotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
}


HttpServer::HttpServer(EventLoop* loop,
    const InternetAddress& listenAddr,
    const std::string& name)
    : server_(loop, listenAddr, name)
    , httpCallback_(defaultHttpCallback)
{
    server_.setConnectionCallback(
        Bind(&HttpServer::onConnection, this, Placeholders _1));
    server_.setMessageCallback(
        Bind(&HttpServer::onMessage, this, Placeholders _1, Placeholders _2));
}

HttpServer::~HttpServer()
{
}

void HttpServer::start()
{
    LOG_TRACE << "HttpServer[" << server_.name()
        << "] starts listenning on " << server_.hostport();
    server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
    std::cout << conn->GetPeerAddress().GetIpAndPort() << " -> "
        << conn->GetLocalAddress().GetIpAndPort() << " is "
        << (conn->IsConnected() ? "UP" : "DOWN") << std::endl;

    if (conn->IsConnected())
    {
        Shared_ptr<HttpParser> httpParser(new HttpParser());
        conn->SetHttpParser(httpParser);
    }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, ChannelBuffer* buf)
{
    Shared_ptr<HttpParser> httpParserPtr = conn->GetHttpParser();
    assert(httpParserPtr);
    if (!HttpParser::parseRequest(buf, httpParserPtr.get()))
    {
        conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->Shutdown();
    }

    
    if (httpParserPtr->gotAllHeaders())
    {
        this->onRequest(conn, httpParserPtr->request());
        httpParserPtr->reset();
    }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, HttpRequest& req)
{
    const std::string& connection = req.getHeader("Connection");
    bool close = connection == "close" ||
        (req.GetVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
    HttpResponse responseHttp(close);
    httpCallback_(req, &responseHttp);
    ChannelBuffer buf;
    responseHttp.appendToBuffer(&buf);
    conn->Send(buf);
    if (responseHttp.closeConnection())
    {
        conn->Shutdown();
    }
}

