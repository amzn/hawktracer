#ifndef HAWKTRACER_CALLSTACK_TIMELINE_H
#define HAWKTRACER_CALLSTACK_TIMELINE_H

#include <hawktracer/callstack_base_timeline.h>

HT_DECLS_BEGIN

void ht_callstack_timeline_int_start(HT_CallstackBaseTimeline* timeline, HT_CallstackEventLabel label);

void ht_callstack_timeline_string_start(HT_CallstackBaseTimeline* timeline, const char* label);

HT_DECLS_END

#endif /* HAWKTRACER_CALLSTACK_TIMELINE_H */
