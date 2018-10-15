#ifndef HAWKTRACER_EVENTS_H
#define HAWKTRACER_EVENTS_H

#include <hawktracer/base_types.h>
#include <hawktracer/mkcreflect.h>

#include <stddef.h>

HT_DECLS_BEGIN

typedef struct _HT_EventKlass HT_EventKlass;

MKCREFLECT_DEFINE_STRUCT(HT_Event,
                         (POINTER, HT_EventKlass*, klass),
                         (INTEGER, HT_TimestampNs, timestamp),
                         (INTEGER, HT_EventId, id))

HT_API HT_EventKlass* ht_HT_Event_get_event_klass_instance(void);
HT_API HT_EventKlassId ht_HT_Event_register_event_klass(void);
HT_API size_t ht_HT_Event_get_size(HT_Event* event);
HT_API size_t ht_HT_Event_fnc_serialize(HT_Event* event, HT_Byte* buffer);

#define HT_EVENT(event) ((HT_Event*)(event))

struct _HT_EventKlass
{
    MKCREFLECT_TypeInfo* type_info;
    size_t (*serialize)(HT_Event* event, HT_Byte* buffer);
    size_t (*get_size)(HT_Event* event);
    HT_EventKlassId klass_id;
};

#define HT_REGISTER_EVENT_KLASS(EVENT_TYPE) ht_##EVENT_TYPE##_register_event_klass()

#define HT_EVENT_GET_KLASS(event) (((HT_Event*)event)->klass)
#define HT_EVENT_KLASS_GET(EVENT_TYPE) ht_##EVENT_TYPE##_get_event_klass_instance()

#define HT_DECL_EVENT(TYPE_NAME, event) \
    TYPE_NAME event; HT_EVENT(&event)->klass = HT_EVENT_KLASS_GET(TYPE_NAME);

HT_DECLS_END

#endif /* HAWKTRACER_EVENTS_H */
