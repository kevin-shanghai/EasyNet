#include "EasyNet/include/net/EventLoop.h"
#include "EasyNet/include/net/Timer.h"

#include <iostream>

void print()
{
    std::cout << "Hello, world!\n" << std::endl;
}

void print_ms()
{
    std::cout << "Hello, world! ms\n" << std::endl;
}

using namespace Net;



int main_TimerManager()
{
    EventLoop loop;
    loop.RunAfter_s(5, print);
    loop.RunAfter_ms(3000, print_ms);
    loop.RunAfter_ms(1000, print_ms);
    loop.RunAfter_ms(1200, print_ms);
    loop.RunAfter_ms(1300, print_ms);

    Timestamp la = Timestamp::AddSeconds(Timestamp::Now(), 15);
    loop.RunAt(la, print);

    loop.Loop();
    return 0;
}