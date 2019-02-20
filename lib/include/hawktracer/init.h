#ifndef HAWKTRACER_INIT_H
#define HAWKTRACER_INIT_H

#include <hawktracer/base_types.h>
#include <hawktracer/macros.h>

HT_DECLS_BEGIN

/**
 * Initializes HawkTracer library.
 *
 * This function must be called before any other function
 * from this library. The only exception is ht_allocator_set(),
 * which must be called before ht_init().
 *
 * The function can be called multiple times, but only the first
 * call initializes the library. All the other calls don't have any effect.
 *
 * This function is not thread-safe, i.e. it must not be called from
 * two different threads at the same time.
 *
 * @param argc a number of arguments of the @a argv array.
 * @param argv an array of strings - arguments for HawkTracer library.
 */
HT_API void ht_init(int argc, char** argv); /* TODO: consider passing allocator here */

/**
 * Uninitializes HawkTracer library.
 *
 * This function must be called as a last function
 * of the library in the program. The only exception is
 * ht_timeline_deinit() which might be called after ht_deinit(),
 * however, it's highly not recommended and should be avoided
 * when possible.
 *
 * This function is not thread-safe, i.e. it must not be called from
 * two different threads at the same time.
 */
HT_API void ht_deinit(void);

/**
 * Checks if the HawkTracer library is initialized.
 *
 * @return #HT_TRUE, if the library is initialized; otherwise, #HT_FALSE.
 */
HT_API HT_Boolean ht_is_initialized(void);

HT_DECLS_END

#endif /* HAWKTRACER_INIT_H */
