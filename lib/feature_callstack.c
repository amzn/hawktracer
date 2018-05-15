#include "hawktracer/feature_callstack.h"
#include "hawktracer/alloc.h"
#include "hawktracer/thread.h"

HT_ErrorCode
ht_feature_callstack_enable(HT_Timeline* timeline)
{
    HT_FeatureCallstack* feature = HT_CREATE_TYPE(HT_FeatureCallstack);
    HT_ErrorCode error_code;

    if (feature == NULL)
    {
        return HT_ERR_OUT_OF_MEMORY;
    }

    error_code = ht_stack_init(&feature->stack, 1024, 32);

    if (error_code != HT_ERR_OK)
    {
        ht_free(feature);
        return error_code;
    }

    ht_timeline_set_feature(timeline, HT_FEATURE_CALLSTACK, feature);

    return HT_ERR_OK;
}

void
ht_feature_callstack_disable(HT_Timeline* timeline)
{
    HT_FeatureCallstack* f = (HT_FeatureCallstack*)ht_timeline_get_feature(timeline, HT_FEATURE_CALLSTACK);
    ht_stack_deinit(&f->stack);
    ht_free(f);
    ht_timeline_set_feature(timeline, HT_FEATURE_CALLSTACK, NULL);
}

void ht_feature_callstack_start(HT_Timeline* timeline, HT_CallstackBaseEvent* event)
{
    HT_FeatureCallstack* f = HT_TIMELINE_FEATURE(timeline, HT_FEATURE_CALLSTACK, HT_FeatureCallstack);

    ht_timeline_init_event(timeline, HT_EVENT(event));
    /* TODO: handle ht_stack_push() error */
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
