#ifndef HAWKTRACER_INTERNAL_EVENT_UTILS_H
#define HAWKTRACER_INTERNAL_EVENT_UTILS_H

#include "hawktracer/events.h"

HT_DECLS_BEGIN

size_t ht_event_utils_serialize_event_to_buffer(HT_Event* event, HT_Byte* buffer, HT_Boolean serialize);

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_EVENT_UTILS_H */
