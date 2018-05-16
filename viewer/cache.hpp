#ifndef HAWKTRACER_VIEWER_CACHE_HPP
#define HAWKTRACER_VIEWER_CACHE_HPP

#include "icache.hpp"

#include <hawktracer/base_types.h>
#include <hawktracer/parser/event.hpp>

namespace HawkTracer
{
namespace viewer
{

class Cache : public ICache
{
public:
    bool range_in_cache(HT_TimestampNs start_ts,
                        HT_TimestampNs stop_ts,
                        HT_EventKlassId klass_id) override;

    bool get_data(HT_EventKlassId klass_id, std::vector<EventRef>& events) override;

    void update(HT_EventKlassId klass_id,
                std::vector<EventRef> new_response) override;

    void insert_event(EventRef event) override;
    
private: 
    std::unordered_map<HT_EventKlassId, std::vector<EventRef>> _events;
};

} // namespace viewer
} // namespace HawkTracer

#endif // HAWKTRACER_VIEWER_CACHE_HPP
