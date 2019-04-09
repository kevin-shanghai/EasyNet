#ifndef LENGTH_FIELD_FRAME_CODEC_H__
#define LENGTH_FIELD_FRAME_CODEC_H__
#include "EasyNet/include/net/ICallBackDefine.h"
#include "EasyNet/include/base/StringPiece.h"

#ifdef EXPORT_NETBASElIGHT
#define CSW_EXPORT_OWNER
#endif


namespace Net
{
    //This class for support the protocol which have a fixed length header, the header indicate the message size//
    class EXPORT_NETBASElIGHT LengthFieldFrameCodec
    {
    public:
        typedef Function<void (const TcpConnectionPtr& conn, const std::string& msg)> CodecMessageCallback;
        LengthFieldFrameCodec(CodecMessageCallback cb)  : messageCallback_(cb)
        {

        }

        void Decode(const TcpConnectionPtr& conn, ChannelBuffer* buf);

        void Encode(const TcpConnectionPtr& conn, StringPiece& message);


    private:
        CodecMessageCallback messageCallback_;
        static const int32_t kHeaderLen = sizeof(int32_t);
    };
}



#endif