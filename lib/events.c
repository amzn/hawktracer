#define MKCREFLECT_IMPL
#include "hawktracer/event_macros_impl.h"
#include "hawktracer/core_events.h"

#define HT_EVENT_COPY_FIELD_(buffer, offset, field) \
    memcpy(buffer + offset, (char*)&field, sizeof(field)), offset += sizeof(field)

size_t HT_EVENT_GET_SIZE_FUNCTION(HT_Event)(HT_Event* event)
{
    return sizeof(event->klass->klass_id) + sizeof(event->timestamp) + sizeof(event->id);
}

size_t HT_EVENT_SERIALIZE_FUNCTION(HT_Event)(HT_Event* event, HT_Byte* buffer)
{
    size_t offset = 0;

    HT_EVENT_COPY_FIELD_(buffer, offset, event->klass->klass_id);
    HT_EVENT_COPY_FIELD_(buffer, offset, event->timestamp);
    HT_EVENT_COPY_FIELD_(buffer, offset, event->id);

    return offset;
}

HT_EVENT_GET_KLASS_INSTANCE_FUNCTION_DEF(HT_Event)
HT_EVENT_REGISTER_KLASS_FUNCTION_DEF(HT_Event)
