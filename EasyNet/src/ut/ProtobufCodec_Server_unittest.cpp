#include "EasyNet/include/net/ProtobufFrameCodec.h"
#include "EasyNet/include/net/NetbaseLightTest.pb.h"
#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/TcpServer.h"
#include "EasyNet/include/net/ProtobufDispatcher.h"
#include "EasyNet/include/net/TcpConnection.h"
#include "google/protobuf/descriptor.h"


using namespace Net;

class ProtobufMessageServer
{
public:
    typedef Shared_ptr<NetbaseLight::Query> QueryPtr;
    ProtobufMessageServer(EventLoop* loop, InternetAddress& addr)
        : codec_(Bind(&ProtobufMessageServer::onMessage, this, Placeholders _1, Placeholders _2))
        , server_(loop, addr, "ProtobufMessageServer")
    {
        server_.setMessageCallback(Bind(&ProtobufFrameCodec::Decode, &codec_, Placeholders _1, Placeholders _2));
        server_.setConnectionCallback(Bind(&ProtobufMessageServer::onConnection, this, Placeholders _1));
        dispatcher_.RegisterCallback<NetbaseLight::Query>( Bind(&ProtobufMessageServer::onQuery, this, Placeholders _1));
        server_.start();
    }

    void onConnection(const TcpConnectionPtr& conn)
    {
        assert(conn);
        std::cout << conn->GetPeerAddress().GetIpAndPort() << " -> "
            << conn->GetLocalAddress().GetIpAndPort() << " is "
            << (conn->IsConnected() ? "UP" : "DOWN") << std::endl;
        conn_ = conn;
    }

    void onMessage(const TcpConnectionPtr& conn, const ProtobufMessagePtr& msg)
    {
        conn;
        dispatcher_.onProtobufMessage(msg);
    }


    void onQuery(const QueryPtr& msg)
    {
        std::cout << "type=" << msg->GetTypeName() 
                << " id=" << msg->id() 
                << " questioner=" << msg->questioner()
                << " quenstion=" << msg->question(0)
                << std::endl;;
    }

    void Send(const google::protobuf::Message& msg)
    {
        codec_.Encode(conn_, msg);
    }



private:
    ProtobufFrameCodec codec_;
    TcpServer server_;
    TcpConnectionPtr conn_;
    ProtobufMessageDispatcher dispatcher_;

};



int main_protobuf_test_server()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    InternetAddress listenAddr(2000);
    EventLoop loop;

    ProtobufMessageServer server(&loop, listenAddr);
    loop.Loop();
   
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}