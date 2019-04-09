#ifndef EVENT_LOOP_THREAD_GROUP_H__
#define EVENT_LOOP_THREAD_GROUP_H__
#include <stdint.h>
#include <vector>
#include "EasyNet/include/net/ICallBackDefine.h"

#ifdef EXPORT_NETBASElIGHT
#define CSW_EXPORT_OWNER
#endif

namespace Net
{
	class EventLoop;
	class EventLoopThread;
	class EXPORT_NETBASElIGHT EventLoopThreadGroup
	{
	public:
        typedef Function<void(EventLoop*)> ThreadInitCallback;
		EventLoopThreadGroup(EventLoop* baseLoop, int threadNum = 0);
		void Start(const ThreadInitCallback& cb = ThreadInitCallback());
		EventLoop* GetNextLoop() const;
		void SetThreadNum(int threadNum) { numThreads_ = threadNum; }



	private:
		EventLoop* baseLoop_;
		int numThreads_;
		bool startFlag_;
		mutable uint32_t loopIndex_;
		std::vector<EventLoop*> loops_;
		std::vector<EventLoopThread*> loopThreads_;
		
	};
}


#endif