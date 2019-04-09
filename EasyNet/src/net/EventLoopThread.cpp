#include "EasyNet/include/net/EventLoopThread.h"
#include "EasyNet/include/net//EventLoop.h"


using namespace base;

namespace Net
{
	EventLoopThread::EventLoopThread() 
                                       : mutex_()
                                       , loopCond_(mutex_)
                                       , loopOfThisThread_(NULL)
                                       , started_(false)
                                       
	{

	}


	EventLoopThread::~EventLoopThread()
	{

	}


	void EventLoopThread::Run()
	{
		EventLoop loop;
		loopOfThisThread_ = &loop;
        loopCond_.NotifyOne();
		loop.Loop();
		loopOfThisThread_ = NULL;
	}


	EventLoop* EventLoopThread::StartLoopThread()
	{
        if (started_)
        {
            return loopOfThisThread_;
        }

		this->Start();
		LockGuard lk(&mutex_);
		while (loopOfThisThread_ == NULL)
		{
			loopCond_.Wait();
		}
		assert(loopOfThisThread_);
        started_ = true;
		return loopOfThisThread_;
	}
}

