#include "internal/event_utils.h"

#include <string.h>

size_t
ht_event_utils_serialize_event_to_buffer(HT_Event* event, HT_Byte* buffer, HT_Boolean serialize)
{
    HT_EventKlass* klass = HT_EVENT_GET_CLASS(event);

    if (serialize)
    {
        return klass->serialize(event, buffer);
    }
    else
    {
        memcpy(buffer, event, klass->type_info->size);
        return klass->type_info->size;
    }
}
