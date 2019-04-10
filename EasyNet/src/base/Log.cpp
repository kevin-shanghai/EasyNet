#include "EasyNet/include/base/Log.h"
#include "EasyNet/include/base/LogImpl.h"
#include "EasyNet/include/base/PlatformDefine.h"
#include <iostream>



namespace Logger
{
	//define the default logLevel, output Function callback and flush Function callback//
	void defaultOutput(const char* msg, uint64_t len);
	void defaultFlush();

	LogLevel G_LOG_LEVEL = Logger::LogLevel::TRACE;
	output_func_call_back G_OUTPUT_FUNC = Bind(defaultOutput, \
		Placeholders _1,
		Placeholders _2);
	flush_func_call_back G_FLUSH_FUNC = Bind(defaultFlush);

    void defaultOutput(const char* msg, uint64_t len)
    {

		size_t n = fwrite(msg, 1, static_cast<size_t>(len), stdout);
        (void)n;
    }

    void defaultFlush()
    {
        fflush(stdout);
    }

    Log::Log(const char* file_name, uint32_t line_num)
        : logImpl_(LogLevel::INFO, file_name, line_num)
    {
        
    }

    Log::Log(const char* file_name, uint32_t line_num, LogLevel level)
        : logImpl_(level, file_name, line_num)
    {

    }

    Log::Log(const char* file_name, uint32_t line_num, LogLevel level,const char* func_name)
        :logImpl_(level, file_name, line_num)
    {
		logImpl_.LogFuncName(func_name);
    }

    Log::Log(const char* file_name, uint32_t line_num, bool toAbort)
        :logImpl_(toAbort ? LogLevel::ERROR : LogLevel::FATAL, file_name, line_num)
    {

    }

    LogStream& Log::GetLogStream() const
    {
        return logImpl_.GetLogStream();
    }

    Log::~Log()
    {
        logImpl_.LogFileNameAndLineNumber();
		G_OUTPUT_FUNC(logImpl_.GetLogStream().GetLogBuffer().GetBufferData(),
			logImpl_.GetLogStream().GetLogBuffer().Length());
		if (logImpl_.GetLocalLogLevel() > GetGlobalLogLevel())	   //write to stdout
		{
			defaultOutput(logImpl_.GetLogStream().GetLogBuffer().GetBufferData(),
				logImpl_.GetLogStream().GetLogBuffer().Length());
			G_FLUSH_FUNC();
		}
        //G_FLUSH_FUNC();
    }
}