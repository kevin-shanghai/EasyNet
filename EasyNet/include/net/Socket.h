#ifndef NETBASELIGHT_SOCKET_H__
#define NETBASELIGHT_SOCKET_H__

#include "EasyNet/include/net/SocketApiWrapper.h"
#include "EasyNet/include/net/InternetAddress.h"
#include "EasyNet/include/base/Noncopyable.h"

namespace Net
{
	class Socket 
	{
	public:
		explicit Socket(socket_t sockfd);

        virtual ~Socket();

		void BindAddress(const InternetAddress& localAddress);
		void Listen();
		socket_t Accept(InternetAddress& peerAddress);

		socket_t GetFd() const { return socketfd_; };

		void ShutdownWrite() { SocketsApi::ShutdownWrite(socketfd_); };


		void SetTcpNoDelay(bool on);

		void SetReuseAddr(bool on);

		void SetReusePort(bool on);

		void SetKeepAlive(bool on);


	private:
		socket_t socketfd_;
	};
}





#endif


