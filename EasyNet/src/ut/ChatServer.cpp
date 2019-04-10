#include "EasyNet/include/net/LengthFieldFrameCodec.h"
#include "EasyNet/include/net/TcpServer.h"
#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/TcpConnection.h"
#include "EasyNet/include/base/Log.h"
#include <set>
#include <iostream>


using namespace Net;

class ChatServer
{

public:
	ChatServer(EventLoop* loop, const InternetAddress& addr)
		: server_(loop, addr, "ChatServer")
		, codec_(Bind(&ChatServer::onMessage, this, Placeholders _1, Placeholders _2))
	{
		server_.setMessageCallback(Bind(&LengthFieldFrameCodec::Decode, codec_, Placeholders _1, Placeholders _2));
        server_.setConnectionCallback(Bind(&ChatServer::onConnection, this, Placeholders _1));
		server_.start();

	}                                     

	void onConnection(const TcpConnectionPtr& conn)
	{
		if (conn->IsConnected())
		{
			connections_.insert(conn);
		}
		else
		{
			auto it = connections_.find(conn);
			assert (it != connections_.end());
			connections_.erase(it);

		}
	}


	void onMessage(const TcpConnectionPtr& conn, const std::string& msg)
	{
        conn;
		static StringPiece sendMsg;
		sendMsg = msg;
		for (auto it=connections_.begin(); it != connections_.end(); it++)
		{
			TcpConnectionPtr conn = *it;
			
			codec_.Encode(conn, sendMsg);
		}
	}


private:
	TcpServer server_;
	LengthFieldFrameCodec codec_;
	std::set<TcpConnectionPtr> connections_;
};

using namespace Logger;
Shared_ptr<SyncLogging> syncLogging(new SyncLogging);
void output_func(const char* msg, uint64_t len)
{
	syncLogging->Append(msg, len);
}

int main()
{
	Log::SetOutputFuncCallback(output_func);
	InternetAddress listenAddr(2000);
	EventLoop loop;

	ChatServer server(&loop, listenAddr);
	loop.Loop();
	return 0;

}