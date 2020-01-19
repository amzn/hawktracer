#include "hawktracer/timeline_listener.h"

#include "hawktracer/alloc.h"
#include "hawktracer/system_info.h"
#include "hawktracer/timeline.h"
#include "internal/bag.h"
#include "internal/registry.h"
#include "internal/mutex.h"
#include "internal/timeline_listener_container.h"
#include "internal/system_info.h"

#include <assert.h>

typedef struct
{
    HT_TimelineListenerCallback callback;
    void* user_data;
    HT_DestroyCallback destroy_cb;
} HT_TimelineListenerEntry;

HT_DECLARE_BAG_TYPE(Listener, _listener, HT_TimelineListenerEntry)
HT_DEFINE_BAG_TYPE(Listener, _listener, HT_TimelineListenerEntry)

struct _HT_TimelineListenerContainer
{
    HT_BagListener entries;
    HT_Mutex* mutex;
    uint32_t id;
    uint32_t refcount;
};

static void
_ht_timeline_listener_container_unregister_all_listeners(
        HT_TimelineListenerContainer* container)
{
    size_t i;

    for (i = 0; i < container->entries.size; i++)
    {
        HT_TimelineListenerEntry* entry = &container->entries.data[i];
        if (entry->destroy_cb != NULL)
        {
            entry->destroy_cb(entry->user_data);
        }
    }

    ht_bag_listener_clear(&container->entries);
}

uint32_t
ht_timeline_listener_container_get_id(HT_TimelineListenerContainer* container)
{
    return container->id;
}

void ht_timeline_listener_container_set_id(HT_TimelineListenerContainer* container, uint32_t id)
{
    container->id = id;
}

void
ht_timeline_listener_container_notify_listeners(HT_TimelineListenerContainer* container, TEventPtr events, size_t size, HT_Boolean serialize_events)
{
    size_t i;
    for (i = 0; i < container->entries.size; i++)
    {
        HT_TimelineListenerEntry* entry = &container->entries.data[i];
        entry->callback(events, size, serialize_events, entry->user_data);
    }
}

HT_TimelineListenerContainer*
ht_timeline_listener_container_create(void)
{
    HT_TimelineListenerContainer* container = HT_CREATE_TYPE(HT_TimelineListenerContainer);

    if (container == NULL)
    {
        goto done;
    }

    if (ht_bag_listener_init(&container->entries, 16) != HT_ERR_OK)
    {
        goto error_init_entries;
    }

    container->mutex = ht_mutex_create();
    if (container->mutex == NULL)
    {
        goto error_create_mutex;
    }

    container->id = 0;
    container->refcount = 1;
    goto done;

error_create_mutex:
    ht_bag_listener_deinit(&container->entries);
error_init_entries:
    ht_free(container);
    container = NULL;
done:
    return container;
}

void
ht_timeline_listener_container_ref(HT_TimelineListenerContainer* container)
{
    assert(container);

    ht_mutex_lock(container->mutex);
    container->refcount++;
    ht_mutex_unlock(container->mutex);
}

void
ht_timeline_listener_container_unref(HT_TimelineListenerContainer* container)
{
    assert(container);

    ht_mutex_lock(container->mutex);

    if (--container->refcount == 0)
    {
        _ht_timeline_listener_container_unregister_all_listeners(container);
        ht_bag_listener_deinit(&container->entries);
        ht_mutex_unlock(container->mutex);
        ht_mutex_destroy(container->mutex);

        ht_free(container);
    }
    else
    {
        ht_mutex_unlock(container->mutex);
    }
}

HT_ErrorCode
ht_timeline_listener_container_register_listener(
        HT_TimelineListenerContainer* container,
        HT_TimelineListenerCallback callback,
        void* user_data,
        HT_DestroyCallback destroy_cb)
{
    HT_ErrorCode error_code;
    HT_TimelineListenerEntry entry = {
        callback, user_data, destroy_cb
    };
    ht_mutex_lock(container->mutex);
    /* weird cast because of ISO C forbids passing argument 2 of
       'ht_bag_add' between function pointer and 'void *' */
    error_code = ht_bag_listener_add(&container->entries, entry);
    if (error_code != HT_ERR_OK)
    {
        goto done;
    }

done:
    ht_mutex_unlock(container->mutex);
    return error_code;
}

void
ht_timeline_listener_container_unregister_all_listeners(
        HT_TimelineListenerContainer* container)
{
    ht_mutex_lock(container->mutex);
    _ht_timeline_listener_container_unregister_all_listeners(container);
    ht_mutex_unlock(container->mutex);
}

HT_TimelineListenerContainer*
ht_find_or_create_listener(const char* name)
{
    HT_TimelineListenerContainer* container;

    if (name == NULL)
    {
        container = ht_timeline_listener_container_create();
    }
    else
    {
        container = ht_registry_find_listener_container(name);
        if (container == NULL)
        {
            container = ht_timeline_listener_container_create();
            if (container != NULL)
            {
                ht_registry_register_listener_container(name, container);
            }
        }
        else
        {
            container->refcount++;
        }
    }

    return container;
}

size_t
ht_timeline_listener_push_metadata(HT_TimelineListenerCallback callback, void* listener, HT_Boolean serialize)
{
    size_t size = 0;

    size += ht_system_info_push_endianness_info_to_listener(callback, listener, serialize);
    size += ht_registry_push_registry_klasses_to_listener(callback, listener, serialize);
    size += ht_system_info_push_system_info_to_listener(callback, listener, serialize);

    return size;
}
