#include "EasyNet/include/net/LengthFieldFrameCodec.h"
#include "EasyNet/include/net/TcpClient.h"
#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/InternetAddress.h"
#include "EasyNet/include/net/EventLoopThread.h"
#include "EasyNet/include/net/ICallBackDefine.h"
#include "EasyNet/include/net/TcpConnection.h"
#include "EasyNet/include/base/StringPiece.h"
#include "EasyNet/include/base/MutexLock.h"
#include <iostream>
#include <stdio.h>


using namespace Net;
using namespace base;
static int kClientIndex = 0;
class ChatClient 
{
public:
	ChatClient(EventLoop* loop, const InternetAddress& serverAddr)
		: client_(loop, serverAddr, "ChatClient")
		, codec_(Bind(&ChatClient::onStringMessage, this, Placeholders _1, Placeholders _2))
		, mutex_()
		, index_(kClientIndex++)
	{
		client_.setConnectionCallback(Bind(&ChatClient::onConnection, this, Placeholders _1));

		client_.setMessageCallback(
			Bind(&LengthFieldFrameCodec::Decode, &codec_, Placeholders _1, Placeholders _2));

	}

	void connect()
	{
		client_.connect();
	}


	void send(StringPiece& message)
	{
		LockGuard lock(&mutex_);
		if (connection_)
		{
			codec_.Encode(connection_, message);
		}
	}

private:

	void onStringMessage(const TcpConnectionPtr&, const StringPiece& message)
	{
		std::cout << "ChatClient_" << index_ << " <<" << message.as_string() << std::endl;
	}

	void onConnection(const TcpConnectionPtr& conn)
	{
		std::cout << "ChatClient_" << index_ << " #"
			<< conn->GetLocalAddress().GetIpAndPort() << " -> "
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
	LengthFieldFrameCodec codec_;
	MutexLock mutex_;
	TcpConnectionPtr connection_;
	size_t index_;
};

int main()
{
	EventLoopThread loopThread;
	uint16_t port = 2000;
	InternetAddress serverAddr("10.8.61.62", port);
	EventLoop* loop = loopThread.StartLoopThread();


	std::vector<ChatClient*> clients;
	static size_t kSelectIOSupportMaxFdNum = 5;
	for (size_t i=0; i<kSelectIOSupportMaxFdNum; i++)
	{
		ChatClient* curClient = new ChatClient(loop, serverAddr);
		clients.push_back (curClient);
		curClient->connect();

	}

	std::string line;
	while (std::getline(std::cin, line))
	{
        StringPiece msg(line);
		clients[0]->send(msg);
	}
	
	return 0;
}

