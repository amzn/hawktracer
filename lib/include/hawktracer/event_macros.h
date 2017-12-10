#ifndef HAWKTRACER_EVENT_MACROS_H
#define HAWKTRACER_EVENT_MACROS_H

#include <hawktracer/mkcreflect.h>
#include <string.h>

#define HT_EVENT_GET_SIZE_FUNCTION(C_TYPE) ht_##C_TYPE##_get_size
#define HT_EVENT_SERIALIZE_FUNCTION(C_TYPE) ht_##C_TYPE##_fnc_serialize

#define HT_EVENT_RUNTIME_SIZEOF_STRUCT(VAR_NAME, FIELD, C_TYPE) HT_EVENT_GET_SIZE_FUNCTION(C_TYPE)(((HT_Event*)&VAR_NAME->FIELD))
#define HT_EVENT_RUNTIME_SIZEOF_INTEGER(VAR_NAME, FIELD, C_TYPE) sizeof(VAR_NAME->FIELD)
#define HT_EVENT_RUNTIME_SIZEOF_STRING(VAR_NAME, FIELD, C_TYPE) (VAR_NAME->FIELD ? strlen(VAR_NAME->FIELD) : 0)
#define HT_EVENT_RUNTIME_SIZEOF__(VAR_NAME, DATA_TYPE, C_TYPE, FIELD, ...) +HT_EVENT_RUNTIME_SIZEOF_##DATA_TYPE(VAR_NAME, FIELD, C_TYPE)
#define HT_EVENT_RUNTIME_SIZEOF_(...) HT_EVENT_RUNTIME_SIZEOF__(__VA_ARGS__, 0)
#define HT_EVENT_RUNTIME_SIZEOF(X, USER_DATA) HT_EVENT_RUNTIME_SIZEOF_(USER_DATA, MKCREFLECT_EXPAND_VA_ X)


#define HT_EVENT_RUNTIME_SERIALIZE_STRUCT(C_TYPE, FIELD) offset += HT_EVENT_SERIALIZE_FUNCTION(C_TYPE)(((HT_Event*)&VAR_NAME->FIELD), buffer);
#define HT_EVENT_RUNTIME_SERIALIZE_INTEGER(C_TYPE, FIELD) memcpy(buffer + offset, (char*)&VAR_NAME->FIELD, sizeof(VAR_NAME->FIELD)), offset += sizeof(VAR_NAME->FIELD);
#define HT_EVENT_RUNTIME_SERIALIZE_STRING(C_TYPE, FIELD) do {\
    size_t len = (VAR_NAME->FIELD) ? strlen(VAR_NAME->FIELD) : 0; /* TODO should not copy if zero */ \
    memcpy(buffer + offset, VAR_NAME->FIELD, len + 1); \
    offset += len; \
} while (0);
#define HT_EVENT_RUNTIME_SERIALIZE__(DATA_TYPE, C_TYPE, FIELD, ...) HT_EVENT_RUNTIME_SERIALIZE_##DATA_TYPE(C_TYPE, FIELD)
#define HT_EVENT_RUNTIME_SERIALIZE_(...) HT_EVENT_RUNTIME_SERIALIZE__(__VA_ARGS__, 0)
#define HT_EVENT_RUNTIME_SERIALIZE(X, USER_DATA) HT_EVENT_RUNTIME_SERIALIZE_ (MKCREFLECT_EXPAND_VA_ X)

#define HT_DECLARE_EVENT_KLASS(TYPE_NAME, BASE_TYPE, ...) \
    MKCREFLECT_DEFINE_STRUCT(TYPE_NAME, \
                             (STRUCT, BASE_TYPE, base), \
                             MKCREFLECT_EXPAND_VA_(__VA_ARGS__)) \
    HT_EventKlass* ht_##TYPE_NAME##_get_event_klass_instance(void); \
    void ht_##TYPE_NAME##_register_event_klass(void); \
    inline static size_t HT_EVENT_GET_SIZE_FUNCTION(TYPE_NAME)(HT_Event* VAR_NAME) \
    { \
        return MKCREFLECT_FOREACH(HT_EVENT_RUNTIME_SIZEOF, ((TYPE_NAME*)VAR_NAME), (STRUCT, BASE_TYPE, base), __VA_ARGS__); \
    } \
    inline static size_t HT_EVENT_SERIALIZE_FUNCTION(TYPE_NAME)(HT_Event* VAR_NAME_, HT_Byte* buffer) \
    { \
        size_t offset = 0; \
        TYPE_NAME* VAR_NAME = (TYPE_NAME*)VAR_NAME_; \
        MKCREFLECT_FOREACH(HT_EVENT_RUNTIME_SERIALIZE, 0, (STRUCT, BASE_TYPE, base), __VA_ARGS__) \
        return offset; \
    }

#define HT_DEFINE_EVENT_KLASS(TYPE_NAME) \
    HT_DEFINE_EVENT_KLASS_DETAILED(TYPE_NAME)

#define HT_DEFINE_EVENT_KLASS_DETAILED(TYPE_NAME) \
    HT_EventKlass* ht_##TYPE_NAME##_get_event_klass_instance(void) \
    { \
        static HT_EventKlass klass_instance = { \
            NULL, \
            HT_EVENT_SERIALIZE_FUNCTION(TYPE_NAME), \
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
