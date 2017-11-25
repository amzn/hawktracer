#include "hawktracer/base_timeline.h"
#include "hawktracer/alloc.h"

#include "internal/timeline_klass.hpp"
#include "internal/mutex.h"

#include <cstring>
#include <cassert>

void
ht_base_timeline_init(HT_BaseTimeline* timeline, va_list args)
{
    size_t buffer_capacity = 1024;
    const char* label = va_arg(args, const char*);;

    while (label != nullptr)
    {
        if (strncmp("buffer-capacity", label, 15) == 0)
        {
            buffer_capacity = va_arg(args, size_t);
        }
        else
        {
            va_arg(args, void*);
        }
        label = va_arg(args, const char*);
    }

    timeline->buffer_usage = 0;
    timeline->buffer_capacity = buffer_capacity;
    timeline->buffer = (uint8_t*)ht_alloc(buffer_capacity);
    timeline->id_provider = ht_event_id_provider_get_default();

    timeline->listeners = (timeline->klass->listeners == NULL) ?
                ht_timeline_listener_container_create() : timeline->klass->listeners;

    timeline->locking_policy = (timeline->klass->thread_safe == HT_TRUE) ?
                ht_mutex_create() : NULL;
}

void
ht_base_timeline_deinit(HT_BaseTimeline* timeline)
{
    assert(timeline);

    ht_free(timeline->buffer);

    if (timeline->klass->listeners == NULL)
    {
        ht_timeline_listener_container_destroy(timeline->listeners);
    }

    if (timeline->locking_policy)
    {
        ht_mutex_destroy(timeline->locking_policy);
    }
}
