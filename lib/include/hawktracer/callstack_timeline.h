#ifndef HAWKTRACER_CALLSTACK_TIMELINE_H
#define HAWKTRACER_CALLSTACK_TIMELINE_H

#include <hawktracer/timeline.h>
#include <hawktracer/core_events.h>
#include <hawktracer/stack.h>

HT_DECLS_BEGIN


#define HT_CALLSTACK_BASE_TIMELINE(timeline) ((HT_CallstackTimeline*)timeline)

typedef struct
{
    HT_BaseTimeline base;

    HT_Stack stack;
} HT_CallstackTimeline;

void ht_callstack_timeline_init(HT_BaseTimeline* timeline, va_list args);

void ht_callstack_timeline_deinit(HT_BaseTimeline* timeline);

void ht_callstack_timeline_start(HT_CallstackTimeline* timeline, HT_CallstackBaseEvent* event);

void ht_callstack_timeline_stop(HT_CallstackTimeline* timeline);

void ht_callstack_timeline_int_start(HT_CallstackTimeline* timeline, HT_CallstackEventLabel label);

void ht_callstack_timeline_string_start(HT_CallstackTimeline* timeline, const char* label);

HT_DECLS_END

#if defined (__cplusplus)

#include <hawktracer/callstack_scoped_tracepoint.hpp>

#define HT_TP_SCOPED_INT(callstack_timeline, label) \
    HawkTracer::CallstackScopedTracepoint<HT_CallstackEventLabel> _ht_scoped_tracepoint(callstack_timeline, ht_callstack_timeline_int_start, label)

#define HT_TP_SCOPED_STRING(callstack_timeline, label) \
    HawkTracer::CallstackScopedTracepoint<const char*> _ht_scoped_tracepoint(callstack_timeline, ht_callstack_timeline_string_start, label)

#elif defined(__GNUC__)

static void __attribute__((unused)) _ht_callstack_timeline_scoped_cleanup(HT_CallstackTimeline** timeline)
{
    ht_callstack_timeline_stop(*timeline);
}

#define _HT_GENERATE_CALLSTACK_TIMELINE_CALLSTACK_RET(type, c_type) \
    inline static HT_CallstackTimeline* _ht_callstack_timeline_##type##_start_and_ret(HT_CallstackTimeline* t, c_type l) \
    { ht_callstack_timeline_##type##_start(t, l); return t; }

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

#endif /* HAWKTRACER_CALLSTACK_TIMELINE_H */
