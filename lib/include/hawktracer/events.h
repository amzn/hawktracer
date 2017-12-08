#ifndef HAWKTRACER_EVENTS_H
#define HAWKTRACER_EVENTS_H

#include <hawktracer/base_types.h>
#include <hawktracer/event_macros.h>

#include <stddef.h>

HT_DECLS_BEGIN

typedef struct _HT_EventKlass HT_EventKlass;

MKCREFLECT_DEFINE_STRUCT(HT_Event,
                         (POINTER, HT_EventKlass*, klass),
                         (INTEGER, HT_TimestampNs, timestamp),
                         (INTEGER, HT_EventId, id))

HT_EventKlass* ht_HT_Event_get_event_klass_instance(void);
void ht_HT_Event_register_event_klass(void);
size_t ht_HT_Event_serialize(HT_Event* event, HT_Byte* buffer);
#define HT_EVENT(event) ((HT_Event*)event)

struct _HT_EventKlass
{
    MKCREFLECT_TypeInfo* type_info;
    size_t (*serialize)(HT_Event* event, HT_Byte* buffer);
    size_t compressed_size;
    HT_EventType type;
};
#define HT_EVENT_GET_CLASS(event) (((HT_Event*)event)->klass)

HT_DECLARE_EVENT_KLASS(HT_CallstackBaseEvent, HT_Event,
                       (INTEGER, HT_DurationNs, duration),
                       (INTEGER, HT_ThreadId, thread_id))
#define HT_CALLSTACK_BASE_EVENT(event) ((HT_CallstackBaseEvent*)event)

typedef uint64_t HT_CallstackEventLabel;
HT_DECLARE_EVENT_KLASS(HT_CallstackIntEvent, HT_CallstackBaseEvent,
                       (INTEGER, HT_CallstackEventLabel, label))

#define HT_CALLSTACK_LABEL_MAX_LEN 31u
HT_DECLARE_EVENT_KLASS(HT_CallstackStringEvent, HT_CallstackBaseEvent,
                       (STRING, char, label, HT_CALLSTACK_LABEL_MAX_LEN + 1))

HT_DECLS_END

#endif /* HAWKTRACER_EVENTS_H */
