#include "hawktracer/global_timeline.h"

struct GlobalTimeline
{
    GlobalTimeline()
    {
        ht_timeline_init(&c_timeline, 1024, HT_FALSE, HT_TRUE, "HT_GlobalTimeline");
        ht_feature_callstack_enable(&c_timeline);
    }

    ~GlobalTimeline()
    {
        ht_feature_callstack_disable(&c_timeline);
        ht_timeline_deinit(&c_timeline);
    }

    HT_Timeline c_timeline;
};

HT_Timeline* ht_global_timeline_get(void)
{
    static thread_local GlobalTimeline timeline;

    return &timeline.c_timeline;
}
