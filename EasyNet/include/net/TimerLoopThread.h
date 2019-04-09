#ifndef TIMER_LOOP_THREAD_H__
#define TIMER_LOOP_THREAD_H__
#include "EasyNet/include/net/EventLoop.h"


#ifdef EXPORT_NETBASElIGHT
#define CSW_EXPORT_OWNER
#endif



namespace Net
{
    class EventLoopThread;
    class TcpServer;

    class EXPORT_NETBASElIGHT TimerLoopThread 
    {
    public:
        TimerLoopThread();

        static TimerLoopThread* Instance();

        void Start();

        EventLoop* GetTimerLoop() const;


    private:
        EventLoopThread* loopThread_;
        EventLoop* loop_;
        TcpServer* server_;
        
    };
}

#endif