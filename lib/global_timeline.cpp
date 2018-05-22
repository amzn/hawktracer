#include "hawktracer/global_timeline.h"

struct GlobalTimeline
{
    GlobalTimeline()
    {
        /* TODO: handle ht_timeline_create() error? */
        c_timeline = ht_timeline_create(1024, HT_FALSE, HT_TRUE, "HT_GlobalTimeline", NULL);
        ht_feature_callstack_enable(c_timeline);
        ht_feature_cached_string_enable(c_timeline);
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
