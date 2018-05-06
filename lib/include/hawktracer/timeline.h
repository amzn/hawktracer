#ifndef HAWKTRACER_TIMELINE_H
#define HAWKTRACER_TIMELINE_H

#include <hawktracer/events.h>
#include <hawktracer/monotonic_clock.h>
#include <hawktracer/timeline_listener.h>
#include <hawktracer/event_id_provider.h>

#include <stddef.h>

#define HT_TIMELINE_FEATURE(timeline, feature_id, feature_type) \
    ((feature_type*)timeline->features[feature_id])

HT_DECLS_BEGIN

#define HT_TIMELINE_MAX_FEATURES 32

typedef struct
{
    void* features[HT_TIMELINE_MAX_FEATURES];
    size_t buffer_capacity;
    size_t buffer_usage;
    HT_Byte* buffer;
    HT_EventIdProvider* id_provider;
    HT_TimelineListenerContainer* listeners;
    struct _HT_Mutex* locking_policy;
    HT_Boolean serialize_events;
} HT_Timeline;

HT_API HT_ErrorCode ht_timeline_init(HT_Timeline* timeline,
                                     size_t buffer_capacity,
                                     HT_Boolean thread_safe,
                                     HT_Boolean serialize_events,
                                     const char* listeners);

HT_API void ht_timeline_deinit(HT_Timeline* timeline);

HT_API HT_ErrorCode ht_timeline_register_listener(
        HT_Timeline* timeline,
        HT_TimelineListenerCallback callback,
        void* user_data);

HT_API void ht_timeline_unregister_all_listeners(HT_Timeline* timeline);

HT_API void ht_timeline_init_event(HT_Timeline* timeline, HT_Event* event);

HT_API void ht_timeline_push_event(HT_Timeline* timeline, HT_Event* event);

HT_API void ht_timeline_flush(HT_Timeline* timeline);

#define HT_TIMELINE_PUSH_EVENT(TIMELINE, EVENT_TYPE, ...) \
    HT_TIMELINE_PUSH_EVENT_PEDANTIC(TIMELINE, EVENT_TYPE, ht_base_event, __VA_ARGS__)

#define HT_TIMELINE_PUSH_EVENT_PEDANTIC(TIMELINE, EVENT_TYPE, ...) \
    do { \
        HT_Event ht_base_event = { \
            ht_##EVENT_TYPE##_get_event_klass_instance(), \
            ht_monotonic_clock_get_timestamp(), \
            ht_event_id_provider_next((TIMELINE)->id_provider) \
        }; \
        EVENT_TYPE ev = {__VA_ARGS__}; \
        ht_timeline_push_event(TIMELINE, HT_EVENT(&ev)); \
    } while (0)

HT_DECLS_END

#endif /* HAWKTRACER_TIMELINE_H */
