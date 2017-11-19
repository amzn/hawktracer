#include <hawktracer/callstack_base_timeline.h>

#include "test_common.h"

#include <gtest/gtest.h>

class TestCallstackBaseTimeline : public ::testing::Test
{
protected:
    void SetUp() override
    {
        _timeline = (HT_CallstackBaseTimeline*)ht_timeline_create("callstack", "buffer-capacity", sizeof(HT_CallstackBaseEvent) * 3, nullptr);
    }

    void TearDown() override
    {
        ht_timeline_unregister_all_listeners(HT_TIMELINE(_timeline));
        ht_timeline_destroy(HT_TIMELINE(_timeline));
    }

    HT_CallstackBaseTimeline* _timeline = nullptr;
};

struct TestCallstackEvent
{
    HT_CallstackBaseEvent base;

    int info;
};

HT_DEFINE_EVENT_KLASS(TestCallstackEvent, 31337);

TEST_F(TestCallstackBaseTimeline, Base)
{
    // Arrange
    NotifyInfo<TestCallstackEvent> info;

    ht_timeline_register_listener(HT_TIMELINE(_timeline), test_listener<TestCallstackEvent>, &info);

    // Act
    for (int i = 0; i < 4; i++)
    {
        HT_DECL_EVENT(TestCallstackEvent, event);
        event.info = i;
        ht_callstack_base_timeline_start(_timeline, (HT_CallstackBaseEvent*)&event);
    }

    for (int i = 0; i < 4; i++)
    {
        ht_callstack_base_timeline_stop(_timeline);
    }

    ht_timeline_flush(HT_TIMELINE(_timeline));

    // Assert
    ASSERT_EQ(4 * sizeof(TestCallstackEvent), info.notified_events);
    ASSERT_EQ(2, info.notify_count);
    ASSERT_EQ(4, info.values.size());
    for (size_t i = 0; i < info.values.size(); i++)
    {
        ASSERT_EQ(3 - i, info.values[i].info);
    }
}

TEST_F(TestCallstackBaseTimeline, MixedEventPublishing)
{
    // Arrange
    NotifyInfo<TestCallstackEvent> info;

    ht_timeline_register_listener(HT_TIMELINE(_timeline), test_listener<TestCallstackEvent>, &info);

    auto start_event = [this] (int info) {
        HT_DECL_EVENT(TestCallstackEvent, event);
        event.info = info;
        ht_callstack_base_timeline_start(_timeline, (HT_CallstackBaseEvent*)&event);
    };

    // Act
    start_event(1);
    start_event(2);
    ht_callstack_base_timeline_stop(_timeline);
    start_event(3);
    start_event(4);
    ht_callstack_base_timeline_stop(_timeline);
    ht_callstack_base_timeline_stop(_timeline);
    ht_callstack_base_timeline_stop(_timeline);

    ht_timeline_flush(HT_TIMELINE(_timeline));

    // Assert
    ASSERT_EQ(4 * sizeof(TestCallstackEvent), info.notified_events);
    ASSERT_EQ(2, info.notify_count);
    ASSERT_EQ(4, info.values.size());
    ASSERT_EQ(2, info.values[0].info);
    ASSERT_EQ(4, info.values[1].info);
    ASSERT_EQ(3, info.values[2].info);
    ASSERT_EQ(1, info.values[3].info);
}
