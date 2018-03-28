#include "hawktracer/alloc.h"
#include "hawktracer/bag.h"
#include "hawktracer/core_events.h"
#include "internal/hash.h"
#include "internal/feature.h"
#include "internal/registry.h"
#include "internal/mutex.h"

#include <assert.h>
#include <string.h>

static HT_Bag event_klass_register;
static HT_Bag listeners_register;
static HT_FeatureDisableCallback feature_disable_callback[HT_TIMELINE_MAX_FEATURES];

static HT_Mutex* listeners_register_mutex;
static HT_Mutex* features_register_mutex;
static HT_Mutex* event_klass_registry_register_mutex;

HT_Boolean
ht_registry_register_feature(uint32_t feature_id, HT_FeatureDisableCallback disable_callback)
{
    assert(disable_callback);
    assert(feature_id < HT_TIMELINE_MAX_FEATURES);

    ht_mutex_lock(features_register_mutex);

    if (!feature_disable_callback[feature_id])
    {
        feature_disable_callback[feature_id] = disable_callback;
        ht_mutex_unlock(features_register_mutex);
        return HT_TRUE;
    }

    ht_mutex_unlock(features_register_mutex);
    return HT_FALSE;
}

void
ht_registry_init(void)
{
    ht_bag_init(&event_klass_register, 8);
    ht_bag_init(&listeners_register, 8);
    listeners_register_mutex = ht_mutex_create();
    features_register_mutex = ht_mutex_create();
    event_klass_registry_register_mutex = ht_mutex_create();
}

HT_EventKlassId
ht_registry_register_event_klass(HT_EventKlass* event_klass)
{
    if (event_klass->klass_id == 0)
    {
        ht_mutex_lock(event_klass_registry_register_mutex);
        ht_bag_add(&event_klass_register, event_klass);
        event_klass->klass_id = event_klass_register.size;
        ht_mutex_unlock(event_klass_registry_register_mutex);
    }

    return event_klass->klass_id;
}

void
ht_registry_push_all_klass_info_events(HT_Timeline* timeline)
{
    size_t i;

    ht_mutex_lock(event_klass_registry_register_mutex);

    for (i = 0; i < event_klass_register.size; i++)
    {
        ht_registry_push_klass_info_event(timeline, (HT_EventKlass*)event_klass_register.data[i]);
    }

    ht_mutex_unlock(event_klass_registry_register_mutex);

    ht_timeline_flush(timeline);
}

void
ht_registry_push_klass_info_event(HT_Timeline* timeline, HT_EventKlass* klass)
{
    size_t j;
    HT_DECL_EVENT(HT_EventKlassInfoEvent, event);
    ht_timeline_init_event(timeline, HT_EVENT(&event));
    event.event_klass_name = klass->type_info->name;
    event.info_klass_id = klass->klass_id;
    event.field_count = (uint8_t) klass->type_info->fields_count;

    ht_timeline_push_event(timeline, HT_EVENT(&event));

    for (j = 0; j < klass->type_info->fields_count; j++)
    {
        MKCREFLECT_FieldInfo* info = &klass->type_info->fields[j];
        HT_DECL_EVENT(HT_EventKlassFieldInfoEvent, field_event);
        ht_timeline_init_event(timeline, HT_EVENT(&field_event));
        field_event.data_type = info->data_type;
        field_event.info_klass_id = event.info_klass_id;
        field_event.field_name = info->field_name;
        field_event.field_type = info->field_type;
        field_event.size = info->size;

        ht_timeline_push_event(timeline, HT_EVENT(&field_event));
    }
}

HT_EventKlass**
ht_registry_get_event_klasses(size_t* out_klass_count)
{
    assert(out_klass_count != NULL);

    *out_klass_count = event_klass_register.size;
    return (HT_EventKlass**)event_klass_register.data;
}

void
ht_registry_deinit(void)
{
    size_t i;
    for (i = 0; i < listeners_register.size; i++)
    {
        ht_timeline_listener_container_unref(listeners_register.data[i]);
    }

    ht_mutex_destroy(features_register_mutex);
    ht_mutex_destroy(event_klass_registry_register_mutex);
    ht_mutex_destroy(listeners_register_mutex);
    ht_bag_deinit(&listeners_register);
    ht_bag_deinit(&event_klass_register);
}

HT_TimelineListenerContainer*
ht_registry_find_listener_container(const char* name)
{
    size_t i;
    uint32_t id = djb2_hash(name);

    ht_mutex_lock(listeners_register_mutex);
    for (i = 0; i < listeners_register.size; i++)
    {
        if (((HT_TimelineListenerContainer*)listeners_register.data[i])->id == id)
        {
            void* container = listeners_register.data[i];
            ht_mutex_unlock(listeners_register_mutex);
            return container;
        }
    }
    ht_mutex_unlock(listeners_register_mutex);

    return NULL;
}

HT_Boolean
ht_registry_register_listener_container(const char* name, HT_TimelineListenerContainer* container)
{
    if (ht_registry_find_listener_container(name) != NULL)
    {
        return HT_FALSE;
    }

    ht_mutex_lock(listeners_register_mutex);
    container->id = djb2_hash(name);
    container->refcount++;
    ht_bag_add(&listeners_register, container);
    ht_mutex_unlock(listeners_register_mutex);

    return HT_TRUE;
}

void
ht_feature_disable(HT_Timeline *timeline, uint32_t id)
{
    assert(timeline);
    assert(feature_disable_callback[id]);

    feature_disable_callback[id](timeline);
}


#include "hawktracer/feature_cached_string.h"
#include "hawktracer/feature_callstack.h"

void
ht_feature_register_core_features(void)
{
    ht_registry_register_feature(HT_FEATURE_CALLSTACK, ht_feature_callstack_disable);
    ht_registry_register_feature(HT_FEATURE_CACHED_STRING, ht_feature_cached_string_disable);
}
