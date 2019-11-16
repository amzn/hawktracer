#ifndef HAWKTRACER_GLOBAL_TIMELINE_H
#define HAWKTRACER_GLOBAL_TIMELINE_H

#include <hawktracer/scoped_tracepoint.h>
#include <hawktracer/feature_cached_string.h>
#include <hawktracer/tracepoint.h>

HT_DECLS_BEGIN

/**
 * Gets a Global Timeline for the current thread.
 *
 * @return a pointer to the Global Timeline of the current thread.
 */
HT_API HT_Timeline* ht_global_timeline_get(void);

HT_DECLS_END

#define HT_TP_GLOBAL_SCOPED_INT(int_label) HT_TP_SCOPED_INT(ht_global_timeline_get(), int_label)

/**
 * @deprecated Use HT_G_TRACE() instead
 */
#define HT_TP_GLOBAL_SCOPED_STRING(string_label) HT_TP_SCOPED_STRING(ht_global_timeline_get(), string_label)

/**
 * @deprecated Use HT_G_TRACE_OPT_STATIC() instead
 */
#define HT_TP_G_STRACEPOINT(label) HT_TP_STRACEPOINT(ht_global_timeline_get(), label)

/**
 * @deprecated Use HT_G_TRACE_OPT_DYNAMIC() instead
 */
#define HT_TP_G_DYN_STRACEPOINT(label) HT_TP_DYN_STRACEPOINT(ht_global_timeline_get(), label)

/**
 * @deprecated Use HT_G_TRACE_FUNCTION() instead
 */
#define HT_TP_G_FUNCTION() HT_TP_FUNCTION(ht_global_timeline_get())

/**
 * Simplified version of the HT_TRACE() macro for the Global Timeline.
 */
#define HT_G_TRACE(string_label) HT_TRACE(ht_global_timeline_get(), string_label)

#ifdef HT_TP_STRACEPOINT
    /**
     * Simplified version of the HT_TRACE_FUNCTION() (or HT_TRACE_FUNCTION_OPT(), depending
     * on the environment) macro for the Global Timeline.
     */
    #define HT_G_TRACE_FUNCTION() HT_TRACE_FUNCTION_OPT(ht_global_timeline_get())
    /**
     * Simplified version of the HT_TRACE_OPT_STATIC() macro for the Global Timeline.
     */
    #define HT_G_TRACE_OPT_STATIC(static_string_label) HT_TRACE_OPT_STATIC(ht_global_timeline_get(), static_string_label)
#else
    #define HT_G_TRACE_FUNCTION() HT_TRACE_FUNCTION(ht_global_timeline_get())
#endif

/**
 * Simplified version of the HT_G_TRACE_OPT_DYNAMIC() macro for the Global Timeline.
 */
#define HT_G_TRACE_OPT_DYNAMIC(static_string_label) HT_TRACE_OPT_DYNAMIC(ht_global_timeline_get(), static_string_label)


#endif /* HAWKTRACER_GLOBAL_TIMELINE_H */
