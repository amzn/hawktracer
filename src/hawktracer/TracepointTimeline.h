#ifndef HAWKTRACER_TRACEPOINTTIMELINE_H_
#define HAWKTRACER_TRACEPOINTTIMELINE_H_

#include <hawktracer/Timeline.h>

namespace hawktracer
{

class FunctionCallMarker
{
public:
    using ActionLabel = Action::Label;

    FunctionCallMarker() : _action_label(0) {}
    explicit FunctionCallMarker(ActionLabel label) : _action_label(label) {}

    void start()
    {
        Timeline::get().start_action(_action_label);
    }

    void stop()
    {
        stop_last();
    }

    static void stop_last()
    {
        Timeline::get().stop_last_action();
    }

private:
      ActionLabel _action_label;
};

template<typename FUNCTION_CALL_MARKER>
class ScopedTracepoint
{
public:
    ScopedTracepoint(typename FUNCTION_CALL_MARKER::ActionLabel label) :
        _marker(label)
    {
        _marker.start();
    }

    ~ScopedTracepoint()
    {
        _marker.stop();
    }

private:
    FUNCTION_CALL_MARKER _marker;
};


} // namespace hawktracer

#endif // HAWKTRACER_TRACEPOINTTIMELINE_H_
