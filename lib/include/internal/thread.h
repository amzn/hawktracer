#ifndef HAWKTRACER_INTERNAL_THREAD_H
#define HAWKTRACER_INTERNAL_THREAD_H

#include "hawktracer/macros.h"
#include "hawktracer/base_types.h"

HT_DECLS_BEGIN

HT_ThreadId
ht_thread_get_current_thread_id(void);

HT_DECLS_END

#endif /* HAWKTRACER_INTERNAL_THREAD_H */
