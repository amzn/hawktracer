#include "hawktracer/feature_cached_string.h"
#include "hawktracer/alloc.h"
#include "hawktracer/core_events.h"
#include "internal/hash.h"
#include "internal/bag.h"
#include "internal/mutex.h"
#include "internal/error.h"
#include "internal/feature.h"
#include "internal/hash_map.h"

#include <assert.h>

static void
ht_feature_cached_string_destroy(HT_Feature* f);

typedef struct
{
    HT_Feature base;
    HT_Mutex* lock;
    HT_HashMap dynamic_hashes;
    HT_HashMap static_hashes;
} HT_FeatureCachedString;

HT_FEATURE_DEFINE(HT_FeatureCachedString, ht_feature_cached_string_destroy)

#define HT_FCS_LOCK_(feature) \
    do { \
        if (feature->lock) ht_mutex_lock(feature->lock); \
    } while (0)

#define HT_FCS_UNLOCK_(feature) \
    do { \
        if (feature->lock) ht_mutex_unlock(feature->lock); \
    } while (0)

static HT_Feature*
ht_feature_cached_string_create(HT_Boolean thread_safe, HT_ErrorCode* out_err)
{
    HT_FeatureCachedString* feature = HT_FeatureCachedString_alloc();
    HT_ErrorCode error_code = HT_ERR_OK;

    if (feature == NULL)
    {
        error_code = HT_ERR_OUT_OF_MEMORY;
        goto create_finished;
    }

    error_code = ht_hash_map_init(&feature->static_hashes);
    if (error_code != HT_ERR_OK)
    {
        goto static_hashes_failed;
    }
    error_code = ht_hash_map_init(&feature->dynamic_hashes);
    if (error_code != HT_ERR_OK)
    {
        goto dynamic_hashes_failed;
    }

    if (thread_safe)
    {
        feature->lock = ht_mutex_create();
        if (feature->lock == NULL)
        {
            error_code = HT_ERR_UNKNOWN;
            goto lock_failed;
        }
    }
    else
    {
        feature->lock = NULL;
    }
    goto create_finished;

lock_failed:
    ht_hash_map_deinit(&feature->dynamic_hashes);
dynamic_hashes_failed:
    ht_hash_map_deinit(&feature->static_hashes);
static_hashes_failed:
    ht_free(feature);
    feature = NULL;
create_finished:
    HT_SET_ERROR(out_err, error_code);
    return (HT_Feature*)feature;
}

static HT_Boolean
ht_feature_cached_string_destry_dynamic_labels(uint64_t key, const char* value, void* ud)
{
    HT_UNUSED(key);
    HT_UNUSED(ud);
    ht_free((void*)value);

    return HT_TRUE;
}

static void
ht_feature_cached_string_destroy(HT_Feature* f)
{
    HT_FeatureCachedString* feature = (HT_FeatureCachedString*)f;

    ht_hash_map_for_each(&feature->dynamic_hashes, ht_feature_cached_string_destry_dynamic_labels, NULL);

    ht_hash_map_deinit(&feature->dynamic_hashes);
    ht_hash_map_deinit(&feature->static_hashes);
    if (feature->lock)
    {
        ht_mutex_destroy(feature->lock);
    }
    ht_free(feature);
}

static uintptr_t
ht_feature_cached_string_add_mapping_(HT_Timeline* timeline, HT_HashMap* map, uintptr_t hash, const char* label)
{
    HT_FeatureCachedString* f = HT_FeatureCachedString_from_timeline(timeline);
    HT_ErrorCode error_code;

    assert(f);

    HT_FCS_LOCK_(f);

    const char* ret = ht_hash_map_insert(map, hash, label, &error_code);

    HT_FCS_UNLOCK_(f);
    if (error_code != HT_ERR_OK)
    {
        return 0;
    }

    if (ret == NULL)
    {
        HT_TIMELINE_PUSH_EVENT(timeline, HT_StringMappingEvent, hash, label);
    }
    return hash;
}

uintptr_t
ht_feature_cached_string_add_mapping(HT_Timeline* timeline, const char* label)
{
    HT_FeatureCachedString* f = HT_FeatureCachedString_from_timeline(timeline);
    return ht_feature_cached_string_add_mapping_(timeline, &f->static_hashes, (uintptr_t)label, label);
}

static HT_Boolean
ht_feature_cached_string_push_event(uint64_t key, const char* value, void* ud)
{
    HT_TIMELINE_PUSH_EVENT((HT_Timeline*)ud, HT_StringMappingEvent, (uintptr_t)key, value);
    return HT_TRUE;
}

void
ht_feature_cached_string_push_map(HT_Timeline* timeline)
{
    HT_FeatureCachedString* f = HT_FeatureCachedString_from_timeline(timeline);

    assert(f);

    HT_FCS_LOCK_(f);

    ht_hash_map_for_each(&f->static_hashes, ht_feature_cached_string_push_event, timeline);
    ht_hash_map_for_each(&f->dynamic_hashes, ht_feature_cached_string_push_event, timeline);

    HT_FCS_UNLOCK_(f);
}

uintptr_t
ht_feature_cached_string_add_mapping_dynamic(HT_Timeline* timeline, const char* label)
{
    HT_FeatureCachedString* f = HT_FeatureCachedString_from_timeline(timeline);
    uintptr_t hash_value;

    assert(f);

    hash_value = djb2_hash(label);

    HT_FCS_LOCK_(f);

    if (ht_hash_map_get_value(&f->dynamic_hashes, hash_value) == NULL)
    {
        HT_FCS_UNLOCK_(f);

        size_t label_len = strlen(label);
        char* new_label = (char*)ht_alloc(label_len + 1);
        memcpy(new_label, label, label_len);
        new_label[label_len] = 0;

        ht_feature_cached_string_add_mapping_(timeline, &f->dynamic_hashes, hash_value, new_label);
    }
    else
    {
        HT_FCS_UNLOCK_(f);
    }

    return hash_value;
}

HT_ErrorCode
ht_feature_cached_string_enable(HT_Timeline* timeline, HT_Boolean thread_safe)
{
    HT_ErrorCode error_code;
    HT_Feature* feature = ht_feature_cached_string_create(thread_safe, &error_code);

    if (!feature)
    {
        return error_code;
    }

    return ht_timeline_set_feature(timeline, feature);
}
