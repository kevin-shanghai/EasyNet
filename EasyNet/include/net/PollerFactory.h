#include "Poller.h"


namespace Net
{
	class EventLoop;
	class PollerFactory
	{
	public:
		static Poller* CreateDefaultPoller(EventLoop* loop);

	};
}