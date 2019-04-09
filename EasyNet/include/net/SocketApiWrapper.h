#ifndef NETBASELIGHT_SOCKET_API_WRAPPER_H__
#define NETBASELIGHT_SOCKET_API_WRAPPER_H__

#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
// sockets
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif // _MSC_VER
#ifndef _MSC_VER
#define IOV_TYPE struct iovec
#define IOV_PTR_FIELD iov_base
#define IOV_LEN_FIELD iov_len
#define IOV_LEN_TYPE size_t
#else
#define NUM_WRITE_IOVEC 16
#define IOV_TYPE WSABUF
#define IOV_PTR_FIELD buf
#define IOV_LEN_FIELD len
#define IOV_LEN_TYPE unsigned long
#endif

#include "EasyNet/include/net/SoketDefine.h"
#include "EasyNet/include/base/Noncopyable.h"
#include <stdint.h>


namespace Net
{
	class SocketsApi : private Noncopyable
	{
	public:
		static socket_t CreateNonBlockingSocket();

        static socket_t CreateBlockingSocket();

		static socket_t Connect(socket_t sockfd, const sockaddr_in_t& addr);

		static void BindAddress(socket_t sockfd, const sockaddr_in_t& addr);

		static void Listen(socket_t sockfd);

		static socket_t Accept(socket_t sockfd, struct sockaddr_in* addr);

		static int Receive(socket_t sockfd, void *buf, size_t count);

		static ssize_t readv(socket_t sockfd, IOV_TYPE *iov, int iovcnt);

		static int64_t Send(socket_t sockfd, const void *buf, size_t count);

		static void Close(socket_t sockfd);

		static void ShutdownWrite(socket_t sockfd);

		static void ToIpPort(char* buf, size_t size, const sockaddr_in& addr);

		static void ToIp(char* buf, size_t size, const struct sockaddr_in& addr);

		static void FromIpPort(const char* ip, uint16_t port, sockaddr_in_t* addr);

		static int GetSocketError(socket_t sockfd);

		static sockaddr_in_t GetLocalAddr(socket_t sockfd);

		static sockaddr_in_t GetPeerAddr(socket_t sockfd);

		static bool IsSelfConnect(socket_t sockfd);

        static bool SetBlocking(socket_t sockfd);

        static bool SetNonBlocking(socket_t sockfd);


	};
}




#endif