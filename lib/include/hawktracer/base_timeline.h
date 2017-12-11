#ifndef HAWKTRACER_BASE_TIMELINE_H
#define HAWKTRACER_BASE_TIMELINE_H

#include <hawktracer/timeline_listener.h>
#include <hawktracer/event_id_provider.h>

#include <stdarg.h>

#define HT_BASE_TIMELINE_IDENTIFIER "BaseTimeline"

#define HT_BASE_TIMELINE_PROPERTY_BUFFER_CAPACITY "buffer-capacity"
#define HT_BASE_TIMELINE_PROPERTY_BUFFER_CAPACITY_DEFAULT 4096

#define HT_BASE_TIMELINE_PROPERTY_THREAD_SAFE "thread-safe"
#define HT_BASE_TIMELINE_PROPERTY_THREAD_SAFE_DEFAULT HT_TRUE

#define HT_BASE_TIMELINE_PROPERTY_SERIALIZE_EVENTS "serialize-events"
#define HT_BASE_TIMELINE_PROPERTY_SERIALIZE_EVENTS_DEFAULT HT_FALSE

HT_DECLS_BEGIN

typedef struct
{
    size_t buffer_capacity;
    size_t buffer_usage;
    HT_Byte* buffer;
    HT_EventIdProvider* id_provider;
    HT_TimelineListenerContainer* listeners;
    struct _HT_Mutex* locking_policy;
    struct _HT_TimelineKlass* klass;
    HT_Boolean serialize_events;
} HT_BaseTimeline;

#define HT_BASE_TIMELINE(timeline) ((HT_BaseTimeline*)timeline)

void ht_base_timeline_init(HT_BaseTimeline* timeline, va_list);

void ht_base_timeline_deinit(HT_BaseTimeline* timeline);

HT_DECLS_END

#endif /* HAWKTRACER_BASE_TIMELINE_H */
