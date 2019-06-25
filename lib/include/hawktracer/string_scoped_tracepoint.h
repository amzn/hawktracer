#ifndef HAWKTRACER_STRING_SCOPED_TRACEPOINT_H
#define HAWKTRACER_STRING_SCOPED_TRACEPOINT_H

#include <hawktracer/ht_config.h>
#include <hawktracer/scoped_tracepoint.h>
#include <hawktracer/feature_cached_string.h>

#ifdef __cplusplus

#define HT_TP_STRACEPOINT(timeline, label) \
    static HT_THREAD_LOCAL uintptr_t HT_UNIQUE_VAR_NAME(fnc_track) = ht_feature_cached_string_add_mapping(timeline, label); \
    HT_TP_SCOPED_INT(timeline, (uintptr_t)HT_UNIQUE_VAR_NAME(fnc_track));

#define HT_TP_DYN_STRACEPOINT(timeline, label) \
    HT_TP_SCOPED_INT(timeline, (uintptr_t)ht_feature_cached_string_add_mapping_dynamic(timeline, label));

#elif defined(HT_USE_PTHREADS) && defined(HT_SCOPED_TRACEPOINT_MACRO_ENABLED)

#include <hawktracer/posix_mapped_tracepoint.h>

#define HT_TP_STRACEPOINT(timeline, label) \
    static HT_THREAD_LOCAL pthread_once_t HT_UNIQUE_VAR_NAME(ht_var_once) = PTHREAD_ONCE_INIT; \
    static HT_THREAD_LOCAL uintptr_t HT_UNIQUE_VAR_NAME(fnc_track) = label; \
    ht_pthread_mapped_tracepoint_enter(timeline, HT_UNIQUE_VAR_NAME(fnc_track)); \
    pthread_once(&HT_UNIQUE_VAR_NAME(ht_var_once), ht_pthread_mapped_tracepoint_add_mapping); \
    ht_pthread_mapped_tracepoint_leave(); \
    HT_TP_SCOPED_INT(timeline, (uintptr_t)HT_UNIQUE_VAR_NAME(fnc_track));

#endif

#define HT_TP_FUNCTION(timeline) \
    HT_TP_STRACEPOINT(timeline, __func__)

#endif /* HAWKTRACER_STRING_SCOPED_TRACEPOINT_H */
