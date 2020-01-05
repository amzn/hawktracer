#include "hawktracer/alloc.h"
#include "internal/bag.h"
#include "internal/hash.h"
#include "internal/hash_map.h"
#include "internal/error.h"

#include <string.h>

#define BUCKET_COUNT(hash_map) (sizeof(hash_map->buckets)/sizeof(hash_map->buckets[0]))

static HT_ErrorCode
ht_hash_bucket_init(HT_HashBucket* bucket)
{
    const size_t min_capacity = 4;
    HT_ErrorCode err = ht_bag_void_ptr_init(&bucket->value_bag, min_capacity);
    if (err != HT_ERR_OK)
    {
        return err;
    }

    err = ht_bag_uint64_init(&bucket->key_bag, min_capacity);
    if (err != HT_ERR_OK)
    {
        ht_bag_void_ptr_deinit(&bucket->value_bag);
    }

    return err;
}

static void
ht_hash_bucket_deinit(HT_HashBucket* bucket)
{
    ht_bag_void_ptr_deinit(&bucket->value_bag);
    ht_bag_uint64_deinit(&bucket->key_bag);
}

static HT_Boolean
ht_hash_bucket_find_value(HT_HashBucket* bucket, uint64_t key, size_t* out_index)
{
    for (size_t i = 0; i < ht_bag_size(bucket->key_bag); i++)
    {
        if (ht_bag_nth(bucket->key_bag, i) == key)
        {
            *out_index = i;
            return HT_TRUE;
        }
    }
    return HT_FALSE;
}

static HT_Boolean
ht_hash_bucket_for_each(HT_HashBucket* bucket, HT_Boolean(*callback)(uint64_t, const char*, void* ud), void* ud)
{
    for (size_t i = 0; i < ht_bag_size(bucket->key_bag); i++)
    {
        if (!callback(ht_bag_nth(bucket->key_bag, i), (const char*)ht_bag_nth(bucket->value_bag, i), ud))
        {
            return HT_FALSE;
        }
    }
    return HT_TRUE;
}

HT_ErrorCode
ht_hash_map_init(HT_HashMap* hash_map)
{
    size_t i = 0;
    HT_ErrorCode err;
    for (i = 0; i < BUCKET_COUNT(hash_map); i++)
    {
        err = ht_hash_bucket_init(&hash_map->buckets[i]);
        if (err != HT_ERR_OK)
        {
            break;
        }
    }
    if (err != HT_ERR_OK)
    {
        for (size_t j = 0; j < i; j++)
        {
            ht_hash_bucket_deinit(&hash_map->buckets[j]);
        }
    }
    return err;
}

void
ht_hash_map_deinit(HT_HashMap* hash_map)
{
    for (size_t i = 0; i < BUCKET_COUNT(hash_map); i++)
    {
        ht_hash_bucket_deinit(&hash_map->buckets[i]);
    }
}

const char*
ht_hash_map_insert(HT_HashMap* map, uint64_t key, const char* value, HT_ErrorCode* out_err)
{
    // TODO:
    // 1) static assert for size of uintptr_t (feature cached string)
    size_t index;
    HT_HashBucket* bucket = &map->buckets[key % BUCKET_COUNT(map)];
    if (ht_hash_bucket_find_value(bucket, key, &index))
    {
        const char* prev = (const char*)ht_bag_nth(bucket->value_bag, index);
        ht_bag_nth(bucket->value_bag, index) = (void*)value;
        HT_SET_ERROR(out_err, HT_ERR_OK);
        return prev;
    }

    HT_ErrorCode err = ht_bag_uint64_add(&bucket->key_bag, key);
    if (err != HT_ERR_OK)
    {
        HT_SET_ERROR(out_err, err);
        return NULL;
    }
    err = ht_bag_void_ptr_add(&bucket->value_bag, (void*)value);
    if (err != HT_ERR_OK)
    {
        ht_bag_uint64_remove_nth(&bucket->key_bag, ht_bag_size(bucket->key_bag));
    }

    HT_SET_ERROR(out_err, err);
    return NULL;
}

const char*
ht_hash_map_get_value(HT_HashMap* map, uint64_t key)
{
    HT_HashBucket* bucket = &map->buckets[key % BUCKET_COUNT(map)];
    size_t index;
    if (ht_hash_bucket_find_value(bucket, key, &index))
    {
        return (const char*)ht_bag_nth(bucket->value_bag, index);
    }
    return NULL;
}

void ht_hash_map_for_each(HT_HashMap* hash_map, HT_Boolean(*callback)(uint64_t, const char*, void* ud), void* ud)
{
    for (size_t i = 0; i < BUCKET_COUNT(hash_map); i++)
    {
        if (!ht_hash_bucket_for_each(&hash_map->buckets[i], callback, ud))
        {
            return;
        }
    }
}
