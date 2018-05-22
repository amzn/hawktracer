#ifndef HT_FEATURE_DURATION_H
#define HT_FEATURE_DURATION_H

#include <hawktracer/core_events.h>
#include <hawktracer/timeline.h>

HT_DECLS_BEGIN

#define HT_FEATURE_DURATION 2

HT_API HT_ErrorCode ht_feature_duration_enable(HT_Timeline* timeline);

HT_API void ht_feature_duration_disable(HT_Timeline* timeline);

HT_API HT_DurationId ht_feature_duration_start(HT_Timeline* timeline, HT_DurationBaseEvent* event);

HT_API HT_DurationNs ht_feature_duration_stop(HT_Timeline* timeline, HT_DurationId id);

HT_API HT_DurationId ht_feature_duration_start_int(HT_Timeline* timeline, HT_DurationEventLabel label);

HT_API HT_DurationId ht_feature_duration_start_string(HT_Timeline* timeline, const char* label);

HT_DECLS_END

#endif /* HT_FEATURE_DURATION_H */
