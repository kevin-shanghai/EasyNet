#ifndef PROTOBUF_FRAME_CODEC_H__
#define	PROTOBUF_FRAME_CODEC_H__


#include <string>
#include "EasyNet/include/net/ICallBackDefine.h"
#include "google/protobuf/message.h"


#ifdef EXPORT_NETBASElIGHT
#define CSW_EXPORT_OWNER
#endif


namespace Net
{
	class ChannelBuffer;
	enum ParseErrorCode
	{
		kNoErrorHappend = 0,
		kInvalidLength,
		kCheckSumError,
		kInvalidNameLen,
		kUnknownMessageType,
		kParseError,
	};

	typedef Function<void(const TcpConnectionPtr&, const ProtobufMessagePtr&)> ProtobufMessageCallback;

	typedef Function<void(const TcpConnectionPtr&, ChannelBuffer*, ParseErrorCode)> ProtobufErrorCallback;

	class EXPORT_NETBASElIGHT ProtobufFrameCodec 
	{
	public:
		ProtobufFrameCodec(const ProtobufMessageCallback& messageCb);
        ProtobufFrameCodec(){}
		~ProtobufFrameCodec();


		void Encode(const TcpConnectionPtr& conn, const google::protobuf::Message& message);
		void Decode(const TcpConnectionPtr& conn, ChannelBuffer* buffer);


        static void EncodeToBuffer(const google::protobuf::Message& message, ChannelBuffer* buf);
        static ProtobufMessagePtr ParseFromBuffer(Net::ChannelBuffer* buf, ParseErrorCode* error, bool bCswProtobufMsg = true);


        static void EncodeToCachedArray(const google::protobuf::Message& message, std::string& buf);
        static ProtobufMessagePtr ParseFromCachedArray(const std::string& buf);

	private:
	    static google::protobuf::Message* CreateProtobufMessage(const std::string& messageTypeName);

        static void defaultErrorCallback(const TcpConnectionPtr&,
            ChannelBuffer*,
            ParseErrorCode);
	private:
		const static int kHeaderLen = sizeof(int32_t);
		const static int kChecksumLen = sizeof(int32_t);
		const static int kMaxMessageLen = 64 * 1024 * 1024;
		ProtobufMessageCallback protobufMessageCb_;
		ProtobufErrorCallback protobufErrorCb_;

	};
}

#endif


