#include "hawktracer/timeline_listener.h"

#include "hawktracer/alloc.h"

#include <assert.h>

HT_TimelineListenerContainer*
ht_timeline_listener_container_create(void)
{
    HT_TimelineListenerContainer* container = HT_CREATE_TYPE(HT_TimelineListenerContainer);

    ht_bag_init(&container->callbacks, 16);
    ht_bag_init(&container->user_datas, 16);

    return container;
}

void
ht_timeline_listener_container_destroy(HT_TimelineListenerContainer* container)
{
    assert(container);

    ht_bag_deinit(&container->callbacks);
    ht_bag_deinit(&container->user_datas);

    ht_free(container);
}

void
ht_timeline_listener_container_register_listener(
        HT_TimelineListenerContainer* container,
        HT_TimelineListenerCallback callback,
        void* user_data)
{
    /* weird cast because of ISO C forbids passing argument 2 of
       ‘ht_bag_add’ between function pointer and ‘void *’ */
    ht_bag_add(&container->callbacks, *(void **)&callback);
    ht_bag_add(&container->user_datas, user_data);
}

void
ht_timeline_listener_container_unregister_all_listeners(
        HT_TimelineListenerContainer* container)
{
    ht_bag_clear(&container->callbacks);
    ht_bag_clear(&container->user_datas);
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
        // TODO
        container = ht_timeline_listener_container_create();
    }

    return container;
}
