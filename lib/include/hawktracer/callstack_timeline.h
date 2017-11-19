#ifndef HAWKTRACER_CALLSTACK_TIMELINE_H
#define HAWKTRACER_CALLSTACK_TIMELINE_H

#include <hawktracer/callstack_base_timeline.h>

HT_DECLS_BEGIN

typedef uint64_t HT_CallstackEventLabel;

typedef struct
{
    HT_CallstackBaseEvent base;

    HT_CallstackEventLabel label;
} HT_CallstackIntEvent;
HT_DECLARE_EVENT_KLASS(HT_CallstackIntEvent);

#define HT_CALLSTACK_LABEL_MAX_LEN 31u

typedef struct
{
    HT_CallstackBaseEvent base;

    char label[HT_CALLSTACK_LABEL_MAX_LEN + 1];
} HT_CallstackStringEvent;
HT_DECLARE_EVENT_KLASS(HT_CallstackStringEvent);

void ht_callstack_timeline_int_start(HT_CallstackBaseTimeline* timeline, HT_CallstackEventLabel label);

void ht_callstack_timeline_string_start(HT_CallstackBaseTimeline* timeline, const char* label);

HT_DECLS_END

#endif /* HAWKTRACER_CALLSTACK_TIMELINE_H */
