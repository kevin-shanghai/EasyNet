#include "EasyNet/include/net/LineBasedFrameCodec.h"


namespace Net
{
	//static const char kEOL = '\n';

	LineBasedFrameCodec::LineBasedFrameCodec(CodecMessageCallback cb) 
						: codecMessageCb_(cb)
	{

	}


	void LineBasedFrameCodec::Encode(TcpConnectionPtr& conn, std::string& msg)
	{
		msg.append(1, kEOL);
		static StringPiece sendMsg;
		sendMsg.clear();
		sendMsg = msg;
		conn->Send(sendMsg);
	}


	void LineBasedFrameCodec::Decode(const TcpConnectionPtr& conn, ChannelBuffer* buf)
	{
		while(buf->ReadableBytes() > 0)
		{
			const std::string& msg = buf->ReadLine();
			if ( !msg.empty() && codecMessageCb_)
			{
				codecMessageCb_(conn, msg);
			}
			else
			{
				//buffer is empty or buffer content is not enough to consist one line//
				break;
			}

		}

	}


}
