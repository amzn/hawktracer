#ifndef HAWKTRACER_INTERNAL_TIMELINE_LISTENER_CONTAINER_H
#define HAWKTRACER_INTERNAL_TIMELINE_LISTENER_CONTAINER_H

#include <hawktracer/timeline_listener.h>

HT_DECLS_BEGIN

void ht_timeline_listener_container_notify_listeners(HT_TimelineListenerContainer* listeners, TEventPtr events, size_t size, HT_Boolean serialize_events);

uint32_t ht_timeline_listener_container_get_id(HT_TimelineListenerContainer* listeners);

void ht_timeline_listener_container_set_id(HT_TimelineListenerContainer* container, uint32_t id);

void ht_timeline_listener_container_ref(HT_TimelineListenerContainer* container);

void ht_timeline_listener_container_unref(HT_TimelineListenerContainer* container);

HT_DECLS_END

#endif /* HAWKTRACER_TIMELINE_LISTENER_CONTAINER_H */
