#include "hawktracer/monotonic_clock.h"
#include "hawktracer/duration_conversion.h"

#if defined(HT_MONOTONIC_CLOCK_IMPL_CUSTOM)
/* Custom implementation provided */
#else

/* Figure out the implementation based on defines */

#if !defined(HT_MONOTONIC_CLOCK_IMPL_CPP11) && defined(HT_CPP11)
#  define HT_MONOTONIC_CLOCK_IMPL_CPP11
#endif /* HT_CPP11 */

#if !defined(HT_MONOTONIC_CLOCK_IMPL_WIN32) && defined(_WIN32)
#  define HT_MONOTONIC_CLOCK_IMPL_WIN32
#endif /* _WIN32 */

#if !defined(HT_MONOTONIC_CLOCK_IMPL_POSIX) && defined(HT_HAVE_UNISTD_H)
#  include <unistd.h>
#  if _POSIX_TIMERS > 0 && defined(_POSIX_MONOTONIC_CLOCK)
#    define HT_MONOTONIC_CLOCK_IMPL_POSIX
#  endif
#endif

#if !defined(HT_MONOTONIC_CLOCK_IMPL_APPLE) && defined(__APPLE__)
#  define HT_MONOTONIC_CLOCK_IMPL_APPLE
#endif

/* include necessary headers */
#if defined(HT_MONOTONIC_CLOCK_IMPL_CPP11)
#  include <chrono>
#elif defined(HT_MONOTONIC_CLOCK_IMPL_POSIX)
#  include <time.h>
#elif defined(HT_MONOTONIC_CLOCK_IMPL_APPLE)
#  include <mach/mach_host.h>
#  include <mach/mach_port.h>
#  include <mach/clock.h>
#elif defined(HT_MONOTONIC_CLOCK_IMPL_WIN32)
#  include<windows.h>
#  pragma section(".CRT$XCU",read)
    static HT_TimestampNs _ht_msvc_frequency = 0;
   static void _ht_msvc_read_frequency(void); \
    __declspec(allocate(".CRT$XCU")) void (*_ht_msvc_read_frequency_)(void) = _ht_msvc_read_frequency; \
    static void _ht_msvc_read_frequency(void)
    {
        LARGE_INTEGER li;
        QueryPerformanceFrequency(&li);
        _ht_msvc_frequency = li.QuadPart;
    }
#endif

HT_TimestampNs
ht_monotonic_clock_get_timestamp(void)
{
#ifdef HT_MONOTONIC_CLOCK_IMPL_CPP11
    return static_cast<HT_TimestampNs>(std::chrono::steady_clock::now().time_since_epoch().count());
#elif defined(HT_MONOTONIC_CLOCK_IMPL_POSIX)
    struct timespec time;
    // Note: Do we need to link to -lrt to define clock_gettime?
    clock_gettime(CLOCK_MONOTONIC, &time);
    return (HT_TimestampNs) (HT_DUR_S(time.tv_sec) + time.tv_nsec);
#elif defined(HT_MONOTONIC_CLOCK_IMPL_APPLE)
    /* TODO: use mach_absolute_time() instead? */
    clock_serv_t cclock;
    mach_timespec_t time;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &time);
    mach_port_deallocate(mach_task_self(), cclock);
    return (HT_TimestampNs) (HT_DUR_S(time.tv_sec) + time.tv_nsec);
#elif defined(HT_MONOTONIC_CLOCK_IMPL_WIN32)
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (li.QuadPart) * 1000000000 / _ht_msvc_frequency;
#else
#  error "Monotonic clock provider has not been found. Define HT_MONOTONIC_CLOCK_IMPL_CUSTOM" \
    "and provide custom implementation of ht_monotonic_clock_get_timestamp() function"
#endif
}

#endif /* HT_MONOTONIC_CLOCK_IMPL_CUSTOM */
