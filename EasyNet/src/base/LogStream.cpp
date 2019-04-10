#include "EasyNet/include/base/LogStream.h"


namespace Logger
{

    LogStream& LogStream::operator << (const std::string& v)
    {
        logBuffer_.Append(v.c_str(), v.size());
        return *this;
    }

    LogStream& LogStream::operator<<(bool b)
    {
        logBuffer_.Append(b ? "true" : "false", 6);
        return *this;
    }

    LogStream& LogStream::operator<<(char v)
    {
        logBuffer_.Append(&v, 1);
        return *this;
    }

    LogStream& LogStream::operator<<(const char* str)
    {
        if (str)
        {
            logBuffer_.Append(str, strlen(str));
        }
        else
        {
            logBuffer_.Append("(null)", 6);
        }
        return *this;
    }

    LogStream& LogStream::operator<<(const unsigned char* str)
    {
        return operator << (reinterpret_cast<const char*>(str));
    }


}