#ifndef NETBASE_LIGHT_SOCKET_DEFINE_H__
#define NETBASE_LIGHT_SOCKET_DEFINE_H__


#ifdef _MSC_VER
#define WINDOWS
#elif defined __linux__
#define LINUX

#else
#define VXWORKS
#endif

#ifdef WINDOWS
#include <ws2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif

#ifdef LINUX
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#endif

#ifdef VXWORKS
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <types/vxTypesOld.h>

#include <vxWorks.h>
#include <sysLib.h>
#include <taskLib.h>
#include "sockLib.h"  
#include "inetLib.h"	
#include "hostLib.h"	
#include <selectLib.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#endif


#ifdef WINDOWS
typedef SOCKET socket_t;
typedef SOCKADDR_IN sockaddr_in_t;
typedef SOCKADDR sockaddr_t;
typedef IN_ADDR in_addr_tt;
typedef unsigned short port_t;
typedef fd_set fd_set_t;
#define INVALID_SOCKET_FD -1


#endif

#ifdef LINUX
typedef int socket_t;
typedef sockaddr_in sockaddr_in_t;
typedef sockaddr sockaddr_t;
typedef in_addr in_addr_tt;
typedef unsigned short port_t;
typedef fd_set fd_set_t;
#endif


#ifdef VXWORKS
typedef int socket_t;
typedef sockaddr_in sockaddr_in_t;
typedef sockaddr sockaddr_t;
typedef in_addr in_addr_tt;
typedef unsigned short port_t;
typedef fd_set fd_set_t;
#endif




#endif
