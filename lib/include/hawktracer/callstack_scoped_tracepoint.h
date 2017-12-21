#ifndef HAWKTRACER_CALLSTACK_SCOPED_TRACEPOINT_H
#define HAWKTRACER_CALLSTACK_SCOPED_TRACEPOINT_H

#include <hawktracer/feature_callstack.h>

#if defined (__cplusplus)

#include <hawktracer/callstack_scoped_tracepoint.hpp>

#define HT_TP_SCOPED_INT(timeline, label) \
    HawkTracer::CallstackScopedTracepoint<HT_CallstackEventLabel> _ht_scoped_tracepoint(timeline, ht_feature_callstack_start_int, label)

#define HT_TP_SCOPED_STRING(timeline, label) \
    HawkTracer::CallstackScopedTracepoint<const char*> _ht_scoped_tracepoint(timeline, ht_feature_callstack_start_string, label)

#elif defined(__GNUC__)

static void __attribute__((unused)) _ht_callstack_timeline_scoped_cleanup(HT_Timeline** timeline)
{
    ht_feature_callstack_stop(*timeline);
}

#define _HT_GENERATE_CALLSTACK_TIMELINE_CALLSTACK_RET(type, c_type) \
    inline static HT_Timeline* _ht_callstack_timeline_##type##_start_and_ret(HT_Timeline* t, c_type l) \
    { ht_feature_callstack_start_##type(t, l); return t; }

_HT_GENERATE_CALLSTACK_TIMELINE_CALLSTACK_RET(int, HT_CallstackEventLabel)
_HT_GENERATE_CALLSTACK_TIMELINE_CALLSTACK_RET(string, const char*)

#define _HT_TP_SCOPED_GENERIC(type, callstack_timeline, label) \
    HT_CallstackTimeline* _ht_callstack_timeline __attribute__ ((__cleanup__(_ht_callstack_timeline_scoped_cleanup))) \
        = _ht_callstack_timeline_##type##_start_and_ret(callstack_timeline, label)

#define HT_TP_SCOPED_INT(callstack_timeline, label) \
    _HT_TP_SCOPED_GENERIC(int, callstack_timeline, label)

#define HT_TP_SCOPED_STRING(callstack_timeline, label) \
    _HT_TP_SCOPED_GENERIC(string, callstack_timeline, label)

#endif

#endif /* HAWKTRACER_CALLSTACK_SCOPED_TRACEPOINT_H */
