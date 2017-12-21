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

typedef struct
{
    void* features[32];
    size_t buffer_capacity;
    size_t buffer_usage;
    HT_Byte* buffer;
    HT_EventIdProvider* id_provider;
    HT_TimelineListenerContainer* listeners;
    struct _HT_Mutex* locking_policy;
    struct _HT_TimelineKlass* klass;
    HT_Boolean serialize_events;
} HT_Timeline;

void ht_timeline_init(HT_Timeline* timeline,
                      size_t buffer_capacity,
                      HT_Boolean thread_safe,
                      HT_Boolean serialize_events,
                      const char* listeners);

void ht_timeline_deinit(HT_Timeline* timeline);

void ht_timeline_register_listener(
        HT_Timeline* timeline,
        HT_TimelineListenerCallback callback,
        void* user_data);

void ht_timeline_unregister_all_listeners(HT_Timeline* timeline);

void ht_timeline_init_event(HT_Timeline* timeline, HT_Event* event);

void ht_timeline_push_event(HT_Timeline* timeline, HT_Event* event);

void ht_timeline_flush(HT_Timeline* timeline);

#define HT_TIMELINE_PUSH_EVENT(TIMELINE, EVENT_TYPE, ...) \
    do { \
        EVENT_TYPE ev = {{ht_##EVENT_TYPE##_get_event_klass_instance(), \
            ht_monotonic_clock_get_timestamp(), \
            ht_event_id_provider_next((TIMELINE)->id_provider)}, __VA_ARGS__}; \
        ht_timeline_push_event(TIMELINE, HT_EVENT(&ev)); \
    } while (0)

HT_DECLS_END

#endif /* HAWKTRACER_TIMELINE_H */
