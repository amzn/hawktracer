#include "Action.h"

#include <cassert>
#include <ctime>
#include <cstring>

#define NANOSECS_PER_MILLISEC 1000000

namespace hawktracer
{

Action::Action() :
    Action(0)
{
    stopTime = startTime;
}

Action::Action(Label label) :
    label(label)
{
    startTime = get_time_now_nanoseconds();
}

NanoTime_t Action::get_time_now_nanoseconds()
{
    return HRClock::get_current_timestamp();
}

void Action::serialize(char *outBuffer, std::size_t outBufferSize)
{
    assert(outBufferSize >= STRUCT_SIZE);

    std::size_t outSize = 0;

#define SERIALIZE_FIELD(FIELD) do { \
    memcpy(outBuffer + outSize, (char*)&FIELD, sizeof(FIELD)); \
    outSize += sizeof(FIELD); \
} while (false)

    SERIALIZE_FIELD(actionId);
    SERIALIZE_FIELD(startTime);
    SERIALIZE_FIELD(stopTime);
    SERIALIZE_FIELD(label);
    SERIALIZE_FIELD(threadId);
#undef SERIALIZE_FIELD
}

void Action::deserialize(const char* inBuffer, std::size_t inBufferSize)
{
    assert(inBufferSize >= STRUCT_SIZE);

    int outSize = 0;

#define DESERIALIZE_FIELD(FIELD) do {					\
    memcpy((char*)&FIELD, inBuffer + outSize, sizeof(FIELD));	\
    outSize += sizeof(FIELD);					\
} while (false)

    DESERIALIZE_FIELD(actionId);
    DESERIALIZE_FIELD(startTime);
    DESERIALIZE_FIELD(stopTime);
    DESERIALIZE_FIELD(label);
    DESERIALIZE_FIELD(threadId);
#undef DESERIALIZE_FIELD
}


double Action::to_milliseconds(NanoTime_t time)
{
    return static_cast<double>(time) / NANOSECS_PER_MILLISEC;
}

} // namespace hawktracer
