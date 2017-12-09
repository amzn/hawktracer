#include <hawktracer/timeline.h>
#include <hawktracer/alloc.h>
#include <hawktracer/monotonic_clock.h>

#include "internal/registry.h"
#include "internal/timeline_klass.hpp"
#include "internal/mutex.h"

#include <cstring>
#include <cassert>

HT_Timeline*
ht_timeline_create(const char* klass_id, ...)
{
    HT_TimelineKlass* klass = ht_registry_find_timeline_class(klass_id);

    if (klass == nullptr)
    {
        // TODO error
        return nullptr;
    }

    ++klass->refcount;

    HT_Timeline* timeline = (HT_Timeline*)ht_alloc(klass->type_size);
    timeline->klass = klass;

    va_list args;
    va_start(args, klass_id);

    klass->init(timeline, args);

    va_end(args);

    return timeline;
}

void ht_timeline_destroy(HT_Timeline* timeline)
{
    ht_timeline_flush(timeline);

    _HT_TimelineKlass* klass = timeline->klass;

    klass->deinit(timeline);
    ht_free(timeline);

    ht_timeline_klass_unref(klass);
}

static inline void
_ht_timeline_notify_listeners(HT_Timeline* timeline)
{
    for (size_t i = 0; i < timeline->listeners->user_datas.size; i++)
    {
        ((HT_TimelineListenerCallback)timeline->listeners->callbacks.data[i])
                (timeline->buffer, timeline->buffer_usage, timeline->listeners->user_datas.data[i]);
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

    if (timeline->buffer_capacity < timeline->buffer_usage + klass->type_info->size)
    {
        ht_timeline_flush(timeline);
    }

#define HT_COPY_EVENT(EventStruct) \
    memcpy(timeline->buffer + timeline->buffer_usage, event, sizeof(EventStruct))

    switch (klass->type_info->size)
    {
    case sizeof(HT_Event):
        HT_COPY_EVENT(HT_Event);
        break;
    default:
        memcpy(timeline->buffer + timeline->buffer_usage, event, klass->type_info->size);
    }

    timeline->buffer_usage += klass->type_info->size;

    if (timeline->locking_policy != NULL)
    {
        ht_mutex_unlock(timeline->locking_policy);
    }
}

void
ht_timeline_flush(HT_Timeline* timeline)
{
    _ht_timeline_notify_listeners(timeline);
    timeline->buffer_usage = 0;
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
