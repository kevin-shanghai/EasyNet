#ifndef POLLER_FACTORY_H__
#define POLLER_FACTORY_H__

#include "EasyNet/include/base/Singleton.h"
#include "EasyNet/include/net/PollerFactory.h"
#include "EasyNet/include/net/SelectPoller.h"

namespace Net
{
	//This poller factory create the default poller base on different platforms//
	Poller* PollerFactory::CreateDefaultPoller(EventLoop* loop)
	{
        return new SelectPoller(loop);
	}
}


#endif