#include "hawktracer/feature_callstack.h"
#include "hawktracer/alloc.h"
#include "internal/thread.h"

void
ht_feature_callstack_enable(HT_Timeline* timeline)
{
    timeline->features[HT_FEATURE_CALLSTACK] = ht_alloc(sizeof(HT_FeatureCallstack));
    ht_stack_init(&((HT_FeatureCallstack*)timeline->features[HT_FEATURE_CALLSTACK])->stack, 1024, 32);
}

void
ht_feature_callstack_disable(HT_Timeline* timeline)
{
    ht_stack_deinit(&((HT_FeatureCallstack*)timeline->features[HT_FEATURE_CALLSTACK])->stack);
    ht_free(timeline->features[HT_FEATURE_CALLSTACK]);
}

void ht_feature_callstack_start(HT_Timeline* timeline, HT_CallstackBaseEvent* event)
{
    HT_FeatureCallstack* f = HT_TIMELINE_FEATURE(timeline, HT_FEATURE_CALLSTACK, HT_FeatureCallstack);

    ht_timeline_init_event(timeline, HT_EVENT(event));
    ht_stack_push(&f->stack, event, event->base.klass->type_info->size);
}

void ht_feature_callstack_stop(HT_Timeline* timeline)
{
    HT_FeatureCallstack* f = HT_TIMELINE_FEATURE(timeline, HT_FEATURE_CALLSTACK, HT_FeatureCallstack);
    HT_CallstackBaseEvent* event = (HT_CallstackBaseEvent*)ht_stack_top(&f->stack);

    event->duration = ht_monotonic_clock_get_timestamp() - HT_EVENT(event)->timestamp;
    event->thread_id = ht_thread_get_current_thread_id();

    ht_timeline_push_event((HT_Timeline*)timeline, HT_EVENT(event));

    ht_stack_pop(&f->stack);
}

void ht_feature_callstack_start_int(HT_Timeline* timeline, HT_CallstackEventLabel label)
{
    HT_DECL_EVENT(HT_CallstackIntEvent, event);
    event.label = label;

    ht_feature_callstack_start(timeline, (HT_CallstackBaseEvent*)&event);
}

void ht_feature_callstack_start_string(HT_Timeline* timeline, const char* label)
{
    HT_DECL_EVENT(HT_CallstackStringEvent, event);
    event.label = label;

    ht_feature_callstack_start(timeline, (HT_CallstackBaseEvent*)&event);
}
