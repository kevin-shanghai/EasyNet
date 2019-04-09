#ifndef TIMER_H__
#define TIMER_H__
#include "EasyNet/include/base/Noncopyable.h"
#include "EasyNet/include/net/ICallBackDefine.h"
#include "EasyNet/include/base/TimeStamp.h"
//#include "Common/CommonUtility/csw_control_logger.h"
#include "EasyNet/include/base/MutexLock.h"
#include "EasyNet/include/base/MessageQueue.h"


namespace Net
{

    class Timer : private Noncopyable
    {
    public:
        Timer(const Functor& cb, Timestamp when)
            : cb_(cb)
            , expiredTimeStamp_(when)
            , index_(0)
        {

        }

        void Run()
        {
            //CLOG_INFO( "timer run at" << " time=" << Timestamp::Now().ToCalenderTime()
            //    << " index=" << index_);
            if (cb_) cb_();
        }

        void UpdateIndex(int64_t index)
        {
            index_ = index;
        }

        int64_t GetIndex() const { return index_; }

        void Reset()     
        {
            expiredTimeStamp_ = Timestamp(-1);
            
        }

        Timestamp GetExpiredTimeStamp() const 
        {
            return expiredTimeStamp_;
        }

        int64_t ExpiredPoint() const { return expiredTimeStamp_.GetMicroSecondsSineEpoch(); }

        bool operator<(const Timer& rhs)
        {
            return this->GetExpiredTimeStamp() < rhs.GetExpiredTimeStamp();
        }

        bool operator ==(const Timer& rhs)
        {
            return this->GetExpiredTimeStamp() == rhs.GetExpiredTimeStamp();
        }

        bool operator <=(const Timer& rhs)
        {
            return this->GetExpiredTimeStamp() <= rhs.GetExpiredTimeStamp();
        }

        bool operator>(const Timer& rhs)
        {
            return this->GetExpiredTimeStamp() > rhs.GetExpiredTimeStamp();
        }



    private:
        const Functor cb_;
        Timestamp expiredTimeStamp_;
        int64_t index_;
    };


    class MinHeap
    {

    public:
        MinHeap(int n=2000)
        {
            MaxSize=n;
            memset(heap, 0, sizeof(Timer*) * MaxSize);
            CurrentSize=0;
        }

        ~MinHeap()
        {

        }

        bool Insert(Timer* x) 
        {
            base::LockGuard lk(&mutex_);
            
            if(IsFull())
            {
                //CLOG_ERROR("timer container is full.");
                return false;
            }
            heap[CurrentSize] = x;
            FilterUp(CurrentSize);
            CurrentSize++;
            static int64_t index = 0;
            index++;
            x->UpdateIndex(index);
            //CLOG_INFO("Insert, timer container size=" << CurrentSize
            //    << " event index=" << index);
            return true;
        }

        Timer* RemoveMin() 
        {
            base::LockGuard lk(&mutex_);
            Timer* x=heap[0];
            heap[0]=heap[CurrentSize-1];
            CurrentSize--;
            FilterDown(0,CurrentSize-1); 
            //CLOG_INFO("RemoveMin, timer container size=" << CurrentSize
            //    << " event index=" << x->GetIndex()
            //    << "expired time stamp=" << x->GetExpiredTimeStamp().GetMicroSecondsSineEpoch());
            return x;
        }

        Timer* GetMin() 
        {
            base::LockGuard lk(&mutex_);
            return heap[0];
        }

        Timer* Root() 
        {
            base::LockGuard lk(&mutex_);
            return heap[0];
        }

        bool IsEmpty() const
        {
            return CurrentSize==0;
        }

        bool IsFull() const
        {
            return CurrentSize == MaxSize;
        }

        void Clear()
        {
            CurrentSize = 0;
        }

    private:

        void FilterDown(const int start,const int end) 
        {
            int i=start,j=2*i+1;
            Timer* temp=heap[i];
            while(j<=end)
            {
                if( (j<end) && (*heap[j]>*heap[j+1]) )
                    j++;
                if(*temp<*heap[j])
                    break;
                else
                {
                    heap[i]=heap[j];
                    i=j;
                    j=2*j+1;
                }
            }
            heap[i]=temp;
        }

