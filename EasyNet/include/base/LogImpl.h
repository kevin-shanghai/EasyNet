#ifndef LOGIMPL_H__
#define LOGIMPL_H__

#include "EasyNet/include/base/LogStream.h"
#include "EasyNet/include/base/Noncopyable.h"

namespace Logger
{
	enum class LogLevel;
	class LogImpl : Noncopyable
	{
	public:
		LogImpl(LogLevel logLevel, const char* source_file_name, uint32_t line_num);
		~LogImpl();
		LogStream& GetLogStream() const;
		void LogFileNameAndLineNumber();
		void LogFuncName(const char* funcName)
		{
			strFuncName_ = funcName;
		}
		LogLevel GetLocalLogLevel()
		{
			return logLevel_;
		}

	private:
		LogStream logStream_;	 
		LogLevel logLevel_;
		std::string sourceFileName_;
		uint32_t lineNum_;
		std::string strFuncName_;
	};
}

#endif