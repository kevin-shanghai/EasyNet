//#include "Common/NetBaseLight/NetBaseLight/include/net/FixedLengthFrameCodec.h"
//#include "Common/NetBaseLight/NetBaseLight/include/net/ICallBackDefine.h"
//#include "Common/NetBaseLight/NetBaseLight/include/net/ChannelBuffer.h"
//
//
//namespace Net
//{
//	FixedLengthFrameCodec::FixedLengthFrameCodec(const CodecMessageCallback& cb, int32_t iFrameDecodeLength)
//		: Codec(cb)
//		, frameLength_(iFrameDecodeLength)
//	{
//
//	}
//
//	FixedLengthFrameCodec::~FixedLengthFrameCodec()
//	{
//
//	}
//
//	void FixedLengthFrameCodec::Encode(const TcpConnectionPtr& conn, const StringPiece& msg)
//	{
//		 //fixlength do not need the encoder
//	}
//
//	void FixedLengthFrameCodec::Decode(const TcpConnectionPtr& conn, ChannelBuffer* buffer)
//	{
//		if (buffer->ReadableBytes() >= frameLength_)
//		{
//			codecMsgCb_(conn, std::move(buffer->ReadBytesAsString(frameLength_)));
//		}
//	}
//
//}