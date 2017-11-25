#include "internal/timeline_registry.h"

#include "hawktracer/alloc.h"
#include "internal/timeline_klass.hpp"
#include "internal/bag.h"

#include <cassert>

static HT_Bag timeline_klass_register;

static uint32_t
djb2_hash(const char *str)
{
    uint32_t hash = 5381;
    int c;

    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

void
ht_timeline_registry_init(void)
{
    ht_bag_init(&timeline_klass_register, 8);
}

_HT_TimelineKlass*
ht_timeline_registry_find_class(const char* klass_id)
{
    assert(klass_id != nullptr);

    uint32_t id = djb2_hash(klass_id);

    for (size_t i = 0; i < timeline_klass_register.size; i++)
    {
        _HT_TimelineKlass* klass = (_HT_TimelineKlass*)timeline_klass_register.data[i];
        if (klass->id == id)
        {
            return klass;
        }
    }

    return nullptr;
}

void ht_timeline_registry_register(
        const char* klass_id,
        size_t type_size,
        HT_Boolean thread_safe,
        HT_Boolean shared_listeners,
        void (*init)(HT_Timeline*, va_list),
        void (*deinit)(HT_Timeline*))
{
    if (ht_timeline_registry_find_class(klass_id) != NULL)
    {
        // TODO error
        return;
    }

    _HT_TimelineKlass* klass = HT_CREATE_TYPE(_HT_TimelineKlass);

    klass->thread_safe = thread_safe;
    klass->listeners = shared_listeners == HT_TRUE ? ht_timeline_listener_container_create() : NULL;
    klass->type_size = type_size;
    klass->init = init;
    klass->deinit = deinit;
    klass->refcount = 1;
    klass->id = djb2_hash(klass_id);

    ht_bag_add(&timeline_klass_register, klass);
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

void
ht_timeline_registry_unregister_all(void)
{
    for (size_t i = 0; i < timeline_klass_register.size; i++)
    {
        _HT_TimelineKlass* klass = (_HT_TimelineKlass*)timeline_klass_register.data[i];
        ht_timeline_klass_unref(klass);
    }

    ht_bag_deinit(&timeline_klass_register);
}
