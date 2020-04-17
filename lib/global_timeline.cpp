#include "internal/global_timeline.h"
#include "hawktracer/global_timeline.h"

static size_t global_timeline_buffer_size = 1024;

void
ht_global_timeline_set_buffer_size(size_t buffer_size)
{
    global_timeline_buffer_size = buffer_size;
}

size_t
ht_global_timeline_get_buffer_size(void)
{
    return global_timeline_buffer_size;
}

static HT_Timeline* _ht_global_timeline_create(void)
{
    HT_Timeline* c_timeline = ht_timeline_create(global_timeline_buffer_size, HT_FALSE, HT_TRUE, "HT_GlobalTimeline", NULL);

    ht_feature_callstack_enable(c_timeline);
    ht_feature_cached_string_enable(c_timeline, HT_FALSE);

    return c_timeline;
}

#ifndef HT_ENABLE_THREADS
/**
 * If the threading support is disabled at compilation time,
 * we assume the HawkTracer is being used in a single-threaded
 * environment, therefore no need for per-thread timeline.
 */
HT_Timeline* ht_global_timeline_get(void)
{
    static HT_Timeline* timeline = NULL;

    if (!timeline)
    {
        // TODO memory leak
        timeline = _ht_global_timeline_create();
    }

    return timeline;
}

#elif defined(HT_CPP11)
struct GlobalTimeline
{
    GlobalTimeline()
    {
        /* TODO: handle ht_timeline_create() error? */
        c_timeline = _ht_global_timeline_create();
    }

    ~GlobalTimeline()
    {
        ht_timeline_destroy(c_timeline);
    }

    HT_Timeline* c_timeline;
};

HT_Timeline* ht_global_timeline_get(void)
{
    static HT_THREAD_LOCAL GlobalTimeline timeline;

    return timeline.c_timeline;
}

#elif defined(HT_HAVE_UNISTD_H)
#  include <unistd.h>
#  ifdef _POSIX_VERSION

#include <pthread.h>

static pthread_key_t timeline_key;
static pthread_once_t timeline_once_control = PTHREAD_ONCE_INIT;

static void _ht_destroy_timeline(void* timeline)
{
    ht_timeline_destroy((HT_Timeline*)timeline);
}

static void create_key(void)
{
    pthread_key_create(&timeline_key, _ht_destroy_timeline);
}

HT_Timeline* ht_global_timeline_get(void)
{
    static HT_THREAD_LOCAL HT_Timeline* c_timeline = NULL;

    if (!c_timeline)
    {
        c_timeline = _ht_global_timeline_create();
        pthread_once(&timeline_once_control, create_key);
        pthread_setspecific(timeline_key, c_timeline);
    }

    return c_timeline;
}

#  endif
#endif