#include "EasyNet/include/base/Condition.h"

using namespace base;

Condition::Condition(MutexLock& mutex) : mutex_(mutex)
{
#ifdef ON_WINDOWS
    InitializeConditionVariable(&cond_);
#else
    pthread_cond_init(&cond_, NULL);
#endif
}

Condition::~Condition()
{
#ifdef ON_WINDOWS
    //nothing
#else
    pthread_cond_destroy(&cond_);
#endif
}

void Condition::Wait()
{
#ifdef ON_WINDOWS
    (void)SleepConditionVariableCS(&cond_, mutex_.GetMutex(), INFINITE);
#else
    (void)pthread_cond_wait(&cond_, mutex_.GetMutex());
#endif
}

bool Condition::WaitFor(int millisecond)
{
    assert(millisecond >= 0);
#ifdef ON_WINDOWS
    bool bRet = SleepConditionVariableCS(&cond_, mutex_.GetMutex(), millisecond);
    return bRet == 0;
#else
    struct timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_sec += millisecond/1000;
    return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.GetMutex(), &abstime);
#endif
}

void Condition::NotifyOne()
{
#ifdef ON_WINDOWS
    WakeConditionVariable(&cond_);
#else
    pthread_cond_signal(&cond_);
#endif
}

void Condition::NotifyAll()
{
#ifdef ON_WINDOWS
    WakeAllConditionVariable(&cond_);
#else
    pthread_cond_broadcast(&cond_);
#endif
}
