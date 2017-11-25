#include "hawktracer/global_callstack_timeline.h"
struct GlobalCallstackTimeline
{
    GlobalCallstackTimeline()
    {
        // Todo should we init here?
        c_timeline = HT_CALLSTACK_BASE_TIMELINE(ht_timeline_create("HT_CallstackBaseTimeline", "buffer-size", 1024, nullptr)); // TODO buffer-size from command line
    }

    ~GlobalCallstackTimeline()
    {
        ht_timeline_destroy(HT_TIMELINE(c_timeline));
    }

    HT_CallstackBaseTimeline* c_timeline;
};

HT_CallstackBaseTimeline* ht_global_callstack_timeline_get(void)
{
    static thread_local GlobalCallstackTimeline timeline;

    return timeline.c_timeline;
}
