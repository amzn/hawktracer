#ifndef HAWKTRACER_TIMELINE_LISTENER_H
#define HAWKTRACER_TIMELINE_LISTENER_H

#include <hawktracer/base_types.h>

#include <stddef.h>

HT_DECLS_BEGIN

typedef struct _HT_TimelineListenerContainer HT_TimelineListenerContainer;

typedef void(*HT_TimelineListenerCallback)(
        TEventPtr events, size_t event_count, HT_Boolean serialized, void* user_data);

/* TODO: init instead? */
HT_API HT_TimelineListenerContainer* ht_timeline_listener_container_create(void);

HT_API void ht_timeline_listener_container_unref(HT_TimelineListenerContainer* container);

/**
 * Registers new listener to a container.
 *
 * @param container a pointer to the container.
 * @param callback a timeline listener callback.
 * @param user_data a user data passed to a @a callback when gets called.
 *
 * @returns #HT_ERR_OK, if registration completed successfully; otherwise, appropriate error code.
 */
HT_API HT_ErrorCode ht_timeline_listener_container_register_listener(
        HT_TimelineListenerContainer* container,
        HT_TimelineListenerCallback callback,
        void* user_data);

/**
 * Removes all timeline listeners from the container.
 *
 * @param container a pointer to the container.
 */
HT_API void ht_timeline_listener_container_unregister_all_listeners(
        HT_TimelineListenerContainer* container);

/**
 * Finds an container in a global registry or create one if container doesn't exist.
 *
 * If @a name is NULL, this method always returns new container.
 *
 * @param name a name of the container to find/create.
 *
 * @return a pointer to a container.
 */
HT_API HT_TimelineListenerContainer* ht_find_or_create_listener(const char* name);

HT_DECLS_END

#endif /* HAWKTRACER_TIMELINE_LISTENER_H */
