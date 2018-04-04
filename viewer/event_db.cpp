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

std::vector<EventRef> EventDB::get_data(HT_TimestampNs start_ts, HT_TimestampNs stop_ts, HT_EventKlassId klass_id)
{
    std::vector<EventRef> events;

    for (const auto& event : _events)
    {
        auto ts = event.get_timestamp();
        if (event.get_klass()->get_id() != klass_id ||
                ts < start_ts || ts > stop_ts)
        {
            continue;
        }
        events.push_back(event);
    }

    return events;
}

} // namespace viewer
} // namespace HawkTracer
