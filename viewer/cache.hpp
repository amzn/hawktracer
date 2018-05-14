#ifndef HAWKTRACER_VIEWER_CACHE_HPP
#define HAWKTRACER_VIEWER_CACHE_HPP

#include <hawktracer/base_types.h>
#include <hawktracer/parser/event.hpp>

#include <unordered_map>
#include <vector>

namespace HawkTracer
{
namespace viewer
{

using EventRef = std::reference_wrapper<const parser::Event>;

class Cache
{
public:
    Cache();

    bool range_in_cache(HT_TimestampNs start_ts,
                        HT_TimestampNs stop_ts,
                        HT_EventKlassId klass_id);

    std::vector<EventRef> get_data(HT_EventKlassId klass_id);

    void update(HT_EventKlassId klass_id,
                std::vector<EventRef> new_response);

    void insert_event(EventRef event, HT_EventKlassId klass_id);
    
private: 
    std::unordered_map<HT_EventKlassId, std::vector<EventRef>> _events;
};

} // namespace viewer
} // namespace HawkTracer

#endif // HAWKTRACER_VIEWER_CACHE_HPP
