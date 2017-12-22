#ifndef HAWKTRACER_FEATURE_CACHED_STRING_H
#define HAWKTRACER_FEATURE_CACHED_STRING_H

#include <hawktracer/bag.h>
#include <hawktracer/timeline.h>

HT_DECLS_BEGIN

typedef struct
{
    HT_Bag cached_data; /* TODO it should be some kind of set */
} HT_FeatureCachedString;

#define HT_FEATURE_CACHED_STRING 1

void ht_feature_cached_string_enable(HT_Timeline* timeline);

void ht_feature_cached_string_disable(HT_Timeline* timeline);

uintptr_t ht_feature_cached_string_push(HT_Timeline* timeline, const char* label);

HT_DECLS_END


#endif /* HAWKTRACER_FEATURE_CACHED_STRING_H */
