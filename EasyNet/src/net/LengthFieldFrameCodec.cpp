#include "EasyNet/include/net/LengthFieldFrameCodec.h"
#include "EasyNet/include/net/ICallBackDefine.h"
#include "EasyNet/include/net/ChannelBuffer.h"
#include "EasyNet/include/net/TcpConnection.h"
#include "EasyNet/include/net/Endian.h"



namespace Net
{
    
    void LengthFieldFrameCodec::Decode(const TcpConnectionPtr& conn, ChannelBuffer* buf)
    {
        while (buf->ReadableBytes() >= kHeaderLen) 
        {
            const void* data = buf->GetReadPointer();
            int32_t be32 = *static_cast<const int32_t*>(data); 
            const int32_t len = Endian::NetworkToHost32(be32);
            if (len > 65536 || len < 0)
            {
                std::cout << "Invalid length " << len;
                //TODO, should support this length//

                conn->Shutdown(); 
                break;
            }
            else if (buf->ReadableBytes() >= len + kHeaderLen)
            {
                buf->Retrieve(kHeaderLen);
                std::string message(buf->GetReadPointer(), len);
                messageCallback_(conn, message);
                buf->Retrieve(len);
            }
            else
            {
                break;
            }
        }
    }

    void LengthFieldFrameCodec::Encode(const TcpConnectionPtr& conn,
        StringPiece& message)
    {
        ChannelBuffer buf;
        buf.Append(message.data(), message.size());
        int32_t len = static_cast<int32_t>(message.size());
        int32_t be32 = Endian::HostToNetwork32(len);
        buf.PrependHeader(&be32, sizeof be32);
        conn->Send(buf);
    }
}