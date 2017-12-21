#include <hawktracer/global_timeline.h>

#include "test_common.h"

TEST(TestGlobalTimeline, SimpleTest)
{
    // Arrange
    HT_Timeline* timeline = ht_global_timeline_get();
    NotifyInfo<HT_CallstackIntEvent> info;
    ht_timeline_register_listener(timeline, test_listener<HT_CallstackIntEvent>, &info);

    // Act
    ht_feature_callstack_start_int(timeline, 1);
    ht_feature_callstack_start_int(timeline, 2);
    ht_feature_callstack_stop(timeline);
    ht_feature_callstack_stop(timeline);

    ht_timeline_flush(timeline);

    // Assert
    HT_DECL_EVENT(HT_CallstackIntEvent, int_ev);
    ASSERT_EQ(2 * HT_EVENT_GET_CLASS(&int_ev)->get_size(HT_EVENT(&int_ev)), info.notified_events);
    ASSERT_EQ(1, info.notify_count);

    ht_timeline_unregister_all_listeners(timeline);
}

TEST(TestGlobalTimeline, ScopedTracepoint)
{
    // Arrange
    HT_Timeline* timeline = ht_global_timeline_get();
    const HT_CallstackEventLabel int_label = 31337;
    const char* string_label = "31337_string";
    MixedNotifyInfo info;

    ht_timeline_register_listener(timeline, mixed_test_listener, &info);

    // Act
    {
        HT_TP_GLOBAL_SCOPED_INT(int_label);
        {
            HT_TP_GLOBAL_SCOPED_STRING(string_label);
        }
    }

    ht_timeline_flush(timeline);

    // Assert
    HT_DECL_EVENT(HT_CallstackBaseEvent, base_ev);
    size_t expected_size = 2 * HT_EVENT_GET_CLASS(&base_ev)->get_size(HT_EVENT(&base_ev)) +
            sizeof(HT_CallstackIntEvent::label) + strlen(string_label) + 1;

    ASSERT_EQ(expected_size, info.notified_events);

    ht_timeline_unregister_all_listeners(timeline);
}
