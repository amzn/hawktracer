#ifndef HAWKTRACER_CALLSTACK_TIMELINE_H
#define HAWKTRACER_CALLSTACK_TIMELINE_H

#include <hawktracer/callstack_base_timeline.h>

HT_DECLS_BEGIN

void ht_callstack_timeline_int_start(HT_CallstackBaseTimeline* timeline, HT_CallstackEventLabel label);

void ht_callstack_timeline_string_start(HT_CallstackBaseTimeline* timeline, const char* label);

HT_DECLS_END

#if defined (__cplusplus)

#include <hawktracer/callstack_scoped_tracepoint.hpp>

#define HT_TP_SCOPED_INT(callstack_timeline, label) \
    HawkTracer::CallstackScopedTracepoint<HT_CallstackEventLabel> _ht_scoped_tracepoint(callstack_timeline, ht_callstack_timeline_int_start, label)

#define HT_TP_SCOPED_STRING(callstack_timeline, label) \
    HawkTracer::CallstackScopedTracepoint<const char*> _ht_scoped_tracepoint(callstack_timeline, ht_callstack_timeline_string_start, label)

#elif defined(__GNUC__)

static void __attribute__((unused)) _ht_callstack_timeline_scoped_cleanup(HT_CallstackBaseTimeline** timeline)
{
    ht_callstack_base_timeline_stop(*timeline);
}

#define _HT_GENERATE_CALLSTACK_TIMELINE_CALLSTACK_RET(type, c_type) \
    inline static HT_CallstackBaseTimeline* _ht_callstack_timeline_##type##_start_and_ret(HT_CallstackBaseTimeline* t, c_type l) \
    { ht_callstack_timeline_##type##_start(t, l); return t; }

_HT_GENERATE_CALLSTACK_TIMELINE_CALLSTACK_RET(int, HT_CallstackEventLabel)
_HT_GENERATE_CALLSTACK_TIMELINE_CALLSTACK_RET(string, const char*)

#define _HT_TP_SCOPED_GENERIC(type, callstack_timeline, label) \
    HT_CallstackBaseTimeline* _ht_callstack_timeline __attribute__ ((__cleanup__(_ht_callstack_timeline_scoped_cleanup))) \
        = _ht_callstack_timeline_##type##_start_and_ret(callstack_timeline, label)

#define HT_TP_SCOPED_INT(callstack_timeline, label) \
    _HT_TP_SCOPED_GENERIC(int, callstack_timeline, label)

#define HT_TP_SCOPED_STRING(callstack_timeline, label) \
    _HT_TP_SCOPED_GENERIC(string, callstack_timeline, label)

#endif

#endif /* HAWKTRACER_CALLSTACK_TIMELINE_H */
