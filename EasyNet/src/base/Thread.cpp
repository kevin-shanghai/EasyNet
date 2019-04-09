#include "EasyNet/include/base/Thread.h"          
#include <iostream>
#ifdef ON_WINDOWS
#include <process.h>
#endif


using namespace base;

#if defined(ON_WINDOWS)
static unsigned WINAPI RunInThread(void *arg)
#else
static void* RunInThread(void *arg)
#endif
{
	Thread* pThread = static_cast<Thread*>(arg);
	pThread->Run();
	return 0;
}


Thread::Thread(const std::string& threadName /* = "UnKnow" */)
: threadName_(std::string(threadName))
{

}


void Thread::Start()
{
#if defined(ON_WINDOWS)
	_beginthreadex(0, 0, &RunInThread, this, 0, &threadID_);
#elif defined ON_LINUX
	if (pthread_create(&threadID_, NULL, &RunInThread, this) != 0);
#else
    threadID_ = taskSpawn(
        (char*)(threadName_.c_str()),          // task name
        static_cast<int>(70),
        VX_FP_TASK,
        static_cast<size_t>(2*1024*1024),
        reinterpret_cast<FUNCPTR>(RunInThread),
        reinterpret_cast<_Vx_usr_arg_t>(this),
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0);
#endif
    std::cout << "thread name " << threadName_
        << " tid: " << threadID_ << std::endl; 
}
