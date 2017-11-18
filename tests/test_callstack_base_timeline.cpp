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
