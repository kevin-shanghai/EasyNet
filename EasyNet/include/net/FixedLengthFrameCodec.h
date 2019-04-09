#ifndef FIXED_LENGTH_FRAME_CODEC_H__
#define FIXED_LENGTH_FRAME_CODEC_H__
#include "Common/NetBaseLight/NetBaseLight/include/net/Codec.h"

#ifdef EXPORT_NETBASElIGHT
#define CSW_EXPORT_OWNER
#endif

namespace Net
{
    template <class T>
	class EXPORT_NETBASElIGHT FixedLengthFrameCodec  : public Codec
	{
	public:
		FixedLengthFrameCodec();
		~FixedLengthFrameCodec();

		virtual void Encode(const TcpConnection& conn, const T& msg) ;
		virtual void Decode(const TcpConnection& conn, ChannelBuffer* buffer) ;


	private:
		int32_t frameLength_;

	};
}





#endif