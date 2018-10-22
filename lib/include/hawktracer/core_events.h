#ifndef HAWKTRACER_CORE_EVENTS_H
#define HAWKTRACER_CORE_EVENTS_H

#include <hawktracer/event_macros.h>

HT_DECLS_BEGIN

HT_DECLARE_EVENT_KLASS(HT_EndiannessInfoEvent, HT_Event,
                       (INTEGER, uint8_t, endianness))

HT_DECLARE_EVENT_KLASS(HT_EventKlassInfoEvent, HT_Event,
                       (INTEGER, HT_EventKlassId, info_klass_id),
                       (STRING, const char*, event_klass_name),
                       (INTEGER, uint8_t, field_count))
HT_DECLARE_EVENT_KLASS(HT_EventKlassFieldInfoEvent, HT_Event,
                       (INTEGER, HT_EventKlassId, info_klass_id),
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

HT_DECLARE_EVENT_KLASS(HT_DurationBaseEvent, HT_Event,
                       (INTEGER, HT_DurationNs, duration))
#define HT_DURATION_BASE_EVENT(event) ((HT_DurationBaseEvent*)event)

typedef uint64_t HT_DurationEventLabel;
HT_DECLARE_EVENT_KLASS(HT_DurationIntEvent, HT_DurationBaseEvent,
                       (INTEGER, HT_DurationEventLabel, label))

HT_DECLARE_EVENT_KLASS(HT_DurationStringEvent, HT_DurationBaseEvent,
                       (STRING, const char*, label))

HT_DECLARE_EVENT_KLASS(HT_StringMappingEvent, HT_Event,
                       (INTEGER, uint64_t, identifier),
                       (STRING, const char*, label))

HT_DECLARE_EVENT_KLASS(HT_SystemInfoEvent, HT_Event,
                       (INTEGER, uint8_t, version_major),
                       (INTEGER, uint8_t, version_minor),
                       (INTEGER, uint8_t, version_patch))

HT_DECLS_END

#endif /* HAWKTRACER_CORE_EVENTS_H */
