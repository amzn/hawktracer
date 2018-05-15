#include "cache.hpp"

#include <hawktracer/base_types.h>

#include <algorithm>

namespace HawkTracer
{
namespace viewer
{

bool Cache::range_in_cache(HT_TimestampNs start_ts,
                           HT_TimestampNs stop_ts,
                           HT_EventKlassId klass_id)
{
    auto events = _events.find(klass_id);
    if (events == _events.end())
    {
        return false;
    }
    HT_TimestampNs cache_start_ts = events->second.front().get().get_timestamp();
    HT_TimestampNs cache_stop_ts = events->second.back().get().get_timestamp();
    if (cache_start_ts != start_ts || cache_stop_ts != stop_ts)
    {
        return false;
    }
    return true;
}

std::vector<EventRef> Cache::get_data(HT_EventKlassId klass_id)
{
    return _events[klass_id];
}

void Cache::update(HT_EventKlassId klass_id,
                   std::vector<EventRef> new_response)
{
    if (new_response.size() == 0)
    {
        return;
    }

    _events[klass_id] = std::move(new_response);
}

void Cache::insert_event(EventRef event, HT_EventKlassId klass_id)
{
    auto events = _events.find(klass_id);
    if (events == _events.end())
    {
        return;
    }
    HT_TimestampNs cache_start_ts = events->second.front().get().get_timestamp();
    HT_TimestampNs cache_stop_ts = events->second.back().get().get_timestamp();
    bool should_insert_event = cache_start_ts <= event.get().get_timestamp() && event.get().get_timestamp() <= cache_stop_ts;

    if (!should_insert_event)
    {
        return;
    }
    
    auto it = std::upper_bound(events->second.begin(), events->second.end(), event.get(),
            [] (const EventRef& e1, const EventRef& e2) {
                return e1.get().get_timestamp() < e2.get().get_timestamp();
            });
    events->second.insert(it, event);
}

} // namespace viewer
} // namespace HawkTracer
