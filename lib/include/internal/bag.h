#ifndef HAWKTRACER_INTERNAL_BAG_H
#define HAWKTRACER_INTERNAL_BAG_H

#include <hawktracer/macros.h>
#include <hawktracer/base_types.h>

#include <stddef.h>

HT_DECLS_BEGIN

/**
 * A container holding pointers.
 */
typedef struct
{
    /* private */
    size_t min_capacity;
    size_t capacity;
    size_t size;
    size_t element_size;
    void* data;
} HT_Bag;

/**
 * Initializes HT_Bag structure.
 *
 * @param bag a pointer to the container.
 * @param min_capacity minimal capacity of the container.
 * @param element_size size of each element in the bag.
 *
 * @returns #HT_ERR_OK, if initialization completed successfully; otherwise, appropriate error code.
 *
 * Initialization might fail, if the process can't allocate enough memory.
 * In that case, #HT_ERR_OUT_OF_MEMORY is returned.
 */
HT_API HT_ErrorCode ht_bag_init(HT_Bag* bag, size_t min_capacity, size_t element_size);

/**
 * Uninitializes bag structure.
 *
 * This function releases all the memory allocated for
 * the bag and resets all the fields. After this function
 * call, @a bag should no longer be used (unless ht_bag_init()
 * is called on the same object).
 *
 * @param bag a pointer to the container.
 */
HT_API void ht_bag_deinit(HT_Bag* bag);

/**
 * Removes element from the container.
 *
 * The function removes all entries which equal
 * @a data value.
 *
 * @param bag a pointer to the container.
 * @param data an object to remove.
 */
HT_API void ht_bag_remove(HT_Bag* bag, void* data);

/**
 * Removes nth element from the container.
 *
 * @param bag a pointer to the container.
 * @param n an index of element to remove.
 */
HT_API void ht_bag_remove_nth(HT_Bag* bag, size_t n);

/**
 * Adds element to the end of the container.
 *
 * @param bag a pointer to the container.
 * @param data a value to be added to the container.
 *
 * @returns #HT_ERR_OK, if element was successfully added to the container; otherwise, appropriate error code.
 *
 * The function might fail, if the process can't allocate enough memory to extend internal data
 * buffer (#HT_ERR_OUT_OF_MEMORY). The function never fails, if ht_bag_init() doesn't fail
 * and number of elements in the bag is less than @a min_capacity.
 */
HT_API HT_ErrorCode ht_bag_add(HT_Bag* bag, void* data);

/**
 * Removes all elements from the container.
 *
 * @param bag a pointer to the container.
 */
HT_API void ht_bag_clear(HT_Bag* bag);

/**
 * Gets last element from the container.
 *
 * @param bag a pointer to the container.
 *
 * @warning If the @a bag is empty, behavior of this function
 * is undefined.
 */
#define ht_bag_last(bag) HT_PTR_ADD(bag.data, (bag.size - 1) * bag.element_size)

HT_API void* ht_bag_get_n(HT_Bag* bag, size_t n);

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_BAG_H */
