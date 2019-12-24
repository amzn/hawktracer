#include "hawktracer/feature_cached_string.h"
#include "hawktracer/alloc.h"
#include "hawktracer/core_events.h"
#include "internal/hash.h"
#include "internal/bag.h"
#include "internal/mutex.h"
#include "internal/error.h"
#include "internal/feature.h"

#include <assert.h>

#ifdef __cplusplus
#include <set>
#include <new>
#endif /* __cplusplus */

static void
ht_feature_cached_string_destroy(HT_Feature* f);

typedef struct
{
    HT_Feature base;
    HT_BagVoidPtr cached_data;
    HT_Mutex* lock;
#ifdef __cplusplus
    std::set<uint32_t> hashes;
#endif /* __cplusplus */
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

static void ht_feature_cached_string_free_(HT_FeatureCachedString* f)
{
    if (f->lock)
    {
        ht_mutex_destroy(f->lock);
    }
#ifdef __cplusplus
    f->~HT_FeatureCachedString();
#endif /* __cplusplus */
    ht_free(f);
}

static HT_Feature*
ht_feature_cached_string_create(HT_Boolean thread_safe, HT_ErrorCode* out_err)
{
    HT_FeatureCachedString* feature = HT_FeatureCachedString_alloc();
    HT_ErrorCode error_code = HT_ERR_OK;

    if (feature == NULL)
    {
        HT_SET_ERROR(out_err, HT_ERR_OUT_OF_MEMORY);
        return NULL;
    }

#ifdef __cplusplus
    new (feature) HT_FeatureCachedString();
    // new() overrides base struct so we need to set it again
    feature->base.klass = HT_FeatureCachedString_get_class();
#endif /* __cplusplus */

    if (thread_safe)
    {
        feature->lock = ht_mutex_create();
        if (feature->lock == NULL)
        {
            HT_SET_ERROR(out_err, HT_ERR_UNKNOWN);
            ht_feature_cached_string_free_(feature);
            return NULL;
        }
    }
    else
    {
        feature->lock = NULL;
    }

    error_code = ht_bag_void_ptr_init(&feature->cached_data, 1024);

    if (error_code != HT_ERR_OK)
    {
        ht_feature_cached_string_free_(feature);
        feature = NULL;
    }

    HT_SET_ERROR(out_err, error_code);
    return (HT_Feature*)feature;
}

static void
ht_feature_cached_string_destroy(HT_Feature* f)
{
    HT_FeatureCachedString* feature = (HT_FeatureCachedString*)f;

    ht_bag_void_ptr_deinit(&feature->cached_data);
    ht_feature_cached_string_free_(feature);
}

static uintptr_t
ht_feature_cached_string_add_mapping_(HT_Timeline* timeline, const char* label, uintptr_t hash)
{
    HT_FeatureCachedString* f = HT_FeatureCachedString_from_timeline(timeline);
    HT_ErrorCode error_code;

    assert(f);

    HT_FCS_LOCK_(f);

    error_code = ht_bag_void_ptr_add(&f->cached_data, (void*)hash);

    HT_FCS_UNLOCK_(f);
    if (error_code != HT_ERR_OK)
    {
        return 0;
    }

    HT_TIMELINE_PUSH_EVENT(timeline, HT_StringMappingEvent, (uintptr_t)hash, label);

    return hash;
}

uintptr_t
ht_feature_cached_string_add_mapping(HT_Timeline* timeline, const char* label)
{
    return ht_feature_cached_string_add_mapping_(timeline, label, (uintptr_t)label);
}

void
ht_feature_cached_string_push_map(HT_Timeline* timeline)
{
    HT_FeatureCachedString* f = HT_FeatureCachedString_from_timeline(timeline);
    size_t i;

    assert(f);

    HT_FCS_LOCK_(f);

    for (i = 0; i < f->cached_data.size; i++)
    {
        HT_TIMELINE_PUSH_EVENT(timeline, HT_StringMappingEvent, (uintptr_t)f->cached_data.data[i], (const char*)f->cached_data.data[i]);
    }

    HT_FCS_UNLOCK_(f);
}

#ifdef __cplusplus
uintptr_t
ht_feature_cached_string_add_mapping_dynamic(HT_Timeline* timeline, const char* label)
{
    HT_FeatureCachedString* f = HT_FeatureCachedString_from_timeline(timeline);
    uintptr_t hash_value;

    assert(f);

    hash_value = (uintptr_t)djb2_hash(label);

    HT_FCS_LOCK_(f);

    auto it = f->hashes.find(hash_value);
    if (it == f->hashes.end())
    {
        f->hashes.insert(hash_value);
        HT_FCS_UNLOCK_(f);
        ht_feature_cached_string_add_mapping_(timeline, label, hash_value);
    }
    else
    {
        HT_FCS_UNLOCK_(f);
    }

    return hash_value;
}
#endif /* __cplusplus */

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
