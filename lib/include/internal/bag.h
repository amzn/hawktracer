#ifndef HAWKTRACER_INTERNAL_BAG_H
#define HAWKTRACER_INTERNAL_BAG_H

#include "hawktracer/macros.h"

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

void ht_bag_init(HT_Bag* bag, size_t capacity);

void ht_bag_deinit(HT_Bag* bag);

void ht_bag_remove(HT_Bag* bag, void* data);

void ht_bag_add(HT_Bag* bag, void* data);

void ht_bag_clear(HT_Bag* bag);

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_BAG_H */
