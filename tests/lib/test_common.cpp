#include "test_common.h"

void mixed_test_listener(TEventPtr events, size_t event_count, HT_Boolean is_serialized, void* user_data)
{
    MixedNotifyInfo* i = static_cast<MixedNotifyInfo*>(user_data);
    i->notified_events += event_count;
    i->notify_count++;

    if (is_serialized)
    {
        return;
    }

    TEventPtr end = events + event_count;
    while (events < end)
    {
        if (HT_EVENT_GET_CLASS(events)->klass_id == ht_HT_CallstackIntEvent_get_event_klass_instance()->klass_id)
        {
            i->int_values.push_back(*((HT_CallstackIntEvent*)events));
        }
        else if (HT_EVENT_GET_CLASS(events)->klass_id == ht_HT_CallstackStringEvent_get_event_klass_instance()->klass_id)
        {
            i->string_values.push_back(*((HT_CallstackStringEvent*)events));
        }
        else
        {
            ASSERT_FALSE("Unexpected event type");
        }

        events += HT_EVENT_GET_CLASS(events)->type_info->size;
    }
}
