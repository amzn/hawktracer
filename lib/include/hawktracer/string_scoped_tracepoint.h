#ifndef HAWKTRACER_STRING_SCOPED_TRACEPOINT_H
#define HAWKTRACER_STRING_SCOPED_TRACEPOINT_H

#include <hawktracer/scoped_tracepoint.h>
#include <hawktracer/feature_cached_string.h>

#ifdef __cplusplus

#define HT_TP_STRACEPOINT(timeline, label) \
    static const char* HT_UNIQUE_VAR_NAME(fnc_track) = ht_feature_cached_string_add_mapping(timeline, label); \
    HT_TP_SCOPED_INT(timeline, (uintptr_t)HT_UNIQUE_VAR_NAME(fnc_track));

#endif

#define HT_TP_FUNCTION(timeline) \
    HT_TP_STRACEPOINT(timeline, __func__)

#endif /* HAWKTRACER_STRING_SCOPED_TRACEPOINT_H */
