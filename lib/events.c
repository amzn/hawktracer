#include "hawktracer/events.h"
#include "hawktracer/registry.h"

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

inline size_t
ht_HT_EventKlassFieldInfoEvent_serialize(HT_Event* event, HT_Byte* buffer)
{
    size_t offset = ht_HT_Event_serialize(event, buffer);
    HT_EventKlassFieldInfoEvent* c_event = (HT_EventKlassFieldInfoEvent*)event;

    _HT_EVENT_COPY_FIELD(buffer, offset, c_event->event_type);
    _HT_EVENT_COPY_FIELD(buffer, offset, c_event->field_type);
    _HT_EVENT_COPY_FIELD(buffer, offset, c_event->field_name);
    _HT_EVENT_COPY_FIELD(buffer, offset, c_event->size);
    _HT_EVENT_COPY_FIELD(buffer, offset, c_event->data_type);

    return offset;
}

inline size_t
ht_HT_EventKlassInfoEvent_serialize(HT_Event* event, HT_Byte* buffer)
{
    size_t offset = ht_HT_Event_serialize(event, buffer);
    HT_EventKlassInfoEvent* c_event = (HT_EventKlassInfoEvent*)event;

    _HT_EVENT_COPY_FIELD(buffer, offset, c_event->event_type);
    _HT_EVENT_COPY_FIELD(buffer, offset, c_event->event_klass_name);
    _HT_EVENT_COPY_FIELD(buffer, offset, c_event->field_count);

    return offset;
}

HT_DEFINE_EVENT_KLASS_DETAILED(HT_Event, mkcreflect_get_HT_Event_type_info()->packed_size)
HT_DEFINE_EVENT_KLASS(HT_EventKlassInfoEvent)
HT_DEFINE_EVENT_KLASS(HT_EventKlassFieldInfoEvent)
HT_DEFINE_EVENT_KLASS(HT_CallstackBaseEvent)
HT_DEFINE_EVENT_KLASS(HT_CallstackIntEvent)
HT_DEFINE_EVENT_KLASS(HT_CallstackStringEvent)
