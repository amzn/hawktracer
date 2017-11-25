#ifndef HAWKTRACER_BASE_TIMELINE_H
#define HAWKTRACER_BASE_TIMELINE_H

#include <hawktracer/macros.h>
#include <hawktracer/timeline_listener.h>
#include <hawktracer/event_id_provider.h>

#include <stdarg.h>

HT_DECLS_BEGIN

typedef struct
{
    size_t buffer_capacity;
    size_t buffer_usage;
    uint8_t* buffer;
    HT_EventIdProvider* id_provider;
    HT_TimelineListenerContainer* listeners;
    struct _HT_Mutex* locking_policy;
    struct _HT_TimelineKlass* klass;
} HT_BaseTimeline;

#define HT_BASE_TIMELINE(timeline) ((HT_BaseTimeline*)timeline)

void ht_base_timeline_init(HT_BaseTimeline* timeline, va_list);

void ht_base_timeline_deinit(HT_BaseTimeline* timeline);

HT_DECLS_END

#endif /* HAWKTRACER_BASE_TIMELINE_H */
