#include "hawktracer/feature_cached_string.h"
#include "hawktracer/alloc.h"
#include "hawktracer/core_events.h"
#include "hawktracer/hash.h"

#include <assert.h>

void
ht_feature_cached_string_enable(HT_Timeline* timeline)
{
    timeline->features[HT_FEATURE_CACHED_STRING] = ht_alloc(sizeof(HT_FeatureCachedString));
    ht_bag_init(&((HT_FeatureCachedString*)timeline->features[HT_FEATURE_CACHED_STRING])->cached_data, 1024);
}

void
ht_feature_cached_string_disable(HT_Timeline* timeline)
{
    ht_bag_deinit(&((HT_FeatureCachedString*)timeline->features[HT_FEATURE_CACHED_STRING])->cached_data);
    ht_free(timeline->features[HT_FEATURE_CACHED_STRING]);
}

uintptr_t
ht_feature_cached_string_push(HT_Timeline* timeline, const char* label)
{
    HT_FeatureCachedString* f = (HT_FeatureCachedString*)timeline->features[HT_FEATURE_CACHED_STRING];
    assert(f);

    uintptr_t id = djb2_hash(label);

    for (size_t i = 0; i < f->cached_data.size; i++)
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
