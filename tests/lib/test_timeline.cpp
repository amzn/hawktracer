#include <hawktracer/timeline.h>
#include <hawktracer/events.h>

#include "test_test_events.h"
#include "test_common.h"

#include <gtest/gtest.h>

#include <thread>

class TestTimeline : public ::testing::Test
{
protected:
    void SetUp() override
    {
        _timeline = ht_timeline_create(sizeof(HT_Event) * 3, HT_FALSE, HT_FALSE, nullptr, nullptr);
    }

    void TearDown() override
    {
        ht_timeline_unregister_all_listeners(_timeline);
        ht_timeline_destroy(_timeline);
    }

    HT_Timeline* _timeline;
};

TEST_F(TestTimeline, PublishHTEventsShouldNotifyListener)
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

TEST_F(TestTimeline, PublishCallstackEventsShouldNotifyListener)
{
    // Arrange
    NotifyInfo<HT_CallstackBaseEvent> info;
    HT_Timeline* timeline = ht_timeline_create(sizeof(HT_CallstackBaseEvent) * 3, HT_FALSE, HT_FALSE, nullptr, nullptr);

    ht_timeline_register_listener(timeline, test_listener<HT_CallstackBaseEvent>, &info);

    // Act
    for (int i = 0; i < 10; i++)
    {
        HT_DECL_EVENT(HT_CallstackBaseEvent, event);
        ht_timeline_push_event(timeline, reinterpret_cast<HT_Event*>(&event));
    }

    // Assert
    ASSERT_EQ(9 * sizeof(HT_CallstackBaseEvent), info.notified_events); // last event not sent, because buffer is not full
    ASSERT_EQ(3, info.notify_count);

    ht_timeline_unregister_all_listeners(timeline);
    ht_timeline_destroy(timeline);
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

TEST_F(TestTimeline, TimelineShouldBeFlushedBeforeUninitialized)
{
    // Arrange
    HT_Timeline* timeline = ht_timeline_create(sizeof(HT_Event) * 3, HT_FALSE, HT_FALSE, nullptr, nullptr);
    NotifyInfo<HT_Event> info;

    ht_timeline_register_listener(timeline, test_listener<HT_Event>, &info);

    HT_DECL_EVENT(HT_Event, event);
    ht_timeline_push_event(timeline, &event);

    // Act
    ht_timeline_destroy(timeline);

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
    ASSERT_LE(ts, event.timestamp);
}

#ifdef HT_ENABLE_THREADS

TEST_F(TestTimeline, ThreadSafeMessageShouldWorkWithMultipleThreads)
{
    // Arrange
    const size_t event_count = 20000;
    HT_Timeline* timeline = ht_timeline_create(sizeof(HT_Event) * 3, HT_TRUE, HT_FALSE, NULL, NULL);

    NotifyInfo<HT_Event> info;
    ht_timeline_register_listener(timeline, test_listener<HT_Event>, &info);

    // Act
    std::thread th = std::thread([timeline, &event_count] {
        for (size_t i = event_count / 2; i < event_count; i++)
        {
            HT_DECL_EVENT(HT_Event, event);
            event.timestamp = i;
            ht_timeline_push_event(timeline, &event);
        }
    });

    for (size_t i = 0; i < event_count / 2; i++)
    {
        HT_DECL_EVENT(HT_Event, event);
        event.timestamp = i;
        ht_timeline_push_event(timeline, &event);
    }

    th.join();

    ht_timeline_flush(timeline);

    // Assert
    std::vector<HT_TimestampNs> all_values(event_count, 1);
    HT_TimestampNs sum = 0;
    for (const auto& event : info.values)
    {
        ASSERT_GT(event_count, event.timestamp);
        sum += all_values[(size_t)event.timestamp];
        all_values[(size_t)event.timestamp] = 0;
    }

    ASSERT_EQ(event_count, sum);

    ht_timeline_destroy(timeline);
}

#endif

TEST_F(TestTimeline, SharedListener)
{
    // Arrange
    HT_Timeline* timeline1 = ht_timeline_create(sizeof(HT_Event) * 3, HT_TRUE, HT_FALSE, "listener", nullptr);
    NotifyInfo<HT_Event> info;
    ht_timeline_register_listener(timeline1, test_listener<HT_Event>, &info);

    HT_DECL_EVENT(HT_Event, event);
    ht_timeline_init_event(_timeline, &event);

    // Act
    HT_Timeline* timeline2 = ht_timeline_create(sizeof(HT_Event) * 3, HT_TRUE, HT_FALSE, "listener", nullptr);

    ht_timeline_push_event(timeline2, &event);
    ht_timeline_flush(timeline2);

    // Assert
    ASSERT_EQ(1u, info.values.size());
    ASSERT_EQ(event.timestamp, info.values.front().timestamp);

    ht_timeline_destroy(timeline1);
    ht_timeline_destroy(timeline2);
}

TEST_F(TestTimeline, TooLargeEventShouldGoStraightToListeners_DisableSerialization)
{
    // Arrange
    HT_Timeline* timeline = ht_timeline_create(1, HT_TRUE, HT_FALSE, nullptr, nullptr);
    NotifyInfo<DoubleTestEvent> info;
    ht_timeline_register_listener(timeline, test_listener<DoubleTestEvent>, &info);

    // Act
    HT_REGISTER_EVENT_KLASS(DoubleTestEvent);
    HT_DECL_EVENT(DoubleTestEvent, event);
    event.field = 31337;
    ht_timeline_push_event(timeline, ((HT_Event*)(&event)));

    // Assert
    ASSERT_EQ(1, info.notify_count);
    ASSERT_EQ(sizeof(DoubleTestEvent), info.notified_events);
    ASSERT_EQ(event.field, info.values.front().field);

    ht_timeline_unregister_all_listeners(timeline);
    ht_timeline_destroy(timeline);
}

TEST_F(TestTimeline, TooLargeEventShouldGoStraightToListeners_EnableSerialization)
{
    // Arrange
    HT_Timeline* timeline = ht_timeline_create(1, HT_TRUE, HT_TRUE, nullptr, nullptr);
    HT_Byte buffer[64];
    ht_timeline_register_listener(timeline, [] (TEventPtr events, size_t event_count, HT_Boolean /* is_serialized */, void* user_data) {
        HT_Byte* data = (HT_Byte*)user_data;
        memcpy(data, events, event_count);
    }, buffer);

    // Act
    HT_REGISTER_EVENT_KLASS(RegistryTestEvent);
    HT_DECL_EVENT(RegistryTestEvent, event);
    event.field = 30;
    ht_timeline_push_event(timeline, ((HT_Event*)(&event)));

    // Assert
    HT_Event tmp_event;
    int read_value = *(int*)(buffer + ht_HT_Event_get_size(&tmp_event));
    ASSERT_EQ(event.field, read_value);

    ht_timeline_unregister_all_listeners(timeline);
    ht_timeline_destroy(timeline);
}

TEST_F(TestTimeline, PushingLargeEventShouldNotCrashApplication)
{
    // Arrange
    HT_REGISTER_EVENT_KLASS(LargeTestEvent);
    HT_REGISTER_EVENT_KLASS(SuperLargeTestEvent);
    HT_Timeline* timeline = ht_timeline_create(1, HT_TRUE, HT_TRUE, nullptr, nullptr);
    bool called = false;
    ht_timeline_register_listener(timeline, [] (TEventPtr /* events */, size_t /* event_count */, HT_Boolean /* is_serialized */, void* user_data) {
        *static_cast<bool*>(user_data) = true;
    }, &called);

    // Act
    HT_DECL_EVENT(SuperLargeTestEvent, event);
    ht_timeline_push_event(timeline, HT_EVENT(&event));

    // Assert
    ASSERT_TRUE(called);

    ht_timeline_unregister_all_listeners(timeline);
    ht_timeline_destroy(timeline);
}

TEST_F(TestTimeline, DestroyShouldCallDestroyCallbacksIfSpecified)
{
    // Arrange
    struct UD
    {
        void* self = nullptr;
        int cb_call_count = 0;
    };
    auto cb = [](TEventPtr, size_t, HT_Boolean, void* data) {
        reinterpret_cast<UD*>(data)->cb_call_count++;
    };
    auto destroy_cb = [](void* data) {
        reinterpret_cast<UD*>(data)->self = data;
    };

    UD user_data;
    HT_Timeline* timeline = ht_timeline_create(1, HT_TRUE, HT_TRUE, nullptr, nullptr);
    ht_timeline_register_listener_full(timeline, cb, &user_data, destroy_cb);

    HT_DECL_EVENT(HT_Event, event);
    ht_timeline_push_event(timeline, &event);

    // Act
    ht_timeline_destroy(timeline);

    // Assert
    ASSERT_EQ(&user_data, user_data.self);
    ASSERT_EQ(1, user_data.cb_call_count);
}


typedef struct
{
    HT_Feature base;
} TimelineTestFeature;
static void
test_feature_destroy(HT_Feature* feature)
{
    ht_free(feature);
}

HT_FEATURE_DEFINE(TimelineTestFeature, test_feature_destroy)

TEST_F(TestTimeline, SettingFeatureIdWithInvalidIdentifierShouldFail)
{
    // Arrange
    TimelineTestFeature_get_class()->id = 99999;
    TimelineTestFeature* feature = TimelineTestFeature_alloc();

    // Act
    HT_ErrorCode ret = ht_timeline_set_feature(_timeline, (HT_Feature*)feature);

    // Assert
    ASSERT_EQ(HT_ERR_FEATURE_NOT_REGISTERED, ret);
}
