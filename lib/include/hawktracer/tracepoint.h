#ifndef HAWKTRACER_TRACEPOINT_H
#define HAWKTRACER_TRACEPOINT_H

#include <hawktracer/string_scoped_tracepoint.h>

#if defined(HT_SCOPED_TRACEPOINT_MACRO_ENABLED)

    #define HT_TRACE(timeline, string_label) \
        HT_TP_SCOPED_STRING(timeline, string_label)

    #define HT_TRACE_OPT_STATIC(timeline, static_string_label) \
        HT_TP_STRACEPOINT(timeline, static_string_label)

    #define HT_TRACE_OPT_DYNAMIC(timeline, dynamic_string_label) \
        HT_TP_DYN_STRACEPOINT(timeline, dynamic_string_label)

    #define HT_TRACE_FUNCTION(timeline) \
        HT_TRACE(timeline, __func__)

    #define HT_TRACE_FUNCTION_OPT(timeline) \
        HT_TP_FUNCTION(timeline)

#endif /* HT_SCOPED_TRACEPOINT_MACRO_ENABLED */

#endif /* HAWKTRACER_TACEPOINT_H */