        int GetParent(int current)
        {
            return (current - 1) / 2;
        }

        void FilterUp(int start) 
        {
            int j=start,i=(j-1)/2; 
            Timer* temp=heap[j];
            while(j>0)
            {
                if(*heap[i]<*temp || *heap[i] == *temp)   
                    break;
                else
                {
                    heap[j]=heap[i]; 
                    j=i;
                    i=(i-1)/2;
                }

            }
            heap[j]=temp;
        }

    private:
        Timer* heap[2000]; 
        int CurrentSize; 
        int MaxSize; 
        base::MutexLock mutex_;
    };

   

    class TimerManager
    {
     public:
        virtual void AddTimer(Timer* timer) = 0;

        virtual Timer* Front() = 0;

        virtual Timer* Pop() = 0;

        virtual int64_t GetEarliestTimer() = 0;

        virtual bool IsEmpty() { return true; }

        void CancelAll()
        {
            while(!IsEmpty())
            {
                Timer* timer = Pop();
                delete timer;
            }

        }

         void Process()
        {
            while(1)
            {
                if (IsEmpty())
                {
                    break;
                }

                static int64_t previousEventIndex = 0;
                

                Timer* timer = Front();
                const Timestamp& now = Timestamp::Now();
                Timestamp expiredTime = timer->GetExpiredTimeStamp();
                if (expiredTime < now)
                {
                    Scoped_ptr<Timer> timerPtr(timer);
                    Pop();
                    timer->Run();
                    previousEventIndex = timer->GetIndex();
                    continue;
                }
                
                break;
            }
        }
        
    };

    class TimerManagerFIFO : public TimerManager
    {
     public:
        virtual void AddTimer(Timer* timer)
        {
            static int64_t index = 0;
            index++;
            timer->UpdateIndex(index);
            asyncQueue_.Put(timer);
            //CLOG_INFO("Insert, timer container size=" << asyncQueue_.Size()
            //    << " event index=" << index
            //    << " expireTimeStamp=" << timer->ExpiredPoint());
        }

        virtual Timer* Front() 
        {
            if (!asyncQueue_.Empty())
            {
                return asyncQueue_.Front();
            }
            return nullptr;
        }

        virtual Timer* Pop()
        {
            assert (!asyncQueue_.Empty());
            Timer* timer = asyncQueue_.Pop();
            //CLOG_INFO("Pop timer index=" << timer->GetIndex());
            return timer;
        }

        virtual bool IsEmpty()
        {
            return asyncQueue_.Empty();
        }

        virtual int64_t GetEarliestTimer()
        {
            assert (!asyncQueue_.Empty());
            const Timestamp& earliestTimer = asyncQueue_.Front()->GetExpiredTimeStamp();
            return Timestamp::DiffInMilliSedonds(earliestTimer, Timestamp::Now());
        }

     private:

       base::MessageQueue<Timer*> asyncQueue_;

    };



    class TimerManagerMinHeap : public TimerManager 
    {  
    public: 
        TimerManagerMinHeap()
        {

        }

        virtual void AddTimer(Timer* timer)
        {
            minHeap_.Insert(timer);
        }

         virtual Timer* Front() 
        {
            if (!minHeap_.IsEmpty())
            {
                return minHeap_.Root();
            }
            return nullptr;
        }

        virtual Timer* Pop()
        {
            assert (!minHeap_.IsEmpty());
            return minHeap_.RemoveMin();
        }

        MinHeap& GetTimerContainer() { return minHeap_; }



        virtual bool IsEmpty() 
        {
            return minHeap_.IsEmpty();
        }

        virtual int64_t GetEarliestTimer()
        {
            assert (!IsEmpty());
            const Timestamp& earliestTimer = minHeap_.Root()->GetExpiredTimeStamp();
            return Timestamp::DiffInMilliSedonds(earliestTimer, Timestamp::Now());
        }

       

    private:
        MinHeap minHeap_;
    };
}



#endif
