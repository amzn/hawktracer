#ifndef HAWKTRACER_INTERNAL_BAG_H
#define HAWKTRACER_INTERNAL_BAG_H

#include <hawktracer/alloc.h>
#include <hawktracer/macros.h>
#include <hawktracer/base_types.h>

#include <stddef.h>

HT_DECLS_BEGIN

#define HT_DECLARE_BAG_TYPE(TYPE_SUFFIX, METHOD_SUFFIX, TYPE) \
    typedef struct \
    { \
        /* private */ \
        size_t min_capacity; \
        size_t capacity; \
        size_t size; \
        TYPE* data; \
    } HT_Bag##TYPE_SUFFIX; \
    \
    HT_API HT_ErrorCode ht_bag##METHOD_SUFFIX##_init(HT_Bag##TYPE_SUFFIX* bag, size_t min_capacity); \
    HT_API void ht_bag##METHOD_SUFFIX##_deinit(HT_Bag##TYPE_SUFFIX* bag); \
    HT_API void ht_bag##METHOD_SUFFIX##_remove_nth(HT_Bag##TYPE_SUFFIX* bag, size_t n); \
    HT_API HT_ErrorCode ht_bag##METHOD_SUFFIX##_add(HT_Bag##TYPE_SUFFIX* bag, TYPE data); \
    HT_API void ht_bag##METHOD_SUFFIX##_clear(HT_Bag##TYPE_SUFFIX* bag);

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

#define ht_bag_last(bag) bag.data[bag.size - 1]
#define ht_bag_size(bag) bag.size
#define ht_bag_nth(bag, nth) bag.data[nth]

HT_DECLARE_BAG_TYPE(UInt64, _uint64, uint64_t)
HT_DECLARE_BAG_TYPE(VoidPtr, _void_ptr, void*)
HT_DECLARE_BAG_TYPE(Int, _int, int)

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_BAG_H */
