#include "event_db.hpp"

#include <algorithm>
#include <queue>

namespace HawkTracer
{
namespace viewer
{

EventDB::EventDB()
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
            if ((*it).get_timestamp() < event.get_timestamp())
            {
                break;
            } 
        }
        _events[klass_id].insert(it.base(), std::move(event));
    }
}

void inplace_merge_events(std::vector<EventRef>& response, std::vector<int>& length, int l, int r)
{
    if (l == r)
    {
        return;
    }

    int offset_begin = l ? length[l - 1] : 0;
    int offset_end = length[r];
    int offset_middle = (offset_begin + offset_end) >> 1;

    if (l + 1 == r)
    {
        std::inplace_merge(response.begin() + offset_begin, response.begin() + offset_middle, response.begin() + offset_end,
                [](const EventRef& e1, const EventRef& e2) {
                return e1.get().get_timestamp() < e2.get().get_timestamp();
                });
        return;
    }

    int m = (l + r) >> 1;
    inplace_merge_events(response, length, l, m);
    inplace_merge_events(response, length, m + 1, r);

    std::inplace_merge(response.begin() + offset_begin, response.begin() + offset_middle, response.begin() + offset_end,
            [](const EventRef& e1, const EventRef& e2) {
            return e1.get().get_timestamp() < e2.get().get_timestamp(); 
            });
}

std::vector<EventRef> EventDB::get_data(HT_TimestampNs start_ts, HT_TimestampNs stop_ts, const Query& query)
{
    std::vector<EventRef> response;

    // Dummy events for calling lower_bound
    DummyEvent dummy_start_event(start_ts);
    DummyEvent dummy_stop_event(stop_ts);

    if (query.klass_id == (HT_EventKlassId)-1)
    {
        // Query all the event klasses
        std::vector<int> lengths(_events.size(), 0);
        int klass_index = -1;
        for (const auto& events : _events)
        {
            const auto first_event = std::lower_bound(events.second.begin(), events.second.end(), dummy_start_event,
                    [] (const parser::Event& e1, const parser::Event& e2) {
                    return e1.get_timestamp() < e2.get_timestamp();
                    });
            const auto last_event = std::lower_bound(events.second.begin(), events.second.end(), dummy_stop_event,
                    [] (const parser::Event& e1, const parser::Event& e2) {
                    return e1.get_timestamp() <= e2.get_timestamp();
                    });
            ++klass_index;
            for (auto it = first_event; it != last_event; ++it)
            {
                response.push_back(*it);
            }
            lengths[klass_index] = response.size();
        }

        // Merge responses from querying all events klass
        if (lengths.size() >= 2)
            inplace_merge_events(response, lengths, 0, lengths.size() - 1);
    }    
    else
    {
        // Query a spectific event klass with id equal to query.klass_id 
        const auto& events = _events[query.klass_id];
        const auto first_event = std::lower_bound(events.begin(), events.end(), dummy_start_event,
                [] (const parser::Event& e1, const parser::Event& e2) {
                return e1.get_timestamp() < e2.get_timestamp();
                });
        const auto last_event = std::lower_bound(events.begin(), events.end(), dummy_stop_event,
                [] (const parser::Event& e1, const parser::Event& e2) {
                return e1.get_timestamp() <= e2.get_timestamp();
                });
        for (auto it = first_event; it != last_event; ++it)
        {
            response.push_back(*it);
        }
    }
    return response;
}

} // namespace viewer
} // namespace HawkTracer
