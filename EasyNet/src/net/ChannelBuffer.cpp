#include "EasyNet/include/net/ChannelBuffer.h"
#include "EasyNet/include/net/SocketApiWrapper.h"
//#include "Common/CommonUtility/csw_control_logger.h"
#include "EasyNet/include/net/Endian.h"
#include <assert.h>
#include <iostream>
#include <string.h>
#include <algorithm>


namespace Net
{
	const char kEOL = '\n';
    const char kCRLF[] = "\r\n";
	//const char kEOL = '\r\n';
	const int kreadBytesEveryTime = 1024 * 64;
	namespace details
	{
		const void* memrchar(const char* sourceBuf, int findChar, uint64_t iFindNum)
		{
#ifdef ON_LINUX
			return ::memrchr(sourceBuf, findChar, iFindNum);
#else 
			while (iFindNum >= 0 && *(sourceBuf + iFindNum - 1) != findChar)
			{
				if (iFindNum-- == 0)
				{
					return NULL;
				}
			}
			return sourceBuf + iFindNum;
#endif
		}
	}

	void ChannelBuffer::AssureEnoughSpace(uint64_t len)
	{
		if (len > (WriteableBytes() + PrependbaleBytes() - kBufferCheapPrepend))
		{
            std::cout << " buffer will resize:" << writeIndex_ + len << std::endl;
			buffer_.resize(writeIndex_ + len);
		}
		else
		{
			//do not need to allocate new memory spaces, just move the data forward
			uint64_t readableBytes = ReadableBytes();
			::memcpy(Begin() + kBufferCheapPrepend, GetReadPointer(), readableBytes);
			readIndex_ = kBufferCheapPrepend;
			writeIndex_ = readIndex_ + readableBytes;
		}
	}

	void ChannelBuffer::Pop(uint64_t len)
	{
        if (len > ReadableBytes())
        {
            //CLOG_INFO( "len" << len << "readble bytes:" << ReadableBytes());
        }
		assert(len <= ReadableBytes());
		if (len < ReadableBytes())
		{
			readIndex_ += len;
		}
		else
		{
			writeIndex_ = readIndex_ = kBufferCheapPrepend;
		}
	}

	std::string ChannelBuffer::ReadBytesAsString(uint64_t len)
	{
		assert(len <= ReadableBytes());
        //TODO, this function is low efficiency, too many temp strings//
		std::string result(GetReadPointer(), len);
		Pop(len);
		return result;
	}

	const char* ChannelBuffer::FindLastEndOfLine()
	{
		return static_cast<const char*>
			(details::memrchar(GetReadPointer(), kEOL, ReadableBytes()));
	}

	void ChannelBuffer::PrependHeader(const void* data, int dataLen)
	{
		assert(readIndex_ >= dataLen);
		const char* d = static_cast<const char*>(data);
		readIndex_ -= dataLen;
		//std::copy(d, d + dataLen, Begin() + readIndex_);
        memcpy(Begin()+readIndex_, d, sizeof(dataLen));

	}

	int64_t ChannelBuffer::ReadFromSocketFD(uint32_t sockFd, int& error)
	{
		static char buffer[kreadBytesEveryTime] = { '\0' };
        memset(buffer, 0, sizeof(buffer));
		int64_t readBytes = SocketsApi::Receive(sockFd, buffer, kreadBytesEveryTime);
		if (readBytes > 0)
		{
			Append(buffer, readBytes);
		}
		else if (readBytes == 0)
		{
			std::cout << "readBytes is 0...." << std::endl;
		}
		else
		{
            //do not close the connection//
			error = errno;
			

		}
		return readBytes;
	}



	int64_t ChannelBuffer::readFd(int fd, int * saveErrno)
	{
        saveErrno;
		char extrabuf[65536];
		IOV_TYPE vec[2];
		const uint32_t writable =  static_cast<uint32_t>(WriteableBytes());

		vec[0].IOV_PTR_FIELD = Begin() + writeIndex_;
		vec[0].IOV_LEN_FIELD = writable;
		vec[1].IOV_PTR_FIELD = extrabuf;
		vec[1].IOV_LEN_FIELD = sizeof(extrabuf);
		const int iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;
		const ssize_t n = SocketsApi::readv(fd, vec, iovcnt);
		if (n < 0)
		{
			//*saveErrno = GetLastError();
            return 0;
		}
		else if(size_t(n) <= writable)
		{
			writeIndex_ += n;
		}
		else
		{
			writeIndex_ = buffer_.size();
			Append(extrabuf, n - writable);
		}
		return  n;
	}



    //skip some contents which the length is len//
    void ChannelBuffer::Retrieve(size_t len)
    {
        assert(len <= ReadableBytes());
        if (len < ReadableBytes())
        {
            readIndex_ += len;
        }
        else
        {
            Reset();
        }
    }

	const char* ChannelBuffer::FindFirstEndOfLine()
	{
		return static_cast<const char*>
			(memchr(GetReadPointer(), kEOL, ReadableBytes()));
	}

    void ChannelBuffer::AppendInt32(int32_t x)
    {
        int32_t be32 = Endian::HostToNetwork32(x);
        Append(&be32, sizeof be32);
    }

    int32_t ChannelBuffer::ReadAsInt32(const char* buf)
    {
        int32_t be32 = 0;
        ::memcpy(&be32, buf, sizeof(be32));
        return Endian::NetworkToHost32(be32);
    }
	
}
