#ifndef HAWKTRACER_FEATURE_CACHED_STRING_H
#define HAWKTRACER_FEATURE_CACHED_STRING_H

#include <hawktracer/bag.h>
#include <hawktracer/timeline.h>

HT_DECLS_BEGIN

typedef struct
{
    HT_Bag cached_data;
    struct _HT_Mutex* lock;
} HT_FeatureCachedString;

#define HT_FEATURE_CACHED_STRING 1

HT_API HT_ErrorCode ht_feature_cached_string_enable(HT_Timeline* timeline);

HT_API void ht_feature_cached_string_disable(HT_Timeline* timeline);

HT_API const char* ht_feature_cached_string_add_mapping(HT_Timeline* timeline, const char* label);

HT_API void ht_feature_cached_string_push_map(HT_Timeline* timeline);

HT_DECLS_END


#endif /* HAWKTRACER_FEATURE_CACHED_STRING_H */
