#include <hawktracer/global_callstack_timeline.h>

#include "test_common.h"

#include <gtest/gtest.h>

TEST(TestGlobalCallstackTimeline, SimpleTest)
{
    // Arrange
    HT_CallstackBaseTimeline* timeline = HT_CALLSTACK_BASE_TIMELINE(ht_global_callstack_timeline_get());
    NotifyInfo<HT_CallstackIntEvent> info;
    ht_timeline_register_listener(HT_TIMELINE(timeline), test_listener<HT_CallstackIntEvent>, &info);

    // Act
    ht_callstack_timeline_int_start(timeline, 1);
    ht_callstack_timeline_int_start(timeline, 2);
    ht_callstack_base_timeline_stop(timeline);
    ht_callstack_base_timeline_stop(timeline);

    ht_timeline_flush(HT_TIMELINE(ht_global_callstack_timeline_get()));

    // Assert
    ASSERT_EQ(2 * sizeof(HT_CallstackIntEvent), info.notified_events);
    ASSERT_EQ(1, info.notify_count);
    ASSERT_EQ(2u, info.values.size());


    ht_timeline_unregister_all_listeners(HT_TIMELINE(timeline));
}
