#ifndef HAWKTRACER_THREAD_H
#define HAWKTRACER_THREAD_H

#include "hawktracer/macros.h"
#include "hawktracer/base_types.h"

HT_DECLS_BEGIN

/**
 * @brief Gets a unique identifier of the current thread.
 *
 * @return an identifier of the current thread.
 */
HT_API HT_ThreadId ht_thread_get_current_thread_id(void);

HT_DECLS_END

#endif /* HAWKTRACER_THREAD_H */
