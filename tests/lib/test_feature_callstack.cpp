#include <hawktracer/tracepoint.h>

#include "test_allocator.h"
#include "test_common.h"
#include "test_test_events.h"

#include <gtest/gtest.h>

#include <thread>

class TestFeatureCallstack : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        HT_REGISTER_EVENT_KLASS(TestCallstackEvent);
    }

    void init_timeline(size_t buff_capacity)
    {
        _timeline = ht_timeline_create(buff_capacity, HT_FALSE, HT_FALSE, nullptr, nullptr);
        ht_feature_callstack_enable(_timeline);
    }

    void TearDown() override
    {
        if (_timeline)
        {
            ht_timeline_unregister_all_listeners(_timeline);
            ht_timeline_destroy(_timeline);
        }
    }

    HT_Timeline* _timeline = nullptr;
};

TEST_F(TestFeatureCallstack, SimpleIntCallstackTest)
{
    // Arrange
    init_timeline(sizeof(HT_CallstackIntEvent) * 3);
    NotifyInfo<HT_CallstackIntEvent> info;

    ht_timeline_register_listener(_timeline, test_listener<HT_CallstackIntEvent>, &info);

    // Act
    for (int i = 0; i < 4; i++)
    {
        ht_feature_callstack_start_int(_timeline, i);
    }

    for (int i = 0; i < 4; i++)
    {
        ht_feature_callstack_stop(_timeline);
    }

    ht_timeline_flush(_timeline);

    // Assert
    ASSERT_EQ(4 * sizeof(HT_CallstackIntEvent), info.notified_events);
    ASSERT_EQ(2, info.notify_count);
    ASSERT_EQ(4u, info.values.size());
    for (size_t i = 0; i < info.values.size(); i++)
    {
        ASSERT_EQ(3 - i, info.values[i].label);
    }
}

TEST_F(TestFeatureCallstack, EnableShouldFailIfMallocReturnsNull)
{
    // Arrange
    HT_Timeline* tm = ht_timeline_create(16u, HT_FALSE, HT_FALSE, nullptr, nullptr);

    ScopedSetAlloc allocator(ht_test_null_realloc);

    // Act
    HT_ErrorCode err = ht_feature_callstack_enable(tm);

    // Assert
    allocator.reset();
    ht_timeline_destroy(tm);
    ASSERT_EQ(HT_ERR_OUT_OF_MEMORY, err);
}

TEST_F(TestFeatureCallstack, EnableShouldFailIfThereIsOnlyEnoughMemoryForFeatureCallstackObject)
{
    // Arrange
    HT_Timeline* tm = ht_timeline_create(16u, HT_FALSE, HT_FALSE, nullptr, nullptr);
    LimitedSizeAllocator alloc_data(64);
    ScopedSetAlloc allocator(&LimitedSizeAllocator::realloc, &alloc_data);

    // Act
    HT_ErrorCode err = ht_feature_callstack_enable(tm);

    // Assert
    allocator.reset();
    ht_timeline_destroy(tm);
    ASSERT_EQ(HT_ERR_OUT_OF_MEMORY, err);
}

#ifdef HT_ENABLE_THREADS
TEST_F(TestFeatureCallstack, CallstackTimelinesRunOnDifferentThreadsShouldGenerateDifferentThreadID)
{
    // Assert
    init_timeline(sizeof(HT_CallstackIntEvent));
    NotifyInfo<HT_CallstackIntEvent> info1;
    NotifyInfo<HT_CallstackIntEvent> info2;
    ht_timeline_register_listener(_timeline, test_listener<HT_CallstackIntEvent>, &info1);

    // Act
    ht_feature_callstack_start_int(_timeline, 1);
    ht_feature_callstack_stop(_timeline);
    ht_timeline_flush(_timeline);
    std::thread([&info2]
    {
        HT_Timeline* tm = ht_timeline_create(sizeof(HT_CallstackIntEvent), HT_FALSE, HT_FALSE, NULL, NULL);
        ht_feature_callstack_enable(tm);
        ht_timeline_register_listener(tm, test_listener<HT_CallstackIntEvent>, &info2);
        ht_feature_callstack_start_int(tm, 2);
        ht_feature_callstack_stop(tm);
        ht_timeline_destroy(tm);
    }).join();

    // Assert
    HT_CallstackIntEvent& event1 = info1.values.front();
    HT_CallstackIntEvent& event2 = info2.values.front();
    ASSERT_NE(event1.base.thread_id, event2.base.thread_id);
}
#endif

