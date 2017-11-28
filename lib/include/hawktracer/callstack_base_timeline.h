#ifndef HAWKTRACER_CALLSTACK_BASE_TIMELINE_H
#define HAWKTRACER_CALLSTACK_BASE_TIMELINE_H

#include <hawktracer/timeline.h>
#include <hawktracer/events.h>
#include <hawktracer/stack.h>

HT_DECLS_BEGIN

#define HT_CALLSTACK_BASE_TIMELINE(timeline) ((HT_CallstackBaseTimeline*)timeline)

typedef struct
{
    HT_BaseTimeline base;

    HT_Stack stack;
} HT_CallstackBaseTimeline;

typedef struct
{
    HT_Event base;

    HT_DurationNs duration;
    HT_ThreadId thread_id;
} HT_CallstackBaseEvent;

HT_DECLARE_EVENT_KLASS(HT_CallstackBaseEvent);

void ht_callstack_base_timeline_init(HT_BaseTimeline* timeline, va_list args);

void ht_callstack_base_timeline_deinit(HT_BaseTimeline* timeline);

void ht_callstack_base_timeline_start(HT_CallstackBaseTimeline* timeline, HT_CallstackBaseEvent* event);

void ht_callstack_base_timeline_stop(HT_CallstackBaseTimeline* timeline);

HT_DECLS_END

#endif /* HAWKTRACER_CALLSTACK_BASE_TIMELINE_H */
