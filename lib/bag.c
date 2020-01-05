#include "internal/bag.h"
#include "hawktracer/alloc.h"

#define HT_DEFINE_BAG_TYPE(TYPE_SUFFIX, METHOD_SUFFIX, TYPE) \
    static HT_INLINE HT_Boolean \
    ht_bag##METHOD_SUFFIX##_resize_(HT_Bag##TYPE_SUFFIX* bag, size_t new_capacity) \
    { \
        TYPE* ptr = (TYPE*)ht_realloc(bag->data, new_capacity * sizeof(void*)); \
        \
        if (ptr == NULL) \
        { \
            return HT_FALSE; \
        } \
        \
        bag->data = ptr; \
        bag->capacity = new_capacity; \
        \
        return HT_TRUE; \
    } \
    \
    HT_ErrorCode \
    ht_bag##METHOD_SUFFIX##_init(HT_Bag##TYPE_SUFFIX* bag, size_t min_capacity) \
    { \
        bag->data = (TYPE*)ht_alloc(min_capacity * sizeof(TYPE)); \
        \
        if (bag->data == NULL) \
        { \
            return HT_ERR_OUT_OF_MEMORY; \
        } \
        \
        bag->min_capacity = min_capacity; \
        bag->capacity = min_capacity; \
        bag->size = 0; \
        \
        return HT_ERR_OK; \
    } \
    \
    void \
    ht_bag##METHOD_SUFFIX##_remove(HT_Bag##TYPE_SUFFIX* bag, TYPE data) \
    { \
        size_t i; \
        for (i = 0; i < bag->size; i++) \
        { \
            if (bag->data[i] == data) \
            { \
                ht_bag##METHOD_SUFFIX##_remove_nth(bag, i); \
            } \
        } \
    } \
    \
    void \
    ht_bag##METHOD_SUFFIX##_remove_nth(HT_Bag##TYPE_SUFFIX* bag, size_t n) \
    { \
        bag->size--; \
        if (bag->size > 0) \
        { \
            bag->data[n] = bag->data[bag->size]; \
        } \
        \
        if (bag->capacity > bag->min_capacity && bag->size < bag->capacity / 4) \
        { \
            ht_bag##METHOD_SUFFIX##_resize_(bag, bag->capacity / 2); \
        } \
    } \
    \
    HT_ErrorCode \
    ht_bag##METHOD_SUFFIX##_add(HT_Bag##TYPE_SUFFIX* bag, TYPE data) \
    { \
        if (bag->capacity == bag->size) \
        { \
            if (ht_bag##METHOD_SUFFIX##_resize_(bag, bag->capacity * 2) == HT_FALSE) \
            { \
                return HT_ERR_OUT_OF_MEMORY; \
            } \
        } \
        \
        bag->data[bag->size++] = data; \
        \
        return HT_ERR_OK; \
    } \
    \
    void \
    ht_bag##METHOD_SUFFIX##_clear(HT_Bag##TYPE_SUFFIX* bag) \
    { \
        ht_bag##METHOD_SUFFIX##_resize_(bag, bag->min_capacity); \
        bag->size = 0; \
    } \
    \
    void \
    ht_bag##METHOD_SUFFIX##_deinit(HT_Bag##TYPE_SUFFIX* bag) \
    { \
        ht_free(bag->data); \
        bag->data = NULL; \
        bag->capacity = 0; \
        bag->size = 0; \
    }

HT_DEFINE_BAG_TYPE(UInt64, _uint64, uint64_t)
HT_DEFINE_BAG_TYPE(VoidPtr, _void_ptr, void*)
HT_DEFINE_BAG_TYPE(Int, _int, int)
