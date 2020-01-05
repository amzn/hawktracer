#ifndef HAWKTRACER_INTERNAL_HASH_MAP_H
#define HAWKTRACER_INTERNAL_HASH_MAP_H

/**
 * This is NOT a real hash map - it expects the key to be a hash already,
 * which perfectly fits the cached string feature needs - this might change
 * when we need hash map for different purposes.
 *
 * For now it's just a basic hash map implementation - we've added benchmarks
 * (only CPU benchmarks for now, memory benchmarks will be added in the future),
 * so any performance improvements are very welcome.
 */

#include "internal/bag.h"

HT_DECLS_BEGIN

typedef struct
{
    HT_BagUInt64 key_bag;
    HT_BagVoidPtr value_bag;
} HT_HashBucket;

typedef struct
{
    HT_HashBucket buckets[1024];
} HT_HashMap;

HT_ErrorCode ht_hash_map_init(HT_HashMap* hash_map);

void ht_hash_map_deinit(HT_HashMap* hash_map);

const char* ht_hash_map_insert(HT_HashMap* map, uint64_t key, const char* value, HT_ErrorCode* out_err);

const char* ht_hash_map_get_value(HT_HashMap* map, uint64_t key);

void ht_hash_map_for_each(HT_HashMap* hash_map, HT_Boolean(*callback)(uint64_t, const char*, void* ud), void* ud);

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_HASH_MAP_H */
