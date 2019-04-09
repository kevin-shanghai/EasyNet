#ifndef NETBASELIGHT_INTERNET_ADDRESS_H__
#define NETBASELIGHT_INTERNET_ADDRESS_H__


#include "EasyNet/include/net/SocketApiWrapper.h"
#include "EasyNet/include/base/PlatformDefine.h"
#include <string>

#ifdef EXPORT_NETBASElIGHT
#define CSW_EXPORT_OWNER
#endif


namespace Net
{
	class EXPORT_NETBASElIGHT InternetAddress
	{
	public:
		explicit InternetAddress(port_t port = 0);
		InternetAddress(const std::string& ip, uint16_t port);
		InternetAddress(sockaddr_in_t& addr);

		std::string GetIp() const;
		std::string GetIpAndPort() const;
		sockaddr_in_t GetInetAddress() const { return socketAddrIn_; }

		void SetSockAddrInet(const sockaddr_in_t& addr) { socketAddrIn_ = addr; }

	private:
		sockaddr_in_t socketAddrIn_;
	};
}





#endif
