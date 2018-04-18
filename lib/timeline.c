#include <hawktracer/timeline.h>
#include <hawktracer/alloc.h>

#include "internal/feature.h"
#include "internal/registry.h"
#include "internal/mutex.h"

#include <string.h>
#include <assert.h>

static inline void
_ht_timeline_notify_listeners(HT_Timeline* timeline)
{
    size_t i;
    for (i = 0; i < timeline->listeners->user_datas.size; i++)
    {
        (*(HT_TimelineListenerCallback*)&timeline->listeners->callbacks.data[i])
                (timeline->buffer, timeline->buffer_usage, timeline->serialize_events, timeline->listeners->user_datas.data[i]);
    }
}

static inline void
_ht_timeline_push_event_to_listeners(HT_Timeline* timeline, HT_Event* event, size_t event_size)
{
    size_t i;
    for (i = 0; i < timeline->listeners->user_datas.size; i++)
    {
        (*(HT_TimelineListenerCallback*)&timeline->listeners->callbacks.data[i])
                ((TEventPtr)event, event_size, timeline->serialize_events, timeline->listeners->user_datas.data[i]);
    }
}

void
ht_timeline_init_event(HT_Timeline* timeline, HT_Event* event)
{
    event->timestamp = ht_monotonic_clock_get_timestamp();
    event->id = ht_event_id_provider_next(timeline->id_provider);
}

void
ht_timeline_push_event(HT_Timeline* timeline, HT_Event* event)
{
    HT_EventKlass* klass = HT_EVENT_GET_CLASS(event);

    assert(timeline);
    assert(event);

    if (timeline->locking_policy != NULL)
    {
        ht_mutex_lock(timeline->locking_policy);
    }

    if (timeline->serialize_events)
    {
        size_t size = klass->get_size(event);
        if (timeline->buffer_capacity < timeline->buffer_usage + size)
        {
            ht_timeline_flush(timeline);
        }

        if (timeline->buffer_capacity < size)
        {
            _ht_timeline_push_event_to_listeners(timeline, event, size);
        }
        else
        {
            event->klass->serialize(event, timeline->buffer + timeline->buffer_usage);
            timeline->buffer_usage += size;
        }
    }
    else
    {
        if (timeline->buffer_capacity < timeline->buffer_usage + klass->type_info->size)
        {
            ht_timeline_flush(timeline);
        }

        if (timeline->buffer_capacity < klass->type_info->size) 
        {
            _ht_timeline_push_event_to_listeners(timeline, event, klass->type_info->size);
        }
        else
        {
            memcpy(timeline->buffer + timeline->buffer_usage, event, klass->type_info->size);
            timeline->buffer_usage += klass->type_info->size;
        }
    }

    if (timeline->locking_policy != NULL)
    {
        ht_mutex_unlock(timeline->locking_policy);
    }
}

void
ht_timeline_flush(HT_Timeline* timeline)
{
    if (timeline->buffer_usage)
    {
        _ht_timeline_notify_listeners(timeline);
        timeline->buffer_usage = 0;
    }
}

void
ht_timeline_register_listener(
        HT_Timeline* timeline,
        HT_TimelineListenerCallback callback,
        void* user_data)
{
    ht_timeline_listener_container_register_listener(
                timeline->listeners, callback, user_data);
}

void
ht_timeline_unregister_all_listeners(HT_Timeline* timeline)
{
    ht_timeline_listener_container_unregister_all_listeners(
                timeline->listeners);
}

void
ht_timeline_init(HT_Timeline* timeline,
                     size_t buffer_capacity,
                     HT_Boolean thread_safe,
                     HT_Boolean serialize_events,
                     const char* listeners)
{
    timeline->buffer_usage = 0;
    timeline->buffer_capacity = buffer_capacity;
    timeline->buffer = (HT_Byte*)ht_alloc(buffer_capacity);
    timeline->id_provider = ht_event_id_provider_get_default();
    timeline->serialize_events = serialize_events;

    timeline->listeners = ht_find_or_create_listener(listeners);

    timeline->locking_policy = thread_safe ? ht_mutex_create() : NULL;

    memset(timeline->features, 0, sizeof(timeline->features));
}

void
ht_timeline_deinit(HT_Timeline* timeline)
{
    size_t i;

    assert(timeline);

    ht_timeline_flush(timeline);
    ht_free(timeline->buffer);

    ht_timeline_listener_container_unref(timeline->listeners);

    for (i = 0; i < sizeof(timeline->features) / sizeof(timeline->features[0]); i++)
    {
        if (timeline->features[i])
        {
            ht_feature_disable(timeline, i);
            timeline->features[i] = NULL;
        }
    }

    if (timeline->locking_policy)
    {
        ht_mutex_destroy(timeline->locking_policy);
    }
}
