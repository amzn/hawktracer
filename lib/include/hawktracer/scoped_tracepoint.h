#ifndef HAWKTRACER_SCOPED_TRACEPOINT_H
#define HAWKTRACER_SCOPED_TRACEPOINT_H

#include <hawktracer/feature_callstack.h>

#if defined (__cplusplus)
#define HT_HAS_SCOPED_TRACEPOINT

#include <hawktracer/scoped_tracepoint.hpp>

#define _HT_TP_SCOPED_GENERIC(type, c_type, timeline, label) \
    HawkTracer::ScopedTracepoint<c_type> HT_UNIQUE_VAR_NAME(ht_tp_scoped_tracepoint)(timeline, ht_feature_callstack_start_##type, label)

#elif defined(__GNUC__)
#define HT_HAS_SCOPED_TRACEPOINT

HT_DECLS_BEGIN
void _ht_callstack_timeline_scoped_cleanup(HT_Timeline** timeline);
HT_Timeline* _ht_callstack_timeline_int_start_and_ret(HT_Timeline* t, HT_CallstackEventLabel l);
HT_Timeline* _ht_callstack_timeline_string_start_and_ret(HT_Timeline* t, const char* l);
HT_DECLS_END

#define _HT_TP_SCOPED_GENERIC(type, c_type, callstack_timeline, label) \
    HT_Timeline* _ht_callstack_timeline __attribute__ ((__cleanup__(_ht_callstack_timeline_scoped_cleanup))) \
        = _ht_callstack_timeline_##type##_start_and_ret(callstack_timeline, label)

#endif

#ifdef HT_HAS_SCOPED_TRACEPOINT

#define HT_TP_SCOPED_INT(timeline, label) _HT_TP_SCOPED_GENERIC(int, HT_CallstackEventLabel, timeline, label)
#define HT_TP_SCOPED_STRING(timeline, label) _HT_TP_SCOPED_GENERIC(string, const char*, timeline, label)

#endif /* HT_HAS_SCOPED_TRACEPOINT */

#endif /* HAWKTRACER_SCOPED_TRACEPOINT_H */
