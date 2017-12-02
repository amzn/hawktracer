#ifndef HAWKTRACER_EVENTS_H
#define HAWKTRACER_EVENTS_H

#include <hawktracer/base_types.h>

#include <stddef.h>

HT_DECLS_BEGIN

#define HT_EVENT_GET_CLASS(event) (((HT_Event*)event)->klass)

#define HT_EVENT_TYPE_RESERVED_FIRST (1)
#define HT_EVENT_TYPE_RESERVED_LAST (128)

#define HT_DEFINE_EVENT_KLASS(EventStruct, event_type) \
    HT_DEFINE_EVENT_KLASS_DETAILED(EventStruct, event_type, ht_##EventStruct##_serialize)

#define HT_DEFINE_EVENT_KLASS_DETAILED(EventStruct, event_type, serialize_method) \
    HT_EventKlass instance_klass_##EventStruct = { \
    sizeof(EventStruct), \
    serialize_method, \
    event_type \
    }

#define HT_DECLARE_EVENT_KLASS(EventStruct) \
    extern HT_EventKlass instance_klass_##EventStruct; \
    size_t ht_##EventStruct##_serialize(HT_Event* event, HT_Byte* buffer);

#define HT_DECL_EVENT(EventStruct, event) \
    EventStruct event; HT_EVENT(&event)->klass = &instance_klass_##EventStruct;

#define HT_EVENT_SERIALIZE(event, buffer) \
    do { \
        if (HT_EVENT_GET_CLASS(event)->serialize != NULL) \
            HT_EVENT_GET_CLASS(event)->serialize(HT_EVENT(event), buffer); \
    } while (0)


typedef enum
{
    HT_EVENT_TYPE_BASE = 1,
    HT_EVENT_TYPE_CALLSTACK_BASE,
    HT_EVENT_TYPE_CALLSTACK_INT,
    HT_EVENT_TYPE_CALLSTACK_STRING
} HT_CoreEventType;

typedef struct _HT_Event HT_Event;

typedef struct
{
    size_t event_size;
    size_t (*serialize)(HT_Event* event, HT_Byte* buffer);
    HT_EventType type;
} HT_EventKlass;

struct _HT_Event
{
    HT_EventKlass* klass;

    HT_TimestampNs timestamp;
    HT_EventId id;
};
HT_DECLARE_EVENT_KLASS(HT_Event)
#define HT_EVENT(event) ((HT_Event*)event)

typedef struct
{
    HT_Event base;

    HT_DurationNs duration;
    HT_ThreadId thread_id;
} HT_CallstackBaseEvent;
HT_DECLARE_EVENT_KLASS(HT_CallstackBaseEvent)
#define HT_CALLSTACK_BASE_EVENT(event) ((HT_CallstackBaseEvent*)event)

typedef uint64_t HT_CallstackEventLabel;

typedef struct
{
    HT_CallstackBaseEvent base;

    HT_CallstackEventLabel label;
} HT_CallstackIntEvent;
HT_DECLARE_EVENT_KLASS(HT_CallstackIntEvent)
#define HT_CALLSTACK_INT_EVENT(event) ((HT_CallstackIntEvent*)event)

#define HT_CALLSTACK_LABEL_MAX_LEN 31u

typedef struct
{
    HT_CallstackBaseEvent base;

    char label[HT_CALLSTACK_LABEL_MAX_LEN + 1];
} HT_CallstackStringEvent;
HT_DECLARE_EVENT_KLASS(HT_CallstackStringEvent)
#define HT_CALLSTACK_STRING_EVENT(event) ((HT_CallstackStringEvent*)event)

HT_DECLS_END

#endif /* HAWKTRACER_EVENTS_H */
