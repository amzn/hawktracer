#include "hawktracer/events.h"
#include "hawktracer/registry.h"

#include <string.h>

#define _HT_EVENT_COPY_FIELD(buffer, offset, field) \
    memcpy(buffer + offset, (char*)&field, sizeof(field)), offset += sizeof(field)

size_t
ht_HT_Event_fnc_serialize(HT_Event* event, HT_Byte* buffer)
{
    size_t offset = 0;

    _HT_EVENT_COPY_FIELD(buffer, offset, event->klass->type);
    _HT_EVENT_COPY_FIELD(buffer, offset, event->timestamp);
    _HT_EVENT_COPY_FIELD(buffer, offset, event->id);

    return offset;
}

HT_DEFINE_EVENT_KLASS_DETAILED(HT_Event)
HT_DEFINE_EVENT_KLASS(HT_EventKlassInfoEvent)
HT_DEFINE_EVENT_KLASS(HT_EventKlassFieldInfoEvent)
HT_DEFINE_EVENT_KLASS(HT_CallstackBaseEvent)
HT_DEFINE_EVENT_KLASS(HT_CallstackIntEvent)
HT_DEFINE_EVENT_KLASS(HT_CallstackStringEvent)
