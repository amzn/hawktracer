#include "hawktracer/callstack_base_timeline.h"
#include "hawktracer/alloc.h"
#include "hawktracer/monotonic_clock.h"
#include "hawktracer/timeline.h"

#include "internal/thread.h"

#include <assert.h>

void
ht_callstack_base_timeline_init(HT_BaseTimeline* timeline, va_list args)
{
    ht_base_timeline_init(timeline, args);
    ht_stack_init(&HT_CALLSTACK_BASE_TIMELINE(timeline)->stack, 1024, 32);
}

void
ht_callstack_base_timeline_deinit(HT_BaseTimeline* timeline)
{
    assert(timeline);

    ht_stack_deinit(&((HT_CallstackBaseTimeline*)timeline)->stack);

    ht_base_timeline_deinit(timeline);
}
int i = 2;
void
ht_callstack_base_timeline_start(HT_CallstackBaseTimeline* timeline, HT_CallstackBaseEvent* event)
{
    ht_timeline_init_event(HT_BASE_TIMELINE(timeline), HT_EVENT(event));
    ht_stack_push(&timeline->stack, event, event->base.klass->event_size);
}

void
ht_callstack_base_timeline_stop(HT_CallstackBaseTimeline* timeline)
{
    HT_CallstackBaseEvent* event = (HT_CallstackBaseEvent*)ht_stack_top(&timeline->stack);

    event->duration = ht_monotonic_clock_get_timestamp() - HT_EVENT(event)->timestamp;
    event->thread_id = ht_thread_get_current_thread_id();

    ht_timeline_push_event((HT_Timeline*)timeline, HT_EVENT(event));

    ht_stack_pop(&timeline->stack);
}
