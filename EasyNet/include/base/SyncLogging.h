#ifndef SYNC_LOGGING_H__
#define SYNC_LOGGING_H__
#include "EasyNet/include/base/Noncopyable.h"
#include "EasyNet/include/base/FileOperation.h"
#include "EasyNet/include/base/PlatformDefine.h"
#include <memory>
#include <mutex>




namespace Logger
{
	class SyncLogging : private Noncopyable
	{
	public:
		SyncLogging();
		~SyncLogging();

		void Append(const char* msg, uint64_t len);
		void Flush();

	private:
		void CreateLogFileDynamiclly();

	private:
		Scoped_ptr<FileOperation> fileOperation_;
		std::mutex ioMutex_;
		const uint64_t logFileRollSize_ = 10 * 1024 * 1024;
	};
}


#endif