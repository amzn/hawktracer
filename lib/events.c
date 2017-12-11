#define HAWKTRACER_DEFINE_EVENTS
#include "hawktracer/core_events.h"

#define _HT_EVENT_COPY_FIELD(buffer, offset, field) \
    memcpy(buffer + offset, (char*)&field, sizeof(field)), offset += sizeof(field)

size_t HT_EVENT_GET_SIZE_FUNCTION(HT_Event)(HT_Event* event)
{
    return sizeof(event->klass->type) + sizeof(event->timestamp) + sizeof(event->id);
}

size_t HT_EVENT_SERIALIZE_FUNCTION(HT_Event)(HT_Event* event, HT_Byte* buffer)
{
    size_t offset = 0;

    _HT_EVENT_COPY_FIELD(buffer, offset, event->klass->type);
    _HT_EVENT_COPY_FIELD(buffer, offset, event->timestamp);
    _HT_EVENT_COPY_FIELD(buffer, offset, event->id);

    return offset;
}

HT_EVENT_GET_KLASS_INSTANCE_FUNCTION_DEF(HT_Event)
HT_EVENT_REGISTER_KLASS_FUNCTION_DEF(HT_Event)
