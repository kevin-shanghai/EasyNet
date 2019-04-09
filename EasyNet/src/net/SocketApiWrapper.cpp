#include "EasyNet/include/net/SocketApiWrapper.h"
#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>




namespace Net
{

    socket_t SocketsApi::CreateBlockingSocket()
    {
#ifdef WINDOWS
        WORD wsaword;
        WSADATA wsadata;
        wsaword = MAKEWORD(2,2);
        uint64_t iError = ::WSAStartup(wsaword, &wsadata);
        if (iError != NOERROR)
        {
            return INVALID_SOCKET;
        }
        if ((2 != LOBYTE(wsadata.wVersion)) || (2 != LOBYTE(wsadata.wHighVersion)))
        {
            ::WSACleanup();
            return INVALID_SOCKET;
        }
        socket_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == INVALID_SOCKET)
        {
            ::WSACleanup();
            return INVALID_SOCKET;
        }

#else
        socket_t sockfd = ::socket(AF_INET, SOCK_STREAM , 0);

#endif
        if (sockfd <= 0)
        {
            //CLOG_ERROR("sockets::CreateBlockingSocket");
        }
        std::cout << "create new socket fd is:" << sockfd << std::endl;
        return sockfd;
    }


	socket_t SocketsApi::CreateNonBlockingSocket()
	{
#ifdef WINDOWS
		WORD wsaword;
		WSADATA wsadata;
		wsaword = MAKEWORD(2,2);
		uint64_t iError = ::WSAStartup(wsaword, &wsadata);
		if (iError != NOERROR)
		{
			return INVALID_SOCKET;
		}
		if ((2 != LOBYTE(wsadata.wVersion)) || (2 != LOBYTE(wsadata.wHighVersion)))
		{
			::WSACleanup();
			return INVALID_SOCKET;
		}
		socket_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == INVALID_SOCKET)
		{
			::WSACleanup();
			return INVALID_SOCKET;
		}

		unsigned long ul = 1;
	    ioctlsocket(sockfd, FIONBIO, (unsigned long *)&ul);

#else
	    socket_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
        fcntl(sockfd,F_SETFL,O_NONBLOCK);
		
