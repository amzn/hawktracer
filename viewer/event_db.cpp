#include "event_db.hpp"

#include <algorithm>

namespace HawkTracer
{
namespace viewer
{

EventDB::EventDB(std::unique_ptr<ICache> cache) :
    _cache(std::move(cache))
{
}

void EventDB::insert(parser::Event event)
{
    auto klass_id = event.get_klass()->get_id();
    if (_events.find(klass_id) == _events.end())
    {
        _events[klass_id] = std::vector<parser::Event>(1, std::move(event));
    }
    else
    {
        std::vector<parser::Event>::reverse_iterator it;
        for (it = _events[klass_id].rbegin(); it != _events[klass_id].rend(); ++it)
        {   
            if (it->get_timestamp() < event.get_timestamp())
            {
                break;
            } 
        }
        _events[klass_id].insert(it.base(), std::move(event));
        _cache->insert_event(_events[klass_id].back());
    }
}

static void append_events(std::vector<EventRef>& dst, 
                          std::vector<parser::Event>::iterator first_event, 
                          std::vector<parser::Event>::iterator last_event) 
{
    for (auto it = first_event; it != last_event; ++it)
    {
        dst.push_back(*it);
    }
}

std::vector<EventRef> EventDB::get_data(HT_TimestampNs start_ts,
                                        HT_TimestampNs stop_ts,
                                        const Query& query)
{
    std::vector<EventRef> response;

    if (start_ts > stop_ts)
    {
        return response;
    }

    if (query.klass_id == (HT_EventKlassId)-1)
    {
        _query_all_event_klasses(response, start_ts, stop_ts);
    }    
    else
    {
        auto events = _events.find(query.klass_id);
        if (events == _events.end())
        {
            return response;
        }

        if (_cache->range_in_cache(start_ts, stop_ts, query.klass_id))
        {
            _cache->get_data(query.klass_id, response);
        }
        else
        {
            _query_event_klass(response, _events[query.klass_id], start_ts, stop_ts);
            _cache->update(query.klass_id, response);
        }
    }
    return response;
}

static void inplace_merge_events(std::vector<EventRef>& response, 
                                 std::vector<int>& length, 
                                 int first,
                                 int last)
{
    if (first == last)
    {
        return;
    }

    int offset_begin = first ? length[first - 1] : 0;
    int offset_end = length[last];
    int offset_middle = (offset_begin + offset_end) >> 1;

    if (first + 1 == last)
    {
        std::inplace_merge(response.begin() + offset_begin, response.begin() + offset_middle, response.begin() + offset_end,
                [](const EventRef& e1, const EventRef& e2) {
                return e1.get().get_timestamp() < e2.get().get_timestamp();
                });
        return;
    }

    int middle = (first + last) >> 1;
    inplace_merge_events(response, length, first, middle);
    inplace_merge_events(response, length, middle + 1, last);

    std::inplace_merge(response.begin() + offset_begin, response.begin() + offset_middle, response.begin() + offset_end,
            [](const EventRef& e1, const EventRef& e2) {
            return e1.get().get_timestamp() < e2.get().get_timestamp(); 
            });
}

void EventDB::_query_all_event_klasses(std::vector<EventRef>& response,
                                       HT_TimestampNs start_ts,
                                       HT_TimestampNs stop_ts)
{  
    std::vector<int> lengths;
    lengths.resize(_events.size());
    for (auto& events : _events)
    {
        _query_event_klass(response, events.second, start_ts, stop_ts);
        lengths.push_back(response.size());
    }

    // Merge responses from querying all event klasses
    if (lengths.size() >= 2)
        inplace_merge_events(response, lengths, 0, lengths.size() - 1);
}

void EventDB::_query_event_klass(std::vector<EventRef>& response,
                                 std::vector<parser::Event>& events,
                                 HT_TimestampNs start_ts,
                                 HT_TimestampNs stop_ts)
{
    std::vector<parser::Event>::iterator first_event;
    std::vector<parser::Event>::iterator last_event;
    _get_range_of_events(events, start_ts, stop_ts, first_event, last_event);
    append_events(response, first_event, last_event);
}

void EventDB::_get_range_of_events(std::vector<parser::Event>& events,
                                   HT_TimestampNs start_ts,
                                   HT_TimestampNs stop_ts,
                                   std::vector<parser::Event>::iterator& first_event,
                                   std::vector<parser::Event>::iterator& last_event)
{
    // Events for calling lower_bound and upper_bound
    CompareTimestampEvent start_event(start_ts);
    CompareTimestampEvent stop_event(stop_ts);

    first_event = std::lower_bound(events.begin(), events.end(), start_event,
            [] (const parser::Event& e1, const parser::Event& e2) {
            return e1.get_timestamp() < e2.get_timestamp();
            });

    last_event = std::upper_bound(events.begin(), events.end(), stop_event,
            [] (const parser::Event& e1, const parser::Event& e2) {
            return e1.get_timestamp() < e2.get_timestamp();
            });
}

} // namespace viewer
} // namespace HawkTracer
