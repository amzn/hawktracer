#ifndef HAWKTRACER_FEATURE_CACHED_STRING_H
#define HAWKTRACER_FEATURE_CACHED_STRING_H

#include <hawktracer/timeline.h>

HT_DECLS_BEGIN

#define HT_FEATURE_CACHED_STRING 1

HT_API HT_ErrorCode ht_feature_cached_string_enable(HT_Timeline* timeline, HT_Boolean thread_safe);

HT_API void ht_feature_cached_string_disable(HT_Timeline* timeline);

HT_API uintptr_t ht_feature_cached_string_add_mapping(HT_Timeline* timeline, const char* label);

HT_API void ht_feature_cached_string_push_map(HT_Timeline* timeline);

#ifdef HT_CPP11
HT_API uintptr_t ht_feature_cached_string_add_mapping_dynamic(HT_Timeline* timeline, const char* label);
#endif

HT_DECLS_END


#endif /* HAWKTRACER_FEATURE_CACHED_STRING_H */
