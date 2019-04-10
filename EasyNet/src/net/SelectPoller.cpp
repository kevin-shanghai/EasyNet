#include "EasyNet/include/net/SoketDefine.h"
#include "EasyNet/include/net/SelectPoller.h"
#include "EasyNet/include/net/Channel.h"
#include "EasyNet/include/base/Log.h"
#include <assert.h>
#include <iostream>


namespace Net
{
	SelectPoller::SelectPoller(EventLoop* loop) : Poller(loop)
	{
		FD_ZERO(&select_readfds_);
		FD_ZERO(&select_writefds_);
		FD_ZERO(&select_expectfds_);
		FD_ZERO(&write_fds_);
		FD_ZERO(&read_fds_);
		FD_ZERO(&expect_fds_);
	}

	void SelectPoller::UpdateChannel(Channel* channel)
	{
		if (polledChannelsMap_.find(channel->GetSocketFd()) == polledChannelsMap_.end())
		{
			polledChannelsMap_.insert(std::pair<socket_t, Channel*>(channel->GetSocketFd(), channel));
			fdSet_.insert(channel->GetSocketFd());
		}
		Update(channel);
	}

	void SelectPoller::RemoveChannel(Channel* channel)
	{
		assert(channel);
		socket_t fd = channel->GetSocketFd();
        LOG_TRACE << "RemoveChannel fd is:" << fd;
		assert(polledChannelsMap_.find(fd) != polledChannelsMap_.end());  
		assert(fdSet_.find(fd) != fdSet_.end());
		FD_CLR(fd, &select_readfds_);
		FD_CLR(fd, &select_writefds_);
		FD_CLR(fd, &select_expectfds_);
		polledChannelsMap_.erase(fd);
		fdSet_.erase(fd);
	}

	void SelectPoller::Update(Channel* channel)
	{
		assert(channel);
        LOG_TRACE << " Update fd = " << channel->GetSocketFd()
             	  << " interestEvent:" << static_cast<int64_t>(channel->GetEventType());
		FD_SET(channel->GetSocketFd(), &select_expectfds_);
		if (channel->GetEventType() & kReadEvent)
		{
			FD_SET(channel->GetSocketFd(), &select_readfds_);
		}
        else
        {
            FD_CLR(channel->GetSocketFd(), &select_readfds_);
        }
		if (channel->GetEventType() & kWriteEvent)
		{
			FD_SET(channel->GetSocketFd(), &select_writefds_);
		}
        else
        {
            FD_CLR(channel->GetSocketFd(), &select_writefds_);
        }
		if (channel->GetEventType() == kInvalidEvent)
		{
            FD_CLR(channel->GetSocketFd(), &select_readfds_);
            FD_CLR(channel->GetSocketFd(), &select_writefds_);
			FD_CLR(channel->GetSocketFd(), &select_expectfds_);
		}
		
       
	}

	void SelectPoller::Poll(int timeoutMs, ChannelList* activeChannels)
	{
		struct timeval tv;
		tv.tv_sec = timeoutMs/1000;
		tv.tv_usec = (timeoutMs % 1000) * 1000;
		read_fds_ = select_readfds_;
		write_fds_ = select_writefds_;
		expect_fds_ = select_expectfds_;
        int maxFd = 0;
        if (fdSet_.empty())
        {
        }
        else
        {
            maxFd = *(fdSet_.begin());
        }
		
		int numActiveEvents = ::select(maxFd + 1, &read_fds_, &write_fds_, &expect_fds_, &tv);
       
		if (numActiveEvents > 0)
		{
			FillActiveChannel(numActiveEvents, activeChannels);
		}
		else if (!numActiveEvents)
		{
            
		}
		else
		{
#ifdef ON_WINDOWS
            int err = WSAGetLastError();
			LOG_ERROR << "select system call error, info:" 
                << " errno:" << err
                << strerror(err) 
                << " read fdcount:" << read_fds_.fd_count
                << " write fdcount:" << write_fds_.fd_count;
#endif
		}
	}

	void SelectPoller::FillActiveChannel(int activeEventsNum, ChannelList* channelList) 
	{
		int readyEvent = kInvalidEvent;
		std::set<socket_t, std::greater<socket_t> >::iterator it = fdSet_.begin();
		for (; it != fdSet_.end() && activeEventsNum > 0; it++)
		{
			socket_t fd = *it;
			if (FD_ISSET(fd, &read_fds_)) readyEvent |= kReadEvent;
			if (FD_ISSET(fd, &write_fds_)) readyEvent |= kWriteEvent;
            if (FD_ISSET(fd, &expect_fds_)) readyEvent |= kErrorEvent;

			if (readyEvent != kInvalidEvent)
			{
				activeEventsNum--;
				Channel* singleActiveChannel = polledChannelsMap_.find(fd)->second;
				assert(singleActiveChannel);
				singleActiveChannel->SetReadyEvent(static_cast<EventType>(readyEvent));
				channelList->push_back(singleActiveChannel);
			}
		}
	}
}
