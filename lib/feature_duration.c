#include "hawktracer/feature_duration.h"
#include "hawktracer/alloc.h"
#include "hawktracer/thread.h"
#include "internal/bag.h"

typedef struct
{
    HT_Bag bag;
} HT_FeatureDuration;

HT_ErrorCode
ht_feature_duration_enable(HT_Timeline* timeline)
{
    HT_FeatureDuration* feature = HT_CREATE_TYPE(HT_FeatureDuration);
    HT_ErrorCode error_code;

    if (feature == NULL)
    {
        return HT_ERR_OUT_OF_MEMORY;
    }

    error_code = ht_bag_init(&feature->bag, 1024, sizeof(HT_DurationStringEvent));

    if (error_code != HT_ERR_OK)
    {
        ht_free(feature);
        return error_code;
    }

    ht_timeline_set_feature(timeline, HT_FEATURE_DURATION, feature);

    return HT_ERR_OK;
}

void
ht_feature_duration_disable(HT_Timeline* timeline)
{
    HT_FeatureDuration* f = HT_TIMELINE_FEATURE(timeline, HT_FEATURE_DURATION, HT_FeatureDuration);
    ht_bag_deinit(&f->bag);
    ht_free(f);
    ht_timeline_set_feature(timeline, HT_FEATURE_DURATION, NULL);
}

HT_DurationId ht_feature_duration_start(HT_Timeline* timeline, HT_DurationBaseEvent* event)
{
    HT_FeatureDuration* f = HT_TIMELINE_FEATURE(timeline, HT_FEATURE_DURATION, HT_FeatureDuration);

    ht_timeline_init_event(timeline, HT_EVENT(event));
    /* TODO: handle ht_bag_add() error */
    ht_bag_add(&f->bag, event);
    return f->bag.size - 1;
}

HT_DurationNs ht_feature_duration_stop(HT_Timeline* timeline, HT_DurationId id)
{
    HT_FeatureDuration* f = HT_TIMELINE_FEATURE(timeline, HT_FEATURE_DURATION, HT_FeatureDuration);
    HT_DurationBaseEvent* event = (HT_DurationBaseEvent*) ht_bag_get_n(&f->bag, id);

    HT_DurationNs duration = ht_monotonic_clock_get_timestamp() - HT_EVENT(event)->timestamp;
    event->duration = duration;
    ht_timeline_push_event((HT_Timeline*)timeline, HT_EVENT(event));
    ht_bag_remove_nth(&f->bag, id);
    return duration;
}

HT_DurationId ht_feature_duration_start_int(HT_Timeline* timeline, HT_DurationEventLabel label)
{
    HT_DECL_EVENT(HT_DurationIntEvent, event);
    event.label = label;

    return ht_feature_duration_start(timeline, (HT_DurationBaseEvent*)&event);
}

HT_DurationId ht_feature_duration_start_string(HT_Timeline* timeline, const char* label)
{
    HT_DECL_EVENT(HT_DurationStringEvent, event);
    event.label = label;

    return ht_feature_duration_start(timeline, (HT_DurationBaseEvent*)&event);
}
