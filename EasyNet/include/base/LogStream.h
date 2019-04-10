#ifndef LOGSTREAM_H__
#define LOGSTREAM_H__
#include <string>	
#include "EasyNet/include/base/Buffer.h"
#include "EasyNet/include/base/Noncopyable.h"

namespace Logger
{
    class LogStream : Noncopyable
    {
    public:
        typedef Buffer<kSmallBuffer> LogBuffer;

        LogStream& operator << (bool b);

        LogStream& operator << (char v);

        LogStream& operator << (const char* str);

        LogStream& operator << (const unsigned char* str);

        LogStream& operator << (const std::string& v);

        template <class T>
		LogStream& operator << (T value)
		{
			static_assert(std::is_arithmetic<T>::value, "Arithmetic required.");
			FormatInteger(value);
			return *this;
		}

        template <class T>
		LogStream& operator << (T* ptr)
		{
			if (nullptr != ptr)
			{
				char ptrStr[10] = { '\0' };
				sprintf(ptrStr, "0x%p", ptr);
				logBuffer_.Append(ptrStr, 10);
			}
			else
			{
				char ptrStr[7] = { '\0' };
				strncpy(ptrStr, "nullptr", 7);
				logBuffer_.Append(ptrStr, 7);
			}
			return *this;
		}

		void Append(const char* data, int len) { logBuffer_.Append(data, len); }

		 const LogBuffer& GetLogBuffer() const { return logBuffer_; }


    private:
        template <class T>
		void FormatInteger(T value)
		{
			if (logBuffer_.AvailableSize() >= 32)
			{
				logBuffer_.Append(std::to_string(value).c_str(), std::to_string(value).size());
			}
		}

    private:
        LogBuffer logBuffer_;

    };
}


#endif