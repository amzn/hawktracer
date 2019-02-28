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

struct _HT_TimelineListenerContainer
{
    /* TODO single struct with pair? */
    HT_BagVoidPtr callbacks;
    HT_BagVoidPtr user_datas;
    HT_Mutex* mutex;
    uint32_t id;
    uint32_t refcount;
};

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
    for (i = 0; i < container->user_datas.size; i++)
    {
        (*(HT_TimelineListenerCallback*)&container->callbacks.data[i])
                (events, size, serialize_events, container->user_datas.data[i]);
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

    if (ht_bag_void_ptr_init(&container->callbacks, 16) != HT_ERR_OK)
    {
        goto error_init_callbacks;
    }

    if (ht_bag_void_ptr_init(&container->user_datas, 16) != HT_ERR_OK)
    {
        goto error_create_user_datas;
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
    ht_bag_void_ptr_deinit(&container->user_datas);
error_create_user_datas:
    ht_bag_void_ptr_deinit(&container->callbacks);
error_init_callbacks:
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
        ht_bag_void_ptr_deinit(&container->callbacks);
        ht_bag_void_ptr_deinit(&container->user_datas);
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
        void* user_data)
{
    HT_ErrorCode error_code;
    ht_mutex_lock(container->mutex);
    /* weird cast because of ISO C forbids passing argument 2 of
       ‘ht_bag_add’ between function pointer and ‘void *’ */
    error_code = ht_bag_void_ptr_add(&container->callbacks, *(void **)&callback);
    if (error_code != HT_ERR_OK)
    {
        goto done;
    }

    error_code = ht_bag_void_ptr_add(&container->user_datas, user_data);
    if (error_code != HT_ERR_OK)
    {
        ht_bag_void_ptr_remove_nth(&container->callbacks, container->callbacks.size - 1);
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
    ht_bag_void_ptr_clear(&container->callbacks);
    ht_bag_void_ptr_clear(&container->user_datas);
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
