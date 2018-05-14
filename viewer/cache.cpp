#include "cache.hpp"

#include <hawktracer/base_types.h>

namespace HawkTracer
{
namespace viewer
{

Cache::Cache()
{
}

bool Cache::range_in_cache(HT_TimestampNs start_ts,
                           HT_TimestampNs stop_ts,
                           HT_EventKlassId klass_id)
{
    if (_events.find(klass_id) == _events.end())
    {
        return false;
    }
    HT_TimestampNs cache_start_ts = _events[klass_id].front().get().get_timestamp();
    HT_TimestampNs cache_stop_ts = _events[klass_id].back().get().get_timestamp();
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
    if (_events.find(klass_id) == _events.end())
    {
        return;
    }
    HT_TimestampNs cache_start_ts = _events[klass_id].front().get().get_timestamp();
    HT_TimestampNs cache_stop_ts = _events[klass_id].back().get().get_timestamp();
    bool should_insert_event = cache_start_ts <= event.get().get_timestamp() && event.get().get_timestamp() <= cache_stop_ts;

    if (!should_insert_event)
    {
        return;
    }
    
    std::vector<EventRef>::iterator it;
    for (it = _events[klass_id].begin(); it != _events[klass_id].end(); ++it)
    {
        if (it->get().get_timestamp() > event.get().get_timestamp())
        {
            break;
        }
    }
    _events[klass_id].insert(it, event);
}

} // namespace viewer
} // namespace HawkTracer
