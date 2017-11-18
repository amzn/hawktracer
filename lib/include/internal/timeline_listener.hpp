#ifndef HAWKTRACER_INTERNAL_TIMELINE_LISTENER_HPP
#define HAWKTRACER_INTERNAL_TIMELINE_LISTENER_HPP

#include "hawktracer/timeline_listener.h"

#include <vector>

struct _HT_TimelineListenerContainer
{
    typedef std::vector<std::pair<HT_TimelineListenerCallback, void*>> Vector;
    Vector listeners;
};

#endif /* HAWKTRACER_INTERNAL_TIMELINE_LISTENER_HPP */
