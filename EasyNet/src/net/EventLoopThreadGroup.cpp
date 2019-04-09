#include "EasyNet/include/net/EventLoopThreadGroup.h"
#include "EasyNet/include/net/EventLoopThread.h"
#include "EasyNet/include/net/EventLoop.h"
#include <assert.h>



namespace Net
{
	EventLoopThreadGroup::EventLoopThreadGroup(EventLoop* baseLoop, int threadNum)
		: baseLoop_(baseLoop)
		, numThreads_(threadNum)
		, startFlag_(false)
		, loopIndex_(0)
	{

	}


	void EventLoopThreadGroup::Start(const ThreadInitCallback& cb)
	{
        assert(!startFlag_);
        baseLoop_->AssertInLoopThreadOrDie();

        startFlag_ = true;

        for (int i = 0; i < numThreads_; ++i)
        {
            EventLoopThread* t = new EventLoopThread();
            loopThreads_.push_back(t);
            loops_.push_back(t->StartLoopThread());
        }
        if (numThreads_ == 0 && cb)
        {
            cb(baseLoop_);
        }
	}


	EventLoop* EventLoopThreadGroup::GetNextLoop() const
	{
		baseLoop_->AssertInLoopThreadOrDie();
		assert(startFlag_);
		EventLoop* loop = baseLoop_;

		if (!loops_.empty())
		{
			// round-robin
			loop = loops_[loopIndex_];
			loopIndex_++;
			if (loopIndex_ >= loops_.size())
			{
				loopIndex_ = 0;
			}
		}
		return loop;
	}
}
