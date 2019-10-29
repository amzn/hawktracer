#include <hawktracer/macros.h>

#if defined(HT_MUTEX_IMPL_CUSTOM) || defined(HT_MUTEX_IMPL_CPP11) || defined(HT_MUTEX_IMPL_WIN32) || defined(HT_MUTEX_IMPL_POSIX)
#  define HT_MUTEX_FORCE_SELECTED
#endif

#if !defined(HT_MUTEX_FORCE_SELECTED) && defined(HT_CPP11)
#  define HT_MUTEX_IMPL_CPP11
#endif /* HT_CPP11 */

#if !defined(HT_MUTEX_FORCE_SELECTED) && defined(_WIN32)
#  define HT_MUTEX_IMPL_WIN32
#endif /* _WIN32 */

#if !defined(HT_MUTEX_FORCE_SELECTED) && defined(HT_HAVE_UNISTD_H)
#  include <unistd.h>
#  ifdef _POSIX_VERSION
#    define HT_MUTEX_IMPL_POSIX
#  endif
#endif

#if defined(HT_MUTEX_IMPL_CUSTOM)
/* Custom implementation provided */
#elif defined(HT_MUTEX_IMPL_CPP11) || defined(HT_MUTEX_IMPL_POSIX) || defined(HT_MUTEX_IMPL_WIN32)

#include "internal/mutex.h"
#include "hawktracer/alloc.h"

#include <assert.h>

#if defined(HT_MUTEX_IMPL_CPP11)
#  include <mutex>
#  include <system_error>
#  define HT_MUTEX_TYPE_ std::mutex
#elif defined(HT_MUTEX_IMPL_POSIX)
#  include <pthread.h>
#  define HT_MUTEX_TYPE_ pthread_mutex_t
#elif defined(HT_MUTEX_IMPL_WIN32)
#  include <windows.h>
#  define HT_MUTEX_TYPE_ HANDLE
#endif

struct _HT_Mutex
{
    HT_MUTEX_TYPE_ mtx;
};

HT_Mutex*
ht_mutex_create(void)
{
    HT_Mutex* mtx = HT_CREATE_TYPE(HT_Mutex);

    if (mtx == NULL)
    {
        return NULL;
    }

#ifdef HT_MUTEX_IMPL_CPP11
    new (&mtx->mtx) std::mutex();
#elif defined(HT_MUTEX_IMPL_POSIX)
    pthread_mutex_init(&mtx->mtx, NULL);
#elif defined(HT_MUTEX_IMPL_WIN32)
    mtx->mtx = CreateMutex(0, FALSE, 0);
#endif

    return mtx;
}

HT_ErrorCode
ht_mutex_destroy(HT_Mutex* mtx)
{
    HT_ErrorCode err = HT_ERR_OK;

    assert(mtx);

#ifdef HT_MUTEX_IMPL_CPP11
    mtx->mtx.~mutex();
#elif defined(HT_MUTEX_IMPL_POSIX)
    err = pthread_mutex_destroy(&mtx->mtx) == 0 ? HT_ERR_OK : HT_ERR_UNKNOWN;
#elif defined(HT_MUTEX_IMPL_WIN32)
    err = CloseHandle(mtx->mtx) != 0 ? HT_ERR_OK : HT_ERR_UNKNOWN;
#endif

    ht_free(mtx);

    return err;
}

HT_ErrorCode
ht_mutex_lock(HT_Mutex* mtx)
{
    assert(mtx);

#ifdef HT_MUTEX_IMPL_CPP11
    try
    {
        mtx->mtx.lock();
        return HT_ERR_OK;
    }
    catch (const std::system_error&)
    {
        return HT_ERR_UNKNOWN;
    }
#elif defined(HT_MUTEX_IMPL_POSIX)
    return pthread_mutex_lock(&mtx->mtx) == 0 ? HT_ERR_OK : HT_ERR_UNKNOWN;
#elif defined(HT_MUTEX_IMPL_WIN32)
    return WaitForSingleObject(mtx->mtx, INFINITE) == WAIT_OBJECT_0 ? HT_ERR_OK : HT_ERR_UNKNOWN;
#endif
}

HT_ErrorCode
ht_mutex_unlock(HT_Mutex* mtx)
{
    assert(mtx);

#ifdef HT_MUTEX_IMPL_CPP11
    mtx->mtx.unlock();
    return HT_ERR_OK;
#elif defined(HT_MUTEX_IMPL_POSIX)
    return pthread_mutex_unlock(&mtx->mtx) == 0 ? HT_ERR_OK : HT_ERR_UNKNOWN;
#elif defined(HT_MUTEX_IMPL_WIN32)
    return ReleaseMutex(mtx->mtx) == 0 ? HT_ERR_OK : HT_ERR_UNKNOWN;
#endif
}

#else
#  error Mutex implementation is not defined. Please define HT_MUTEX_IMPL_CUSTOM
#  error and provide custom implementation of mutex API, or define one of:
#  error HT_MUTEX_IMPL_CPP11, HT_MUTEX_IMPL_POSIX, HT_MUTEX_IMPL_WIN32 to use
#  error one of existing mutex implementations.
#endif
