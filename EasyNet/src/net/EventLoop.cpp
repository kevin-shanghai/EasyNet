#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/PollerFactory.h"
#include "EasyNet/include/net/Poller.h"
#include "EasyNet/include/net/Channel.h"
#include "EasyNet/include/base/ThreadLocal.h"
#include "EasyNet/include/base/PlatformDefine.h"
#include "EasyNet/include/base/Thread.h"
#include "EasyNet/include/base/MutexLock.h"
#include "EasyNet/include/base/TimeStamp.h"
#include "EasyNet/include/net/SocketApiWrapper.h"
#include "EasyNet/include/net/Timer.h"
#include "EasyNet/include/base/Log.h"
#include <iostream>
#include <assert.h>
#include <algorithm>

using namespace base;          
namespace Net
{
	TLS EventLoop* t_threadLocalEventLoop = NULL;

	EventLoop::EventLoop()
		: eventLoopOwnThreadID_(this_thread_::get_id())
		, currentActiveChannel_(NULL)
		, eventHandlingFlag_(false)
        , runFlag_(true)
        , mutex_(new MutexLock())
        , timerManagerPtr_(new TimerManagerFIFO())
        , timerOutMs_(kCoarser)
	{
        poller_ = (PollerFactory::CreateDefaultPoller(this));
		if (t_threadLocalEventLoop)
		{
			LOG_INFO << "Another EventLoop:" << t_threadLocalEventLoop 
				<< "in this thread " << this_thread_::get_id();
		}
		else
		{
			t_threadLocalEventLoop = this;
		}
#ifdef ON_WINDOWS
       wakeupChannel_ = new Channel(this, SocketsApi::CreateNonBlockingSocket());
       wakeupChannel_->EnableReading();
#endif
	}

    int64_t EventLoop::GetTimeout() const
    {
        int64_t iTimeout = timerOutMs_;
        if (!timerManagerPtr_->IsEmpty())
        {
            iTimeout = timerManagerPtr_->GetEarliestTimer();
        }
        return iTimeout;
    }

	void EventLoop::Loop()
	{
		AssertInLoopThreadOrDie();
		while(runFlag_)
		{
			activeChannelList_.clear();
            int64_t iTimeout = GetTimeout();
            if (iTimeout <= 0)
            {
                //handle the timer which is time out immediatly//
                LOG_ERROR << "time out for timer executer.";
                timerManagerPtr_->Process();
                continue;
            }
			poller_->Poll(iTimeout, &activeChannelList_);
            timerManagerPtr_->Process();
	        PrintActiveChannels();
			eventHandlingFlag_ = true;
			ChannelList::iterator it = activeChannelList_.begin();
			for (; it != activeChannelList_.end(); it++)
			{
				currentActiveChannel_ = *it;
				currentActiveChannel_->HandleEvent(Timestamp::Now());
			}
			currentActiveChannel_ = NULL;
			eventHandlingFlag_ = false;
            DoPendingFunctors();
		} 

	}


	void EventLoop::UpdateChannel(Channel* channel)
	{
		poller_->UpdateChannel(channel);
	}

	void EventLoop::RemoveChannel(Channel* channel)
	{
		assert(channel->GetOwnerLoop() == this);
        if (eventHandlingFlag_)
        {
            assert(currentActiveChannel_ == channel ||
                std::find(activeChannelList_.begin()
                , activeChannelList_.end(), channel) == activeChannelList_.end());
        }
       
		poller_->RemoveChannel(channel);
	}


	void EventLoop::AssertInLoopThreadOrDie()
	{
		assert(IsInEventLoopOwnThread());  
	}

	bool EventLoop::IsInEventLoopOwnThread()
	{
		return eventLoopOwnThreadID_ == this_thread_::get_id();
	}

	void EventLoop::PrintActiveChannels()
	{
#if 0
		ChannelList::const_iterator it = activeChannelList_.begin();
		for (; it != activeChannelList_.end(); it++)
		{
            //for debug
            std::cout << "ActiveChannels####{ " <<  "Channel:" << *it 
                << " , Event:" << (*it)->RedayEventsToString().c_str()
                << " }" << std::endl;
		}
#endif
	}


    void EventLoop::DoPendingFunctors()
    {
        std::vector<Functor> functors;
        {
            LockGuard lk(mutex_);
            functors.swap(pendingFunctors_);
        }
        for (size_t i = 0; i < functors.size(); ++i)
        {
            functors[i]();
        }
    }


    void EventLoop::RunInLoop(Functor& cb)
    {
        if (IsInEventLoopOwnThread())
        {
            cb();
        }
        else
        {
            QueueInLoop(cb);
        }
    }

    void EventLoop::QueueInLoop(Functor& cb)
    {
        LockGuard lock(mutex_);
        pendingFunctors_.push_back(cb);
    }

    void EventLoop::RunAt(const Timestamp& time, const Functor& cb)
    {
        Timer* timer = new Timer(cb, time);
        timerManagerPtr_->AddTimer(timer);

    }

    void EventLoop::RunAfter_ms(double delay, const Functor& cb)
    {
        Timestamp time = Timestamp::AddMilliseconds(Timestamp::Now(), delay);
        Timer* timer = new Timer(cb, time);
        timerManagerPtr_->AddTimer(timer);
    }

    void EventLoop::RunAfter_s(double delay, const Functor& cb)
    {
        Timestamp time = Timestamp::AddSeconds(Timestamp::Now(), delay);
        Timer* timer = new Timer(cb, time);
        timerManagerPtr_->AddTimer(timer);
    }

    void EventLoop::CancelAllTimers()
    {
        timerManagerPtr_->CancelAll();
    }


}
