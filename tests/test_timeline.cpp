#include <hawktracer/timeline.h>
#include <hawktracer/events.h>

#include "test_common.h"

#include <gtest/gtest.h>

class TestTimeline : public ::testing::Test
{
protected:
    void SetUp() override
    {
        _timeline = ht_timeline_create("simple_ts", "buffer-capacity", sizeof(HT_Event) * 3, nullptr);
    }

    void TearDown() override
    {
        ht_timeline_destroy(_timeline);
    }

    HT_Timeline* _timeline = nullptr;
};

TEST_F(TestTimeline, PublishEventsShouldNotifyListener)
{
    // Arrange
    NotifyInfo<HT_Event> info;

        ht_timeline_register_listener(_timeline, test_listener<HT_Event>, &info);

    // Act
    for (int i = 0; i < 10; i++)
    {
        HT_DECL_EVENT(HT_Event, event);
        ht_timeline_push_event(_timeline, &event);
    }

    // Assert
    ASSERT_EQ(9 * sizeof(HT_Event), info.notified_events); // last event not sent, because buffer is not full
    ASSERT_EQ(3, info.notify_count);
}

TEST_F(TestTimeline, FlushEventsShouldNotifyListener)
{
    // Arrange
    NotifyInfo<HT_Event> info;

    ht_timeline_register_listener(_timeline, test_listener<HT_Event>, &info);

    HT_DECL_EVENT(HT_Event, event);
    ht_timeline_push_event(_timeline, &event);

    // Act
    ht_timeline_flush(_timeline);

    // Assert
    ASSERT_EQ(1 * sizeof(HT_Event), info.notified_events);
    ASSERT_EQ(1, info.notify_count);
}

TEST_F(TestTimeline, InitEventTwiceShouldIncreaseId)
{
    // Arrange
    HT_DECL_EVENT(HT_Event, event);

    ht_timeline_init_event(_timeline, &event);
    HT_EventId id = event.id;

    // Act
    ht_timeline_init_event(_timeline, &event);

    // Assert
    ASSERT_EQ(id, event.id - 1);
}

TEST_F(TestTimeline, InitEventShouldSetMonotonicTimestamp)
{
    // Arrange
    HT_DECL_EVENT(HT_Event, event);

    ht_timeline_init_event(_timeline, &event);
    HT_TimestampNs ts = event.timestamp;

    // Act
    ht_timeline_init_event(_timeline, &event);

    // Assert
    ASSERT_LT(ts, event.timestamp);
}
