#include "internal/mutex.h"

#include "hawktracer/alloc.h"

#include <mutex>
#include <cassert>

struct _HT_Mutex
{
    std::mutex mtx;
};

HT_Mutex*
ht_mutex_create(void)
{
    HT_Mutex* mtx = HT_CREATE_TYPE(HT_Mutex);

    if (mtx == NULL)
    {
        return NULL;
    }

    new (&mtx->mtx) std::mutex();

    return mtx;
}

void
ht_mutex_destroy(HT_Mutex* mtx)
{
    assert(mtx);

    mtx->mtx.~mutex();
    ht_free(mtx);
}

void
ht_mutex_lock(HT_Mutex* mtx)
{
    assert(mtx);

    mtx->mtx.lock();
}

void
ht_mutex_unlock(HT_Mutex* mtx)
{
    assert(mtx);

    mtx->mtx.unlock();
}
