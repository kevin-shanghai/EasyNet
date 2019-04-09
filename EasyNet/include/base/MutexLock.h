#ifndef NETBASELIGHT_MUTEX_H_
#define NETBASELIGHT_MUTEX_H_

#include "EasyNet/include/base/PlatformDefine.h"
#include "EasyNet/include/base/Noncopyable.h"


namespace base
{
    class MutexLock : public Noncopyable
    {
    public:
        explicit MutexLock(void);

        virtual ~MutexLock(void);

        void Lock(void);

        bool TryLock(void);

        void UnLock(void);

        MutexLockType* GetMutex();

    private:
        MutexLockType mutex_;
    };


    class LockGuard : private Noncopyable
    {
    public:
        LockGuard(MutexLock* mutex);

        virtual ~LockGuard();
    private:
        MutexLock* mutex_;
    };

}


#endif
