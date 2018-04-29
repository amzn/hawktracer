#ifndef HAWKTRACER_BAG_H
#define HAWKTRACER_BAG_H

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
    void** data;
} HT_Bag;

/**
 * Initializes HT_Bag structure.
 *
 * @param bag a pointer to the container.
 * @param min_capacity minimal capacity of the container.
 *
 * @returns #HT_TRUE, if initialization completed successfully; otherwise, #HT_FALSE.
 *
 * Initialization might fail, if the process can't allocate enough memory.
 */
HT_API HT_Boolean ht_bag_init(HT_Bag* bag, size_t min_capacity);

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
 * @returns  #HT_TRUE, if element was successfully added to the container; otherwise, #HT_FALSE.
 *
 * The function might fail, if the process can't allocate enough memory to extend internal data
 * buffer. The function never fails, if ht_bag_init() doesn't fail and number of elements in the bag is
 * less than @a min_capacity.
 */
HT_API HT_Boolean ht_bag_add(HT_Bag* bag, void* data);

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
#define ht_bag_last(bag) bag.data[bag.size - 1]

HT_DECLS_END

#endif /* HAWKTRACER_BAG_H */
