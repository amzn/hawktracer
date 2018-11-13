#ifndef HAWKTRACER_SCOPED_TRACEPOINT_HPP
#define HAWKTRACER_SCOPED_TRACEPOINT_HPP

#ifdef __cplusplus

#include <hawktracer/feature_callstack.h>

namespace HawkTracer
{

#ifdef HT_CPP11
template<typename ...T>
#else
template<typename T>
#endif
class ScopedTracepoint
{
public:
    ScopedTracepoint(
            HT_Timeline* timeline,
#ifdef HT_CPP11
            void (*start_fnc)(HT_Timeline*, T...), T... label
#else
            void (*start_fnc)(HT_Timeline*, T), T label
#endif
           ):
        _timeline(timeline)
    {
#ifdef HT_CPP11
        start_fnc(_timeline, label...);
#else
        start_fnc(_timeline, label);
#endif
    }

    ~ScopedTracepoint()
    {
        ht_feature_callstack_stop(_timeline);
    }

private:
    HT_Timeline* _timeline;
};

} /* namespace HawkTracer */

#endif /* __cplusplus */

#endif /* HAWKTRACER_SCOPED_TRACEPOINT_HPP */
