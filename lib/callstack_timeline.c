#include "hawktracer/callstack_timeline.h"
#include "internal/thread.h"

#include <assert.h>
#include <string.h>

void
ht_callstack_timeline_init(HT_BaseTimeline* timeline, va_list args)
{
    ht_base_timeline_init(timeline, args);
    ht_stack_init(&HT_CALLSTACK_BASE_TIMELINE(timeline)->stack, 1024, 32);
}

void
ht_callstack_timeline_deinit(HT_BaseTimeline* timeline)
{
    assert(timeline);

    ht_stack_deinit(&((HT_CallstackTimeline*)timeline)->stack);

    ht_base_timeline_deinit(timeline);
}

void
ht_callstack_timeline_start(HT_CallstackTimeline* timeline, HT_CallstackBaseEvent* event)
{
    ht_timeline_init_event(HT_BASE_TIMELINE(timeline), HT_EVENT(event));
    ht_stack_push(&timeline->stack, event, event->base.klass->type_info->size);
}

void
ht_callstack_timeline_stop(HT_CallstackTimeline* timeline)
{
    HT_CallstackBaseEvent* event = (HT_CallstackBaseEvent*)ht_stack_top(&timeline->stack);

    event->duration = ht_monotonic_clock_get_timestamp() - HT_EVENT(event)->timestamp;
    event->thread_id = ht_thread_get_current_thread_id();

    ht_timeline_push_event((HT_Timeline*)timeline, HT_EVENT(event));

    ht_stack_pop(&timeline->stack);
}

void
ht_callstack_timeline_int_start(HT_CallstackTimeline* timeline, HT_CallstackEventLabel label)
{
    HT_DECL_EVENT(HT_CallstackIntEvent, event);
    event.label = label;

    ht_callstack_timeline_start(timeline, (HT_CallstackBaseEvent*)&event);
}

void
ht_callstack_timeline_string_start(HT_CallstackTimeline* timeline, const char* label)
{
    HT_DECL_EVENT(HT_CallstackStringEvent, event);
    event.label = label;

    ht_callstack_timeline_start(timeline, (HT_CallstackBaseEvent*)&event);
}
