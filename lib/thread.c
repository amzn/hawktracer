#include "hawktracer/thread.h"

static HT_ThreadId _ht_current_thread_id = 0; // TODO: this must be atomic

HT_ThreadId
ht_thread_get_current_thread_id(void)
{
    static HT_THREAD_LOCAL HT_ThreadId thread_id;

    if (!thread_id)
    {
        thread_id = ++_ht_current_thread_id;
    }

    return thread_id;
}
