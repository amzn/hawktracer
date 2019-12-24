#include "hawktracer/feature_callstack.h"
#include "hawktracer/alloc.h"
#include "hawktracer/thread.h"
#include "internal/stack.h"
#include "internal/error.h"
#include "internal/feature.h"

typedef struct
{
    HT_Feature base;
    HT_Stack stack;
} HT_FeatureCallstack;

static void
ht_feature_callstack_destroy(HT_Feature* feature);

HT_FEATURE_DEFINE(HT_FeatureCallstack, ht_feature_callstack_destroy)

static HT_Feature*
ht_feature_callstack_create(HT_ErrorCode* out_err)
{
    HT_FeatureCallstack* feature = HT_FeatureCallstack_alloc();
    HT_ErrorCode error_code = HT_ERR_OK;

    if (feature == NULL)
    {
        HT_SET_ERROR(out_err, HT_ERR_OUT_OF_MEMORY);
        return NULL;
    }

    error_code = ht_stack_init(&feature->stack, 1024, 32);

    if (error_code != HT_ERR_OK)
    {
        ht_free(feature);
        feature = NULL;
    }

    HT_SET_ERROR(out_err, error_code);

    return (HT_Feature*)feature;
}

static void
ht_feature_callstack_destroy(HT_Feature* feature)
{
    HT_FeatureCallstack* f = (HT_FeatureCallstack*)feature;
    ht_stack_deinit(&f->stack);
    ht_free(f);
}

void
ht_feature_callstack_start(HT_Timeline* timeline, HT_CallstackBaseEvent* event)
{
    HT_FeatureCallstack* f = HT_FeatureCallstack_from_timeline(timeline);

    ht_timeline_init_event(timeline, HT_EVENT(event));
    /* TODO: handle ht_stack_push() error */
    ht_stack_push(&f->stack, event, event->base.klass->type_info->size);
}

void
ht_feature_callstack_stop(HT_Timeline* timeline)
{
    HT_FeatureCallstack* f = HT_FeatureCallstack_from_timeline(timeline);
    HT_CallstackBaseEvent* event = (HT_CallstackBaseEvent*)ht_stack_top(&f->stack);

    event->duration = ht_monotonic_clock_get_timestamp() - HT_EVENT(event)->timestamp;
    event->thread_id = ht_thread_get_current_thread_id();

    ht_timeline_push_event((HT_Timeline*)timeline, HT_EVENT(event));

    ht_stack_pop(&f->stack);
}

void
ht_feature_callstack_start_int(HT_Timeline* timeline, HT_CallstackEventLabel label)
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

HT_ErrorCode
ht_feature_callstack_enable(HT_Timeline* timeline)
{
    HT_ErrorCode error_code;
    HT_Feature* feature = ht_feature_callstack_create(&error_code);

    if (!feature)
    {
        return error_code;
    }

    return ht_timeline_set_feature(timeline, feature);
}
