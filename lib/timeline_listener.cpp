#include "internal/timeline_listener.hpp"

#include "hawktracer/alloc.h"

#include <cassert>

HT_TimelineListenerContainer*
ht_timeline_listener_container_create(void)
{
    HT_TimelineListenerContainer* container = HT_CREATE_TYPE(HT_TimelineListenerContainer);

    new (&container->listeners) _HT_TimelineListenerContainer::Vector();

    return container;
}

void
ht_timeline_listener_container_destroy(HT_TimelineListenerContainer* container)
{
    assert(container);

    container->listeners.~vector();
    ht_free(container);
}

void
ht_timeline_listener_container_register_listener(
        HT_TimelineListenerContainer* container,
        HT_TimelineListenerCallback callback,
        void* user_data)
{
    container->listeners.push_back(std::make_pair(callback, user_data));
}
