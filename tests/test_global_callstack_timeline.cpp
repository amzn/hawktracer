#include <hawktracer/global_callstack_timeline.h>

#include "test_common.h"

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

TEST(TestGlobalCallstackTimeline, ScopedTracepoint)
{
    // Arrange
    HT_CallstackBaseTimeline* timeline = HT_CALLSTACK_BASE_TIMELINE(ht_global_callstack_timeline_get());
    const HT_CallstackEventLabel int_label = 31337;
    const char* string_label = "31337_string";
    MixedNotifyInfo info;

    ht_timeline_register_listener(HT_TIMELINE(timeline), mixed_test_listener, &info);

    // Act
    {
        HT_TP_GLOBAL_SCOPED_INT(int_label);
        {
            HT_TP_GLOBAL_SCOPED_STRING(string_label);
        }
    }

    ht_timeline_flush(HT_TIMELINE(timeline));

    // Assert
    ASSERT_EQ(sizeof(HT_CallstackIntEvent) + sizeof(HT_CallstackStringEvent), info.notified_events);
    ASSERT_EQ(1u, info.int_values.size());
    ASSERT_EQ(1u, info.string_values.size());
    ASSERT_EQ(int_label, info.int_values[0].label);
    ASSERT_STREQ(string_label, info.string_values[0].label);

    ht_timeline_unregister_all_listeners(HT_TIMELINE(timeline));
}
