#include "EasyNet/include/base/TimeStamp.h"
#include "EasyNet/include/base/PlatformDefine.h"
#include <assert.h>
#include <sstream>
#include <time.h>
#include <iomanip>
#ifdef ON_VXWORKS
#include <sys/time.h>
#endif

static const int64_t kMicroSecondsPerSeconds = 1000 * 1000;
static const int64_t kMicrosecondsPerMillisedonds = 1000;
//Number of micro-seconds between the beginning of the Windows epoch (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970)
static const int64_t kMicroseondsSinceEpochOnWindows = 116444736000000000ULL;



Timestamp::Timestamp(int64_t iMicroSecondsSinceEpoch)
    : microSecondsSinceEpoch_(iMicroSecondsSinceEpoch)
{

}

Timestamp::Timestamp(const Timestamp& rhs)
{
    this->microSecondsSinceEpoch_ = rhs.microSecondsSinceEpoch_;
}

Timestamp Timestamp::Now()
{
    int64_t microSecondsSinceEpoch = 0;
#ifdef ON_WINDOWS
    FILETIME ft;
    LARGE_INTEGER li;
    GetSystemTimeAsFileTime(&ft);		  // accuracy 100 ns, 0.1 microseconds
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    microSecondsSinceEpoch = (li.QuadPart - kMicroseondsSinceEpochOnWindows) / 10;
#elif defined ON_LINUX
    struct timeval tv;
    ::gettimeofday(&tv, NULL);
    microSecondsSinceEpoch = tv.tv_sec * kMicroSecondsPerSeconds + tv.tv_usec;

#else	//VXworks

    struct timeval tv;
    ::gettimeofday(&tv, NULL);
    microSecondsSinceEpoch = tv.tv_sec * kMicroSecondsPerSeconds + tv.tv_usec;

    //struct timespec tp;
    //if  ( (clock_gettime(CLOCK_REALTIME, &tp))==0)
    //{
    //    microSecondsSinceEpoch = tp.tv_sec * kMicroSecondsPerSeconds + tp.tv_nsec / 1000;
    //}
#endif
    assert(microSecondsSinceEpoch > 0);
    return Timestamp(microSecondsSinceEpoch);
}


std::string Timestamp::ToCalenderTime()
{
    std::stringstream ss;
    ss << std::setw(6)<< std::setfill('0') << (microSecondsSinceEpoch_ % kMicroSecondsPerSeconds);
    return GetCurrentLocalTime() + "." + ss.str();

}

double Timestamp::DiffInSeconds(const Timestamp& t1, const Timestamp& t2)
{
    int64_t timeDiff = t1.microSecondsSinceEpoch_ - t2.microSecondsSinceEpoch_;
    double tempMicrosecondsPerSeconds = kMicroSecondsPerSeconds;
    return   timeDiff / tempMicrosecondsPerSeconds;
}

double Timestamp::DiffInMilliSedonds(const Timestamp& t1, const Timestamp& t2)
{
    int64_t timeDiff = t1.microSecondsSinceEpoch_ - t2.microSecondsSinceEpoch_;
    double tempMicrosecondsPerMilliSeconds = kMicrosecondsPerMillisedonds;
    return timeDiff / tempMicrosecondsPerMilliSeconds;
}

int64_t Timestamp::DiffInMicroSeconds(const Timestamp& t1, const Timestamp& t2)
{
    int64_t timeDiff = t1.microSecondsSinceEpoch_ - t2.microSecondsSinceEpoch_;
    return timeDiff;
}


std::string Timestamp::GetCurrentLocalTime()
{
    time_t rawTime = time(&rawTime);
    tm* timeInfo = localtime(&rawTime);
    char buffer[25] = { '\0' };
    strftime(buffer, 25, "%Y%m%d %H%M%S", timeInfo);
    return buffer;
}

Timestamp Timestamp::AddSeconds(Timestamp timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * kMicroSecondsPerSeconds);
    return Timestamp(timestamp.GetMicroSecondsSineEpoch() + delta);
}

Timestamp Timestamp::AddMilliseconds(Timestamp timestamp, double milliSeconds)
{
    int64_t delta = static_cast<int64_t>(milliSeconds * kMicrosecondsPerMillisedonds);
    return Timestamp(timestamp.GetMicroSecondsSineEpoch() + delta);
}


