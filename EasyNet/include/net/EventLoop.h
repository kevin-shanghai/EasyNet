#ifndef EVENT_LOOP_H__
#define EVENT_LOOP_H__

#include "EasyNet/include/base/Noncopyable.h"
#include "EasyNet/include/base/TimeStamp.h"
#include "EasyNet/include/net/ICallBackDefine.h"
#include <list>
#include <vector>

#ifdef EXPORT_NETBASElIGHT
#define CSW_EXPORT_OWNER
#endif


namespace base
{
    class MutexLock;
}
namespace Net
{
	class Channel;
    class Poller;
    class TimerManager;
	

	class EXPORT_NETBASElIGHT EventLoop : private Noncopyable
	{
	public:
        typedef std::list<Channel*> ChannelList;
        typedef Function<void()> Functor;

        enum Timeout
        {
            kStrict = 100,
            kCoarser = 500
        };

		EventLoop();

		void Loop();

		void UpdateChannel(Channel* channel);

		void RemoveChannel(Channel* channel);

        void RunInLoop(Functor& cb);

        void QueueInLoop(Functor& cb);

		void AssertInLoopThreadOrDie();

		bool IsInEventLoopOwnThread();

        bool EventHandling() const { return eventHandlingFlag_; }

        void ExitLoop() { runFlag_ = false; }

		bool HasChannel(Channel* channel);

        void RunAt(const Timestamp& time, const Functor& cb);
  
        void RunAfter_ms(double delay, const Functor& cb);

        void RunAfter_s(double delay, const Functor& cb);

        void CancelAllTimers();

        void SetStrictTimeoutInDelivery()
        {
           timerOutMs_ = kStrict;
        }

        void SetCorserTimeout()
        {
            timerOutMs_ = kCoarser;
        }

        // if no timer, use default time out(1000ms), otherwise use the earliest timer//
        int64_t GetTimeout() const;

    private:

        void DoPendingFunctors();

        void PrintActiveChannels();

		
	private:
		ChannelList activeChannelList_;
		Channel* currentActiveChannel_;
        Channel* wakeupChannel_;
		bool eventHandlingFlag_;
        bool runFlag_;
		Poller* poller_;
        ThreadIdType eventLoopOwnThreadID_;
        std::vector<Functor> pendingFunctors_;
        TimerManager* timerManagerPtr_;
        base::MutexLock* mutex_;
        int64_t timerOutMs_;
	};
}



#endif