#ifndef HAWKTRACER_INTERNAL_SYSTEM_INFO_H
#define HAWKTRACER_INTERNAL_SYSTEM_INFO_H

#include "hawktracer/timeline_listener.h"

size_t ht_system_info_push_endianness_info_to_listener(HT_TimelineListenerCallback callback, void* listener, HT_Boolean serialize);

size_t ht_system_info_push_system_info_to_listener(HT_TimelineListenerCallback callback, void* listener, HT_Boolean serialize);

#endif /* HAWKTRACER_INTERNAL_SYSTEM_INFO_H */
