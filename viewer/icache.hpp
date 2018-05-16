#ifndef HAWKTRACER_VIEWER_ICACHE_HPP
#define HAWKTRACER_VIEWER_ICACHE_HPP

#include <hawktracer/base_types.h>
#include <hawktracer/parser/event.hpp>

namespace HawkTracer
{
namespace viewer
{

using EventRef = std::reference_wrapper<const parser::Event>;

class ICache
{
public:
    virtual ~ICache() {}

    virtual bool range_in_cache(HT_TimestampNs start_ts,
                                HT_TimestampNs stop_ts,
                                HT_EventKlassId klass_id) = 0;

    virtual bool get_data(HT_EventKlassId klass_id, std::vector<EventRef>& events) = 0;

    virtual void update(HT_EventKlassId klass_id,
                        std::vector<EventRef> new_response) = 0;

    virtual void insert_event(EventRef event) = 0;
};

} // namespace viewer
} // namespace HawkTracer

#endif // HAWKTRACER_VIEWER_ICACHE_HPP
