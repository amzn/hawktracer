#include "internal/timeline_registry.h"

#include "hawktracer/alloc.h"
#include "internal/timeline_klass.hpp"

#include <unordered_map>
#include <cassert>

std::unordered_map<uint32_t, _HT_TimelineKlass*> timeline_klass_register; // todo quark

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

_HT_TimelineKlass*
ht_timeline_registry_find_class(const char* klass_id)
{
    assert(klass_id != nullptr);

    auto it = timeline_klass_register.find(djb2_hash(klass_id));

    return it == timeline_klass_register.end() ? nullptr : it->second;
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

    timeline_klass_register[djb2_hash(klass_id)] = klass;
}

void
ht_timeline_registry_unregister_all(void)
{
    while (!timeline_klass_register.empty())
    {
        _HT_TimelineKlass* klass = timeline_klass_register.begin()->second;

        if (klass->listeners)
        {
            ht_timeline_listener_container_destroy(klass->listeners);
        }

        ht_free(klass);

        timeline_klass_register.erase(timeline_klass_register.begin());
    }
}
