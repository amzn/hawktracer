#include "internal/bag.h"
#include "hawktracer/alloc.h"

#include <string.h>

static inline HT_Boolean
_ht_bag_resize(HT_Bag* bag, size_t new_capacity)
{
    void* ptr = ht_realloc(bag->data, new_capacity * bag->element_size);

    if (ptr == NULL)
    {
        return HT_FALSE;
    }

    bag->data = ptr;
    bag->capacity = new_capacity;

    return HT_TRUE;
}

HT_ErrorCode
ht_bag_init(HT_Bag* bag, size_t min_capacity, size_t element_size)
{
    bag->data = ht_alloc(min_capacity * element_size);

    if (bag->data == NULL)
    {
        return HT_ERR_OUT_OF_MEMORY;
    }

    bag->min_capacity = min_capacity;
    bag->capacity = min_capacity;
    bag->size = 0;
    bag->element_size = element_size;

    return HT_ERR_OK;
}

void
ht_bag_remove(HT_Bag* bag, void* data)
{
    size_t i;
    for (i = 0; i < bag->size; i++)
    {
        if (!memcmp(ht_bag_get_n(bag, i), data, bag->element_size))
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
        memcpy(ht_bag_get_n(bag, n), ht_bag_get_n(bag, bag->size), bag->element_size);
    }

    if (bag->capacity > bag->min_capacity && bag->size < bag->capacity / 4)
    {
        _ht_bag_resize(bag, bag->capacity / 2);
    }
}

HT_ErrorCode
ht_bag_add(HT_Bag* bag, void* data)
{
    if (bag->capacity == bag->size)
    {
        if (_ht_bag_resize(bag, bag->capacity * 2) == HT_FALSE)
        {
            return HT_ERR_OUT_OF_MEMORY;
        }
    }

    memcpy(ht_bag_get_n(bag, bag->size), data, bag->element_size);
    bag->size++;

    return HT_ERR_OK;
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
    bag->element_size = 0;
    bag->capacity = 0;
    bag->size = 0;
}

void* 
ht_bag_get_n(HT_Bag* bag, size_t n)
{
    return HT_PTR_ADD(bag->data, n * bag->element_size);
}
