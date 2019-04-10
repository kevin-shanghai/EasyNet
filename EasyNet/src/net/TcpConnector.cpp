#include "EasyNet/include/net/TcpConnector.h"
#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/Channel.h"
#include "EasyNet/include/base/Log.h"
#include <assert.h>
#include <iostream>


using namespace Net;


Connector::Connector(EventLoop* loop, const InternetAddress& serverAddr)
	:loop_(loop),
	serverAddr_(serverAddr),
	connect_(false),
	state_(kDisconnected),
	retryDelayMs_(kInitRetryDelayMs)
{
}

Connector::~Connector()
{
	assert(!channel_);
}

void Connector::start()
{
	connect_ = true;
    Functor functor = Bind(&Connector::startInLoop, this);
	loop_->RunInLoop(functor);
}
void Connector::startInLoop()
{
	loop_->AssertInLoopThreadOrDie();
	assert(state_ == kDisconnected);
	if (connect_)
	{
		connect();
	}
}
void Connector::stop()
{
	connect_ = false;
    Functor functor = Bind(&Connector::stopInLoop, this);
	loop_->QueueInLoop(functor);
}
void Connector::stopInLoop()
{
	loop_->AssertInLoopThreadOrDie();
	if (state_ == kConnecting)
	{
		setState(kDisconnected);
		int sockfd = removeAndResetChannel();
		retry(sockfd);
	}
}


int GetLastErrorInfo()
{
#ifdef ON_WINDOWS
    return GetLastError();
#else
    return errno;
#endif
}

void Connector::connect()
{
	int sockfd = SocketsApi::CreateNonBlockingSocket();
	int ret = SocketsApi::Connect(sockfd, serverAddr_.GetInetAddress());
    int savedErrno = (ret == 0) ? 0 : GetLastErrorInfo();
	switch (savedErrno)
	{
	case 0:
	case EINPROGRESS:
	case EISCONN:
    case EINTR:
#ifdef ON_WINDOWS
    case WSAEINTR:
	case WSAEWOULDBLOCK:
#else
#endif
		connecting(sockfd);
		break;

	case EAGAIN:
	case EADDRINUSE:
	case EADDRNOTAVAIL:
	case ECONNREFUSED:
	case ENETUNREACH:
		retry(sockfd);
		break;

	case EACCES:
	case EPERM:
	case EAFNOSUPPORT:
	case EALREADY:
	case EBADF:
	case EFAULT:
	case ENOTSOCK:
		LOG_TRACE << "connect error in Connector::startInLoop " << savedErrno;
		SocketsApi::Close(sockfd);
		break;

	default:
		LOG_TRACE << "Unexpected error in Connector::startInLoop " << savedErrno;
		SocketsApi::Close(sockfd);
		// connectErrorCallback_();
		break;
	}
}

void Connector::restart()
{
	loop_->AssertInLoopThreadOrDie();
	setState(kDisconnected);
	retryDelayMs_ = kInitRetryDelayMs;
	connect_ = true;
	startInLoop();
}

void Connector::connecting(int sockfd)
{
	setState(kConnecting);
	assert(!channel_);
	channel_.reset(new Channel(loop_, sockfd));
	channel_->SetWriteCallback(std::bind(&Connector::handleWrite, this));
	channel_->SetErrorCallback(std::bind(&Connector::handleError, this));

	channel_->EnableWriting();
}

int Connector::removeAndResetChannel()
{
	channel_->DisableAll();
	channel_->Remove();
	int sockfd = channel_->GetSocketFd();
    Functor functor = Bind(&Connector::resetChannel, this);
	loop_->QueueInLoop(functor);
	return sockfd;
}

void Connector::resetChannel()
{
	channel_.reset();
}

void Connector::handleWrite()
{
	if (state_ == kConnecting)
	{
		int sockfd = removeAndResetChannel();
		int err = SocketsApi::GetSocketError(sockfd);
		if (err)
		{
			LOG_ERROR << "Connector::handleWrite - SO_ERROR = "
				<< err << " " << strerror(err);
			retry(sockfd);
		}
		else if (SocketsApi::IsSelfConnect(sockfd))
		{
			retry(sockfd);
		}
		else
		{
			setState(kConnected);
			if (connect_)
			{
				newConnectionCallback_(sockfd);
			}
			else
			{
				SocketsApi::Close(sockfd);
			}
		}
	}
	else
	{
		// what happened?
		assert(state_ == kDisconnected);
	}
}

void Connector::handleError()
{
	if (state_ == kConnecting)
	{
		int sockfd = removeAndResetChannel();
		int err = SocketsApi::GetSocketError(sockfd);
		LOG_ERROR << "SO_ERROR = " << err << " " << strerror(err);
		retry(sockfd);
	}
}
void Connector::retry(int sockfd)
{
	SocketsApi::Close(sockfd);
	setState(kDisconnected);
	if (connect_)
	{
		LOG_TRACE << "Connector::retry - Retry connecting to " << serverAddr_.GetIpAndPort();
        //reconnect server every 2 seconds//
        loop_->RunAfter_s(2, Bind(&Connector::startInLoop, this));
	}
}