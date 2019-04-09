#ifndef CPP11SUPPORT_H__
#define CPP11SUPPORT_H__


#ifdef _MSC_VER
#pragma warning (disable: 4005)
#define WIN32_LEAN_AND_MEAN
#pragma warning (default: 4005)
#include <Windows.h>
#include <io.h>
#define ON_WINDOWS
#define	snprintf	sprintf_s
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#pragma warning(disable: 4800)
#pragma warning(disable: 4244)
#pragma warning(disable: 4005)
#pragma warning(disable: 4267)
#pragma warning(disable: 4512)
#pragma warning(disable: 4355)
#pragma warning(disable: 4125)
#pragma warning(disable: 4127)
#pragma warning(disable: 4267)
#pragma warning(disable: 4275)

#elif defined __linux__
#define ON_LINUX
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/time.h>

#else
#define ON_VXWORKS
#include <pthread.h>
#include <taskLibCommon.h>
#include <sysLib.h>
#include <vxWorks.h>
#include <sysLib.h>
#include <taskLib.h>
#include <stdint.h>
#include <tickLib.h>
#endif

#ifdef ON_WINDOWS
#define EXPORT_NETBASElIGHT __declspec (dllexport)
#else
#define EXPORT_NETBASElIGHT
#endif


#ifdef ON_WINDOWS
typedef CRITICAL_SECTION MutexLockType;
typedef CONDITION_VARIABLE  ConditionType;

#else
typedef pthread_mutex_t MutexLockType;
typedef pthread_cond_t ConditionType;
#endif

#ifdef ON_WINDOWS
#define TLS __declspec(thread)
#else
#define TLS __thread
#endif

#if defined(ON_WINDOWS)
typedef unsigned int ThreadIdType;
#else
#include <unistd.h>
#include <errno.h>
typedef pthread_t ThreadIdType;
#endif




//cpp11 support
#ifdef _MSC_VER
//VS2013(1800)
#if _MSC_VER < 1800
#include <boost/functional.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

#define Function boost::function
#define Thread11   boost::thread
#define Shared_ptr boost::shared_ptr
#define Scoped_ptr boost::scoped_ptr
#define Bind boost::bind
#define enable_shared_from_this boost::enable_shared_from_this
#define Static_Pointer_Cast boost::static_pointer_cast
#define Placeholders


#else

#include <functional>
#include <thread>

#define Function std::function
#define Thread std::thread
#define Shared_ptr std::shared_ptr
#define Scoped_ptr std::unique_ptr
#define Bind std::bind
#define enable_shared_from_this std::enable_shared_from_this
#define Placeholders std::placeholders::
#define Static_Pointer_Cast std::static_pointer_cast
#endif

#elif __cplusplus > 199711L
#include <functional>
#include <thread>

#define Function std::function
#define Thread11 std::thread
#define Shared_ptr std::shared_ptr
#define Scoped_ptr std::unique_ptr
#define Bind std::bind
#define enable_shared_from_this std::enable_shared_from_this
#define Placeholders std::placeholders::
#define Static_Pointer_Cast std::static_pointer_cast

#else
#include <boost/functional.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#define Function boost::function
#define Thread   boost::thread
#define Shared_ptr boost::shared_ptr
#define Scoped_ptr boost::scoped_ptr
#define Bind boost::bind
#define enable_shared_from_this boost::enable_shared_from_this
#define Placeholders  
#define Static_Pointer_Cast boost::static_pointer_cast

#endif



#endif