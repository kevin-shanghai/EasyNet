#include "EasyNet/include/base/FileOperation.h"
#include <stdio.h>
#include <mutex>
#include <assert.h>
#include <memory>
#include <iostream>

namespace Logger
{
	FileOperation::FileOperation(const char* sFileName)
		: logFile_(::fopen(sFileName, "w"), fclose)
	{
		assert(logFile_.get());
		::setbuf(logFile_.get(), buffer_);
	}

	void FileOperation::AppendLogFile(const char* logline, uint64_t len)
	{
		uint64_t n = WriteLogFile(logline, len);
		uint64_t remain = len - n;
		while (remain > 0)
		{
			uint64_t x = WriteLogFile(logline + n, remain);
			if (x == 0)
			{
				int err = ferror(logFile_.get());
				if (err)
				{
					;
				}
				break;
			}
			n += x;
			remain = len - n; // remain -= x
		}
		iBytesWrited_ += n;
	}


	uint64_t FileOperation::WriteLogFile(const char* msg, uint64_t msg_len)
	{
		#ifdef WIN32 
			return ::_fwrite_nolock(msg, 1, msg_len, logFile_.get());
		#elif defined(__linux__)
            int count = ::fwrite_unlocked(msg, 1, msg_len, logFile_.get());
            fflush(logFile_.get());
            return count;
            
		#else
			return 0; 
		#endif
	}

	void FileOperation::FlushLogFile()
	{
		::fflush(logFile_.get());
	}

}