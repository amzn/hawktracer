#include "internal/bag.h"
#include "hawktracer/feature_cached_string.h"
#include "hawktracer/alloc.h"
#include "hawktracer/core_events.h"
#include "internal/mutex.h"

#include <assert.h>


typedef struct
{
    HT_Bag cached_data;
    HT_Mutex* lock;
} HT_FeatureCachedString;

HT_ErrorCode
ht_feature_cached_string_enable(HT_Timeline* timeline)
{
    HT_FeatureCachedString* feature = HT_CREATE_TYPE(HT_FeatureCachedString);
    HT_ErrorCode error_code;

    if (feature == NULL)
    {
        return HT_ERR_OUT_OF_MEMORY;
    }

    feature->lock = ht_mutex_create();
    if (feature->lock == NULL)
    {
        ht_free(feature);
        return HT_ERR_OK;
    }

    error_code = ht_bag_init(&feature->cached_data, 1024, sizeof(void*));

    if (error_code != HT_ERR_OK)
    {
        ht_mutex_destroy(feature->lock);
        ht_free(feature);
        return error_code;
    }

    ht_timeline_set_feature(timeline, HT_FEATURE_CACHED_STRING, feature);

    return error_code;
}

void
ht_feature_cached_string_disable(HT_Timeline* timeline)
{
    HT_FeatureCachedString* feature = HT_TIMELINE_FEATURE(timeline, HT_FEATURE_CACHED_STRING, HT_FeatureCachedString);

    ht_bag_deinit(&feature->cached_data);
    ht_mutex_destroy(feature->lock);
    ht_free(feature);
    ht_timeline_set_feature(timeline, HT_FEATURE_CACHED_STRING, NULL);
}

const char*
ht_feature_cached_string_add_mapping(HT_Timeline* timeline, const char* label)
{
    HT_FeatureCachedString* f = HT_TIMELINE_FEATURE(timeline, HT_FEATURE_CACHED_STRING, HT_FeatureCachedString);
    HT_ErrorCode error_code;

    assert(f);

    ht_mutex_lock(f->lock);
    error_code = ht_bag_add(&f->cached_data, (void*)&label);
    ht_mutex_unlock(f->lock);
    if (error_code != HT_ERR_OK)
    {
        return NULL;
    }

    HT_TIMELINE_PUSH_EVENT(timeline, HT_StringMappingEvent, (uintptr_t)label, label);

    return label;
}

void
ht_feature_cached_string_push_map(HT_Timeline* timeline)
{
    HT_FeatureCachedString* f = HT_TIMELINE_FEATURE(timeline, HT_FEATURE_CACHED_STRING, HT_FeatureCachedString);
    size_t i;

    assert(f);

    ht_mutex_lock(f->lock);

    for (i = 0; i < f->cached_data.size; i++)
    {
        HT_TIMELINE_PUSH_EVENT(timeline, HT_StringMappingEvent, *(uintptr_t*)ht_bag_get_n(&f->cached_data, i), *(void**)ht_bag_get_n(&f->cached_data, i));
    }

    ht_mutex_unlock(f->lock);
}
