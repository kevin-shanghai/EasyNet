#ifndef SELECT_POLLER_H__
#define SELECT_POLLER_H__
#include "EasyNet/include/net/Poller.h"
#include "EasyNet/include/net/SoketDefine.h"
#include <functional> //std::greater
#include <map>
#include <set>

namespace Net	 
{
	class SelectPoller : public Poller
	{
	public:

		SelectPoller(EventLoop* loop);

		virtual void UpdateChannel(Channel* channel) ;

		virtual void RemoveChannel(Channel* channel) ;

		virtual void Poll(int timeoutMs, ChannelList* activeChannels) ;


	private:
		void FillActiveChannel(int activeEventsNum, ChannelList* channelList);

		void Update(Channel* channel);

	private:
		std::map<socket_t, Channel*> polledChannelsMap_;
		fd_set select_readfds_;
		fd_set select_writefds_;
		fd_set select_expectfds_;

		fd_set write_fds_;
		fd_set read_fds_;
		fd_set expect_fds_;

		std::set<socket_t, std::greater<socket_t> > fdSet_;

	};
}


#endif
