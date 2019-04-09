#ifndef NETBASELIGHT_CHANNEL_BUFFER_H__
#define NETBASELIGHT_CHANNEL_BUFFER_H__

#include <stdint.h>
#include <vector>
#include <assert.h>
#include <string.h>
#include <string>

#include <iostream>
#include <algorithm>


namespace Net
{
    extern const char kEOL;
    extern const char kCRLF[];
    const uint8_t kBufferCheapPrepend = 8;
    const uint64_t kBufferInitialSize = 1024 * 4;
    class ChannelBuffer
    {
    public:
        ChannelBuffer()
            : buffer_(kBufferCheapPrepend + kBufferInitialSize)
            , writeIndex_(kBufferCheapPrepend)
            , readIndex_(kBufferCheapPrepend)
        {

        }

        void Reset()
        {
            writeIndex_ = readIndex_ = kBufferCheapPrepend;
        }

        void Retrieve(size_t len);

        const char* GetReadPointer() { return (const char*)(Begin() + readIndex_); }

        char* GetWriteablePointer() { return Begin() + writeIndex_; }

        uint64_t ReadableBytes(){ return writeIndex_ - readIndex_; }

        uint64_t WriteableBytes() { return Size() - writeIndex_; }

        uint64_t PrependbaleBytes(){ return readIndex_; }

        void PrependHeader(const void* data, int dataLen);

        void Append(const std::string& buf){ Append(buf.c_str(), strlen(buf.c_str())); }

        std::string ReadAllAsString() { return ReadBytesAsString(ReadableBytes()); }

        std::string ReadBytesAsString(uint64_t len);

        const char* FindCRLF() const
        {
            const char* crlf = std::search(begin() + readIndex_, begin() + writeIndex_, kCRLF, kCRLF+2);
            
            return crlf == (begin() + writeIndex_) ? NULL : crlf;
        }

          std::string ReadContentsUntilLastEOF()
        {
            const char* lastEofPositon = FindLastEndOfLine();
            if (lastEofPositon)
            {
                return ReadBytesAsString(FindLastEndOfLine() - GetReadPointer());
            }
            return std::string();
        }

        std::string ReadLine()
        {
            static const std::string& kEmptyStr = "";
            const char* firstEofPositon = FindFirstEndOfLine();
            if (firstEofPositon)
            {
                uint64_t singleLineLen = FindFirstEndOfLine() - GetReadPointer() + 1;
                return ReadBytesAsString(singleLineLen);
            }
            return kEmptyStr;
        }

        int64_t ReadFromSocketFD(uint32_t sockFd, int& error);

        int64_t readFd(int fd, int* saveErrno);

        void AppendInt32(int32_t x);

        template <class T>
        void Append(T* dataPtr, uint64_t len)
        {
            if (len > WriteableBytes())
            {
                AssureEnoughSpace(len);
            }
            if (WriteableBytes() < len)
            {
                std::cout << "error....." << std::endl;
            }
            assert(WriteableBytes() >= len);
            ::memcpy(GetWriteablePointer(), dataPtr, len);
            writeIndex_ += len;
        }


        void EnsureWritableBytes(size_t len)
        {
            if (WriteableBytes() < len)
            {
                AssureEnoughSpace(len);
            }
            assert(WriteableBytes() >= len);

        }

        void HasWritten(size_t len)
          {
               assert(len <= WriteableBytes());
               writeIndex_ += len;
          }

          void Pop(uint64_t len);

        void PopInt32() { Pop(sizeof(int32_t)); };
        void PopInt64() { Pop(sizeof(int64_t)); };

        int32_t ReadAsInt32(const char* buf);

        const char* begin() const { return &*buffer_.begin(); }
        char* Begin() { return &*buffer_.begin(); }
        char* End() { return &*buffer_.end(); }
        uint64_t Size() { return buffer_.size(); }

    private:
        void AssureEnoughSpace(uint64_t len);

        const char* FindLastEndOfLine();
        const char* FindFirstEndOfLine();



    private:
        std::vector<char> buffer_;
        uint64_t writeIndex_;
        uint64_t readIndex_;
    };
}

#endif
