#include "hawktracer/bag.h"
#include "hawktracer/alloc.h"

static inline void
_ht_bag_resize(HT_Bag* bag, size_t new_capacity)
{
    bag->capacity = new_capacity;
    bag->data = ht_realloc(bag->data, bag->capacity * sizeof(void*));
}

void
ht_bag_init(HT_Bag* bag, size_t capacity)
{
    bag->min_capacity = capacity;
    bag->capacity = capacity;
    bag->size = 0;
    bag->data = ht_alloc(capacity * sizeof(void*));
}

void
ht_bag_remove(HT_Bag* bag, void* data)
{
    size_t i;
    for (i = 0; i < bag->size; i++)
    {
        if (bag->data[i] == data)
        {
            ht_bag_remove_nth(bag, i);
        }
    }
}

void
ht_bag_remove_nth(HT_Bag* bag, size_t n)
{
    bag->size--;
    if (bag->size > 0)
    {
        bag->data[n] = bag->data[bag->size];
    }

    if (bag->capacity > bag->min_capacity && bag->size < bag->capacity / 4)
    {
        _ht_bag_resize(bag, bag->capacity / 2);
    }
}

void
ht_bag_add(HT_Bag* bag, void* data)
{
    if (bag->capacity == bag->size)
    {
        _ht_bag_resize(bag, bag->capacity * 2);
    }

    bag->data[bag->size++] = data;
}

void
ht_bag_clear(HT_Bag* bag)
{
    _ht_bag_resize(bag, bag->min_capacity);
    bag->size = 0;
}

void
ht_bag_deinit(HT_Bag* bag)
{
    ht_free(bag->data);
    bag->data = NULL;
    bag->capacity = 0;
    bag->size = 0;
}
