#ifndef LINE_BASED_FRAME_CODEC
#define LINE_BASED_FRAME_CODEC

#include "EasyNet/include/net/TcpConnection.h"


namespace Net
{
	class  LineBasedFrameCodec
	{
	public:
		typedef Function< void(const TcpConnectionPtr& conn, const StringPiece& msg) > CodecMessageCallback;
		LineBasedFrameCodec(CodecMessageCallback cb);

		void Encode(TcpConnectionPtr& conn,  std::string& msg);

		void Decode(const TcpConnectionPtr& conn, ChannelBuffer* buf);



	private:
		CodecMessageCallback codecMessageCb_;
	};
}

#endif