#ifndef THREAD_H__
#define THREAD_H__

#include "EasyNet/include/base/PlatformDefine.h"
#include "EasyNet/include/base/Noncopyable.h"
#include <string>
#include <stdint.h>


namespace base
{
    class Thread : private Noncopyable
    {
    public:
        explicit Thread(const std::string& threadName = "UnKnow");
        ThreadIdType GetThreadID() { return threadID_; }
        void Start();
        virtual void Run() = 0;
        const std::string& GetThreadName() { return threadName_; }

    private:
        std::string threadName_;
        ThreadIdType threadID_;
    };

}



namespace this_thread_
{
	inline int64_t get_id()
	{
		int64_t threadID = 0;
#ifdef ON_LINUX
		threadID = (int64_t)syscall(SYS_gettid);
#elif defined ON_WINDOWS
		threadID = ::GetCurrentThreadId();
#else
		threadID = (int64_t)pthread_self();
#endif
		return threadID;
	}

    inline void SleepMs(int32_t iMs)
    {
#ifdef ON_WINDOWS
        Sleep(iMs); 
#elif defined  ON_VXWORKS
        taskDelay(sysClkRateGet()*iMs/1000+1); 
#else
        usleep(iMs * 1000);
#endif

    }

    inline int64_t CurrentTimeStamp()
    {
        int64_t iTime = 0;
#if defined ON_WINDOWS
        iTime = GetCurrentTime();
#elif defined ON_VXWORKS
        iTime = tickGet();
#elif defined ON_LINUX
        struct timeval tv;
        ::gettimeofday(&tv, NULL);
        return tv.tv_usec / 1000;
#endif
        return iTime;
    }
}

#endif