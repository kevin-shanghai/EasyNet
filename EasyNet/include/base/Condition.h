#ifndef NETBASELIGHT_CONDITION_H__
#define NETBASELIGHT_CONDITION_H__

#include "EasyNet/include/base/MutexLock.h"
#include <assert.h>


namespace base
{
    class Condition 
    {
    public:
        explicit Condition(MutexLock& mu);

        ~Condition();

    public:
        void Wait();

        bool WaitFor(int millisecond);

        void NotifyOne();

        void NotifyAll();

    private:
        MutexLock&    mutex_;
        ConditionType cond_;

    private:
        bool operator =(const Condition& rhs);
    };
}


#endif
