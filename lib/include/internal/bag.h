#ifndef HAWKTRACER_INTERNAL_BAG_H
#define HAWKTRACER_INTERNAL_BAG_H

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
    HT_API void ht_bag##METHOD_SUFFIX##_remove(HT_Bag##TYPE_SUFFIX* bag, TYPE data); \
    HT_API void ht_bag##METHOD_SUFFIX##_remove_nth(HT_Bag##TYPE_SUFFIX* bag, size_t n); \
    HT_API HT_ErrorCode ht_bag##METHOD_SUFFIX##_add(HT_Bag##TYPE_SUFFIX* bag, TYPE data); \
    HT_API void ht_bag##METHOD_SUFFIX##_clear(HT_Bag##TYPE_SUFFIX* bag);

#define ht_bag_last(bag) bag.data[bag.size - 1]
#define ht_bag_size(bag) bag.size
#define ht_bag_nth(bag, nth) bag.data[nth]

HT_DECLARE_BAG_TYPE(UInt64, _uint64, uint64_t)
HT_DECLARE_BAG_TYPE(VoidPtr, _void_ptr, void*)
HT_DECLARE_BAG_TYPE(Int, _int, int)

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_BAG_H */
