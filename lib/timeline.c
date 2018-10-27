#include <hawktracer/timeline.h>
#include <hawktracer/alloc.h>

#include "internal/feature.h"
#include "internal/registry.h"
#include "internal/mutex.h"
#include "internal/timeline_listener_container.h"

#include <string.h>
#include <assert.h>

struct _HT_Timeline
{
    void* features[HT_TIMELINE_MAX_FEATURES];
    size_t buffer_capacity;
    size_t buffer_usage;
    HT_Byte* buffer;
    HT_EventIdProvider* id_provider;
    HT_TimelineListenerContainer* listeners;
    struct _HT_Mutex* locking_policy;
    HT_Boolean serialize_events;
};

void
ht_timeline_init_event(HT_Timeline* timeline, HT_Event* event)
{
    event->timestamp = ht_monotonic_clock_get_timestamp();
    event->id = ht_event_id_provider_next(timeline->id_provider);
}

void
ht_timeline_push_event(HT_Timeline* timeline, HT_Event* event)
{
    HT_EventKlass* klass = HT_EVENT_GET_KLASS(event);

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
            HT_Byte local_buffer[128];
            if (size > sizeof(local_buffer)/sizeof(local_buffer[0]))
            {
                HT_Byte* buff = ht_alloc(size);
                event->klass->serialize(event, buff);
                ht_timeline_listener_container_notify_listeners(timeline->listeners, buff, size, timeline->serialize_events);
                ht_free(buff);
            }
            else
            {
                event->klass->serialize(event, local_buffer);
                ht_timeline_listener_container_notify_listeners(timeline->listeners, local_buffer, size, timeline->serialize_events);
            }
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
            ht_timeline_listener_container_notify_listeners(timeline->listeners, (TEventPtr)event, klass->type_info->size, timeline->serialize_events);
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
        ht_timeline_listener_container_notify_listeners(timeline->listeners, timeline->buffer, timeline->buffer_usage, timeline->serialize_events);
        timeline->buffer_usage = 0;
    }
}

void
ht_timeline_set_feature(HT_Timeline* timeline, size_t feature_id, void* feature)
{
    timeline->features[feature_id] = feature;
}

void*
ht_timeline_get_feature(HT_Timeline* timeline, size_t feature_id)
{
    return timeline->features[feature_id];
}


HT_ErrorCode
ht_timeline_register_listener(
        HT_Timeline* timeline,
        HT_TimelineListenerCallback callback,
        void* user_data)
{
    return ht_timeline_listener_container_register_listener(
                timeline->listeners, callback, user_data);
}

void
ht_timeline_unregister_all_listeners(HT_Timeline* timeline)
{
    ht_timeline_listener_container_unregister_all_listeners(
                timeline->listeners);
}

HT_Timeline*
ht_timeline_create(size_t buffer_capacity,
                   HT_Boolean thread_safe,
                   HT_Boolean serialize_events,
                   const char* listeners,
                   HT_ErrorCode* out_err)
{
    HT_ErrorCode error_code = HT_ERR_OK;
    HT_Timeline* timeline = HT_CREATE_TYPE(HT_Timeline);

    if (timeline == NULL)
    {
        goto done;
    }

    timeline->buffer = (HT_Byte*)ht_alloc(buffer_capacity);

    if (timeline->buffer == NULL)
    {
        error_code = HT_ERR_OUT_OF_MEMORY;
        goto error_allocate_buffer;
    }

    timeline->listeners = ht_find_or_create_listener(listeners);
    if (timeline->listeners == NULL)
    {
        error_code = HT_ERR_CANT_CREATE_LISTENER_CONTAINER;
        goto error_create_listener;
    }

    if (thread_safe)
    {
        timeline->locking_policy = ht_mutex_create();
        if (timeline->locking_policy == NULL)
        {
            error_code = HT_ERR_OUT_OF_MEMORY;
            goto error_locking_policy;
        }
    }
    else
    {
        timeline->locking_policy = NULL;
    }

    timeline->buffer_usage = 0;
    timeline->buffer_capacity = buffer_capacity;
    timeline->id_provider = ht_event_id_provider_get_default();
    timeline->serialize_events = serialize_events;
    memset(timeline->features, 0, sizeof(timeline->features));

    goto done;

error_locking_policy:
    ht_timeline_listener_container_unref(timeline->listeners);
error_create_listener:
    ht_free(timeline->buffer);
error_allocate_buffer:
    ht_free(timeline);
    timeline = NULL;
done:
    if (out_err != NULL)
    {
        *out_err = error_code;
    }

    return timeline;
}

void
ht_timeline_destroy(HT_Timeline* timeline)
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

    ht_free(timeline);
}

HT_EventIdProvider*
ht_timeline_get_id_provider(HT_Timeline* timeline)
{
    return timeline->id_provider;
}
