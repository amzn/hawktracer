#ifndef HAWKTRACER_EVENT_MACROS_H
#define HAWKTRACER_EVENT_MACROS_H

#include <hawktracer/mkcreflect.h>

#define HT_DECLARE_EVENT_KLASS(TYPE_NAME, BASE_TYPE, ...) \
    MKCREFLECT_DEFINE_STRUCT(TYPE_NAME, \
                             (STRUCT, BASE_TYPE, base), \
                             MKCREFLECT_EXPAND_VA_(__VA_ARGS__)) \
    HT_EventKlass* ht_##TYPE_NAME##_get_instance_klass(void); \
    size_t ht_##TYPE_NAME##_serialize(HT_Event* event, HT_Byte* buffer); \
    void ht_##TYPE_NAME##_register_event(void);

#define HT_DEFAULT_SERIALIZED_SIZEOF_(TYPE_NAME) \
    mkcreflect_get_##TYPE_NAME##_type_info()->packed_size - sizeof(val.base) + \
        HT_EVENT_GET_CLASS(&val)->compressed_size;

#define HT_DEFINE_EVENT_KLASS(TYPE_NAME, EVENT_TYPE) \
    HT_DEFINE_EVENT_KLASS_DETAILED(TYPE_NAME, EVENT_TYPE, HT_DEFAULT_SERIALIZED_SIZEOF_(TYPE_NAME))

#define HT_DEFINE_EVENT_KLASS_DETAILED(TYPE_NAME, EVENT_TYPE, SERIALIZED_SIZEOF) \
    HT_EventKlass* ht_##TYPE_NAME##_get_instance_klass(void) \
    { \
        static HT_EventKlass instance_klass = { \
            NULL, \
            ht_##TYPE_NAME##_serialize, \
            0, \
            EVENT_TYPE \
        }; \
        return &instance_klass; \
    } \
    void ht_##TYPE_NAME##_register_event(void) \
    { \
        HT_DECL_EVENT(TYPE_NAME, val); \
        ht_##TYPE_NAME##_get_instance_klass()->type_info = mkcreflect_get_##TYPE_NAME##_type_info(); \
        ht_##TYPE_NAME##_get_instance_klass()->compressed_size = SERIALIZED_SIZEOF; \
    }

#define HT_DECL_EVENT(EventStruct, event) \
    EventStruct event; HT_EVENT(&event)->klass = ht_##EventStruct##_get_instance_klass();

#define HT_EVENT_SERIALIZE(event, buffer) \
    do { \
        if (HT_EVENT_GET_CLASS(event)->serialize != NULL) \
            HT_EVENT_GET_CLASS(event)->serialize(HT_EVENT(event), buffer); \
    } while (0)

#endif /* HAWKTRACER_EVENT_MACROS_H */
