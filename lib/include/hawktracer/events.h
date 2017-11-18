#ifndef HAWKTRACER_EVENTS_H
#define HAWKTRACER_EVENTS_H

#include <hawktracer/macros.h>
#include <hawktracer/base_types.h>

#include <stddef.h>

HT_DECLS_BEGIN

typedef struct
{
    size_t size;
    HT_EventType type;
} HT_EventKlass;

#define HT_EVENT_GET_CLASS(event) (((HT_Event*)event)->klass)
#define HT_EVENT(event) ((HT_Event*)event)

#define HT_DEFINE_EVENT_KLASS(EventStruct, event_type) \
    HT_EventKlass instance_klass_##EventStruct = { \
    sizeof(EventStruct), \
    event_type \
    }
#define HT_DECLARE_EVENT_KLASS(EventStruct) extern HT_EventKlass instance_klass_##EventStruct

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
} HT_DurationEvent;
HT_DECLARE_EVENT_KLASS(HT_DurationEvent);

#define HT_DECL_EVENT(EventStruct, event) \
    EventStruct event; HT_EVENT(&event)->klass = &instance_klass_##EventStruct;

void _ht_event_init(HT_EventType* type);

HT_DECLS_END

#endif /* HAWKTRACER_EVENTS_H */
