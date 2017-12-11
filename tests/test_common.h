#ifndef HAWKTRACER_TESTS_TEST_COMMON_H
#define HAWKTRACER_TESTS_TEST_COMMON_H

#include <hawktracer/core_events.h>

#include <gtest/gtest.h>

#include <vector>

template<typename T>
struct NotifyInfo
{
    int notify_count = 0;
    size_t notified_events = 0;
    std::vector<T> values;
};

struct MixedNotifyInfo
{
    int notify_count = 0;
    size_t notified_events = 0;
    std::vector<HT_CallstackIntEvent> int_values;
    std::vector<HT_CallstackStringEvent> string_values;
};

template<typename T>
void test_listener(TEventPtr events, size_t event_count, HT_Boolean is_serialized, void* user_data)
{
    NotifyInfo<T>* i = static_cast<NotifyInfo<T>*>(user_data);

    i->notify_count++;
    i->notified_events += event_count;

    if (is_serialized)
    {
        return;
    }

    TEventPtr end = events + event_count;
    while (events < end)
    {
        i->values.push_back(*((T*)events));
        events += HT_EVENT_GET_CLASS(events)->type_info->size;
    }
}

void mixed_test_listener(TEventPtr events, size_t event_count, HT_Boolean is_serialized, void* user_data);

#endif /* HAWKTRACER_TESTS_TEST_COMMON_H */
