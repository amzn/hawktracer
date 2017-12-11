#ifndef HAWKTRACER_CORE_EVENTS_H
#define HAWKTRACER_CORE_EVENTS_H

#include <hawktracer/event_macros.h>

HT_DECLS_BEGIN

HT_DECLARE_EVENT_KLASS(HT_EventKlassInfoEvent, HT_Event,
                       (INTEGER, HT_EventType, event_type),
                       (STRING, const char*, event_klass_name),
                       (INTEGER, int8_t, field_count))
HT_DECLARE_EVENT_KLASS(HT_EventKlassFieldInfoEvent, HT_Event,
                       (INTEGER, HT_EventType, event_type),
                       (STRING, const char*, field_type),
                       (STRING, const char*, field_name),
                       (INTEGER, uint64_t, size),
                       (INTEGER, uint8_t, data_type))

HT_DECLARE_EVENT_KLASS(HT_CallstackBaseEvent, HT_Event,
                       (INTEGER, HT_DurationNs, duration),
                       (INTEGER, HT_ThreadId, thread_id))
#define HT_CALLSTACK_BASE_EVENT(event) ((HT_CallstackBaseEvent*)event)

typedef uint64_t HT_CallstackEventLabel;
HT_DECLARE_EVENT_KLASS(HT_CallstackIntEvent, HT_CallstackBaseEvent,
                       (INTEGER, HT_CallstackEventLabel, label))

HT_DECLARE_EVENT_KLASS(HT_CallstackStringEvent, HT_CallstackBaseEvent,
                       (STRING, const char*, label))

HT_DECLS_END

#endif /* HAWKTRACER_CORE_EVENTS_H */
