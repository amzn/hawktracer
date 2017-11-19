#include "hawktracer/callstack_timeline.h"

#include <string.h>

HT_DEFINE_EVENT_KLASS(HT_CallstackIntEvent, 3); /* TODO don't use numbers */
HT_DEFINE_EVENT_KLASS(HT_CallstackStringEvent, 4);

void
ht_callstack_timeline_int_start(HT_CallstackBaseTimeline* timeline, HT_CallstackEventLabel label)
{
    HT_DECL_EVENT(HT_CallstackIntEvent, event);
    event.label = label;

    ht_callstack_base_timeline_start(timeline, (HT_CallstackBaseEvent*)&event);
}

void
ht_callstack_timeline_string_start(HT_CallstackBaseTimeline* timeline, const char* label)
{
    HT_DECL_EVENT(HT_CallstackStringEvent, event);
    strncpy(event.label, label, HT_CALLSTACK_LABEL_MAX_LEN);
    event.label[HT_CALLSTACK_LABEL_MAX_LEN] = 0;

    ht_callstack_base_timeline_start(timeline, (HT_CallstackBaseEvent*)&event);
}
