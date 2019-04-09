#include "EasyNet/include/net/LineBasedFrameCodec.h"
#include "EasyNet/include/net/TcpClient.h"
#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/InternetAddress.h"
#include "EasyNet/include/net/EventLoopThread.h"
#include "EasyNet/include/base/MutexLock.h"

#include <iostream>
#include <stdio.h>

using namespace Net;
using namespace base;

class LogClient 
{
public:
	LogClient(EventLoop* loop, const InternetAddress& serverAddr)
		: client_(loop, serverAddr, "ChatClient")
		, codec_(Bind(&LogClient::onStringMessage, this, Placeholders _1, Placeholders _2))
		, mutex_()
	{

		client_.setConnectionCallback(Bind(&LogClient::onConnection, this, Placeholders _1));

		client_.setMessageCallback(
			Bind(&LineBasedFrameCodec::Decode, &codec_, Placeholders _1, Placeholders _2));

	}

	void connect()
	{
		client_.connect();
	}


	void write(const StringPiece& message)
	{
		LockGuard lock(&mutex_);
		if (connection_)
		{
            std::string msg = message.as_string();
			codec_.Encode(connection_, msg);
		}
	}

private:

	void onStringMessage(const TcpConnectionPtr&,
		const StringPiece& message)
	{
		std::cout << "<<< " <<  message.as_string() << std::endl;
	}

	void onConnection(const TcpConnectionPtr& conn)
	{
		std::cout << conn->GetLocalAddress().GetIpAndPort() << " -> "
			<< conn->GetPeerAddress().GetIpAndPort() << " is "
			<< (conn->IsConnected() ? "UP" : "DOWN") << std::endl;

		LockGuard lock(&mutex_);
		if (conn->IsConnected())
		{
			connection_ = conn;
		}
		else
		{
			connection_.reset();
		}
	}

	TcpClient client_;
	LineBasedFrameCodec codec_;
	MutexLock mutex_;
	TcpConnectionPtr connection_;
};

int main_LogClient()
{
	EventLoopThread loopThread;
	uint16_t port = 2000;
	InternetAddress serverAddr("10.8.61.53", port);
	EventLoop* loop = loopThread.StartLoopThread();
	LogClient client(loop, serverAddr);
	client.connect();
	std::string line;
	while (std::getline(std::cin, line))
	{
		client.write(line);
	}
	return 0;
}

