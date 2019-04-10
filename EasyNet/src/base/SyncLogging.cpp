#include "EasyNet/include/base/SyncLogging.h"
#include "EasyNet/include/base/FileOperation.h"
#include "EasyNet/include/base/TimeStamp.h"
#include <iostream>

namespace Logger
{
	namespace details
	{

		std::string GetLogFileName()
		{
			return Timestamp::Now().ToCalenderTime() + ".log";
		}
	}

	SyncLogging::SyncLogging()
	{
		CreateLogFileDynamiclly();
	}

	SyncLogging::~SyncLogging()
	{

	}

	void SyncLogging::Append(const char* msg, uint64_t len)
	{
		std::lock_guard<std::mutex> lk(ioMutex_);
		if (fileOperation_->GetWrittenBytes() >= logFileRollSize_)
		{
			fileOperation_->FlushLogFile();
			CreateLogFileDynamiclly();
		}
		fileOperation_->AppendLogFile(msg, len);
	}

	void SyncLogging::Flush()
	{
		std::lock_guard<std::mutex> lk(ioMutex_);
		fileOperation_->FlushLogFile();
	}

	void SyncLogging::CreateLogFileDynamiclly()
	{
		fileOperation_.reset(new FileOperation(details::GetLogFileName().c_str()));
	}
}