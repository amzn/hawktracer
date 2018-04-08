#ifndef HAWKTRACER_EVENT_MACROS_IMPL_H
#define HAWKTRACER_EVENT_MACROS_IMPL_H

#include <hawktracer/event_macros.h>

#ifdef MKCREFLECT_DEFINE_GET_METHOD
#undef MKCREFLECT_DEFINE_GET_METHOD
#endif

#define MKCREFLECT_DEFINE_GET_METHOD(TYPE_NAME, ...) \
    MKCREFLECT_TypeInfo* mkcreflect_get_##TYPE_NAME##_type_info(void) \
    { \
        static MKCREFLECT_FieldInfo fields_info[MKCREFLECT_FOREACH(MKCREFLECT_SUM, 0, __VA_ARGS__)] = \
        { \
            MKCREFLECT_FOREACH(MKCREFLECT_FIELD_INFO, TYPE_NAME, __VA_ARGS__) \
        }; \
        static MKCREFLECT_TypeInfo type_info = \
        { \
            #TYPE_NAME, \
            MKCREFLECT_FOREACH(MKCREFLECT_SUM, 0, __VA_ARGS__), \
            sizeof(TYPE_NAME), \
            MKCREFLECT_FOREACH(MKCREFLECT_SIZEOF, 0, __VA_ARGS__), \
            fields_info \
        }; \
        return &type_info; \
    }

#ifdef HT_EVENT_DEFINITIONS
#undef HT_EVENT_DEFINITIONS
#endif

#define HT_EVENT_DEFINITIONS(TYPE_NAME, BASE_TYPE, ...) \
    HT_EVENT_DEFINITIONS_(TYPE_NAME, BASE_TYPE, __VA_ARGS__)

#endif /* HAWKTRACER_EVENT_MACROS_IMPL_H */
