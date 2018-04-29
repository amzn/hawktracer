#include "hawktracer/bag.h"
#include "hawktracer/alloc.h"

static inline HT_Boolean
_ht_bag_resize(HT_Bag* bag, size_t new_capacity)
{
    void** ptr = (void**)ht_realloc(bag->data, new_capacity * sizeof(void*));

    if (ptr == NULL)
    {
        return HT_FALSE;
    }

    bag->data = ptr;
    bag->capacity = new_capacity;

    return HT_TRUE;
}

HT_Boolean
ht_bag_init(HT_Bag* bag, size_t min_capacity)
{
    bag->data = ht_alloc(min_capacity * sizeof(void*));

    if (bag->data == NULL)
    {
        return HT_FALSE;
    }

    bag->min_capacity = min_capacity;
    bag->capacity = min_capacity;
    bag->size = 0;

    return HT_TRUE;
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

HT_Boolean
ht_bag_add(HT_Bag* bag, void* data)
{
    if (bag->capacity == bag->size)
    {
        if (_ht_bag_resize(bag, bag->capacity * 2) == HT_FALSE)
        {
            return HT_FALSE;
        }
    }

    bag->data[bag->size++] = data;

    return HT_TRUE;
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
