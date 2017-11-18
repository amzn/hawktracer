#ifndef HAWKTRACER_MUTEX_H
#define HAWKTRACER_MUTEX_H

#include <hawktracer/macros.h>

HT_DECLS_BEGIN

typedef struct _HT_Mutex HT_Mutex;

HT_Mutex* ht_mutex_create(void);

void ht_mutex_destroy(HT_Mutex* mtx);

void ht_mutex_lock(HT_Mutex* mtx);

void ht_mutex_unlock(HT_Mutex* mtx);

HT_DECLS_END

#endif /* HAWKTRACER_MUTEX_H */
