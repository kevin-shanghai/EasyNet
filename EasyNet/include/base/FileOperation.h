#ifndef LOG_FILE_UTIL_H__
#define LOG_FILE_UTIL_H__
#include <stdint.h>
#include <mutex>
#include <stdio.h>
#include <memory>
#include <atomic>
#include "EasyNet/include/base/Noncopyable.h"


namespace Logger
{
	class FileOperation : Noncopyable
	{
	public:
		FileOperation(const char* sFileName);
		void AppendLogFile(const char* logline, uint64_t len);
		void FlushLogFile();
		uint64_t GetWrittenBytes() const { return iBytesWrited_; }

	private:
		uint64_t WriteLogFile(const char* msg, uint64_t msg_len);
		std::string GetLogFileName();


	private:
		std::unique_ptr<FILE, int(*)(FILE*)> logFile_;
		std::mutex mutex_; 
		std::atomic<uint64_t> iBytesWrited_{0};
		char buffer_[64 * 1024];

	};
}



#endif