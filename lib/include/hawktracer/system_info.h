#ifndef HAWKTRACER_SYSTEM_INFO_H
#define HAWKTRACER_SYSTEM_INFO_H

#include <hawktracer/core_events.h>

HT_DECLS_BEGIN

HT_API HT_Endianness ht_system_info_get_endianness(void);

HT_API size_t ht_system_info_push_endianness_info_to_listener(HT_TimelineListenerCallback callback, void* listener, HT_Boolean serialize);

HT_API size_t ht_system_info_push_system_info_to_listener(HT_TimelineListenerCallback callback, void* listener, HT_Boolean serialize);

HT_DECLS_END

#endif /* HAWKTRACER_SYSTEM_INFO_H */
