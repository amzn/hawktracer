#ifndef HAWKTRACER_ALLOC_H
#define HAWKTRACER_ALLOC_H

#include <hawktracer/macros.h>

#include <stddef.h>

HT_DECLS_BEGIN

/**
 * Callback function for a custom allocator.
 *
 * HawkTracer requires only one function, which depending on
 * parameter values, performs: alloc, free, and realloc operations.
 *
 * To allocate memory, @p ptr must be NULL.
 * To free memory, @p size must be equal to 0.
 * To realloc memory, @p size must be greater than 0, and @p ptr must not
 * be NULL.
 *
 * @param ptr a pointer to a memory block for alloc/realloc/free operations.
 * @param size a size of memory block to allocate for alloc/realloc operations.
 * @param user_data an user data specified in ht_allocator_set().
 *
 * @return a pointer to allocated memory block for alloc/realloc operations.
 */
typedef void* (*realloc_function)(void* ptr, size_t size, void* user_data);

/**
 * Sets a global allocator for HawkTracer library.
 *
 * The allocator is used for every allocation in the library. The function
 * must be called before ht_init() function call.
 * If custom allocator is not specified, default allocator will be used.
 *
 * @param func an allocation function.
 * @param user_data an opaque pointer passed to the allocator as a last argument.
 */
HT_API void ht_allocator_set(realloc_function func, void* user_data);

/**
 * Allocates memory using HawkTracer allocator.
 *
 * @param size a number of bytes to allocate.
 *
 * @return a pointer to allocated memory.
 */
HT_API void* ht_alloc(size_t size);

/**
 * Changes the size of the memory block.
 *
 * @param ptr a pointer to the memory block.
 * @param size new size of the memory block.
 *
 * @return a pointer to reallocated memory.
 */
HT_API void* ht_realloc(void* ptr, size_t size);

/**
 * Releases memory allocated by ht_alloc.
 *
 * @param ptr a pointer to the memory block to release.
 */
HT_API void ht_free(void* ptr);

/**
 * Allocates memory for a specific @a Type.
 *
 * @param Type a type of an object to allocate memory for.
 *
 * @return a pointer to allocated memory.
 */
#define HT_CREATE_TYPE(Type) (Type*)ht_alloc(sizeof(Type))

HT_DECLS_END

#endif /* HAWKTRACER_ALLOC_H */
