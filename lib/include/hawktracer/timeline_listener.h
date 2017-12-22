#ifndef HAWKTRACER_TIMELINE_LISTENER_H
#define HAWKTRACER_TIMELINE_LISTENER_H

#include <hawktracer/base_types.h>
#include <hawktracer/bag.h>

#include <stddef.h>

HT_DECLS_BEGIN

typedef struct
{
    /* TODO single struct with pair? */
    HT_Bag callbacks;
    HT_Bag user_datas;
    uint32_t id;
    int refcount; /* TODO atomic */
} HT_TimelineListenerContainer;

typedef void(*HT_TimelineListenerCallback)(
        TEventPtr events, size_t event_count, HT_Boolean serialized, void* user_data);

/* TODO: init instead? */
HT_TimelineListenerContainer* ht_timeline_listener_container_create(void);

void ht_timeline_listener_container_unref(HT_TimelineListenerContainer* container);

void ht_timeline_listener_container_register_listener(
        HT_TimelineListenerContainer* container,
        HT_TimelineListenerCallback callback,
        void* user_data);

void ht_timeline_listener_container_unregister_all_listeners(
        HT_TimelineListenerContainer* container);

HT_TimelineListenerContainer* ht_find_or_create_listener(const char* name);

HT_DECLS_END

#endif /* HAWKTRACER_TIMELINE_LISTENER_H */
