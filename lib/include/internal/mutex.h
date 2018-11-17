#ifndef HAWKTRACER_INTERNAL_MUTEX_H
#define HAWKTRACER_INTERNAL_MUTEX_H

#include <hawktracer/base_types.h>

HT_DECLS_BEGIN

typedef struct _HT_Mutex HT_Mutex;

HT_Mutex* ht_mutex_create(void);

HT_ErrorCode ht_mutex_destroy(HT_Mutex* mtx);

HT_ErrorCode ht_mutex_lock(HT_Mutex* mtx);

HT_ErrorCode ht_mutex_unlock(HT_Mutex* mtx);

HT_DECLS_END

#endif /* HAWKTRACER_MUTEX_H */
