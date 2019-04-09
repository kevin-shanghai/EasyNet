#include "EasyNet/include/net/ProtobufFrameCodec.h"
#include "EasyNet/include/net/Endian.h"
#include "EasyNet/include/net/TcpConnection.h"
#include "google/protobuf/descriptor.h"


namespace Net
{
	const int32_t kHeaderLength = 4;

    void ProtobufFrameCodec::defaultErrorCallback(const TcpConnectionPtr& conn,
        ChannelBuffer* buf,
        ParseErrorCode errorCode)
    {
        buf;
        std::cout << "ProtobufCodec::defaultErrorCallback - " << (errorCode);
        if (conn && conn->IsConnected())
        {
            conn->Shutdown();
        }
    }


	ProtobufFrameCodec::ProtobufFrameCodec(const ProtobufMessageCallback& messageCb)
		: protobufMessageCb_(messageCb)
		
	{
          protobufErrorCb_ = defaultErrorCallback;
    }

	ProtobufFrameCodec::~ProtobufFrameCodec()
	{

	}

	void ProtobufFrameCodec::Encode(const TcpConnectionPtr& conn, const google::protobuf::Message& message)
	{
        ChannelBuffer buf;
        EncodeToBuffer(message, &buf);
        if (conn)
        {
            conn->Send(buf);
        }
        
	}

	void ProtobufFrameCodec::Decode(const TcpConnectionPtr& conn, ChannelBuffer* buffer)
	{
        while (buffer->ReadableBytes() >= kHeaderLen*2)
        {
            const int32_t len = buffer->ReadAsInt32(buffer->GetReadPointer());
            if (len > kMaxMessageLen || len < kHeaderLen * 2)
            {
                protobufErrorCb_(conn, buffer, kInvalidLength);
                break;
            }
            else if (buffer->ReadableBytes() >= (len + kHeaderLen))
            {
                ParseErrorCode errorCode = kNoErrorHappend;
                ProtobufMessagePtr message = ParseFromBuffer(buffer, &errorCode, false);
                if (errorCode == kNoErrorHappend && message)
                {
                    protobufMessageCb_(conn, message);
                    // clear the buffer//
                    buffer->Pop(len);
                }
                else
                {
                    protobufErrorCb_(conn, buffer, errorCode);
                    break;
                }
            }
            else
            {
                break;
            }
        }
	}

	google::protobuf::Message* ProtobufFrameCodec::CreateProtobufMessage(const std::string& messageTypeName)
	{
		::google::protobuf::Message* message = nullptr;
		const google::protobuf::Descriptor* descriptor = 
			google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(messageTypeName);
		if (descriptor)
		{
			auto protoType = google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
			if (protoType)
			{
				message = protoType->New();
			}
		}
		return message;
	}



    void ProtobufFrameCodec::EncodeToBuffer(const google::protobuf::Message& message, ChannelBuffer* buf)
    {
        assert(buf->ReadableBytes() == 0);

        const std::string& typeName = message.GetTypeName();
        int32_t nameLen = static_cast<int32_t>(typeName.size()+1);
        buf->AppendInt32(nameLen);
        buf->Append(typeName.c_str(), nameLen);

        int byte_size = message.ByteSize();
        buf->EnsureWritableBytes(byte_size);

        uint8_t* start = reinterpret_cast<uint8_t*>(buf->GetWriteablePointer());
        uint8_t* end = message.SerializeWithCachedSizesToArray(start);
        if (end - start != byte_size)
        {
            std::cout << "serialize error with cache..." << std::endl;
        }
        buf->HasWritten(byte_size);


        assert(buf->ReadableBytes() == (sizeof (nameLen)) + nameLen + byte_size );

        uint32_t allLen = buf->ReadableBytes();
        int32_t len = Endian::HostToNetwork32((allLen));
        buf->PrependHeader(&len, sizeof(len));
    }


    void ProtobufFrameCodec::EncodeToCachedArray(const google::protobuf::Message& message, std::string& buf)
    {
        ChannelBuffer buffer;
        EncodeToBuffer(message, &buffer);

        buf.assign(buffer.GetReadPointer(), buffer.ReadableBytes());
    }



    int32_t asInt32(const char* buf)
    {
        int32_t be32 = 0;
        ::memcpy(&be32, buf, sizeof(be32));
        return Endian::NetworkToHost32(be32);
    }


    ProtobufMessagePtr ProtobufFrameCodec::ParseFromBuffer(Net::ChannelBuffer* buf, ParseErrorCode* error, bool bCswProtobufMsg )
    {
        ProtobufMessagePtr message;

        int32_t allLen = buf->ReadAsInt32(buf->GetReadPointer());
        int32_t nameLen = buf->ReadAsInt32(buf->GetReadPointer() + kHeaderLength);
        if (nameLen >= 2 && nameLen <= allLen - kHeaderLength)
        {
            buf->PopInt32();
            std::string typeName(buf->GetReadPointer() + kHeaderLength, buf->GetReadPointer() + kHeaderLength + nameLen - 1);
            // create message object
           message.reset(CreateProtobufMessage(typeName));
           
            if (message)
            {
                // parse from buffer
                const char* data = buf->GetReadPointer() + kHeaderLen + nameLen;
                int32_t dataLen = allLen - nameLen - kHeaderLen;
                if (message->ParseFromArray(data, dataLen))
                {
                    *error = kNoErrorHappend;
                }
                else
                {
                    *error = kParseError;
                }
            }
            else
            {
                *error = kUnknownMessageType;
            }
        }
        else
        {
            *error = kInvalidNameLen;
        }
        return message;
    }

    ProtobufMessagePtr ProtobufFrameCodec::ParseFromCachedArray(const std::string& buf)
    {
        ChannelBuffer buffer;
        buffer.Append(buf.data(), buf.length());
        ParseErrorCode errorCode;
        return ParseFromBuffer(&buffer, &errorCode);
    }



}