#include "EasyNet/include/net/TcpClient.h"
#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/InternetAddress.h"
#include "EasyNet/include/net/TcpConnection.h"
#include <stdio.h>
#include <string>

using namespace Net;

class EchoClient 
{
public:
	EchoClient(EventLoop* loop, const InternetAddress& listenAddr, const std::string& id)
		: loop_(loop),
		client_(loop, listenAddr, "EchoClient"+id)
	{
		client_.setConnectionCallback(
			Bind(&EchoClient::onConnection, this, Placeholders _1));
		client_.setMessageCallback(
			Bind(&EchoClient::onMessage, this, Placeholders _1, Placeholders _2));
	}

	void connect()
	{
		client_.connect();
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
		std::string msg(buf->ReadAllAsString());
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
	TcpClient client_;
};

int main_EchoClient(int argc, char* argv[])
{
	Net::EventLoop loop;
	InternetAddress serverAddr("10.8.61.53", 2000);

	int n = 1;
	if (argc > 2)
	{
		n = atoi(argv[2]);
	}

	std::vector<EchoClient*> clients;
	for (int i = 0; i < n; ++i)
	{
		char buf[32];
		snprintf(buf, sizeof buf, "%d", i+1);
		clients.push_back(new EchoClient(&loop, serverAddr, buf));
	}

	clients[0]->connect();
	loop.Loop();
	return 0;
}

