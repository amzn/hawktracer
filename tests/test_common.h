#ifndef HAWKTRACER_TESTS_TEST_COMMON_H
#define HAWKTRACER_TESTS_TEST_COMMON_H

#include <hawktracer/events.h>

#include <vector>

template<typename T>
struct NotifyInfo
{
    int notify_count = 0;
    size_t notified_events = 0;
    std::vector<T> values;
};

template<typename T>
void test_listener(TEventPtr events, size_t event_count, void* user_data)
{
    NotifyInfo<T>* i = static_cast<NotifyInfo<T>*>(user_data);
    i->notified_events += event_count;
    i->notify_count++;
    TEventPtr end = events + event_count;
    while (events < end)
    {
        i->values.push_back(*((T*)events));
        events += HT_EVENT_GET_CLASS(events)->size;
    }
}

#endif /* HAWKTRACER_TESTS_TEST_COMMON_H */
