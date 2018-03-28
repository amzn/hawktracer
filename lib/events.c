#define HAWKTRACER_DEFINE_EVENTS
#include "hawktracer/core_events.h"

MKCREFLECT_TypeInfo* mkcreflect_get_HT_Event_type_info(void)
{
    static MKCREFLECT_FieldInfo fields_info[3] = {
        { "HT_EventKlass*", "klass", sizeof(HT_EventKlass*), offsetof(HT_Event, klass), 0, -1, MKCREFLECT_TYPES_POINTER },
        { "HT_TimestampNs", "timestamp", sizeof(HT_TimestampNs), offsetof(HT_Event, timestamp), 0, -1, MKCREFLECT_TYPES_INTEGER },
        { "HT_EventId", "id", sizeof(HT_EventId), offsetof(HT_Event, id), 0, -1, MKCREFLECT_TYPES_INTEGER }
    };

    static MKCREFLECT_TypeInfo type_info = {
        "HT_Event",
        3,
        sizeof(HT_Event),
        sizeof(HT_EventKlass*) + sizeof(HT_TimestampNs) + sizeof(HT_EventId),
        fields_info
    };

    return &type_info;
}

#define _HT_EVENT_COPY_FIELD(buffer, offset, field) \
    memcpy(buffer + offset, (char*)&field, sizeof(field)), offset += sizeof(field)

size_t HT_EVENT_GET_SIZE_FUNCTION(HT_Event)(HT_Event* event)
{
    return sizeof(event->klass->klass_id) + sizeof(event->timestamp) + sizeof(event->id);
}

size_t HT_EVENT_SERIALIZE_FUNCTION(HT_Event)(HT_Event* event, HT_Byte* buffer)
{
    size_t offset = 0;

    _HT_EVENT_COPY_FIELD(buffer, offset, event->klass->klass_id);
    _HT_EVENT_COPY_FIELD(buffer, offset, event->timestamp);
    _HT_EVENT_COPY_FIELD(buffer, offset, event->id);

    return offset;
}

HT_EVENT_GET_KLASS_INSTANCE_FUNCTION_DEF(HT_Event)
HT_EVENT_REGISTER_KLASS_FUNCTION_DEF(HT_Event)
