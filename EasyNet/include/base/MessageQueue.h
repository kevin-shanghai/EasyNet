#ifndef ASYNC_QUEUE_H__
#define ASYNC_QUEUE_H__
#include <deque>
#include <iostream>
#include "EasyNet/include/base/Condition.h"
#include "EasyNet/include/base/MutexLock.h"

namespace base
{
    template <class T, uint64_t QUEUE_SIZE = 10000>
    class MessageQueue
    {
    public:
        MessageQueue() 
            : mutex_()
            , notEmptyCond_(mutex_)
            , notFullCond_(mutex_)
            , queue_()
        {

        }

        void Put(const T& value)
        {
            LockGuard lk(&mutex_);
            if (Full())
            {
                notFullCond_.Wait();
            }
            queue_.push_back(value);
            notEmptyCond_.NotifyOne();
        }

        T Take()
        {
            LockGuard lk(&mutex_);
            while (Empty())
            {
                notEmptyCond_.Wait();
            }
            T frontValue = queue_.front(); 
            queue_.pop_front();
            notFullCond_.NotifyOne();
            return frontValue;
        }

        uint64_t Size() const
        {
            return queue_.size();
        }

        void Clear() 
        { 
            LockGuard lk(&mutex_);
            queue_.clear(); 
        }

        T Front() 
        {
            return queue_.front();
        }

        bool Empty()
        {
            return queue_.empty();
        }

        T Pop()
        {
            T temp = Front();
            queue_.pop_front();
            return temp;
        }



    private:

        bool Full()	const
        {
            //dangerous with lock, avoid dead lock here
            return (queue_.size() >= QUEUE_SIZE);
        }


    private:
        mutable MutexLock mutex_;
        Condition notEmptyCond_;
        Condition notFullCond_;
        std::deque<T> queue_;
    };

}


#endif
