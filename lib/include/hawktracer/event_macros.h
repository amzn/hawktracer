#ifndef HAWKTRACER_EVENT_MACROS_H
#define HAWKTRACER_EVENT_MACROS_H

#include <hawktracer/mkcreflect.h>
#include <string.h>

#define HT_EVENT_GET_SIZE_FUNCTION(C_TYPE) ht_##C_TYPE##_get_size

#define HT_EVENT_RUNTIME_SIZEOF_STRUCT(VAR_NAME, FIELD, C_TYPE) HT_EVENT_GET_SIZE_FUNCTION(C_TYPE)(((HT_Event*)&VAR_NAME->FIELD))
#define HT_EVENT_RUNTIME_SIZEOF_INTEGER(VAR_NAME, FIELD, C_TYPE) sizeof(VAR_NAME->FIELD)
#define HT_EVENT_RUNTIME_SIZEOF_STRING(VAR_NAME, FIELD, C_TYPE) (VAR_NAME->FIELD ? strlen(VAR_NAME->FIELD) : 0)
#define HT_EVENT_RUNTIME_SIZEOF__(VAR_NAME, DATA_TYPE, C_TYPE, FIELD) +HT_EVENT_RUNTIME_SIZEOF_##DATA_TYPE(VAR_NAME, FIELD, C_TYPE)
#define HT_EVENT_RUNTIME_SIZEOF_(...) HT_EVENT_RUNTIME_SIZEOF__(__VA_ARGS__)
#define HT_EVENT_RUNTIME_SIZEOF(X, USER_DATA) HT_EVENT_RUNTIME_SIZEOF_(USER_DATA, MKCREFLECT_EXPAND_VA_ X)

#define HT_DECLARE_EVENT_KLASS(TYPE_NAME, BASE_TYPE, ...) \
    MKCREFLECT_DEFINE_STRUCT(TYPE_NAME, \
                             (STRUCT, BASE_TYPE, base), \
                             MKCREFLECT_EXPAND_VA_(__VA_ARGS__)) \
    HT_EventKlass* ht_##TYPE_NAME##_get_event_klass_instance(void); \
    size_t ht_##TYPE_NAME##_serialize(HT_Event* event, HT_Byte* buffer); \
    void ht_##TYPE_NAME##_register_event_klass(void); \
    inline static size_t HT_EVENT_GET_SIZE_FUNCTION(TYPE_NAME)(HT_Event* VAR_NAME) \
    { \
        return MKCREFLECT_FOREACH(HT_EVENT_RUNTIME_SIZEOF, ((TYPE_NAME*)VAR_NAME), (STRUCT, BASE_TYPE, base), __VA_ARGS__); \
    }

#define HT_DEFINE_EVENT_KLASS(TYPE_NAME) \
    HT_DEFINE_EVENT_KLASS_DETAILED(TYPE_NAME)

#define HT_DEFINE_EVENT_KLASS_DETAILED(TYPE_NAME) \
    HT_EventKlass* ht_##TYPE_NAME##_get_event_klass_instance(void) \
    { \
        static HT_EventKlass klass_instance = { \
            NULL, \
            ht_##TYPE_NAME##_serialize, \
            0, \
            0 \
        }; \
        return &klass_instance; \
    } \
    void ht_##TYPE_NAME##_register_event_klass(void) \
    { \
        HT_DECL_EVENT(TYPE_NAME, val); \
        ht_##TYPE_NAME##_get_event_klass_instance()->type_info = mkcreflect_get_##TYPE_NAME##_type_info(); \
        ht_##TYPE_NAME##_get_event_klass_instance()->get_size = HT_EVENT_GET_SIZE_FUNCTION(TYPE_NAME); \
        ht_registry_register_event_klass(ht_##TYPE_NAME##_get_event_klass_instance()); \
    }

#define HT_DECL_EVENT(TYPE_NAME, event) \
    TYPE_NAME event; HT_EVENT(&event)->klass = ht_##TYPE_NAME##_get_event_klass_instance();

#define HT_EVENT_SERIALIZE(event, buffer) \
    do { \
        if (HT_EVENT_GET_CLASS(event)->serialize != NULL) \
            HT_EVENT_GET_CLASS(event)->serialize(HT_EVENT(event), buffer); \
    } while (0)

#endif /* HAWKTRACER_EVENT_MACROS_H */
