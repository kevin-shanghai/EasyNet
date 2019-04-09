#include "EasyNet/include/net/Channel.h"
#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/SocketApiWrapper.h"

//#include "Common/CommonUtility/csw_control_logger.h"

#include <iostream>
#include <assert.h>

namespace Net
{
	Channel::Channel(EventLoop* loop, socket_t fd)
		: loop_(loop)
		, fd_(fd)
		, intrestEvent_(kInvalidEvent)
		, readyEvent_(kInvalidEvent)
	{

	}

	Channel::~Channel()
	{
		//CLOG_INFO("Channel dtor...");
        intrestEvent_ = kInvalidEvent;
        readyEvent_ = kInvalidEvent;

		//warning, do not close the socket, for connector used//
        //SocketsApi::Close(fd_);
	}


	void Channel::SetInterestEvent(EventType intrestEvent)
	{
		intrestEvent_ |= intrestEvent;
        //CLOG_INFO(" ---SetInterestEvent:" << intrestEvent_);
		Update();
	}

	void Channel::Update()
	{
		loop_->UpdateChannel(this);
	}

	void Channel::Remove()
	{
		assert(intrestEvent_ == kInvalidEvent);
		loop_->RemoveChannel(this);
	}


	std::string Channel::RedayEventsToString() const
	{
		static std::string readyEventStr;
        readyEventStr.clear();
		if (readyEvent_ & kErrorEvent)
		{
			readyEventStr.append("kErrorEvent ");
		}
		if (readyEvent_& kReadEvent)
		{
			readyEventStr.append("kReadEvent ");
		}

		if (readyEvent_ & kWriteEvent)
		{
			readyEventStr.append("kWriteEvent ");
		}

		return readyEventStr;
	}


    void Channel::HandleEvent(Timestamp receiveTime)
    {
        receiveTime;
        if (readyEvent_ & kErrorEvent)
        {
            if (errorCallback_) errorCallback_();
        }
        if (readyEvent_ & kReadEvent)
        {
            if (readCallback_) readCallback_();
        }
        if (readyEvent_ & kWriteEvent)
        {
            if (writeCallback_) writeCallback_();
        }
    }
    


}
