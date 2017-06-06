#include "hawktracer/HRClock.h"

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <ctime>

#define NANOSECS_PER_SECOND 1000000000u

namespace hawktracer
{

NanoTime_t HRClock::get_current_timestamp()
{
    timespec now;

#ifdef __MACH__
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    now.tv_sec = mts.tv_sec;
    now.tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_MONOTONIC, &now);
#endif

    return static_cast<NanoTime_t>(now.tv_sec) * NANOSECS_PER_SECOND + static_cast<NanoTime_t>(now.tv_nsec);
}
    
} // namespace hawktracer

