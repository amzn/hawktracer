#include "hawktracer/feature_cached_string.h"
#include "hawktracer/alloc.h"
#include "hawktracer/core_events.h"
#include "internal/hash.h"

#include <assert.h>

HT_ErrorCode
ht_feature_cached_string_enable(HT_Timeline* timeline)
{
    HT_FeatureCachedString* feature = HT_CREATE_TYPE(HT_FeatureCachedString);
    HT_ErrorCode error_code;

    if (feature == NULL)
    {
        return HT_ERR_OUT_OF_MEMORY;
    }

    error_code = ht_bag_init(&feature->cached_data, 1024);

    if (error_code != HT_ERR_OK)
    {
        ht_free(feature);
        return error_code;
    }

    timeline->features[HT_FEATURE_CACHED_STRING] = feature;

    return error_code;
}

void
ht_feature_cached_string_disable(HT_Timeline* timeline)
{
    ht_bag_deinit(&((HT_FeatureCachedString*)timeline->features[HT_FEATURE_CACHED_STRING])->cached_data);
    ht_free(timeline->features[HT_FEATURE_CACHED_STRING]);
    timeline->features[HT_FEATURE_CACHED_STRING] = NULL;
}

uintptr_t
ht_feature_cached_string_push(HT_Timeline* timeline, const char* label)
{
    HT_FeatureCachedString* f = (HT_FeatureCachedString*)timeline->features[HT_FEATURE_CACHED_STRING];
    assert(f);
    size_t i;
    uintptr_t id = djb2_hash(label);

    for (i = 0; i < f->cached_data.size; i++)
    {
        if ((uintptr_t)f->cached_data.data[i] == id)
        {
            return id;
        }
    }

    ht_bag_add(&f->cached_data, (void*)id);
    HT_TIMELINE_PUSH_EVENT(timeline, HT_StringMappingEvent, id, label);

    return id;
}
