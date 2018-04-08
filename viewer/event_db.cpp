#include "event_db.hpp"

namespace HawkTracer
{
namespace viewer
{

EventDB::EventDB()
{

}

void EventDB::insert(parser::Event event)
{
    _events.push_back(std::move(event));
}

std::vector<EventRef> EventDB::get_data(HT_TimestampNs start_ts, HT_TimestampNs stop_ts, const Query& query)
{
    std::vector<EventRef> events;

    for (const auto& event : _events)
    {
        auto ts = event.get_timestamp();
        bool get_id_ok = query.klass_id == (HT_EventKlassId)-1 || event.get_klass()->get_id() == query.klass_id;
        bool ts_ok = ts >= start_ts && ts <= stop_ts;

        if (!get_id_ok || !ts_ok)
        {
            continue;
        }
        events.push_back(event);
    }

    return events;
}

} // namespace viewer
} // namespace HawkTracer