TEST_F(TestFeatureCallstack, MixedCallstackEventTypes)
{
    // Arrange
    init_timeline(sizeof(HT_CallstackIntEvent) + sizeof(HT_CallstackStringEvent));
    MixedNotifyInfo info;

    ht_timeline_register_listener(_timeline, mixed_test_listener, &info);

    const char* label1 = "label1";
    const char* label2 = "label2";

    // Act
    ht_feature_callstack_start_int(_timeline, 1);
    ht_feature_callstack_start_string(_timeline, label1);
    ht_feature_callstack_start_int(_timeline, 2);
    ht_feature_callstack_stop(_timeline);
    ht_feature_callstack_start_string(_timeline, label2);
    ht_feature_callstack_stop(_timeline);
    ht_feature_callstack_stop(_timeline);
    ht_feature_callstack_stop(_timeline);

    ht_timeline_flush(_timeline);

    // Assert
    ASSERT_EQ(2 * (sizeof(HT_CallstackIntEvent) + sizeof(HT_CallstackStringEvent)), info.notified_events);
    ASSERT_EQ(2, info.notify_count);
    ASSERT_EQ(2u, info.int_values.size());
    ASSERT_EQ(2u, info.string_values.size());
    ASSERT_EQ(2u, info.int_values[0].label);
    ASSERT_EQ(1u, info.int_values[1].label);
    ASSERT_STREQ(label2, info.string_values[0].label);
    ASSERT_STREQ(label1, info.string_values[1].label);
}

TEST_F(TestFeatureCallstack, TestScopedTracepoint)
{
    // Arrange
    init_timeline(sizeof(HT_CallstackIntEvent) + sizeof(HT_CallstackStringEvent));

    const HT_CallstackEventLabel int_label = 31337;
    const char* string_label = "31337_string";
    MixedNotifyInfo info;

    ht_timeline_register_listener(_timeline, mixed_test_listener, &info);

    // Act
    {
        HT_TP_SCOPED_INT(_timeline, int_label);
        {
            HT_TRACE(_timeline, string_label);
        }
    }

    ht_timeline_flush(_timeline);

    // Assert
    ASSERT_EQ(sizeof(HT_CallstackIntEvent) + sizeof(HT_CallstackStringEvent), info.notified_events);
    ASSERT_EQ(1u, info.int_values.size());
    ASSERT_EQ(1u, info.string_values.size());
    ASSERT_EQ(int_label, info.int_values[0].label);
    ASSERT_STREQ(string_label, info.string_values[0].label);
}

TEST_F(TestFeatureCallstack, Base)
{
    // Arrange
    init_timeline(sizeof(HT_CallstackBaseEvent) * 3);
    NotifyInfo<TestCallstackEvent> info;

    ht_timeline_register_listener(_timeline, test_listener<TestCallstackEvent>, &info);

    // Act
    for (int i = 0; i < 100; i++)
    {
        HT_DECL_EVENT(TestCallstackEvent, event);
        event.info = i;
        ht_feature_callstack_start(_timeline, (HT_CallstackBaseEvent*)&event);
    }

    for (int i = 0; i < 100; i++)
    {
        ht_feature_callstack_stop(_timeline);
    }

    ht_timeline_flush(_timeline);

    // Assert
    ASSERT_EQ(100 * sizeof(TestCallstackEvent), info.notified_events);
    ASSERT_EQ(50, info.notify_count);
    ASSERT_EQ(100u, info.values.size());
    for (size_t i = 0; i < info.values.size(); i++)
    {
        ASSERT_EQ(99 - static_cast<int>(i), info.values[i].info);
    }
}

TEST_F(TestFeatureCallstack, MixedEventPublishing)
{
    // Arrange
    init_timeline(sizeof(HT_CallstackBaseEvent) * 3);
    NotifyInfo<TestCallstackEvent> info;

    ht_timeline_register_listener(_timeline, test_listener<TestCallstackEvent>, &info);

    auto start_event = [this] (int info) {
        HT_DECL_EVENT(TestCallstackEvent, event);
        event.info = info;
        ht_feature_callstack_start(_timeline, (HT_CallstackBaseEvent*)&event);
    };

    // Act
    start_event(1);
    start_event(2);
    ht_feature_callstack_stop(_timeline);
    start_event(3);
    start_event(4);
    ht_feature_callstack_stop(_timeline);
    ht_feature_callstack_stop(_timeline);
    ht_feature_callstack_stop(_timeline);

    ht_timeline_flush(_timeline);

    // Assert
    ASSERT_EQ(4 * sizeof(TestCallstackEvent), info.notified_events);
    ASSERT_EQ(2, info.notify_count);
    ASSERT_EQ(4u, info.values.size());
    ASSERT_EQ(2, info.values[0].info);
    ASSERT_EQ(4, info.values[1].info);
    ASSERT_EQ(3, info.values[2].info);
    ASSERT_EQ(1, info.values[3].info);
}
