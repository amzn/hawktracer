#ifndef HAWKTRACER_MEMORY_USAGE_H
#define HAWKTRACER_MEMORY_USAGE_H

#include <hawktracer/base_types.h>
#include <hawktracer/config.h>

#ifndef HT_PLATFORM_FEATURE_MEMORY_USAGE_ENABLED
#  error "This feature is not enabled therefore this file should not be included."
#endif /* HT_PLATFORM_FEATURE_MEMORY_USAGE_ENABLED */

HT_DECLS_BEGIN

/** Defines an invalid value for memory which is returned when
 * getting a memory usage fails. */
#define HT_MEMORY_USAGE_INVALID_VALUE ((size_t)-1)

/** A forward declaration for memory usage context. This structure
  * should be defined in the implementation file. */
typedef struct _HT_MemoryUsageContext HT_MemoryUsageContext;

/**
 * Creates a context for a memory usage feature.
 * This context is used for getting memory usage, and the
 * structure is platform specific. The context should always be
 * destroyed using ht_memory_usage_context_destroy().
 *
 * @param process_id a platform specific description of process ID,
 * or NULL to get memory usage of the current process.
 * For linux-based platforms, it should be a pointer to a variable
 * of type integer (int), which holds the process ID.
 *
 * @return the memory usage context or NULL if the function failed to create a context.
 */
HT_API HT_MemoryUsageContext* ht_memory_usage_context_create(void* process_id);

/**
 * Destroys memory context.
 *
 * @param context a pointer to the context.
 */
HT_API void ht_memory_usage_context_destroy(HT_MemoryUsageContext* context);

/**
 * Gets a memory usage of the process in kilobytes.
 *
 * The process ID should be specified in ht_memory_usage_context_create().
 *
 * @param context a memory usage context.
 *
 * @return a memory usage of a specified process in kilobytes, or
 * HT_MEMORY_USAGE_INVALID_VALUE value on error.
 */

/**
 * Gets a memory usage of the process in bytes.
 *
 * @param context a memory usage context.
 *
 * @param virtual_memory_bytes a pointer to a variable where virtual memory
 * used by the process will be stored, or NULL if the value should not be read.
 * @param shared_memory_bytes a pointer to a variable where shared memory
 * used by the process will be stored, or NULL if the value should not be read.
 * @param resident_memory_bytes a pointer to a variable where resident memory
 * used by the process will be stored, or NULL if the value should not be read.
 *
 * @returns #HT_ERR_OK, if memory has been read succesfully; otherwise, appropriate error code.
 */
HT_API HT_ErrorCode ht_memory_usage_get_usage(HT_MemoryUsageContext* context,
                                              size_t* virtual_memory_bytes,
                                              size_t* shared_memory_bytes,
                                              size_t* resident_memory_bytes);

HT_DECLS_END

#endif /* HAWKTRACER_MEMORY_USAGE_H */
