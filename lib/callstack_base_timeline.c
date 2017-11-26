#include "hawktracer/callstack_base_timeline.h"
#include "hawktracer/alloc.h"
#include "hawktracer/monotonic_clock.h"
#include "hawktracer/timeline.h"

#include "internal/thread.h"

#include <string.h>
#include <assert.h>

HT_DEFINE_EVENT_KLASS(HT_CallstackBaseEvent, 2);

void
ht_callstack_base_timeline_init(HT_BaseTimeline* timeline, va_list args)
{
    ht_base_timeline_init(timeline, args);
    HT_CallstackBaseTimeline* callstack_timeline = (HT_CallstackBaseTimeline*)timeline;
    ht_bag_init(&callstack_timeline->sizes_stack, 1024);
    callstack_timeline->data = (uint8_t*)ht_alloc(1024);
    callstack_timeline->pos = 0;
}

void
ht_callstack_base_timeline_deinit(HT_BaseTimeline* timeline)
{
    assert(timeline);

    ht_free(((HT_CallstackBaseTimeline*)timeline)->data);
    ht_bag_deinit(&((HT_CallstackBaseTimeline*)timeline)->sizes_stack);

    ht_base_timeline_deinit(timeline);
}

void
ht_callstack_base_timeline_start(HT_CallstackBaseTimeline* timeline, HT_CallstackBaseEvent* event)
{
    ht_timeline_init_event(HT_BASE_TIMELINE(timeline), HT_EVENT(event));
    ht_bag_add(&timeline->sizes_stack, timeline->data + timeline->pos);
    memcpy(timeline->data + timeline->pos, event, event->base.klass->size);
    timeline->pos += event->base.klass->size;
}

void
ht_callstack_base_timeline_stop(HT_CallstackBaseTimeline* timeline)
{
    size_t last_stack = timeline->sizes_stack.size - 1;
    HT_CallstackBaseEvent* event = (HT_CallstackBaseEvent*)(timeline->sizes_stack.data[last_stack]);
    timeline->pos -= HT_EVENT(event)->klass->size;
    ht_bag_remove_nth(&timeline->sizes_stack, last_stack);

    event->duration = ht_monotonic_clock_get_timestamp() - HT_EVENT(event)->timestamp;
    event->thread_id = ht_thread_get_current_thread_id();

    ht_timeline_push_event((HT_Timeline*)timeline, HT_EVENT(event));
}
