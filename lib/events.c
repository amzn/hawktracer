#include "hawktracer/events.h"

#include <string.h>

#define _HT_EVENT_COPY_FIELD(buffer, offset, field) \
    memcpy(buffer + offset, (char*)&field, sizeof(field)), offset += sizeof(field)

inline size_t
ht_HT_Event_serialize(HT_Event* event, HT_Byte* buffer)
{
    size_t offset = 0;

    _HT_EVENT_COPY_FIELD(buffer, offset, event->klass->type);
    _HT_EVENT_COPY_FIELD(buffer, offset, event->timestamp);
    _HT_EVENT_COPY_FIELD(buffer, offset, event->id);

    return offset;
}

inline size_t
ht_HT_CallstackBaseEvent_serialize(HT_Event* event, HT_Byte* buffer)
{
    size_t offset = ht_HT_Event_serialize(event, buffer);
    HT_CallstackBaseEvent* c_event = (HT_CallstackBaseEvent*)event;

    _HT_EVENT_COPY_FIELD(buffer, offset, c_event->duration);
    _HT_EVENT_COPY_FIELD(buffer, offset, c_event->thread_id);

    return offset;
}

inline size_t
ht_HT_CallstackIntEvent_serialize(HT_Event* event, HT_Byte* buffer)
{
    size_t offset = ht_HT_CallstackBaseEvent_serialize(event, buffer);
    HT_CallstackIntEvent* c_event = (HT_CallstackIntEvent*)event;

    _HT_EVENT_COPY_FIELD(buffer, offset, c_event->label);

    return offset;
}

inline size_t
ht_HT_CallstackStringEvent_serialize(HT_Event* event, HT_Byte* buffer)
{
    size_t offset = ht_HT_CallstackBaseEvent_serialize(event, buffer);
    HT_CallstackStringEvent* c_event = (HT_CallstackStringEvent*)event;

    _HT_EVENT_COPY_FIELD(buffer, offset, c_event->label);

    return offset;
}

HT_DEFINE_EVENT_KLASS(HT_Event, HT_EVENT_TYPE_BASE)
HT_DEFINE_EVENT_KLASS(HT_CallstackBaseEvent, HT_EVENT_TYPE_CALLSTACK_BASE)
HT_DEFINE_EVENT_KLASS(HT_CallstackIntEvent, HT_EVENT_TYPE_CALLSTACK_INT)
HT_DEFINE_EVENT_KLASS(HT_CallstackStringEvent, HT_EVENT_TYPE_CALLSTACK_STRING)
