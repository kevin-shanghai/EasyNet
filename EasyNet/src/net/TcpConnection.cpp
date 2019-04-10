#include "EasyNet/include/net/TcpConnection.h"
#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/base/Log.h"
#include <iostream>


using namespace base;

namespace Net
{

    void defaultConnectionCallback(const TcpConnectionPtr& conn)
    {
        LOG_TRACE << conn->GetLocalAddress().GetIpAndPort() << " -> "
            << conn->GetPeerAddress().GetIpAndPort() << " is "
            << (conn->IsConnected() ? "UP" : "DOWN");	
    }

    void defaultMessageCallback(const TcpConnectionPtr& conn, ChannelBuffer* buffer)
    {
        conn;
        buffer->ReadAllAsString();
    }

	TcpConnection::TcpConnection(EventLoop* loop,
		const std::string& name,
		int sockfd,
		const InternetAddress& localAddr,
		const InternetAddress& peerAddr)
		: loop_(loop)
		, connectionState_(kConnecting)
		, rawName_(name)
		, socket_(new Socket(sockfd))
		, localAddress_(localAddr)
		, peerAddress_(peerAddr)
		, connectionChannelPtr_ (new Channel(loop, sockfd))
        , mutex_(new MutexLock())
	{
        connectionChannelPtr_->SetReadCallback(Bind(&TcpConnection::HandleRead, this));
		connectionChannelPtr_->SetWriteCallback(Bind(&TcpConnection::HandleWrite, this));
		connectionChannelPtr_->SetErrorCallback(Bind(&TcpConnection::HandleError, this));
		connectionChannelPtr_->SetCloseCallback(Bind(&TcpConnection::HandleClose, this));
        socket_->SetKeepAlive(true);
	}

	TcpConnection::~TcpConnection()
	{
		std::cout << "TcpConnection dtor..." << std::endl;   
	}


	void TcpConnection::Send(const StringPiece& message)
	{
		if (connectionState_ == kConnected)
		{
			SendStringPieceInLoop(message);
		}
	}

	void TcpConnection::Send(ChannelBuffer& message)
	{
		if (connectionState_ == kConnected)
		{
            int64_t iReadableBytes = message.ReadableBytes();
            SendDataInLoop(message.GetReadPointer(), iReadableBytes);
		}
	}


	void TcpConnection::SendStringPieceInLoop(const StringPiece& message)
	{
		SendDataInLoop(message.data(), message.size());
	}

	void TcpConnection::SendDataInLoop(const void* data, size_t len)
	{                                                                                                                                  
		int64_t iWriteBytes = 0;
		int64_t remaining = len;
		bool faultError = false;
		if (connectionState_ == kDisconnected)
		{
			LOG_INFO << "disconnected, give up writing";
			return;
		}
		// if nothing in output queue, try writing directly
		if (!connectionChannelPtr_->IsWriting() && outputBuf_.ReadableBytes() == 0)
		{
			int64_t iWriteBytes = SocketsApi::Send(connectionChannelPtr_->GetSocketFd(), data, len);
			if (iWriteBytes >= 0)
			{
				remaining = len - iWriteBytes;
			}
			else 
			{
				iWriteBytes = 0;
				if (errno != EWOULDBLOCK)
				{
					if (errno == EPIPE || errno == ECONNRESET) 
					{
						faultError = true;
					}
				}
			}
		}
		assert(size_t(remaining) <= len);
		if (!faultError && remaining > 0)
		{
			outputBuf_.Append(static_cast<const char*>(data) + iWriteBytes, remaining);
            LOG_INFO <<"outbuffer size:" << outputBuf_.Size()
                << " remaining:" << remaining;
			if (!connectionChannelPtr_->IsWriting())
			{
                connectionChannelPtr_->EnableWriting();
			}
		}
	}


	void TcpConnection::ShutDownWriteForHalfClose()
	{
		loop_->AssertInLoopThreadOrDie();
		// shutDown write for half close, only can read at this time.
		if (!connectionChannelPtr_->IsWriting())
		{
			socket_->ShutdownWrite();
		}
	}


