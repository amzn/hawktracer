#ifndef HAWKTRACER_CALLSTACK_BASE_TIMELINE_H
#define HAWKTRACER_CALLSTACK_BASE_TIMELINE_H

#include <hawktracer/timeline.h>

HT_DECLS_BEGIN

#define HT_CALLSTACK_BASE_TIMELINE(timeline) ((HT_CallstackTimeline*)timeline)

typedef struct _HT_Stack HT_Stack;

typedef struct
{
    HT_Timeline base;

    HT_Stack* stack;
} HT_CallstackBaseTimeline;

typedef struct
{
    HT_Event base;

    uint32_t thread_id;
    HT_DurationNs duration;
} HT_CallstackBaseEvent;

HT_DECLARE_EVENT_KLASS(HT_CallstackBaseEvent);

void ht_callstack_base_timeline_init(HT_Timeline* timeline, va_list args);

void ht_callstack_base_timeline_deinit(HT_Timeline* timeline);

void ht_callstack_base_timeline_start(HT_CallstackBaseTimeline* timeline, HT_CallstackBaseEvent* event);

void ht_callstack_base_timeline_stop(HT_CallstackBaseTimeline* timeline);

HT_DECLS_END

#endif /* HAWKTRACER_CALLSTACK_BASE_TIMELINE_H */
