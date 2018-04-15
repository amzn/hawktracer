#ifndef HAWKTRACER_VIEWER_EVENT_DB_HPP
#define HAWKTRACER_VIEWER_EVENT_DB_HPP

#include <hawktracer/parser/event.hpp>
#include <hawktracer/base_types.h>

#include <unordered_map>
#include <functional>

struct Query
{
    size_t klass_id = (size_t)-1; // TODO: optional<>
};

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

    std::vector<EventRef> get_data(HT_TimestampNs start_ts, HT_TimestampNs stop_ts, const Query& query);

private:
    std::vector<parser::Event> _events;
};

} // namespace viewer
} // namespace HawkTracer

#endif // HAWKTRACER_VIEWER_EVENT_DB_HPP
