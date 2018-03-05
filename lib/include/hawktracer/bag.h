#ifndef HAWKTRACER_BAG_H
#define HAWKTRACER_BAG_H

#include <hawktracer/macros.h>

#include <stddef.h>

HT_DECLS_BEGIN

typedef struct _HT_Bag HT_Bag;

struct _HT_Bag
{
    size_t min_capacity;
    size_t capacity;
    size_t size;
    void** data;
};

HT_API void ht_bag_init(HT_Bag* bag, size_t capacity);

HT_API void ht_bag_deinit(HT_Bag* bag);

HT_API void ht_bag_remove(HT_Bag* bag, void* data);

HT_API void ht_bag_remove_nth(HT_Bag* bag, size_t n);

HT_API void ht_bag_add(HT_Bag* bag, void* data);

HT_API void ht_bag_clear(HT_Bag* bag);

#define ht_bag_last(bag) bag.data[bag.size - 1]

HT_DECLS_END

#endif /* HAWKTRACER_BAG_H */
