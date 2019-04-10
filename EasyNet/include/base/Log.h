#ifndef LOG_H__
#define LOG_H__

#include <stdint.h>
#include "EasyNet/include/base/PlatformDefine.h"
#include "EasyNet/include/base/LogImpl.h"
#include "EasyNet/include/base/Noncopyable.h"
#include "EasyNet/include/base/SyncLogging.h"


namespace Logger
{
	using output_func_call_back = Function<void(const char* output_log_buffer, int64_t buffer_len)>;
	using flush_func_call_back = Function<void(void)>;
	enum class LogLevel{ TRACE, DEBUG, INFO, WARN, ERROR, FATAL, LOG_LEVEL_NUM };
	extern LogLevel G_LOG_LEVEL;
	extern output_func_call_back G_OUTPUT_FUNC;
	extern flush_func_call_back G_FLUSH_FUNC;

	class Log : Noncopyable
	{
	public:
		
		Log(const char* file_name, uint32_t line_num);
		Log(const char* file_name, uint32_t line_num, LogLevel level);
		Log(const char* file_name, uint32_t line_num, LogLevel level, const char* func_name);
		Log(const char* file_name, uint32_t line_num, bool toAbort);
        ~Log();

	public:
		static LogLevel GetGlobalLogLevel()
		{
			return Logger::G_LOG_LEVEL;
		}
		static void SetOutputFuncCallback(output_func_call_back output_cb)
		{
			G_OUTPUT_FUNC = output_cb;
		}
		static void SetFlushFuncCallback(flush_func_call_back flush_cb)
		{
			G_FLUSH_FUNC = flush_cb;
		}
		static void SetGlobalLogLevel(LogLevel loglevel)
		{
			G_LOG_LEVEL = loglevel;
		}
		
		LogStream& GetLogStream() const;


	private:
		// use the stack object in order to avaoid the memory segmentation 
		LogImpl logImpl_;  
	};
} 





#define LOG_TRACE if (Logger::Log::GetGlobalLogLevel() <= Logger::LogLevel::TRACE) \
	(Logger::Log(__FILE__, __LINE__, Logger::LogLevel::TRACE, __FUNCTION__).GetLogStream())

#define LOG_DEBUG if (Logger::Log::GetGlobalLogLevel() <= Logger::LogLevel::DEBUG) \
	Logger::Log(__FILE__, __LINE__, Logger::LogLevel::DEBUG, __FUNCTION__).GetLogStream()

#define LOG_INFO if (Logger::Log::GetGlobalLogLevel() <= Logger::LogLevel::INFO) \
	(Logger::Log(__FILE__, __LINE__).GetLogStream())

#define LOG_WARN Logger::Log(__FILE__, __LINE__, Logger::LogLevel::WARN).GetLogStream()
#define LOG_ERROR Logger::Log(__FILE__, __LINE__, Logger::LogLevel::ERROR).GetLogStream()
#define LOG_FATAL Logger::Log(__FILE__, __LINE__, Logger::LogLevel::FATAL).GetLogStream()
#define LOG_SYSERR Logger::Log(__FILE__, __LINE__, false).GetLogStream()
#define LOG_SYSFATAL Logger::Log(__FILE__, __LINE__, true).GetLogStream()


#endif