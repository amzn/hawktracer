#include "hawktracer/global_timeline.h"

struct GlobalTimeline
{
    GlobalTimeline()
    {
        c_timeline = HT_CALLSTACK_BASE_TIMELINE(
                    ht_timeline_create("HT_GlobalTimeline", "buffer-size", 1024,
                                       HT_BASE_TIMELINE_PROPERTY_THREAD_SAFE, HT_FALSE,
                                       HT_BASE_TIMELINE_PROPERTY_SERIALIZE_EVENTS, HT_TRUE, nullptr)); // TODO buffer-size from command line
    }

    ~GlobalTimeline()
    {
        ht_timeline_destroy(HT_TIMELINE(c_timeline));
    }

    HT_CallstackTimeline* c_timeline;
};

HT_CallstackTimeline* ht_global_timeline_get(void)
{
    static thread_local GlobalTimeline timeline;

    return timeline.c_timeline;
}
