#ifndef HAWKTRACER_CALLSTACK_SCOPED_TRACEPOINT_HPP
#define HAWKTRACER_CALLSTACK_SCOPED_TRACEPOINT_HPP

#include <hawktracer/feature_callstack.h>

namespace HawkTracer
{

template<typename T>
class CallstackScopedTracepoint
{
public:
    CallstackScopedTracepoint(
            HT_Timeline* timeline,
            void (*start_fnc)(HT_Timeline*, T), T label) :
        _timeline(timeline)
    {
        start_fnc(_timeline, label);
    }

    ~CallstackScopedTracepoint()
    {
        ht_feature_callstack_stop(_timeline);
    }

private:
    HT_Timeline* _timeline;
};

} /* namespace HawkTracer */

#endif /* HAWKTRACER_CALLSTACK_SCOPED_TRACEPOINT_HPP */
