#ifndef HAWKTRACER_TIMELINE_H
#define HAWKTRACER_TIMELINE_H

#include <hawktracer/base_timeline.h>
#include <hawktracer/events.h>
#include <hawktracer/monotonic_clock.h>

#include <stddef.h>

HT_DECLS_BEGIN

typedef HT_BaseTimeline HT_Timeline;

#define HT_TIMELINE(timeline) ((HT_Timeline*)timeline)

void ht_timeline_register_listener(
        HT_Timeline* timeline,
        HT_TimelineListenerCallback callback,
        void* user_data);

void ht_timeline_unregister_all_listeners(HT_Timeline* timeline);

HT_Timeline* ht_timeline_create(const char* klass_id, ...);

void ht_timeline_destroy(HT_Timeline* timeline);

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
