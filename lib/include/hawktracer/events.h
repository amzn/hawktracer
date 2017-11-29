#ifndef HAWKTRACER_EVENTS_H
#define HAWKTRACER_EVENTS_H

#include <hawktracer/base_types.h>

#include <stddef.h>

HT_DECLS_BEGIN

#define HT_EVENT_GET_CLASS(event) (((HT_Event*)event)->klass)
#define HT_EVENT(event) ((HT_Event*)event)

#define HT_EVENT_TYPE_RESERVED_FIRST (1)
#define HT_EVENT_TYPE_RESERVED_LAST (128)

#define HT_DEFINE_EVENT_KLASS(EventStruct, event_type) \
    HT_EventKlass instance_klass_##EventStruct = { \
    sizeof(EventStruct), \
    event_type \
    }

#define HT_DECLARE_EVENT_KLASS(EventStruct) extern HT_EventKlass instance_klass_##EventStruct

#define HT_DECL_EVENT(EventStruct, event) \
    EventStruct event; HT_EVENT(&event)->klass = &instance_klass_##EventStruct;

typedef enum
{
    HT_EVENT_TYPE_BASE,
    HT_EVENT_TYPE_CALLSTACK_BASE,
    HT_EVENT_TYPE_CALLSTACK_INT,
    HT_EVENT_TYPE_CALLSTACK_STRING
} HT_CoreEventType;

typedef struct
{
    size_t event_size;
    HT_EventType type;
} HT_EventKlass;

typedef struct
{
    HT_EventKlass* klass;

    HT_TimestampNs timestamp;
    HT_EventId id;
} HT_Event;
HT_DECLARE_EVENT_KLASS(HT_Event);

typedef struct
{
    HT_Event base;

    HT_DurationNs duration;
    HT_ThreadId thread_id;
} HT_CallstackBaseEvent;
HT_DECLARE_EVENT_KLASS(HT_CallstackBaseEvent);

typedef uint64_t HT_CallstackEventLabel;

typedef struct
{
    HT_CallstackBaseEvent base;

    HT_CallstackEventLabel label;
} HT_CallstackIntEvent;
HT_DECLARE_EVENT_KLASS(HT_CallstackIntEvent);

#define HT_CALLSTACK_LABEL_MAX_LEN 31u

typedef struct
{
    HT_CallstackBaseEvent base;

    char label[HT_CALLSTACK_LABEL_MAX_LEN + 1];
} HT_CallstackStringEvent;
HT_DECLARE_EVENT_KLASS(HT_CallstackStringEvent);

HT_DECLS_END

#endif /* HAWKTRACER_EVENTS_H */
