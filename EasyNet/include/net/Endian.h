#ifndef NETBASELIGHT_ENDIAN_H__
#define NETBASELIGHT_ENDIAN_H__
#include <stdint.h>
#include "EasyNet/include/net/SoketDefine.h"


namespace Net
{
	class Endian
	{
	public:
		//static uint64_t HostToNetwork64(uint64_t host64)
		//{
		//	return ::htonll(host64);	
		//}

		static uint32_t HostToNetwork32(uint32_t host32)
		{
			return htonl(host32);
		}

		static uint16_t HostToNetwork16(uint16_t host16)
		{
			return htons(host16);
		}

		//static uint64_t NetworkToHost64(uint64_t net64)
		//{
		//	return ::ntohll(net64);
		//}

		static uint32_t NetworkToHost32(uint32_t net32)
		{
			return ntohl(net32);
		}

		static uint16_t networkToHost16(uint16_t net16)
		{
			return ntohs(net16);
		}
	};
}


#endif
