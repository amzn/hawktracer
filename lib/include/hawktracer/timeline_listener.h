#ifndef HAWKTRACER_TIMELINE_LISTENER_H
#define HAWKTRACER_TIMELINE_LISTENER_H

#include <hawktracer/base_types.h>

#include <stddef.h>

HT_DECLS_BEGIN

typedef struct _HT_TimelineListenerContainer HT_TimelineListenerContainer;

typedef void(*HT_TimelineListenerCallback)(
        TEventPtr events, size_t event_count, HT_Boolean serialized, void* user_data);

/**
 * Pushes all metadata events (registry klasses, system information) to a listener.
 * @param callback a listener callback.
 * @param listener a pointer to the listener
 * @param serialize indicates whether events should be serialized before pushing to listener or not.
 *
 * @return number of bytes which have been pushed to a listener.
 */
HT_API size_t ht_timeline_listener_push_metadata(HT_TimelineListenerCallback callback, void* listener, HT_Boolean serialize);

HT_DECLS_END

#endif /* HAWKTRACER_TIMELINE_LISTENER_H */
