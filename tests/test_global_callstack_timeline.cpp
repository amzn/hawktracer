#include <hawktracer/global_callstack_timeline.h>

#include "test_common.h"

TEST(TestGlobalCallstackTimeline, SimpleTest)
{
    // Arrange
    HT_CallstackTimeline* timeline = HT_CALLSTACK_BASE_TIMELINE(ht_global_callstack_timeline_get());
    NotifyInfo<HT_CallstackIntEvent> info;
    ht_timeline_register_listener(HT_TIMELINE(timeline), test_listener<HT_CallstackIntEvent>, &info);

    // Act
    ht_callstack_timeline_int_start(timeline, 1);
    ht_callstack_timeline_int_start(timeline, 2);
    ht_callstack_timeline_stop(timeline);
    ht_callstack_timeline_stop(timeline);

    ht_timeline_flush(HT_TIMELINE(ht_global_callstack_timeline_get()));

    // Assert
    HT_DECL_EVENT(HT_CallstackIntEvent, int_ev);
    ASSERT_EQ(2 * HT_EVENT_GET_CLASS(&int_ev)->get_size(HT_EVENT(&int_ev)), info.notified_events);
    ASSERT_EQ(1, info.notify_count);

    ht_timeline_unregister_all_listeners(HT_TIMELINE(timeline));
}

TEST(TestGlobalCallstackTimeline, ScopedTracepoint)
{
    // Arrange
    HT_CallstackTimeline* timeline = HT_CALLSTACK_BASE_TIMELINE(ht_global_callstack_timeline_get());
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
    HT_DECL_EVENT(HT_CallstackBaseEvent, base_ev);
    size_t expected_size = 2 * HT_EVENT_GET_CLASS(&base_ev)->get_size(HT_EVENT(&base_ev)) +
            sizeof(HT_CallstackIntEvent::label) + strlen(string_label) + 1;

    ASSERT_EQ(expected_size, info.notified_events);

    ht_timeline_unregister_all_listeners(HT_TIMELINE(timeline));
}
