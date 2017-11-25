#include "internal/timeline_klass.hpp"

#include "hawktracer/alloc.h"

HT_TimelineKlass *ht_timeline_klass_create(
        uint32_t klass_id,
        size_t type_size,
        HT_Boolean shared_listeners,
        void (*init)(HT_Timeline*, va_list),
        void (*deinit)(HT_Timeline*))
{
    _HT_TimelineKlass* klass = HT_CREATE_TYPE(_HT_TimelineKlass);

    klass->listeners = shared_listeners == HT_TRUE ? ht_timeline_listener_container_create() : NULL;
    klass->type_size = type_size;
    klass->init = init;
    klass->deinit = deinit;
    klass->refcount = 1;
    klass->id = klass_id;

    return klass;
}

uint32_t
ht_timeline_klass_get_id(HT_TimelineKlass* klass)
{
    return klass->id;
}

void
ht_timeline_klass_unref(_HT_TimelineKlass* klass)
{
    if (--klass->refcount == 0)
    {
        if (klass->listeners)
        {
            ht_timeline_listener_container_destroy(klass->listeners);
        }

        ht_free(klass);
    }
}
