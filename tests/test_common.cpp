#include "test_common.h"

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
        case HT_EVENT_TYPE_CALLSTACK_INT:
            i->int_values.push_back(*((HT_CallstackIntEvent*)events));
            break;
        case HT_EVENT_TYPE_CALLSTACK_STRING:
            i->string_values.push_back(*((HT_CallstackStringEvent*)events));
            break;
        default:
            ASSERT_FALSE("Unexpected event type");
        }

        events += HT_EVENT_GET_CLASS(events)->event_size;
    }
}
