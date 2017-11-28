#include <hawktracer/callstack_timeline.h>

#include "test_common.h"

#include <gtest/gtest.h>

struct MixedNotifyInfo
{
    int notify_count = 0;
    size_t notified_events = 0;
    std::vector<HT_CallstackIntEvent> int_values;
    std::vector<HT_CallstackStringEvent> string_values;
};

void mixed_test_listener(TEventPtr events, size_t event_count, void* user_data)
{
    MixedNotifyInfo* i = static_cast<MixedNotifyInfo*>(user_data);
    i->notified_events += event_count;
    i->notify_count++;
    TEventPtr end = events + event_count;
    while (events < end)
    {
        switch (HT_EVENT_GET_CLASS(events)->type)
        {
        case 3: /* TODO don't use numbers */
            i->int_values.push_back(*((HT_CallstackIntEvent*)events));
            break;
        case 4: /* TODO don't use numbers */
            i->string_values.push_back(*((HT_CallstackStringEvent*)events));
            break;
        default:
            ASSERT_FALSE("Unexpected event type");
        }

        events += HT_EVENT_GET_CLASS(events)->size;
    }
}

class TestCallstackTimeline : public ::testing::Test
{
protected:
    void TearDown() override
    {
        ht_timeline_unregister_all_listeners(HT_TIMELINE(_timeline));
        ht_timeline_destroy(HT_TIMELINE(_timeline));
    }

    HT_CallstackBaseTimeline* _timeline = nullptr;
};

TEST_F(TestCallstackTimeline, LabelStringLongerThanMaxShouldBeTruncated)
{
    // Arrange
    _timeline = (HT_CallstackBaseTimeline*)ht_timeline_create("HT_CallstackBaseTimeline", "buffer-capacity", sizeof(HT_CallstackStringEvent) * 3, nullptr);
    NotifyInfo<HT_CallstackStringEvent> info;
    std::string postfix = "Very very very long stirng which exceeds max label length."
                          "Lorem ipsum dolor sit amet, mel no timeam electram instructior,"
                          "pri omnis quodsi audiam cu. Eos fuisset iracundia ex. Illud verear"
                          "indoctum no mei, mea singulis adolescens et. Dicam audire iudicabit"
                          "ut est, duo ei graecis adipisci dissentiet. Modus oratio ea nec,"
                          "nam ne facete repudiandae.";

    ht_timeline_register_listener(HT_TIMELINE(_timeline), test_listener<HT_CallstackStringEvent>, &info);

    // Act
    for (int i = 0; i < 4; i++)
    {
        HT_DECL_EVENT(HT_CallstackStringEvent, event);
        std::string label = std::to_string(i) + postfix;

        ht_callstack_timeline_string_start(_timeline, label.c_str());
    }

    for (int i = 0; i < 4; i++)
    {
        ht_callstack_base_timeline_stop(_timeline);
    }

    ht_timeline_flush(HT_TIMELINE(_timeline));

    // Assert
    ASSERT_EQ(4 * sizeof(HT_CallstackStringEvent), info.notified_events);
    ASSERT_EQ(2, info.notify_count);
    ASSERT_EQ(4u, info.values.size());
    for (size_t i = 0; i < info.values.size(); i++)
    {
        std::string expected = std::to_string(3 - i) + postfix.substr(0, HT_CALLSTACK_LABEL_MAX_LEN - 1);
        ASSERT_EQ(HT_CALLSTACK_LABEL_MAX_LEN, strlen(info.values[i].label));
        ASSERT_STREQ(expected.c_str(), info.values[i].label);
    }
}

TEST_F(TestCallstackTimeline, SimpleIntCallstackTest)
{
    // Arrange
    _timeline = (HT_CallstackBaseTimeline*)ht_timeline_create("HT_CallstackBaseTimeline", "buffer-capacity", sizeof(HT_CallstackIntEvent) * 3, nullptr);
    NotifyInfo<HT_CallstackIntEvent> info;

    ht_timeline_register_listener(HT_TIMELINE(_timeline), test_listener<HT_CallstackIntEvent>, &info);

    // Act
    for (int i = 0; i < 4; i++)
    {
        HT_DECL_EVENT(HT_CallstackIntEvent, event);

        ht_callstack_timeline_int_start(_timeline, i);
    }

    for (int i = 0; i < 4; i++)
    {
        ht_callstack_base_timeline_stop(_timeline);
    }

    ht_timeline_flush(HT_TIMELINE(_timeline));

    // Assert
    ASSERT_EQ(4 * sizeof(HT_CallstackIntEvent), info.notified_events);
    ASSERT_EQ(2, info.notify_count);
    ASSERT_EQ(4u, info.values.size());
    for (size_t i = 0; i < info.values.size(); i++)
    {
        ASSERT_EQ(3 - i, info.values[i].label);
    }
}

TEST_F(TestCallstackTimeline, MixedCallstackEventTypes)
{
    // Arrange
    _timeline = (HT_CallstackBaseTimeline*)ht_timeline_create("HT_CallstackBaseTimeline", "buffer-capacity", sizeof(HT_CallstackIntEvent) + sizeof(HT_CallstackStringEvent), nullptr);
    MixedNotifyInfo info;

    ht_timeline_register_listener(HT_TIMELINE(_timeline), mixed_test_listener, &info);

    const char* label1 = "label1";
    const char* label2 = "label2";

    // Act
    ht_callstack_timeline_int_start(_timeline, 1);
    ht_callstack_timeline_string_start(_timeline, label1);
    ht_callstack_timeline_int_start(_timeline, 2);
    ht_callstack_base_timeline_stop(_timeline);
    ht_callstack_timeline_string_start(_timeline, label2);
    ht_callstack_base_timeline_stop(_timeline);
    ht_callstack_base_timeline_stop(_timeline);
    ht_callstack_base_timeline_stop(_timeline);

    ht_timeline_flush(HT_TIMELINE(_timeline));

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
