#ifndef HAWKTRACER_ACTION_H_
#define HAWKTRACER_ACTION_H_

#if ACTION_LABEL_SIZE == 32
#define HAWKTRACER_ACTION_LABEL_T uint32_t
#elif ACTION_LABEL_SIZE == 64
#define HAWKTRACER_ACTION_LABEL_T uint64_t
#endif

#include <hawktracer/HRClock.h>

#include <cstddef>
#include <cstdint>

namespace hawktracer
{

struct Action
{
    using Id = uint32_t;
    using Label = HAWKTRACER_ACTION_LABEL_T;

    NanoTime_t startTime = 0;
    NanoTime_t stopTime = 0;
    Label label = 0;
    Id actionId = 0;
    uint8_t threadId = 0;

    enum { STRUCT_SIZE = sizeof(label) + sizeof(startTime) + sizeof(stopTime) + sizeof(threadId) + sizeof(actionId) };

    Action();
    explicit Action(Label label);

    static NanoTime_t get_time_now_nanoseconds();

    void serialize(char *outBuffer, size_t outBufferSize);
    void deserialize(const char* inBuffer, size_t inBufferSize);

    static double to_milliseconds(NanoTime_t time);
};

} // namespace hawktracer

#endif // HAWKTRACER_ACTION_H_
