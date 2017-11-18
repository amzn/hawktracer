#include "hawktracer/callstack_base_timeline.h"

#include "hawktracer/alloc.h"

#include <cstring>
#include <stack>
#include <cassert>
#include <new>

HT_DEFINE_EVENT_KLASS(HT_CallstackBaseEvent, 2);

struct _HT_Stack
{
    std::stack<size_t> sizes_stack;
    uint8_t* data;
    size_t pos;
};

void
ht_callstack_base_timeline_init(HT_Timeline* timeline, va_list args)
{
    ht_timeline_init(timeline, args);
    HT_CallstackBaseTimeline* callstack_timeline = (HT_CallstackBaseTimeline*)timeline;
    callstack_timeline->stack = HT_CREATE_TYPE(HT_Stack);
    new(callstack_timeline->stack) _HT_Stack();
    callstack_timeline->stack->data = (uint8_t*)ht_alloc(1024);
    callstack_timeline->stack->pos = 0;
}

void
ht_callstack_base_timeline_deinit(HT_Timeline* timeline)
{
    assert(timeline);

    ht_free(((HT_CallstackBaseTimeline*)timeline)->stack->data);
    ((HT_CallstackBaseTimeline*)timeline)->stack->~_HT_Stack();
    ht_free(((HT_CallstackBaseTimeline*)timeline)->stack);

    ht_timeline_deinit(timeline);
}

void
ht_callstack_base_timeline_start(HT_CallstackBaseTimeline* timeline, HT_CallstackBaseEvent* event)
{
    memcpy(timeline->stack->data + timeline->stack->pos, event, event->base.klass->size);
    timeline->stack->sizes_stack.push(event->base.klass->size);
    timeline->stack->pos += event->base.klass->size;
}

void
ht_callstack_base_timeline_stop(HT_CallstackBaseTimeline* timeline)
{
    size_t size = timeline->stack->sizes_stack.top();
    timeline->stack->sizes_stack.pop();
    timeline->stack->pos -= size;

    ht_timeline_push_event((HT_Timeline*)timeline, (HT_Event*)(timeline->stack->data + timeline->stack->pos));
}
