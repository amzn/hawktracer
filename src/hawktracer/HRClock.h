#ifndef HAWKTRACER_HRCLOCK_H_
#define HAWKTRACER_HRCLOCK_H_

#include <cstdint>

namespace hawktracer
{

using NanoTime_t = int64_t;

class HRClock
{
public:
    static NanoTime_t get_current_timestamp();
};

} // namespace hawktracer

#endif // HAWKTRACER_HRCLOCK_H_
