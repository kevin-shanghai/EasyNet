#include "EasyNet/include/base/LogImpl.h"
#include "EasyNet/include/base/Log.h"
#include "EasyNet/include/base/TimeStamp.h"
#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <sstream>
#include <string.h> //for strrchr in linux



namespace Logger
{
	namespace details
	{
		std::string GetCurrentLocalTime();
		std::string Now()
		{
			return Timestamp::Now().ToCalenderTime();
		}

		std::string GetCurrentThreadId()
		{
			std::stringstream ss;
			ss << std::this_thread::get_id();
			return ss.str();
		}

		const char* GetFileBaseName(const char* sourceFileName)
		{
			#ifdef WIN32 
			auto const file_path_split = '\\';
			#elif defined(__linux__)
			auto const file_path_split = '/';
			#endif
			const char* slash = ::strrchr(sourceFileName, file_path_split);
			return slash ? slash + 1 : nullptr;
		}
	}

	std::map<LogLevel, std::string> LogLevelNameMap{
		std::pair<LogLevel, std::string>(LogLevel::TRACE, "TRACE"),
		std::pair<LogLevel, std::string>(LogLevel::DEBUG, "DEBUG"),
		std::pair<LogLevel, std::string>(LogLevel::INFO, "INFO"),
		std::pair<LogLevel, std::string>(LogLevel::WARN, "WARN"),
		std::pair<LogLevel, std::string>(LogLevel::ERROR, "ERROR"),
		std::pair<LogLevel, std::string>(LogLevel::FATAL, "FATAL"),
	};

	LogImpl::LogImpl(LogLevel logLevel, const char* source_file_name, uint32_t line_num)
		: logLevel_(logLevel)
		, sourceFileName_(source_file_name)
		, lineNum_(line_num)
		, strFuncName_("")
		, logStream_()
	{
		//auto currentTime = detail::GetCurrentLocalTime();
		auto currentTime = details::Now();
		logStream_ << LogLevelNameMap[logLevel] << " " << currentTime << " " <<  details::GetCurrentThreadId() << " ";
	}

	LogImpl::~LogImpl()
	{
	}

	LogStream& LogImpl::GetLogStream() const
	{
		return const_cast<LogStream&>(logStream_);
	}

	void LogImpl::LogFileNameAndLineNumber()
	{
		if (strFuncName_.empty())
			logStream_ << " "<< details::GetFileBaseName(sourceFileName_.c_str()) << ":" << lineNum_ << "\n";
		else
			logStream_ <<  " " << strFuncName_ << " " << details::GetFileBaseName(sourceFileName_.c_str()) << ":" << lineNum_ << "\n";
	}
}