#ifndef HAWKTRACER_ALLOC_HOOKS_H
#define HAWKTRACER_ALLOC_HOOKS_H

#include <hawktracer/base_types.h>
#include <hawktracer/config.h>

#ifndef HT_PLATFORM_FEATURE_ALLOC_HOOKS_ENABLED
#  error "This feature is not enabled therefore this file should not be included."
#endif /* HT_PLATFORM_FEATURE_ALLOC_HOOKS_ENABLED */

/** @cond skip */
HT_DECLS_BEGIN
/** @endcond */

/**
 * A function type for a malloc callback.
 *
 * @param ret_ptr return value of the malloc call. Is @a NULL for malloc pre-hook.
 * @param size a @a size parameter value of the malloc call.
 * @param user_data user defined pointer specified in ht_alloc_hooks_register_hooks() function call.
 */
typedef void(*HT_AllocHookMalloc)(void* ret_ptr, size_t size, void* user_data);

/**
 * A function type for a calloc callback.
 *
 * @param ret_ptr return value of the calloc call. Is @a NULL for calloc pre-hook.
 * @param num a @a num parameter value of the calloc call.
 * @param size a @a size parameter value of the calloc call.
 * @param user_data user defined pointer specified in ht_alloc_hooks_register_hooks() function call.
 */
typedef void(*HT_AllocHookCalloc)(void* ret_ptr, size_t num, size_t size, void* user_data);

/**
 * A function type for a realloc callback.
 *
 * @param ret_ptr return value of the realloc call. Is @a NULL for realloc pre-hook.
 * @param ptr a @a ptr parameter value of the realloc call.
 * @param size a @a size parameter value of the realloc call.
 * @param user_data user defined pointer specified in ht_alloc_hooks_register_hooks() function call.
 */
typedef void(*HT_AllocHookRealloc)(void* ret_ptr, void* ptr, size_t size, void* user_data);

/**
 * A function type for a free callback.
 *
 * @param ptr a @a ptr parameter value of the free call.
 * @param user_data user defined pointer specified in ht_alloc_hooks_register_hooks() function call.
 */
typedef void(*HT_AllocHookFree)(void* ptr, void* user_data);

/**
 * Registers allocation callbacks.
 *
 * @param pre_malloc_hook a callback that's being called before each malloc() call.
 * @param post_malloc_hook a callback that's being called after each malloc() call.
 * @param pre_calloc_hook a callback that's being called before each calloc() call.
 * @param post_calloc_hook a callback that's being called after each calloc() call.
 * @param pre_realloc_hook a callback that's being called before each realloc() call.
 * @param post_realloc_hook a callback that's being called after each realloc() call.
 * @param pre_free_hook a callback that's being called before each free() call.
 * @param post_free_hook a callback that's being called after each free() call.
 * @param user_data a user pointer that's being passed to every callback call as a last argument.
 */
HT_API void ht_alloc_hooks_register_hooks(HT_AllocHookMalloc pre_malloc_hook,
                                          HT_AllocHookMalloc post_malloc_hook,
                                          HT_AllocHookCalloc pre_calloc_hook,
                                          HT_AllocHookCalloc post_calloc_hook,
                                          HT_AllocHookRealloc pre_realloc_hook,
                                          HT_AllocHookRealloc post_realloc_hook,
                                          HT_AllocHookFree pre_free_hook,
                                          HT_AllocHookFree post_free_hook,
                                          void* user_data);

/**
 * Calls a malloc() function, without calling a malloc() hook.
 * The function should be used for allocating memory in an allocation hook to avoid infinite recursion.
 *
 * @param size size of the memory block, in bytes.
 * @return on success, a pointer to the memory block allocated by function. On failure, @a NULL value.
 */
HT_API void *ht_alloc_hooks_malloc_skip_hook(size_t size);

/**
 * Calls a calloc() function, without calling a calloc() hook.
 * The function should be used for allocating memory in an allocation hook to avoid infinite recursion.
 *
 * @param num number of elements to allocate.
 * @param size size of each element.
 * @return on success, a pointer to the memory block allocated by function. On failure, @a NULL value.
 */
HT_API void *ht_alloc_hooks_calloc_skip_hook(size_t num, size_t size);

/**
 * Calls a realloc() function, without calling a realloc() hook.
 * The function should be used for re-allocating memory in an allocation hook to avoid infinite recursion.
 *
 * @param ptr pointer to a memory block previously allocated
 * @param size new size for the memory block, in bytes.
 * @return a pointer to the reallocated memory block, which may be either the same as ptr or a new location.
 */
HT_API void *ht_alloc_hooks_realloc_skip_hook(void* ptr, size_t size);

/**
 * Calls a free() function, without calling a free() hook.
 * @param ptr pointer to memory block to be freed.
 */
HT_API void ht_alloc_hooks_free_skip_hook(void* ptr);

HT_DECLS_END

#endif /* HAWKTRACER_ALLOC_HOOKS_H */