#endif
        if (sockfd <= 0)
        {
            //CLOG_ERROR("sockets::createNonblockingOrDie");
        }
        std::cout << "create new socket fd is:" << sockfd << std::endl;
        return sockfd;
	}

	socket_t SocketsApi::Connect(socket_t sockfd, const sockaddr_in_t& addr)
	{
		return ::connect(sockfd, (sockaddr_t*)&addr, static_cast<socklen_t>(sizeof addr));
	}

	void SocketsApi::BindAddress(socket_t sockfd, const sockaddr_in_t& addr)
	{
		int ret = ::bind(sockfd, (sockaddr_t*)(&addr), static_cast<socklen_t>(sizeof addr));
        assert (!ret);
		if (ret < 0)
		{
			//CLOG_ERROR( "SocketsApi::bind error---" );
		}
	}


	void SocketsApi::Listen(socket_t sockfd)
	{
		int listenRet = ::listen(sockfd, SOMAXCONN);
		if (listenRet < 0)
		{
			//LOG_SYSFATAL << "sockets::listenOrDie";
		}
	}

	socket_t SocketsApi::Accept(socket_t listenSockfd, sockaddr_in_t* addr)
	{
		socklen_t addrLen = static_cast<socklen_t>(sizeof *addr);
#ifdef WINDOWS
		socket_t connSockFd = ::accept(listenSockfd, (sockaddr_t*)addr, &addrLen);
		if (connSockFd >= 0)
		{
			unsigned long ul = 1;
			ioctlsocket(connSockFd, FIONBIO, (unsigned long *)&ul);
		}
        return connSockFd;
#else
        socket_t connSockFd = ::accept(listenSockfd, (sockaddr_t*)(addr), &addrLen);
		int flags = fcntl(connSockFd, F_GETFL, 0);
		fcntl(connSockFd, F_SETFL, flags | O_NONBLOCK);
		return connSockFd;
#endif
	}

	int SocketsApi::Receive(socket_t sockfd, void *buf, size_t count)
	{
		return ::recv(sockfd, static_cast<char*>(buf), int(count), 0);
	}

	//ssize_t readv(socket_t sockfd, const struct iovec *iov, int iovcnt)
	ssize_t SocketsApi::readv(socket_t sockfd, IOV_TYPE *iov, int iovcnt)
	{
#ifdef _MSC_VER
	//WSABUF vecs[4];
	DWORD bytesRead;
	DWORD flags = 0;
	if (WSARecv(sockfd, iov, iovcnt, &bytesRead, &flags, NULL, NULL)) {
		/* The read failed. It might be a close,
		* or it might be an error. */
		if (GetLastError() == WSAECONNABORTED)
			return  0;
		else
			return -1;
	}
	else
		return bytesRead;
#else
	return ::readv(sockfd, iov, iovcnt);
#endif
	}

	int64_t SocketsApi::Send(socket_t sockfd, const void *buf, size_t count)
	{
		return ::send(sockfd, static_cast<const char*>(buf), int(count), 0);
	}

	void SocketsApi::Close(socket_t sockfd)
	{
#ifdef WINDOWS
		if (::closesocket(sockfd) < 0)
		{
			//LOG_SYSERR << "sockets::close";
		}
#else
		if (::close(sockfd) < 0)
		{
			//LOG_SYSERR << "sockets::close";
		}
#endif
	}

	void SocketsApi::ShutdownWrite(socket_t sockfd)
	{
#ifdef WINDOWS
		if (::shutdown(sockfd, SD_SEND) < 0)
		{
			//LOG_SYSERR << "sockets::shutdownWrite";
		}
#elif defined LINUX
		if (::shutdown(sockfd, SHUT_WR) < 0)
		{
			//LOG_SYSERR << "sockets::shutdownWrite";
		}
#endif	
	}

	void SocketsApi::ToIpPort(char* buf, size_t size, const sockaddr_in_t& addr)
	{
#ifdef WINDOWS
#define snprintf sprintf_s
#endif
		assert(size >= INET_ADDRSTRLEN);
		::inet_ntop(AF_INET, (void*)&addr.sin_addr, buf, static_cast<socklen_t>(size));   
		size_t end = ::strlen(buf);

		//uint16_t port = sockets::networkToHost16(addr.sin_port);
		uint16_t port = ntohs(addr.sin_port);
		//uint16_t port = ::be16toh(addr.sin_port);
		assert(size > end);
		snprintf(buf + end, size - end, ":%u", port);
	}

	void SocketsApi::ToIp(char* buf, size_t size, const struct sockaddr_in& addr)
	{
		assert(size >= INET_ADDRSTRLEN);
		::inet_ntop(AF_INET, (void*)&addr.sin_addr, buf, static_cast<socklen_t>(size));
	}

	void SocketsApi::FromIpPort(const char* ip, uint16_t port, sockaddr_in_t* addr)
	{
        addr->sin_family = AF_INET;
        //addr->sin_port = hostToNetwork16(port);
        addr->sin_port = htons(port);
        if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
        {
            std::cout  << " Error, sockets::fromIpPort" << std::endl;
            exit(-1);
        }
        //return Success;
	}

	//int GetSocketError(socket_t sockfd);

	sockaddr_in_t SocketsApi::GetLocalAddr(socket_t sockfd)
	{
		sockaddr_in_t localaddr;
		memset(&localaddr, 0, sizeof(localaddr));
		socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
		if (::getsockname(sockfd, (sockaddr_t*)(&localaddr), &addrlen) < 0)
		{
			//LOG_SYSERR << "sockets::getLocalAddr";
		}
		return localaddr;
	}

	sockaddr_in_t SocketsApi::GetPeerAddr(socket_t sockfd)
	{
		sockaddr_in_t peeraddr;
		memset(&peeraddr, 0, sizeof(peeraddr));
		socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
		if (::getpeername(sockfd, (sockaddr_t*)(&peeraddr), &addrlen) < 0)
		{
			//LOG_SYSERR << "sockets::getPeerAddr";
		}
		return peeraddr;
	}

	bool SocketsApi::IsSelfConnect(socket_t sockfd)
	{
	    sockaddr_in_t localaddr = GetLocalAddr(sockfd);
		sockaddr_in_t peeraddr = GetPeerAddr(sockfd);
		return localaddr.sin_port == peeraddr.sin_port
			&& localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr;
	}

	int SocketsApi::GetSocketError(socket_t sockfd)
	{
		char optval;
		socklen_t optlen = static_cast<socklen_t>(sizeof optval);

		if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
		{
			return errno;
		}
		else
		{
#ifdef ON_WINDOWS
            int err = WSAGetLastError();
            std::cout << "errcode is:" << err << std::endl;
#endif
			return optval;
		}
	}

    bool SocketsApi::SetBlocking(socket_t sockfd)
    {
        bool bRet = false;
#ifdef WINDOWS
        unsigned long ul = 0;
        bRet = !!ioctlsocket(sockfd, FIONBIO, (unsigned long *)&ul);
#else
        bRet = fcntl(sockfd, F_SETFL, O_NONBLOCK);
#endif
        if (bRet)
        {
            std::cout << "SetBlocking failed..." << std::endl;
        }
        return !bRet;
    }

    bool SocketsApi::SetNonBlocking(socket_t sockfd)
    {
#ifdef WINDOWS
       unsigned long ul = 1;
       ioctlsocket(sockfd, FIONBIO, (unsigned long *)&ul);
#else
       fcntl(sockfd,F_SETFL,O_NONBLOCK);
#endif
       return true;
    }


}
