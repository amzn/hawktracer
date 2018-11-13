#ifndef HAWKTRACER_CPU_USAGE_H
#define HAWKTRACER_CPU_USAGE_H

#include <hawktracer/base_types.h>
#include <hawktracer/config.h>

#ifdef HT_PLATFORM_FEATURE_CPU_USAGE_ENABLED

HT_DECLS_BEGIN

/** A forward declaration for CPU Usage context. This structure
  * should be defined in the implementation file. */
typedef struct _HT_CPUUsageContext HT_CPUUsageContext;

/**
 * Creates a context for a CPU usage feature.
 * This context is used for getting percentage CPU usage, and the
 * structure is platform specific. The context should always be
 * destroyed using ht_cpu_usage_context_destroy().
 *
 * @param process_id a platform specific description of process ID,
 * or NULL to get CPU usage of the current process.
 * For linux-based platforms, it should be a pointer to a variable
 * of type integer (int), which holds the process ID.
 *
 * @return the CPU Usage context.
 */
HT_API HT_CPUUsageContext* ht_cpu_usage_context_create(void* process_id);

/**
 * Destroys CPU Usage context.
 *
 * @param context a pointer to the context.
 */
HT_API void ht_cpu_usage_context_destroy(HT_CPUUsageContext* context);

/**
 * Gets a CPU usage percentage for a process.
 *
 * The process ID should be specified in ht_cpu_usage_context_create().
 *
 * @param context a CPU Usage context.
 *
 * @return a CPU usage percentage of a specified process, or negative
 * value on error.
 */
HT_API float ht_cpu_usage_get_percentage(HT_CPUUsageContext* context);

HT_DECLS_END

#endif /* HT_PLATFORM_FEATURE_CPU_USAGE_ENABLED */

#endif /* HAWKTRACER_CPU_USAGE_H */
