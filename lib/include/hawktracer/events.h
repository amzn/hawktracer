#ifndef HAWKTRACER_EVENTS_H
#define HAWKTRACER_EVENTS_H

#include <hawktracer/base_types.h>
#include <hawktracer/event_macros.h>

#include <stddef.h>

HT_DECLS_BEGIN

#define HT_EVENT_TYPE_RESERVED_FIRST (1)
#define HT_EVENT_TYPE_RESERVED_LAST (128)

typedef enum
{
    HT_EVENT_TYPE_BASE = 1,
    HT_EVENT_TYPE_CALLSTACK_BASE,
    HT_EVENT_TYPE_CALLSTACK_INT,
    HT_EVENT_TYPE_CALLSTACK_STRING
} HT_CoreEventType;

typedef struct _HT_EventKlass HT_EventKlass;

HT_DECLARE_EVENT_KLASS(HT_Event,
                       (POINTER, HT_EventKlass*, klass),
                       (INTEGER, HT_TimestampNs, timestamp),
                       (INTEGER, HT_EventId, id))
#define HT_EVENT(event) ((HT_Event*)event)

struct _HT_EventKlass
{
    MKCREFLECT_TypeInfo* type_info;
    size_t (*serialize)(HT_Event* event, HT_Byte* buffer);
    HT_EventType type;
};
#define HT_EVENT_GET_CLASS(event) (((HT_Event*)event)->klass)

HT_DECLARE_EVENT_KLASS(HT_CallstackBaseEvent,
                       (STRUCT, HT_Event, base),
                       (INTEGER, HT_DurationNs, duration),
                       (INTEGER, HT_ThreadId, thread_id))
#define HT_CALLSTACK_BASE_EVENT(event) ((HT_CallstackBaseEvent*)event)

typedef uint64_t HT_CallstackEventLabel;
HT_DECLARE_EVENT_KLASS(HT_CallstackIntEvent,
                       (STRUCT, HT_CallstackBaseEvent, base),
                       (INTEGER, HT_CallstackEventLabel, label))

#define HT_CALLSTACK_LABEL_MAX_LEN 31u
HT_DECLARE_EVENT_KLASS(HT_CallstackStringEvent,
                       (STRUCT, HT_CallstackBaseEvent, base),
                       (STRING, char, label, HT_CALLSTACK_LABEL_MAX_LEN + 1))

HT_DECLS_END

#endif /* HAWKTRACER_EVENTS_H */
