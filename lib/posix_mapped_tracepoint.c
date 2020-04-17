#include "hawktracer/posix_mapped_tracepoint.h"

#ifdef HT_USE_PTHREADS

#include "hawktracer/feature_cached_string.h"

#include "internal/mutex.h"

#include <pthread.h>

static HT_Mutex* _posix_mapped_tracepoint_mapped_mtx = NULL;
static HT_Timeline* _mapped_timeline;
static const char* _mapped_ptr;

void
_ht_posix_mapped_tracepoint_init(void)
{
    _posix_mapped_tracepoint_mapped_mtx = ht_mutex_create();
}

void
_ht_posix_mapped_tracepoint_deinit(void)
{
    if (_posix_mapped_tracepoint_mapped_mtx != NULL)
    {
        ht_mutex_destroy(_posix_mapped_tracepoint_mapped_mtx);
    }
}

void
ht_pthread_mapped_tracepoint_enter(HT_Timeline* timeline, const char* label)
{
    ht_mutex_lock(_posix_mapped_tracepoint_mapped_mtx);
    _mapped_timeline = timeline;
    _mapped_ptr = label;
}

void
ht_pthread_mapped_tracepoint_leave(void)
{
    ht_mutex_unlock(_posix_mapped_tracepoint_mapped_mtx);
}

void
ht_pthread_mapped_tracepoint_add_mapping(void)
{
    ht_feature_cached_string_add_mapping(_mapped_timeline, _mapped_ptr);
}

#endif /* HT_USE_PTHREADS */
