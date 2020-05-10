#ifndef HAWKTRACER_STRING_SCOPED_TRACEPOINT_H
#define HAWKTRACER_STRING_SCOPED_TRACEPOINT_H

#include <hawktracer/ht_config.h>
#include <hawktracer/scoped_tracepoint.h>
#include <hawktracer/feature_cached_string.h>

#ifdef __cplusplus

/**
 * Creates a tracepoint that measures time spent in the scope.
 *
 * The label must be a static string. If it's not, use HT_TP_DYN_STRACEPOINT() instead.
 *
 * @param timeline a timeline where the event will be posted to.
 * @param label a string label of the tracepoint.
 */
#define HT_TP_STRACEPOINT(timeline, label) \
    static HT_THREAD_LOCAL uintptr_t HT_UNIQUE_VAR_NAME(fnc_track) = ht_feature_cached_string_add_mapping(timeline, label); \
    HT_TP_SCOPED_INT(timeline, (uintptr_t)HT_UNIQUE_VAR_NAME(fnc_track));

#elif defined(HT_USE_PTHREADS) && defined(HT_SCOPED_TRACEPOINT_MACRO_ENABLED)

#include <pthread.h>
#include <hawktracer/posix_mapped_tracepoint.h>

#define HT_TP_STRACEPOINT(timeline, label) \
    static HT_THREAD_LOCAL pthread_once_t HT_UNIQUE_VAR_NAME(ht_var_once) = PTHREAD_ONCE_INIT; \
    static HT_THREAD_LOCAL uintptr_t HT_UNIQUE_VAR_NAME(fnc_track) = label; \
    ht_pthread_mapped_tracepoint_enter(timeline, HT_UNIQUE_VAR_NAME(fnc_track)); \
    pthread_once(&HT_UNIQUE_VAR_NAME(ht_var_once), ht_pthread_mapped_tracepoint_add_mapping); \
    ht_pthread_mapped_tracepoint_leave(); \
    HT_TP_SCOPED_INT(timeline, (uintptr_t)HT_UNIQUE_VAR_NAME(fnc_track));

#endif

/**
 * Creates a tracepoint that measures time spent in the scope.
 * This method is slower than HT_TP_STRACEPOINT(), but allows user to use
 * dynamic strings for label.
 *
 * @param timeline a timeline where the event will be posted to.
 * @param label a string label of the tracepoint.
 */
#define HT_TP_DYN_STRACEPOINT(timeline, label) \
    HT_TP_SCOPED_INT(timeline, (uintptr_t)ht_feature_cached_string_add_mapping_dynamic(timeline, label));

/**
 * Generates a tracepoint that measures time spent in the function.
 *
 * The macro shoudl be called as a first instruction of the traced function.
 *
 * @param timeline a timeline where the event will be posted to.
 */
#define HT_TP_FUNCTION(timeline) \
    HT_TP_STRACEPOINT(timeline, __func__)

#endif /* HAWKTRACER_STRING_SCOPED_TRACEPOINT_H */
