#ifndef CHANNEL_H__
#define CHANNEL_H__

#include "EasyNet/include/net/SoketDefine.h"
#include "EasyNet/include/base/Noncopyable.h"
#include "EasyNet/include/base/PlatformDefine.h"
#include "EasyNet/include/base/TimeStamp.h"
#include "EasyNet/include/net/ICallBackDefine.h"
#include <string>

namespace Net
{
	class EventLoop;
	enum EventType 
	{ 
        kInvalidEvent = 0,
        kReadEvent =    0x0001,
        kWriteEvent =   0x0010,
        kErrorEvent =   0x0100
	};

	class Channel 
	{
	public:

		Channel(EventLoop* loop, socket_t fd);

		~Channel();

        void SetReadCallback(const EventCallback& cb)
        { 
            readCallback_ = cb; 
        }
        void SetWriteCallback(const EventCallback& cb)
        { 
            writeCallback_ = cb; 
        }
        void SetCloseCallback(const EventCallback& cb)
        { 
            closeCallback_ = cb; 
        }

        void SetErrorCallback(const EventCallback& cb)
        { 
            errorCallback_ = cb; 
        }

		void HandleEvent(Timestamp receiveTime);

		void SetInterestEvent(EventType intrestEvent);

		void SetReadyEvent(EventType event) { readyEvent_ = event; }

		void DisableAll() { intrestEvent_ &= kInvalidEvent; Update();}

        void EnableReading() { SetInterestEvent(kReadEvent); }

        void EnableWriting() { SetInterestEvent(kWriteEvent); }

		void DisableWriting() 
		{ 
			intrestEvent_ = intrestEvent_ & (~kWriteEvent);
			Update(); 
		}

		socket_t GetSocketFd() const { return fd_; }

		EventType GetEventType() const { return static_cast<EventType>(intrestEvent_); }

		bool IsWriting()
		{ 
			return !!(intrestEvent_ & kWriteEvent);
		}

		void Remove();

		EventLoop* GetOwnerLoop() const { return loop_; }
		
		std::string RedayEventsToString() const;


	private:
		void Update();

	private:
		EventLoop* loop_;
		socket_t fd_;
		int intrestEvent_;
		int readyEvent_;

        EventCallback readCallback_;
        EventCallback writeCallback_;
        EventCallback closeCallback_;
        EventCallback errorCallback_;
	};
}

#endif
