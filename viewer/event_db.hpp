#ifndef HAWKTRACER_VIEWER_EVENT_DB_HPP
#define HAWKTRACER_VIEWER_EVENT_DB_HPP

#include <parser/event.hpp>
#include <hawktracer/base_types.h>

#include <unordered_map>
#include <functional>

namespace HawkTracer
{
namespace viewer
{

using EventRef = std::reference_wrapper<const parser::Event>;

class EventDB
{
public:
    EventDB();

    void insert(parser::Event event);

    std::vector<EventRef> get_data(HT_TimestampNs start_ts, HT_TimestampNs stop_ts, HT_EventKlassId klass_id);

private:
    std::vector<parser::Event> _events;
};

} // namespace viewer
} // namespace HawkTracer

#endif // HAWKTRACER_VIEWER_EVENT_DB_HPP