	void TcpConnection::HandleRead()
	{
        loop_->AssertInLoopThreadOrDie();
        int saveErrno = 0;
        //int64_t n = inputBuf_.ReadFromSocketFD(connectionChannelPtr_->GetSocketFd(), saveErrno);
		int64_t n = inputBuf_.readFd(connectionChannelPtr_->GetSocketFd(), &saveErrno);
        if (n > 0)
        {
            messageCallback_(shared_from_this(), &inputBuf_);
        }
        else if(n == 0) 
        {
            HandleClose();
        }
        else
        {
            errno = saveErrno;
            LOG_ERROR << "TcpConnection::handleRead Error...";
            HandleError();
        }
	}



	void TcpConnection::HandleWrite()
	{
        
		if (connectionChannelPtr_->IsWriting())
		{
			int n = SocketsApi::Send(connectionChannelPtr_->GetSocketFd(), 
									outputBuf_.GetReadPointer(), 
									outputBuf_.ReadableBytes());
			if (n > 0)
			{
				outputBuf_.Pop(n);
				if (outputBuf_.ReadableBytes() == 0)
				{
					connectionChannelPtr_->DisableWriting();
					if (connectionState_ == kDisconnecting)
					{
						ShutDownWriteForHalfClose();
					}
				}
			}
			else if (n == 0)
			{
				//write nothing
			}
			else
			{
				//TODO, error handler
			}
		}
		else
		{
			LOG_INFO << "Connection fd = " << connectionChannelPtr_->GetSocketFd()
				<< " is down, can not write any more...";
		}
		
	}

	void TcpConnection::HandleClose()
	{
        loop_->AssertInLoopThreadOrDie();
        std::cout << "fd = " << connectionChannelPtr_->GetSocketFd() << " state = " << StateToString();
        assert(connectionState_ == kConnected || connectionState_ == kDisconnecting);
        SetState(kDisconnected);
        connectionChannelPtr_->DisableAll();

        TcpConnectionPtr guardThis(shared_from_this());
        connectionCallback_(guardThis);
        // must be the last line
        closeCallback_(guardThis);
	}


	void TcpConnection::HandleError()
	{
		int err = SocketsApi::GetSocketError(connectionChannelPtr_->GetSocketFd());
		//errno = GetLastError();
		LOG_ERROR << "TcpConnection::handleError [" << name_ << "] -SO_ERROR = " << err << " " << strerror(err);

	}

	void TcpConnection::ConnectionEstablished()
	{
		assert (connectionState_ == kConnecting);
		SetState(kConnected);
        connectionChannelPtr_->EnableReading();
		connectionCallback_(shared_from_this());
	}

	void TcpConnection::ConnectionDestroyed()
	{
		loop_->AssertInLoopThreadOrDie();
		if (connectionState_ == kConnected)
		{
			SetState(kDisconnected);
			connectionChannelPtr_->DisableAll();
			connectionCallback_(shared_from_this());
		}
		connectionChannelPtr_->Remove();

	}

    const char* TcpConnection::StateToString()const
    {
        switch (connectionState_)
        {
        case kDisconnected:
            return "kDisconnected";
        case kConnecting:
            return "kConnecting";
        case kConnected:
            return "kConnected";
        case kDisconnecting:
            return "kDisconnecting";
        default:
            return "unknown state";
        }
    }

    void TcpConnection::Shutdown()
    {
        if (connectionState_ == kConnected)
        {
            SetState(kDisconnecting);
            Functor functor = Bind(&TcpConnection::ShutdownInLoop, this);
            loop_->RunInLoop(functor);
        }
    }

    void TcpConnection::ShutdownInLoop()
    {
        loop_->AssertInLoopThreadOrDie();
        if (!connectionChannelPtr_->IsWriting())
        {
            // we are not writing
            socket_->ShutdownWrite();
        }
    }


}
