#include "EasyNet/include/net/ProtobufFrameCodec.h"
#include "EasyNet/include/net/NetbaseLightTest.pb.h"
#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/TcpClient.h"
#include "EasyNet/include/net/EventLoopThread.h"
#include "EasyNet/include/net/TcpConnection.h"


using namespace Net;
using namespace base;

class ProtobufMessageClient
{
public:
    ProtobufMessageClient(EventLoop* loop, InternetAddress& addr)
        : codec_(Bind(&ProtobufMessageClient::onMessage, this, Placeholders _1, Placeholders _2))
        , client_(loop, addr, "ProtobufMessageClient")
    {
        client_.setMessageCallback(Bind(&ProtobufFrameCodec::Decode, &codec_, Placeholders _1, Placeholders _2));
        client_.setConnectionCallback(Bind(&ProtobufMessageClient::onConnection, this, Placeholders _1));

    }

    void onConnection(const TcpConnectionPtr& conn)
    {
        std::cout 
            << conn->GetLocalAddress().GetIpAndPort() << " -> "
            << conn->GetPeerAddress().GetIpAndPort() << " is "
            << (conn->IsConnected() ? "UP" : "DOWN") << std::endl;


        if (conn->IsConnected())
        {
            connection_ = conn;
        }
        else
        {
            connection_.reset();
        }
    }

    void onMessage(const TcpConnectionPtr& conn, const ProtobufMessagePtr& msg)
    {                                    
        conn;
        std::cout << "type=" << msg->GetTypeName() << std::endl;;
    }

    void Send(const google::protobuf::Message& msg)
    {
        codec_.Encode(connection_, msg);
    }

    void connect()
    {
        client_.connect();
    }



private:
    ProtobufFrameCodec codec_;
    TcpClient client_;
    TcpConnectionPtr connection_;
};



int main_ptotobufClient()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;


    InternetAddress listenAddr("10.8.61.53", 2000);
    EventLoopThread loopThread;
    EventLoop* loop = loopThread.StartLoopThread();

    ProtobufMessageClient client(loop, listenAddr);
    client.connect();

    NetbaseLight::Query query;
    query.set_id(1);
    query.set_questioner("unknown");
    query.add_question("who am i?");

    std::string line;
    while (std::getline(std::cin, line))
    {
        StringPiece msg(line);
        client.Send(query);
    }


    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}