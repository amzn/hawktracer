#ifndef HAWKTRACER_CALLSTACK_SCOPED_TRACEPOINT_HPP
#define HAWKTRACER_CALLSTACK_SCOPED_TRACEPOINT_HPP

#include <hawktracer/callstack_timeline.h>

namespace HawkTracer
{

template<typename T>
class CallstackScopedTracepoint
{
public:
    CallstackScopedTracepoint(
            HT_CallstackTimeline* timeline,
            void (*start_fnc)(HT_CallstackTimeline*, T), T label) :
        _timeline(timeline)
    {
        start_fnc(_timeline, label);
    }

    ~CallstackScopedTracepoint()
    {
        ht_callstack_timeline_stop(_timeline);
    }

private:
    HT_CallstackTimeline* _timeline;
};

} /* namespace HawkTracer */

#endif /* HAWKTRACER_CALLSTACK_SCOPED_TRACEPOINT_HPP */
