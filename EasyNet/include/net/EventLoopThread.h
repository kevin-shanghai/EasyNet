#ifndef EVENT_LOOP_THREAD_H__
#define EVENT_LOOP_THREAD_H__

#include "EasyNet/include/base/Thread.h"
#include "EasyNet/include/base/MutexLock.h"
#include "EasyNet/include/base/Condition.h"


#ifdef EXPORT_EASYNET
#define CSW_EXPORT_OWNER
#endif


namespace Net
{
	class EventLoop;
	class EXPORT_NETBASElIGHT EventLoopThread : public base::Thread
	{
	public:
		EventLoopThread();

		~EventLoopThread();

		EventLoop* StartLoopThread();

	private:
		virtual void Run();


	private:
		EventLoop* loopOfThisThread_;
		base::MutexLock mutex_;
		base::Condition loopCond_;	
        bool started_;
        
	};
}



#endif