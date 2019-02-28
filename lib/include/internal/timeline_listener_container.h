#ifndef HAWKTRACER_INTERNAL_TIMELINE_LISTENER_CONTAINER_H
#define HAWKTRACER_INTERNAL_TIMELINE_LISTENER_CONTAINER_H

#include <hawktracer/timeline_listener.h>

HT_DECLS_BEGIN

void ht_timeline_listener_container_notify_listeners(HT_TimelineListenerContainer* listeners, TEventPtr events, size_t size, HT_Boolean serialize_events);

uint32_t ht_timeline_listener_container_get_id(HT_TimelineListenerContainer* listeners);

void ht_timeline_listener_container_set_id(HT_TimelineListenerContainer* container, uint32_t id);

void ht_timeline_listener_container_ref(HT_TimelineListenerContainer* container);

void ht_timeline_listener_container_unref(HT_TimelineListenerContainer* container);

HT_TimelineListenerContainer* ht_timeline_listener_container_create(void);

/**
 * Registers new listener to a container.
 *
 * @param container a pointer to the container.
 * @param callback a timeline listener callback.
 * @param user_data a user data passed to a @a callback when gets called.
 *
 * @returns #HT_ERR_OK, if registration completed successfully; otherwise, appropriate error code.
 */
HT_ErrorCode ht_timeline_listener_container_register_listener(
        HT_TimelineListenerContainer* container,
        HT_TimelineListenerCallback callback,
        void* user_data);

/**
 * Finds an container in a global registry or create one if container doesn't exist.
 *
 * If @a name is NULL, this method always returns new container.
 *
 * @param name a name of the container to find/create.
 *
 * @return a pointer to a container.
 */
HT_TimelineListenerContainer* ht_find_or_create_listener(const char* name);

/**
 * Removes all timeline listeners from the container.
 *
 * @param container a pointer to the container.
 */
void ht_timeline_listener_container_unregister_all_listeners(
        HT_TimelineListenerContainer* container);

HT_DECLS_END

#endif /* HAWKTRACER_TIMELINE_LISTENER_CONTAINER_H */
