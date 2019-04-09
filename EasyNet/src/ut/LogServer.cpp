#include "EasyNet/include/net/LineBasedFrameCodec.h"
#include "EasyNet/include/net/TcpServer.h"
#include "EasyNet/include/net/EventLoop.h"
#include <iostream>


using namespace Net;

class LogServer
{

public:
	LogServer(EventLoop* loop, const InternetAddress& addr)
		: server_(loop, addr, "LogServer")
		, codec_(Bind(&LogServer::onMessage, this, Placeholders _1, Placeholders _2))
	{
		server_.setMessageCallback(Bind(&LineBasedFrameCodec::Decode, codec_, Placeholders _1, Placeholders _2));
		server_.start();

	}


	void onMessage(const TcpConnectionPtr& conn, StringPiece msg)
	{
		//when receive a single line log, do io operation//
        conn;
		std::cout << "log line:" << msg.as_string() << std::endl;
	}


private:
	TcpServer server_;
	LineBasedFrameCodec codec_;
};

int main_LogServer()
{
	InternetAddress listenAddr(2000);
	EventLoop loop;

	LogServer server(&loop, listenAddr);
	loop.Loop();
	return 0;

}