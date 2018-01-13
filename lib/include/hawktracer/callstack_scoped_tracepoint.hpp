#ifndef HAWKTRACER_CALLSTACK_SCOPED_TRACEPOINT_HPP
#define HAWKTRACER_CALLSTACK_SCOPED_TRACEPOINT_HPP

#include <hawktracer/feature_callstack.h>

namespace HawkTracer
{

#ifdef HT_CPP11
template<typename ...T>
#else
template<typename ...T>
#endif
class CallstackScopedTracepoint
{
public:
    CallstackScopedTracepoint(
            HT_Timeline* timeline,
#ifdef HT_CPP11
            void (*start_fnc)(HT_Timeline*, T...), T... label
#else
            void (*start_fnc)(HT_Timeline*, T), T label
#endif
           ):
        _timeline(timeline)
    {
        start_fnc(_timeline, label...);
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
