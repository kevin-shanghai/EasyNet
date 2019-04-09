#include "EasyNet/include/net/TimerLoopThread.h"
#include "EasyNet/include/net/EventLoopThread.h"
#include "EasyNet/include/net/TcpServer.h"



namespace Net
{
    
    TimerLoopThread::TimerLoopThread() : loopThread_(new EventLoopThread())
                                       , loop_(new EventLoop())
    {

    }

    TimerLoopThread* TimerLoopThread::Instance()
    {
        static TimerLoopThread timerLoopThread;
        return &timerLoopThread;
    }

    void TimerLoopThread::Start()
    {

        InternetAddress addr(8888);
        
        server_ = new TcpServer(loop_, addr, "");

        server_->start();

        loop_->Loop();
    }

    EventLoop* TimerLoopThread::GetTimerLoop() const { return loop_; }


}