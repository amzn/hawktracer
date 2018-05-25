#include "hawktracer/timeline_listener.h"

#include "hawktracer/alloc.h"
#include "internal/registry.h"
#include "internal/mutex.h"
#include "internal/timeline_listener_container.h"

#include <assert.h>

HT_TimelineListenerContainer*
ht_timeline_listener_container_create(void)
{
    HT_TimelineListenerContainer* container = HT_CREATE_TYPE(HT_TimelineListenerContainer);

    if (container == NULL)
    {
        goto done;
    }

    if (ht_bag_init(&container->callbacks, 16) != HT_ERR_OK)
    {
        goto error_init_callbacks;
    }

    if (ht_bag_init(&container->user_datas, 16) != HT_ERR_OK)
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
    ht_bag_deinit(&container->user_datas);
error_create_user_datas:
    ht_bag_deinit(&container->callbacks);
error_init_callbacks:
    ht_free(container);
    container = NULL;
done:
    return container;
}

void
ht_timeline_listener_container_unref(HT_TimelineListenerContainer* container)
{
    assert(container);

    if (--container->refcount == 0)
    {
        ht_bag_deinit(&container->callbacks);
        ht_bag_deinit(&container->user_datas);
        ht_mutex_destroy(container->mutex);

        ht_free(container);
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
    error_code = ht_bag_add(&container->callbacks, *(void **)&callback);
    if (error_code != HT_ERR_OK)
    {
        goto done;
    }

    error_code = ht_bag_add(&container->user_datas, user_data);
    if (error_code != HT_ERR_OK)
    {
        ht_bag_remove_nth(&container->callbacks, container->callbacks.size - 1);
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
    ht_bag_clear(&container->callbacks);
    ht_bag_clear(&container->user_datas);
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
