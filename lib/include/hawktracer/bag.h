#ifndef HAWKTRACER_BAG_H
#define HAWKTRACER_BAG_H

#include <hawktracer/macros.h>

#include <stddef.h>

HT_DECLS_BEGIN

/**
 * @brief A container holding pointers.
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
 * @brief Initializes HT_Bag structure.
 *
 * @param bag a pointer to the container.
 * @param min_capacity minimal capacity of the container.
 */
HT_API void ht_bag_init(HT_Bag* bag, size_t min_capacity);

/**
 * @brief Uninitializes bag structure.
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
 * @brief Removes element from the container.
 *
 * The function removes all entries which equal
 * @a data value.
 *
 * @param bag a pointer to the container.
 * @param data an object to remove.
 */
HT_API void ht_bag_remove(HT_Bag* bag, void* data);

/**
 * @brief Removes nth element from the container.
 *
 * @param bag a pointer to the container.
 * @param n an index of element to remove.
 */
HT_API void ht_bag_remove_nth(HT_Bag* bag, size_t n);

/**
 * @brief Adds element to the end of the container.
 *
 * @param bag a pointer to the container.
 * @param data a value to be added to the container.
 */
HT_API void ht_bag_add(HT_Bag* bag, void* data);

/**
 * @brief Removes all elements from the container.
 *
 * @param bag a pointer to the container.
 */
HT_API void ht_bag_clear(HT_Bag* bag);

/**
 * @brief Gets last element from the container.
 *
 * @param bag a pointer to the container.
 *
 * @warning If the @a bag is empty, behavior of this function
 * is undefined.
 */
#define ht_bag_last(bag) bag.data[bag.size - 1]

HT_DECLS_END

#endif /* HAWKTRACER_BAG_H */
