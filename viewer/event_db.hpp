#ifndef HAWKTRACER_VIEWER_EVENT_DB_HPP
#define HAWKTRACER_VIEWER_EVENT_DB_HPP

#include "cache.hpp"

#include <hawktracer/parser/event.hpp>
#include <hawktracer/base_types.h>

struct Query
{
    HT_EventKlassId klass_id = (HT_EventKlassId)-1; // TODO: optional<>
};

namespace HawkTracer
{
namespace viewer
{

class EventDB
{
public:
    EventDB(std::unique_ptr<ICache> cache);

    void insert(parser::Event event);

    std::vector<EventRef> get_data(HT_TimestampNs start_ts,
                                   HT_TimestampNs stop_ts,
                                   const Query& query);
private:
    std::unordered_map<HT_EventKlassId, std::vector<parser::Event>> _events;
    std::unique_ptr<ICache> _cache;
    
    class CompareTimestampEvent : public parser::Event
    {
    public:
        CompareTimestampEvent(HT_TimestampNs timestamp)
        {
            _timestamp = timestamp;
        }
    };

    void _query_all_event_klasses(std::vector<EventRef>& response,
                                  HT_TimestampNs start_ts,
                                  HT_TimestampNs stop_ts);
    void _query_event_klass(std::vector<EventRef>& response,
                            std::vector<parser::Event>& events,
                            HT_TimestampNs start_ts,
                            HT_TimestampNs stop_ts);
    void _get_range_of_events(std::vector<parser::Event>& events,
                              HT_TimestampNs start_ts,
                              HT_TimestampNs stop_ts,
                              std::vector<parser::Event>::iterator& first_event,
                              std::vector<parser::Event>::iterator& last_event);
};

} // namespace viewer
} // namespace HawkTracer

#endif // HAWKTRACER_VIEWER_EVENT_DB_HPP
