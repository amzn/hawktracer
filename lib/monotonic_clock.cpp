#include <hawktracer/monotonic_clock.h>

#include <chrono>

HT_TimestampNs
ht_monotonic_clock_get_timestamp()
{
    auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();

    return static_cast<HT_TimestampNs>(timestamp);
}

