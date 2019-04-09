#include "EasyNet/include/net/HttpServer.h"
#include "EasyNet/include/net/HttpParser.h"
#include "EasyNet/include/net/HttpRequest.h"
#include "EasyNet/include/net/HttpResponse.h"
#include "EasyNet/include/net/ChannelBuffer.h"
#include "EasyNet/include/net/HttpServer.h"
#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/base/TimeStamp.h"


using namespace Net;

void testHttpRequest()
{
    HttpParser context;
    ChannelBuffer input;
    std::string test = "test1""test2""test3";

    input.Append("GET /index.html HTTP/1.1\r\n"
        "Host: www.chenshuo.com\r\n"
        "User-Agent:\r\n"
        "Accept-Encoding: \r\n"
        "\r\n");
    HttpParser::parseRequest(&input, &context);
}



bool benchmark = false;

void onRequest(const HttpRequest& req, HttpResponse* resp)
{
    std::cout << "Headers " << req.methodString() << " " << req.path() << std::endl;
    if (!benchmark)
    {
        const std::map<std::string, std::string>& headers = req.headers();
        for (auto it = headers.begin(); it != headers.end(); ++it)
        {
            std::cout << it->first << ": " << it->second << std::endl;
        }
    }

    if (req.path() == "/")
    {
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("text/html");
        resp->addHeader("Server", "CswHttpServer");
        std::string now = Timestamp::Now().ToCalenderTime();
        resp->setBody("<html><head><title>This is title</title></head>"
            "<body><h1>Hello</h1>Now is " + now +
            "</body></html>");
    }
    else if (req.path() == "/hello")
    {
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("text/plain");
        resp->addHeader("Server", "csw");
        resp->setBody("hello, world!");
    }
    else
    {
        resp->setStatusCode(HttpResponse::k404NotFound);
        resp->setStatusMessage("Not Found");
        resp->setCloseConnection(true);
    }
}


int main_HttpServer()
{
    EventLoop loop;
    HttpServer server(&loop, InternetAddress(8080), "dummy");
    server.setHttpCallback(onRequest);
    server.start();
    loop.Loop();

    testHttpRequest();
    return 0;
}